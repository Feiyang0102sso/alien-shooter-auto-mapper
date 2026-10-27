// Main editor window: owns the project, wires panels, canvas and hotkeys to editor commands.
#pragma once
#include "editor/catalog/catalog.h"
#include "editor/config.h"
#include "editor/hotkeys/hotkey_manager.h"
#include "editor/i18n/locale.h"
#include "editor/project/project.h"
#include "editor/ui/canvas/viewport.h"
#include <QFutureWatcher>
#include <QHash>
#include <QMainWindow>

class QCheckBox;
class QPushButton;

namespace editor {

class DrawingToolbar;
class InspectorPanel;
class LeftShelfPanel;
class SpritePreviewDialog;

// Result of one background map generation, translated on the UI thread.
struct ExportResult {
    bool success = false;
    // Exception text from the generator; empty when it simply returned false.
    QString engineError;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    explicit MainWindow(const QString &localeName = LOCALE_EN_US, QWidget *parent = nullptr);
    ~MainWindow() override;

    // Declaration order matters: Catalog reads its labels from Locale.
    Locale locale;
    Catalog catalog;
    Project project;
    Viewport *viewport = nullptr;

    void replaceProject(const Project &value);
    bool openProject(const QString &path);
    bool saveProject(const QString &path);
    bool exportMap(const QString &path);
    void chooseShelf(bool decorations);
    void chooseSet(int index);
    void chooseItem(int index);
    // Inspector member count, sprite preview image and game folder notice of one stamp.
    void showStampDetails(const QString &stampId);
    void activateMode(DrawingMode mode);
    void showHotkeys();
    bool isDirty() const;

  protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

  private:
    // Construction.
    void buildCommandToolbar();
    void buildDocks();
    void connectViewport();
    void configureHotkeys();

    // Keep panels in step with project and canvas state.
    void rebuildShelf();
    // Render each Decoration Stamp Series card's preview from the series' first stamp.
    void refreshStampSeriesPreviews();
    void syncControls();
    void syncSelection();
    void syncMode();
    void markChanged();
    void showStatus(const QString &message, int timeoutMs = 0);

    // Commands.
    bool confirmDiscard();
    void newProject();
    void selectVersion(const QString &version);
    void importDialog();
    bool saveDialog();
    void exportDialog();
    void chooseLanguage(const QString &localeName);
    // Pick the AS2 game folder for the decoration sprite preview and remember it in AutoMapper.cfg.
    void chooseGameFolder(GameFamily family);
    // Open the sprite preview window under its toolbar button.
    void showSpritePreview();
    void toggleDoorsOpen(bool checked);
    void applyMapSize(double width, double height);
    void deleteSelectedDecoration();
    void errorDialog(const QString &title, const QString &message);

    hotkeys::HotkeyManager hotkeys_;
    DrawingToolbar *drawingToolbar_ = nullptr;
    LeftShelfPanel *leftShelf_ = nullptr;
    InspectorPanel *inspector_ = nullptr;
    QCheckBox *floor_ = nullptr;
    QCheckBox *ceiling_ = nullptr;
    QCheckBox *doors_ = nullptr;
    QCheckBox *random_ = nullptr;
    QPushButton *languageButton_ = nullptr;
    QPushButton *spritePreviewButton_ = nullptr;
    SpritePreviewDialog *spritePreview_ = nullptr;
    QFutureWatcher<ExportResult> exportWatcher_;

    bool dirty_ = false;
    bool decorationShelf_ = false;
    QString projectPath_;
    QString outputPath_;
    // Cards currently on the shelf: wall set ids, or stamp series ids.
    QList<int> visibleWallSets_;
    QStringList visibleSeries_;
    // Items of the current card: wall parts, or stamp ids.
    QList<WallPart> visibleParts_;
    QStringList visibleStamps_;
    // Each Editor Set remembers its selected Editor Set Item for this session.
    QString currentSetKey_;
    QHash<QString, int> itemMemory_;
    int selectedStampSeries_ = 0;
    // Last wall type per game family ("AS1" / "AS2"), restored after a project switch.
    QHash<QString, int> rememberedWallTypes_;
    QString lastAS2Version_ = PROJECT_VERSION_AS2R;
};

} // namespace editor
