#include "editor/ui/main_window.h"
#include "editor/config.h"
#include "editor/hotkeys/hotkey_config.h"
#include "editor/hotkeys/hotkey_dialog.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/panels/inspector.h"
#include "editor/ui/panels/left_shelf.h"
#include "editor/ui/panels/sprite_preview_dialog.h"
#include "editor/ui/tools/drawing_toolbar.h"
#include "editor/ui/widget_helpers.h"
#include <QtConcurrent>
#include <QtWidgets>
#include <algorithm>
#include <stdexcept>

namespace editor {
namespace {
constexpr int WINDOW_WIDTH = 1440;
constexpr int WINDOW_HEIGHT = 900;
constexpr int TOOLBAR_BUTTON_WIDTH = 112;
constexpr int TOOLBAR_BUTTON_HEIGHT = 52;
// Status messages that report a finished command stay visible a little longer.
constexpr int STATUS_SHORT_MS = 6000;
constexpr int STATUS_LONG_MS = 10000;
// Extra map units suggested around a stamp that does not fit the map.
constexpr double STAMP_MAP_SIZE_MARGIN = 200.0;
constexpr const char *DEFAULT_PROJECT_FILE = "auto_mapper_project.json";
constexpr const char *DEFAULT_MAP_FILE = "ui_output.map";
// Shown in both languages on purpose, so users can always find the language switch.
constexpr const char *LANGUAGE_BUTTON_TEXT = "语言\nlanguage [F10]";
// Keys of rememberedWallTypes_.
constexpr const char *FAMILY_AS1 = "AS1";
constexpr const char *FAMILY_AS2 = "AS2";

// Toolbar checkboxes must not toggle on Space: Space belongs to the canvas.
class CommandCheckBox : public QCheckBox {
  public:
    explicit CommandCheckBox(const QString &text) : QCheckBox(text) {}

  protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Space) {
            event->ignore();
            return;
        }
        QCheckBox::keyPressEvent(event);
    }
};

// Gray but clickable "unsupported in this project format" state, styled by QSS.
void setUnavailable(QWidget *widget, bool unavailable) {
    widget->setProperty("unavailable", unavailable);
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}
} // namespace

MainWindow::MainWindow(const QString &localeName, QWidget *parent)
    : QMainWindow(parent), locale(localeName), catalog(locale), hotkeys_(this) {
    setObjectName("AutoMapperWindow");
    setWindowTitle(locale.tr(TextKey::WINDOW_TITLE) + "[*]");
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    viewport = new Viewport(project, catalog, this);
    setCentralWidget(viewport);

    drawingToolbar_ = new DrawingToolbar(locale, this);
    addToolBar(Qt::LeftToolBarArea, drawingToolbar_);
    connect(drawingToolbar_, &DrawingToolbar::modeTriggered, this, &MainWindow::activateMode);
    buildCommandToolbar();
    buildDocks();
    // The saved game folder was opened while the toolbar was built, before the inspector existed.
    inspector_->setSpritePreviewReady(viewport->hasGameFolder());
    connectViewport();
    configureHotkeys();

    connect(&exportWatcher_, &QFutureWatcher<ExportResult>::finished, this, [this] {
        setEnabled(true);
        unsetCursor();
        ExportResult result = exportWatcher_.result();
        if (result.success) {
            qInfo() << "Generated map:" << outputPath_;
            showStatus(locale.tr(TextKey::STATUS_GENERATED_MAP, {{"output_path", outputPath_}}), STATUS_LONG_MS);
            return;
        }
        QString message = locale.tr(TextKey::ERROR_CPP_ENGINE_GENERATE_FAILED);
        if (!result.engineError.isEmpty())
            message = locale.tr(TextKey::ERROR_CPP_ENGINE, {{"error", result.engineError}});
        errorDialog(locale.tr(TextKey::DIALOG_GENERATE_MAP), message);
    });

    // Window-wide hotkeys: filter key presses before focused child widgets consume them.
    qApp->installEventFilter(this);
    syncControls();
    rebuildShelf();
    showStatus(locale.tr(TextKey::STATUS_READY));
    qInfo() << "Main window ready, locale" << locale.name();
}

MainWindow::~MainWindow() {
    qApp->removeEventFilter(this);
    exportWatcher_.waitForFinished();
}

bool MainWindow::isDirty() const {
    return dirty_;
}

// ---------------------------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------------------------

void MainWindow::buildCommandToolbar() {
    auto *toolbar = new QToolBar(locale.tr(TextKey::TOOLBAR_MAIN_TOOLS));
    toolbar->setObjectName("mainToolbar");
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addToolBar(Qt::TopToolBarArea, toolbar);

    // Command text carries its hotkey on a second line, e.g. "New\n[F1]".
    auto commandText = [this](const char *textKey, const char *commandId) {
        return locale.tr(textKey) + "\n" + hotkeys::hotkeyHint(commandId);
    };
    QAction *action = toolbar->addAction(commandText(TextKey::ACTION_NEW, hotkeys::COMMAND_NEW));
    connect(action, &QAction::triggered, this, &MainWindow::newProject);
    toolbar->addSeparator();
    action = toolbar->addAction(commandText(TextKey::ACTION_IMPORT_JSON, hotkeys::COMMAND_IMPORT_JSON));
    connect(action, &QAction::triggered, this, &MainWindow::importDialog);
    action = toolbar->addAction(commandText(TextKey::ACTION_EXPORT_JSON, hotkeys::COMMAND_EXPORT_JSON));
    connect(action, &QAction::triggered, this, [this] {
        saveDialog();
    });
    toolbar->addSeparator();
    action = toolbar->addAction(commandText(TextKey::ACTION_GENERATE_MAP, hotkeys::COMMAND_GENERATE_MAP));
    connect(action, &QAction::triggered, this, &MainWindow::exportDialog);
    toolbar->addSeparator();

    floor_ = new CommandCheckBox(commandText(TextKey::CHECK_FLOOR, hotkeys::COMMAND_FLOOR));
    floor_->setObjectName("floorCheck");
    floor_->setToolTip(locale.tr(TextKey::TOOLTIP_FLOOR));
    ceiling_ = new CommandCheckBox(commandText(TextKey::CHECK_CEILING, hotkeys::COMMAND_CEILING));
    ceiling_->setObjectName("ceilingCheck");
    ceiling_->setToolTip(locale.tr(TextKey::TOOLTIP_CEILING));
    doors_ = new CommandCheckBox(commandText(TextKey::CHECK_IS_DOOR_OPEN, hotkeys::COMMAND_DOOR_STATE));
    doors_->setObjectName("isDoorOpenCheck");
    doors_->setToolTip(locale.tr(TextKey::TOOLTIP_IS_DOOR_OPEN));
    random_ = new CommandCheckBox(commandText(TextKey::CHECK_RANDOM_DIRECTION, hotkeys::COMMAND_RANDOM_DIRECTION));
    random_->setObjectName("randomDirectionCheck");
    random_->setToolTip(locale.tr(TextKey::TOOLTIP_RANDOM_DIRECTION));
    // Direction Randomization is recommended, so it starts enabled.
    random_->setChecked(true);
    for (QCheckBox *box : {floor_, ceiling_, doors_, random_})
        toolbar->addWidget(box);
    connect(ceiling_, &QCheckBox::toggled, this, [this](bool checked) {
        inspector_->ceiling()->setProjectState(project.isAS1(), checked);
        qInfo() << "Ceiling generation changed:" << checked;
    });
    connect(doors_, &QCheckBox::toggled, this, &MainWindow::toggleDoorsOpen);

    auto *spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    // Sprite preview window: game folder plus the decoration display options, all kept in AutoMapper.cfg.
    spritePreviewButton_ = new QPushButton(locale.tr(TextKey::ACTION_SPRITE_PREVIEW));
    spritePreviewButton_->setObjectName("spritePreviewButton");
    spritePreviewButton_->setToolTip(locale.tr(TextKey::TOOLTIP_SPRITE_PREVIEW));
    spritePreviewButton_->setFixedSize(TOOLBAR_BUTTON_WIDTH, TOOLBAR_BUTTON_HEIGHT);
    toolbar->addWidget(spritePreviewButton_);
    connect(spritePreviewButton_, &QPushButton::clicked, this, &MainWindow::showSpritePreview);

    viewport->stampCrossesWithSprites = loadStampCrossesWithSprites();
    viewport->stampAlwaysShowFrame = loadStampAlwaysShowFrame();
    viewport->stampSpriteOpacity = loadStampSpriteOpacity();
    spritePreview_ = new SpritePreviewDialog(locale, this);
    spritePreview_->setOptions(viewport->stampCrossesWithSprites, viewport->stampAlwaysShowFrame,
                               viewport->stampSpriteOpacity);
    connect(spritePreview_, &SpritePreviewDialog::chooseGameFolderRequested, this, &MainWindow::chooseGameFolder);
    connect(spritePreview_, &SpritePreviewDialog::crossesWithSpritesChanged, this, [this](bool checked) {
        viewport->stampCrossesWithSprites = checked;
        viewport->update();
        try {
            saveStampCrossesWithSprites(checked);
        } catch (const std::exception &error) {
            errorDialog(locale.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
        }
    });
    connect(spritePreview_, &SpritePreviewDialog::alwaysShowFrameChanged, this, [this](bool checked) {
        viewport->stampAlwaysShowFrame = checked;
        viewport->update();
        try {
            saveStampAlwaysShowFrame(checked);
        } catch (const std::exception &error) {
            errorDialog(locale.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
        }
    });
    connect(spritePreview_, &SpritePreviewDialog::spriteOpacityChanged, this, [this](int percent) {
        viewport->stampSpriteOpacity = percent;
        viewport->update();
    });
    // Dragging previews every value; the cfg is written once the value settles.
    connect(spritePreview_, &SpritePreviewDialog::spriteOpacityCommitted, this, [this](int percent) {
        try {
            saveStampSpriteOpacity(percent);
        } catch (const std::exception &error) {
            errorDialog(locale.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
        }
    });

    // A saved folder that no longer works only logs a warning; stamps fall back to green crosses.
    for (GameFamily family : {GameFamily::AS1, GameFamily::AS2}) {
        QString gamePath = loadGamePath(family);
        if (!gamePath.isEmpty())
            viewport->openGameFolder(family, gamePath);
        spritePreview_->setGameFolder(family, gamePath);
    }

    auto *help = new QPushButton(locale.tr(hotkeys::hotkeyTextKey("button")) + "\n" +
                                 hotkeys::hotkeyHint(hotkeys::COMMAND_HOTKEY_HELP));
    help->setObjectName("hotkeyHelpButton");
    help->setToolTip(locale.tr(hotkeys::hotkeyTextKey("tooltip")));
    help->setFixedSize(TOOLBAR_BUTTON_WIDTH, TOOLBAR_BUTTON_HEIGHT);
    toolbar->addWidget(help);
    connect(help, &QPushButton::clicked, this, &MainWindow::showHotkeys);

    languageButton_ = new QPushButton(LANGUAGE_BUTTON_TEXT);
    languageButton_->setObjectName("languageToggleButton");
    languageButton_->setToolTip(locale.tr(TextKey::TOOLTIP_LANGUAGE));
    languageButton_->setFixedSize(TOOLBAR_BUTTON_WIDTH, TOOLBAR_BUTTON_HEIGHT);
    auto *menu = new QMenu(languageButton_);
    auto *languages = new QActionGroup(menu);
    languages->setExclusive(true);
    // Language names are shown in their own language.
    const QList<QPair<QString, QString>> choices{{LOCALE_EN_US, "English"}, {LOCALE_ZH_CN, "中文"}};
    for (const auto &choice : choices) {
        QString localeName = choice.first;
        QAction *item = menu->addAction(choice.second);
        item->setCheckable(true);
        item->setChecked(locale.name() == localeName);
        languages->addAction(item);
        connect(item, &QAction::triggered, this, [this, localeName] {
            chooseLanguage(localeName);
        });
    }
    languageButton_->setMenu(menu);
    toolbar->addWidget(languageButton_);
}

void MainWindow::buildDocks() {
    // Left: family / shelf mode controls and the card shelf.
    leftShelf_ = new LeftShelfPanel(catalog);
    connect(leftShelf_, &LeftShelfPanel::familyRequested, this, [this](bool as1) {
        if (as1)
            selectVersion(PROJECT_VERSION_AS1);
        else
            selectVersion(lastAS2Version_);
    });
    connect(leftShelf_, &LeftShelfPanel::as2FormatRequested, this, &MainWindow::selectVersion);
    connect(leftShelf_, &LeftShelfPanel::shelfModeRequested, this, &MainWindow::chooseShelf);
    connect(leftShelf_, &LeftShelfPanel::setChosen, this, &MainWindow::chooseSet);
    // The legacy layout wraps the shelf in a tab widget whose tab bar is hidden.
    auto *tabs = new QTabWidget;
    tabs->setObjectName("leftShelfTabs");
    tabs->addTab(leftShelf_, locale.tr(TextKey::PANEL_WALL_SETS));
    tabs->tabBar()->hide();
    auto *leftDock = new QDockWidget(this);
    leftDock->setObjectName("themeShelfDock");
    leftDock->setTitleBarWidget(new QWidget);
    leftDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    leftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    leftDock->setWidget(tabs);
    addDockWidget(Qt::LeftDockWidgetArea, leftDock);

    // Right: inspector inside a vertical scroll area.
    inspector_ = new InspectorPanel(locale);
    connect(inspector_, &InspectorPanel::mapSizeApplied, this, &MainWindow::applyMapSize);
    connect(inspector_, &InspectorPanel::componentChosen, this, &MainWindow::chooseItem);
    connect(inspector_, &InspectorPanel::deleteDecorationRequested, this, &MainWindow::deleteSelectedDecoration);
    connect(inspector_, &InspectorPanel::spacingChanged, this, [this](double itemSpacing, double rowSpacing) {
        viewport->setSpacing(itemSpacing, rowSpacing);
        showStatus(locale.tr(TextKey::STATUS_DECORATION_SPACING,
                             {{"item_spacing_scale", QString::number(itemSpacing, 'f', 2)},
                              {"row_spacing_scale", QString::number(rowSpacing, 'f', 2)}}));
    });
    connect(inspector_->ceiling(), &AS1CeilingPropertiesWidget::layersChanged, this,
            [this](int standardLayers, int labLayers) {
                project.standardLayers = standardLayers;
                project.labLayers = labLayers;
                markChanged();
            });
    connect(inspector_->eraser(), &EraserPropertiesWidget::sizeChanged, this, [this](int size) {
        viewport->eraserSize = size;
        viewport->update();
        qInfo() << "Eraser size changed:" << size;
        showStatus(locale.tr(TextKey::STATUS_ERASER_SIZE, {{"size", QString::number(size)}}));
    });
    QScrollArea *scroll = scrolled(inspector_, "inspectorScrollArea");
    scroll->viewport()->setObjectName("inspectorScrollViewport");
    auto *rightDock = new QDockWidget(this);
    rightDock->setObjectName("inspectorDock");
    rightDock->setMinimumWidth(310);
    rightDock->setTitleBarWidget(new QWidget);
    rightDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    rightDock->setWidget(scroll);
    addDockWidget(Qt::RightDockWidgetArea, rightDock);
}

void MainWindow::connectViewport() {
    connect(viewport, &Viewport::projectChanged, this, &MainWindow::markChanged);
    connect(viewport, &Viewport::selectionChanged, this, &MainWindow::syncSelection);
    connect(viewport, &Viewport::modeChanged, this, &MainWindow::syncMode);
    connect(viewport, &Viewport::cursorGridChanged, this, [this](QPoint grid) {
        showStatus(locale.tr(TextKey::STATUS_HOVER_GRID_POINT,
                             {{"grid_x", QString::number(grid.x())}, {"grid_y", QString::number(grid.y())}}));
    });
    connect(viewport, &Viewport::viewChanged, this, [this](double zoom) {
        int zoomPercent = static_cast<int>(zoom * 100);
        showStatus(locale.tr(TextKey::STATUS_CANVAS_ZOOM, {{"zoom_percent", QString::number(zoomPercent)}}));
    });
    connect(viewport, &Viewport::segmentStarted, this, [this](QPoint grid) {
        showStatus(locale.tr(TextKey::STATUS_WALL_START,
                             {{"grid_x", QString::number(grid.x())}, {"grid_y", QString::number(grid.y())}}));
    });
    connect(viewport, &Viewport::segmentCreated, this, [this](int x1, int y1, int x2, int y2, int count) {
        showStatus(locale.tr(TextKey::STATUS_WALL_SEGMENT,
                             {{"count", QString::number(count)},
                              {"x1", QString::number(x1)},
                              {"y1", QString::number(y1)},
                              {"x2", QString::number(x2)},
                              {"y2", QString::number(y2)}}));
    });
    connect(viewport, &Viewport::doorCreated, this, [this](QPoint grid, int count) {
        showStatus(locale.tr(TextKey::STATUS_DOOR, {{"count", QString::number(count)},
                                                    {"grid_x", QString::number(grid.x())},
                                                    {"grid_y", QString::number(grid.y())}}));
    });
    connect(viewport, &Viewport::decorationCreated, this, [this](int count) {
        showStatus(locale.tr(TextKey::STATUS_DECORATION_CREATED, {{"count", QString::number(count)}}));
    });
    connect(viewport, &Viewport::stampPlaced, this, [this](const QString &stampId, bool outside) {
        if (!outside) {
            showStatus(locale.tr(TextKey::STATUS_STAMP_PLACED, {{"stamp_name", catalog.stampLabel(stampId)}}));
            return;
        }
        // Suggest a map size that would hold the whole stamp frame.
        QSizeF frame = catalog.stampFrameSize(project.isAS1(), stampId);
        QString suggestedX = QString::number(frame.width() + STAMP_MAP_SIZE_MARGIN, 'f', 0);
        QString suggestedY = QString::number(frame.height() + STAMP_MAP_SIZE_MARGIN, 'f', 0);
        showStatus(locale.tr(TextKey::STATUS_MAP_TOO_SMALL_FOR_STAMP,
                             {{"suggested_x", suggestedX}, {"suggested_y", suggestedY}}),
                   STATUS_LONG_MS);
    });
    connect(viewport, &Viewport::decorationSelected, this, [this] {
        showStatus(locale.tr(TextKey::STATUS_DECORATION_SELECTED));
    });
    connect(viewport, &Viewport::decorationDeselected, this, [this] {
        showStatus(locale.tr(TextKey::STATUS_DECORATION_DESELECTED));
    });
    connect(viewport, &Viewport::placingEnded, this, [this] {
        showStatus(locale.tr(TextKey::STATUS_DECORATION_PLACING_ENDED));
    });
    connect(viewport, &Viewport::drawingCancelled, this, [this] {
        showStatus(locale.tr(TextKey::STATUS_WALL_DRAWING_CANCELLED));
    });
}

void MainWindow::configureHotkeys() {
    using namespace hotkeys;
    hotkeys_.registerCallback(COMMAND_NEW, [this] {
        newProject();
    });
    hotkeys_.registerCallback(COMMAND_IMPORT_JSON, [this] {
        importDialog();
    });
    hotkeys_.registerCallback(COMMAND_EXPORT_JSON, [this] {
        saveDialog();
    });
    hotkeys_.registerCallback(COMMAND_GENERATE_MAP, [this] {
        exportDialog();
    });
    hotkeys_.bindButton(COMMAND_FLOOR, floor_, [this] {
        floor_->click();
    });
    hotkeys_.bindButton(COMMAND_CEILING, ceiling_, [this] {
        ceiling_->click();
    });
    hotkeys_.bindButton(COMMAND_DOOR_STATE, doors_, [this] {
        doors_->click();
    });
    hotkeys_.bindButton(COMMAND_RANDOM_DIRECTION, random_, [this] {
        random_->click();
    });
    hotkeys_.registerCallback(COMMAND_HOTKEY_HELP, [this] {
        showHotkeys();
    });
    hotkeys_.bindButton(COMMAND_LANGUAGE, languageButton_, [this] {
        languageButton_->showMenu();
    });
    hotkeys_.registerCallback(COMMAND_PREVIOUS_SET, [this] {
        leftShelf_->selectRelativeSet(-1);
    });
    hotkeys_.registerCallback(COMMAND_NEXT_SET, [this] {
        leftShelf_->selectRelativeSet(1);
    });
    hotkeys_.registerCallback(COMMAND_PREVIOUS_ITEM, [this] {
        inspector_->selectRelativeComponent(-1);
    });
    hotkeys_.registerCallback(COMMAND_NEXT_ITEM, [this] {
        inspector_->selectRelativeComponent(1);
    });
    hotkeys_.registerCallback(COMMAND_SELECT, [this] {
        activateMode(DrawingMode::Select);
    });
    hotkeys_.registerCallback(COMMAND_POLYLINE, [this] {
        activateMode(DrawingMode::Polyline);
    });
    hotkeys_.registerCallback(COMMAND_STRAIGHT_LINE, [this] {
        activateMode(DrawingMode::Straight);
    });
    hotkeys_.registerCallback(COMMAND_RECTANGLE, [this] {
        activateMode(DrawingMode::Rectangle);
    });
    hotkeys_.registerCallback(COMMAND_ERASER, [this] {
        activateMode(DrawingMode::Eraser);
    });
    hotkeys_.registerCallback(COMMAND_DELETE_DECORATION, [this] {
        deleteSelectedDecoration();
    });
}

// ---------------------------------------------------------------------------------------------
// Shelf, items and tools
// ---------------------------------------------------------------------------------------------

void MainWindow::chooseShelf(bool decorations) {
    decorationShelf_ = decorations;
    viewport->resetInteraction();
    // Leave a tool the new shelf hides: Eraser lives on the wall shelf, Select on the decoration shelf.
    if (decorations && viewport->mode == DrawingMode::Eraser)
        viewport->mode = DrawingMode::Select;
    if (!decorations && viewport->mode == DrawingMode::Select)
        viewport->mode = DrawingMode::Polyline;
    qInfo() << "Shelf mode changed, decorations:" << decorations;
    rebuildShelf();
}

void MainWindow::rebuildShelf() {
    int index = 0;
    visibleWallSets_.clear();
    visibleSeries_.clear();
    if (decorationShelf_) {
        // AS1 and AS2 projects each show their own family's library maps.
        visibleSeries_ = catalog.stampSeries(project.isAS1());
        leftShelf_->showStampSeries(project.isAS1(), visibleSeries_);
        refreshStampSeriesPreviews();
        index = std::clamp(selectedStampSeries_, 0, std::max(0, int(visibleSeries_.size()) - 1));
    } else {
        visibleWallSets_ = catalog.wallSetOrder(project.isAS1());
        leftShelf_->showWallSets(visibleWallSets_);
        // Reselect the card owning the active wall type.
        int ownerSet = catalog.wallSetOf(viewport->wallType);
        for (int row = 0; row < visibleWallSets_.size(); ++row) {
            if (visibleWallSets_[row] == ownerSet)
                index = row;
        }
    }
    leftShelf_->setDecorationShelf(decorationShelf_);
    {
        QSignalBlocker blocker(leftShelf_->shelf());
        leftShelf_->shelf()->setCurrentRow(index, false);
    }
    chooseSet(index);
    syncMode();
}

void MainWindow::refreshStampSeriesPreviews() {
    if (!decorationShelf_)
        return;
    bool spritesReady = viewport->hasGameFolder();
    for (int row = 0; row < visibleSeries_.size(); ++row) {
        QImage preview;
        if (spritesReady)
            preview = viewport->renderStampPreview(catalog.seriesVariants(project.isAS1(), visibleSeries_[row]).first());
        leftShelf_->shelf()->setCardPreview(row, spritesReady, preview);
    }
}

void MainWindow::chooseSet(int index) {
    QString title;
    QStringList labels;
    if (decorationShelf_) {
        if (index < 0 || index >= visibleSeries_.size())
            return;
        selectedStampSeries_ = index;
        QString seriesId = visibleSeries_[index];
        currentSetKey_ = "stamp:" + seriesId;
        title = catalog.seriesLabel(seriesId);
        visibleStamps_ = catalog.seriesVariants(project.isAS1(), seriesId);
        for (const QString &stampId : visibleStamps_)
            labels.append(catalog.stampLabel(stampId));
    } else {
        if (index < 0 || index >= visibleWallSets_.size())
            return;
        int setId = visibleWallSets_[index];
        viewport->setMode(DrawingMode::Polyline);
        currentSetKey_ = "wall:" + QString::number(setId);
        title = catalog.wallShortLabel(setId);
        visibleParts_ = catalog.wallSet(setId).parts;
        for (const auto &part : visibleParts_)
            labels.append(catalog.partLabel(part));
    }
    int maximum = std::max(0, int(labels.size()) - 1);
    int selected = std::clamp(itemMemory_.value(currentSetKey_, 0), 0, maximum);
    inspector_->setComponentSet(title, labels, selected);
    chooseItem(selected);
    qInfo() << "Editor Set selected:" << currentSetKey_;
    if (decorationShelf_)
        showStatus(locale.tr(TextKey::STATUS_DECORATION_TOOL_SELECTED, {{"decoration_name", title}}));
    else
        showStatus(locale.tr(TextKey::STATUS_WALL_SET_SELECTED, {{"wall_name", title}}));
}

void MainWindow::showStampDetails(const QString &stampId) {
    int memberCount = static_cast<int>(catalog.stamp(project.isAS1(), stampId)->members.size());
    inspector_->showStamp(memberCount, viewport->hasGameFolder(), viewport->renderStampPreview(stampId));
}

void MainWindow::chooseItem(int index) {
    if (decorationShelf_) {
        if (index < 0 || index >= visibleStamps_.size())
            return;
        itemMemory_[currentSetKey_] = index;
        QString stampId = visibleStamps_[index];
        viewport->armStamp(stampId);
        showStampDetails(stampId);
        qInfo() << "Stamp tool selected:" << stampId;
        showStatus(locale.tr(TextKey::STATUS_DECORATION_TOOL_SELECTED, {{"decoration_name", catalog.stampLabel(stampId)}}));
        return;
    }
    if (index < 0 || index >= visibleParts_.size())
        return;
    itemMemory_[currentSetKey_] = index;
    const WallPart &part = visibleParts_[index];
    viewport->setWall(part.wallType, part.id);
    QString family = FAMILY_AS2;
    if (project.isAS1())
        family = FAMILY_AS1;
    rememberedWallTypes_[family] = part.wallType;
    inspector_->showWallPart(catalog.imagePath(part.preview), part.nvidText);
    qInfo() << "Drawable part selected:" << part.id << "wall type" << part.wallType;
    showStatus(locale.tr(TextKey::STATUS_DRAWABLE_PART_SELECTED, {{"part_id", part.id}}));
}

void MainWindow::activateMode(DrawingMode mode) {
    // Select only acts on decorations and Eraser only on walls; each is hidden on the other shelf.
    if (mode == DrawingMode::Select && !decorationShelf_)
        return;
    if (mode == DrawingMode::Eraser && decorationShelf_)
        return;
    viewport->setMode(mode);
    qInfo() << "Drawing mode changed:" << static_cast<int>(mode);
    showStatus(locale.tr(TextKey::STATUS_DRAWING_TOOL_SELECTED, {{"mode_label", drawingToolbar_->modeLabel(mode)}}));
}

void MainWindow::syncControls() {
    if (!project.isAS1())
        lastAS2Version_ = project.version;
    leftShelf_->setProjectVersion(project.version);
    inspector_->setMapSize(project.width, project.height);

    // AS2 Series ignore the Global Door State Override: gray it out but keep it clickable.
    {
        QSignalBlocker blocker(doors_);
        doors_->setChecked(project.isAS1() && project.doorsOpen);
    }
    setUnavailable(doors_, !supportsGlobalDoorState(project.version));

    inspector_->ceiling()->setLayerCounts(project.standardLayers, project.labLayers);
    inspector_->ceiling()->setProjectState(project.isAS1(), ceiling_->isChecked());
    syncSelection();
    syncMode();
}

void MainWindow::syncSelection() {
    int selected = viewport->selectedDecoration;
    bool valid = selected >= 0 && selected < static_cast<int>(project.decorations.size()) && viewport->isEditing();
    if (valid)
        inspector_->setDecorationSelection(&project.decorations[selected]);
    else
        inspector_->setDecorationSelection(nullptr);
}

void MainWindow::syncMode() {
    // Placing owns the left button, so no drawing tool stays checked.
    drawingToolbar_->setCheckedMode(viewport->mode, !viewport->isPlacing());
    drawingToolbar_->setModeVisible(DrawingMode::Select, decorationShelf_);
    drawingToolbar_->setModeVisible(DrawingMode::Eraser, !decorationShelf_);
    inspector_->eraser()->setVisible(viewport->mode == DrawingMode::Eraser);
    syncSelection();
}

void MainWindow::markChanged() {
    dirty_ = true;
    setWindowModified(true);
}

void MainWindow::showStatus(const QString &message, int timeoutMs) {
    statusBar()->showMessage(message, timeoutMs);
}

// ---------------------------------------------------------------------------------------------
// Project commands
// ---------------------------------------------------------------------------------------------

void MainWindow::replaceProject(const Project &value) {
    project = value;
    viewport->resetInteraction();
    // Restore the last wall type used in this game family (AS1 Standard / AS2 Set 1 by default).
    if (project.isAS1())
        viewport->wallType = rememberedWallTypes_.value(FAMILY_AS1, catalog.wallSetOrder(true).first());
    else
        viewport->wallType = rememberedWallTypes_.value(FAMILY_AS2, catalog.wallSetOrder(false).first());
    decorationShelf_ = false;
    // The game family may have changed, and each family has its own sprite preview folder.
    inspector_->setSpritePreviewReady(viewport->hasGameFolder());
    // Generation options are per session and start off for every loaded project.
    floor_->setChecked(false);
    ceiling_->setChecked(false);
    syncControls();
    rebuildShelf();
    viewport->fitMap();
    dirty_ = false;
    setWindowModified(false);
}

bool MainWindow::confirmDiscard() {
    if (!dirty_)
        return true;
    auto answer = QMessageBox::question(this, locale.tr(TextKey::DIALOG_UNSAVED_PROJECT),
                                        locale.tr(TextKey::MESSAGE_SAVE_BEFORE_CONTINUE),
                                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                        QMessageBox::Save);
    if (answer == QMessageBox::Cancel)
        return false;
    if (answer == QMessageBox::Save)
        return saveDialog();
    return true;
}

void MainWindow::newProject() {
    if (!confirmDiscard())
        return;
    replaceProject(Project::createEmpty(project.version));
    projectPath_.clear();
    qInfo() << "New project created:" << project.version;
    showStatus(locale.tr(TextKey::STATUS_CANVAS_CLEARED));
}

void MainWindow::selectVersion(const QString &version) {
    if (version == project.version)
        return;
    bool familyChanged = (version == PROJECT_VERSION_AS1) != project.isAS1();
    if (!familyChanged) {
        // AS2 / AS2R / AS2OE share geometry: only the output format changes.
        project.version = version;
        lastAS2Version_ = version;
        markChanged();
        qInfo() << "Project version changed:" << version;
        return;
    }
    // Switching game family starts a new project in that family.
    if (!confirmDiscard()) {
        syncControls();
        return;
    }
    replaceProject(Project::createEmpty(version));
    projectPath_.clear();
    qInfo() << "Project family changed:" << version;
}

void MainWindow::chooseGameFolder(GameFamily family) {
    QString current = loadGamePath(family);
    QString path = QFileDialog::getExistingDirectory(this, locale.tr(TextKey::DIALOG_GAME_FOLDER), current);
    if (path.isEmpty())
        return;
    bool opened = viewport->openGameFolder(family, path);
    if (!opened) {
        errorDialog(locale.tr(TextKey::DIALOG_GAME_FOLDER),
                    locale.tr(TextKey::MESSAGE_GAME_FOLDER_INVALID, {{"path", QDir::toNativeSeparators(path)}}));
        // Keep showing the folder of this family that worked before.
        if (!current.isEmpty())
            viewport->openGameFolder(family, current);
        return;
    }
    try {
        saveGamePath(family, path);
    } catch (const std::exception &error) {
        errorDialog(locale.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
    }
    spritePreview_->setGameFolder(family, path);
    inspector_->setSpritePreviewReady(viewport->hasGameFolder());
    refreshStampSeriesPreviews();
    // Redraw the inspector preview of the stamp on show with the new sprites.
    int shownStamp = inspector_->componentIndex();
    if (decorationShelf_ && shownStamp >= 0 && shownStamp < visibleStamps_.size())
        showStampDetails(visibleStamps_[shownStamp]);
    showStatus(locale.tr(TextKey::STATUS_GAME_FOLDER_OPENED, {{"path", QDir::toNativeSeparators(path)}}),
               STATUS_SHORT_MS);
}

void MainWindow::showSpritePreview() {
    if (!spritePreview_->isVisible()) {
        // Open under the toolbar button with right edges aligned, like a drop-down.
        spritePreview_->adjustSize();
        QPoint corner = spritePreviewButton_->mapToGlobal(QPoint(spritePreviewButton_->width(), spritePreviewButton_->height()));
        spritePreview_->move(corner.x() - spritePreview_->width(), corner.y() + 6);
    }
    spritePreview_->show();
    spritePreview_->raise();
    spritePreview_->activateWindow();
}

void MainWindow::errorDialog(const QString &title, const QString &message) {
    qWarning() << message;
    QMessageBox::critical(this, title, message);
}

bool MainWindow::openProject(const QString &path) {
    try {
        Project loaded = Project::load(path);
        // Stamps come from the decoration library maps; ids it does not know are not supported.
        for (const auto &decoration : loaded.decorations) {
            if (decoration.type == DECORATION_TYPE_ROOM_STAMP && catalog.stamp(loaded.isAS1(), decoration.profileId) == nullptr)
                throw std::runtime_error(("Unknown decoration stamp: " + decoration.profileId).toStdString());
        }
        replaceProject(loaded);
        projectPath_ = path;
        showStatus(locale.tr(TextKey::STATUS_IMPORTED_PROJECT,
                             {{"segment_count", QString::number(project.segments.size())},
                              {"door_count", QString::number(project.doors.size())},
                              {"file_path", path}}),
                   STATUS_SHORT_MS);
        return true;
    } catch (const std::exception &error) {
        QString message = locale.tr(TextKey::ERROR_INVALID_PROJECT_DATA, {{"error", QString::fromUtf8(error.what())}});
        errorDialog(locale.tr(TextKey::DIALOG_IMPORT_JSON), message);
        return false;
    }
}

bool MainWindow::saveProject(const QString &path) {
    try {
        project.save(path);
        projectPath_ = path;
        dirty_ = false;
        setWindowModified(false);
        showStatus(locale.tr(TextKey::STATUS_EXPORTED_PROJECT,
                             {{"segment_count", QString::number(project.segments.size())},
                              {"door_count", QString::number(project.doors.size())},
                              {"file_path", path}}),
                   STATUS_SHORT_MS);
        return true;
    } catch (const std::exception &error) {
        QString message = locale.tr(TextKey::ERROR_EXPORT_JSON_FAILED, {{"error", QString::fromUtf8(error.what())}});
        errorDialog(locale.tr(TextKey::DIALOG_EXPORT_JSON), message);
        return false;
    }
}

void MainWindow::importDialog() {
    if (!confirmDiscard())
        return;
    QString path = QFileDialog::getOpenFileName(this, locale.tr(TextKey::DIALOG_IMPORT_JSON), projectPath_,
                                                locale.tr(TextKey::FILE_FILTER_JSON));
    if (!path.isEmpty())
        openProject(path);
}

bool MainWindow::saveDialog() {
    QString initialPath = projectPath_;
    if (initialPath.isEmpty())
        initialPath = DEFAULT_PROJECT_FILE;
    QString path = QFileDialog::getSaveFileName(this, locale.tr(TextKey::DIALOG_EXPORT_JSON), initialPath,
                                                locale.tr(TextKey::FILE_FILTER_JSON));
    if (path.isEmpty())
        return false;
    if (QFileInfo(path).suffix().isEmpty())
        path += ".json";
    return saveProject(path);
}

bool MainWindow::exportMap(const QString &path) {
    if (exportWatcher_.isRunning())
        return false;
    if (project.empty()) {
        QMessageBox::warning(this, locale.tr(TextKey::DIALOG_GENERATE_MAP), locale.tr(TextKey::ERROR_GENERATE_EMPTY));
        return false;
    }
    outputPath_ = path;
    Project snapshot = project;
    bool floor = floor_->isChecked();
    bool ceiling = ceiling_->isChecked();
    bool random = random_->isChecked();
    // Generation runs off the UI thread; the window stays disabled until it finishes.
    setEnabled(false);
    setCursor(Qt::WaitCursor);
    showStatus(locale.tr(TextKey::STATUS_GENERATING_MAP));
    qInfo() << "Generating map:" << path << "floor" << floor << "ceiling" << ceiling << "random" << random;
    const auto *library = &catalog.decorationLibrary(snapshot.isAS1());
    exportWatcher_.setFuture(QtConcurrent::run([snapshot, path, floor, ceiling, random, library] {
        ExportResult result;
        try {
            result.success = snapshot.generate(path, floor, ceiling, random, library);
        } catch (const std::exception &error) {
            result.engineError = QString::fromUtf8(error.what());
        }
        return result;
    }));
    return true;
}

void MainWindow::exportDialog() {
    QString initialPath = outputPath_;
    if (initialPath.isEmpty())
        initialPath = DEFAULT_MAP_FILE;
    QString path = QFileDialog::getSaveFileName(this, locale.tr(TextKey::DIALOG_GENERATE_MAP), initialPath,
                                                locale.tr(TextKey::FILE_FILTER_MAP));
    if (path.isEmpty())
        return;
    if (QFileInfo(path).suffix().isEmpty())
        path += ".map";
    exportMap(path);
}

void MainWindow::chooseLanguage(const QString &localeName) {
    try {
        saveLanguage(localeName);
    } catch (const std::exception &error) {
        errorDialog(locale.tr(TextKey::DIALOG_ERROR), QString::fromUtf8(error.what()));
        return;
    }
    QString name = locale.tr(TextKey::LANGUAGE_NAME_ENGLISH);
    if (localeName == LOCALE_ZH_CN)
        name = locale.tr(TextKey::LANGUAGE_NAME_CHINESE);
    showStatus(locale.tr(TextKey::STATUS_LANGUAGE_SAVED, {{"language_name", name}}));
    // The UI is built once per start, so a new language applies after a restart.
    QMessageBox::information(this, locale.tr(TextKey::DIALOG_RESTART_REQUIRED),
                             locale.tr(TextKey::MESSAGE_LANGUAGE_CHANGED, {{"language_name", name}}));
}

void MainWindow::toggleDoorsOpen(bool checked) {
    if (!supportsGlobalDoorState(project.version)) {
        QSignalBlocker blocker(doors_);
        doors_->setChecked(false);
        QMessageBox::warning(this, locale.tr(TextKey::DIALOG_DOOR_OPTION),
                             locale.tr(TextKey::ERROR_AS2_DOOR_OPTION_UNAVAILABLE));
        showStatus(locale.tr(TextKey::STATUS_AS2_DOOR_OPTION_UNAVAILABLE));
        qInfo() << "Global door option unavailable:" << project.version;
        return;
    }
    project.doorsOpen = checked;
    markChanged();
    viewport->update();
    qInfo() << "Global is_door_open changed:" << checked;
}

void MainWindow::applyMapSize(double width, double height) {
    project.width = width;
    project.height = height;
    viewport->fitMap();
    markChanged();
    qInfo() << "Map size applied:" << width << "x" << height;
    showStatus(locale.tr(TextKey::STATUS_MAP_SIZE_APPLIED, {{"map_size_x", QString::number(width, 'f', 1)},
                                                            {"map_size_y", QString::number(height, 'f', 1)}}));
}

void MainWindow::deleteSelectedDecoration() {
    if (viewport->deleteSelection())
        showStatus(locale.tr(TextKey::STATUS_DECORATION_DELETED));
}

void MainWindow::showHotkeys() {
    hotkeys::HotkeyHelpDialog dialog(locale, this);
    dialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (exportWatcher_.isRunning() || !confirmDiscard())
        event->ignore();
    else
        event->accept();
}

bool MainWindow::eventFilter(QObject *, QEvent *event) {
    if (event->type() != QEvent::KeyPress)
        return false;
    auto *key = static_cast<QKeyEvent *>(event);
    if (!hotkeys_.acceptsShortcut(key))
        return false;
    // Esc is not a listed hotkey: it always cancels the pending canvas action.
    if (key->key() == Qt::Key_Escape) {
        viewport->cancel();
        return true;
    }
    return hotkeys_.dispatch(key);
}

} // namespace editor
