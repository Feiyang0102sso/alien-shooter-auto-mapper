// Native interaction and project compatibility regression tests.
#include "editor/config.h"
#include "editor/loot_presets.h"
#include "editor/hotkeys/hotkey_config.h"
#include "editor/i18n/text_keys.h"
#include "editor/ui/main_window.h"
#include "editor/ui/panels/inspector.h"
#include "editor/ui/panels/loot_table_dialog.h"
#include "editor/ui/panels/sprite_preview_dialog.h"
#include "editor/ui/panels/theme_shelf.h"
#include "editor/ui/tools/eraser/eraser_geometry.h"
#include <QJsonArray>
#include <QScopeGuard>
#include <QtTest>
#include <QtWidgets>
#include <QTemporaryDir>
#include <cmath>
#include <numbers>

using namespace editor;

class EditorTest : public QObject {
    Q_OBJECT
  private slots:
    void referenceLayoutAndEraserControl() {
        initEditorResources();
        QFile style(":/styles/dark_theme.qss");
        QVERIFY(style.open(QIODevice::ReadOnly));
        qApp->setStyle("windows11");
        qApp->setFont(QFont("Microsoft YaHei UI", 9));
        qApp->setStyleSheet(QString::fromUtf8(style.readAll()));
        MainWindow window("zh_CN");
        window.show();
        QApplication::processEvents();
        auto *toolbar = window.findChild<QToolBar *>("drawingToolbar");
        QVERIFY(toolbar);
        int separators = 0;
        for (auto *action : toolbar->actions()) {
            if (action->isSeparator())
                ++separators;
        }
        QCOMPARE(separators, 2);
        QCOMPARE(window.findChild<QAction *>("drawingMode4")->text(), QString("橡皮擦 [4]"));
        QVERIFY(!window.findChild<QComboBox *>("as2FormatCombo")->isVisible());
        auto *ceiling = window.findChild<QGroupBox *>("as1CeilingPropertiesGroup");
        QVERIFY(ceiling->isVisible());
        QVERIFY(!ceiling->isEnabled());
        QString capture = qEnvironmentVariable("AUTO_MAPPER_CAPTURE_DIR");
        if (!capture.isEmpty()) {
            QDir().mkpath(capture);
            QTest::qWait(250);
            QVERIFY(window.grab().save(capture + "/native-default.png"));
        }
        window.activateMode(DrawingMode::Eraser);
        auto *slider = window.findChild<QSlider *>("eraserSizeSlider");
        auto *spin = window.findChild<QSpinBox *>("eraserSize");
        QVERIFY(slider->isVisible());
        QCOMPARE(slider->maximum(), 10);
        slider->setValue(7);
        QCOMPARE(spin->value(), 7);
        QCOMPARE(window.viewport->eraserSize, 7);
        spin->setValue(3);
        QCOMPARE(slider->value(), 3);
        slider->setValue(1);
        QApplication::processEvents();
        if (!capture.isEmpty()) {
            QTest::qWait(250);
            QVERIFY(window.grab().save(capture + "/native-eraser.png"));
            QJsonArray geometry;
            for (auto *widget : window.findChildren<QWidget *>()) {
                if (!widget->isVisible())
                    continue;
                const auto rect = widget->geometry();
                geometry.append(
                    QJsonObject{{"name", widget->objectName()},
                                {"class", widget->metaObject()->className()},
                                {"geometry", QJsonArray{rect.x(), rect.y(), rect.width(), rect.height()}},
                                {"font", widget->font().toString()}});
            }
            QFile geometryFile(capture + "/native-layout.json");
            QVERIFY(geometryFile.open(QIODevice::WriteOnly));
            geometryFile.write(QJsonDocument(geometry).toJson());
            auto *scroll = window.findChild<QScrollArea *>("inspectorScrollArea");
            scroll->ensureWidgetVisible(slider);
            QApplication::processEvents();
            QVERIFY(window.grab().save(capture + "/native-eraser-slider.png"));
        }
        Project project;
        project.version = "AS2R";
        project.width = 3000;
        project.height = 3000;
        // One placed stamp so the capture shows its frame and member placeholders.
        Decoration placed;
        placed.profileId = "01dorm:3";
        placed.origin = {1500, 1500};
        project.decorations.push_back(placed);
        window.replaceProject(project);
        window.chooseShelf(ShelfMode::Decorations);
        QCOMPARE(window.findChild<QComboBox *>("as2FormatCombo")->currentText(), QString("AS2R"));
        QVERIFY(window.viewport->isPlacing());
        QVERIFY(!window.findChild<QAction *>("drawingMode0")->isChecked());
        QApplication::processEvents();
        if (!capture.isEmpty()) {
            QTest::qWait(250);
            QVERIFY(window.grab().save(capture + "/native-stamps.png"));
            QJsonArray stampGeometry;
            for (auto *widget : window.findChildren<QWidget *>()) {
                if (!widget->isVisible())
                    continue;
                auto point = widget->mapTo(&window, QPoint{});
                stampGeometry.append(QJsonObject{{"name", widget->objectName()},
                    {"class", widget->metaObject()->className()},
                    {"geometry", QJsonArray{point.x(), point.y(), widget->width(), widget->height()}}});
            }
            QFile stampGeometryFile(capture + "/native-stamps-layout.json");
            QVERIFY(stampGeometryFile.open(QIODevice::WriteOnly));
            stampGeometryFile.write(QJsonDocument(stampGeometry).toJson());
            // 1:1 close-up of the placed stamp: sprites when AutoMapper.cfg names a game folder, else crosses.
            window.viewport->zoom = 1.0;
            window.viewport->pan = QPointF(window.viewport->rect().center()) - placed.origin;
            window.viewport->update();
            QTest::qWait(100);
            QVERIFY(window.viewport->grab().save(capture + "/native-stamps-zoom.png"));
            window.findChild<QPushButton *>("spritePreviewButton")->click();
            auto *spritePreview = window.findChild<QDialog *>("spritePreviewDialog");
            QTest::qWait(100);
            QVERIFY(spritePreview->grab().save(capture + "/native-sprite-preview.png"));
            spritePreview->close();
            QTimer::singleShot(250, [&] {
                auto *dialog = qobject_cast<QDialog *>(QApplication::activeModalWidget());
                if (!dialog)
                    return;
                dialog->grab().save(capture + "/native-hotkeys.png");
                dialog->reject();
            });
            window.showHotkeys();
        }
    }
    void legacyJsonRoundTrip() {
        Project legacy = Project::fromJson({});
        QCOMPARE(legacy.version, QString("AS1"));
        for (const auto &version : {"AS1", "AS2", "AS2R", "AS2OE"}) {
            Project project;
            project.version = version;
            project.doorsOpen = true;
            project.segments.push_back({{0, 0}, {0, 3}, 0});
            project.doors.push_back({{0, 1}, 0, 0, 1, 0, 1, 0});
            project.standardLayers = 17;
            project.labLayers = 8;
            Decoration stamp;
            stamp.profileId = "01dorm:1";
            stamp.origin = {430, 290};
            project.decorations.push_back(stamp);
            project.lootTables[LootBoxKind::Wood] = {{"EMPTY Ammo_shotgun Obj_money3", 40.5}, {"Obj_money1", 9.5}};
            // Loot boxes exist only in the AS2 Series.
            if (!project.isAS1())
                project.lootBoxes.push_back({auto_mapper::core::LOOT_VID_WOOD_BOX, 300.5f, 410.0f, 25.0f, 28});
            auto json = project.toJson();
            auto copy = Project::fromJson(json);
            QCOMPARE(copy.toJson(), json);
            QCOMPARE(json.contains("is_door_open"), project.isAS1());
            QCOMPARE(json.contains("loot_boxes"), !project.isAS1());
        }
        QJsonObject as1Loot{{"loot_boxes", QJsonArray{QJsonObject{{"vid", 1225}, {"x", 1}, {"y", 1}}}}};
        QVERIFY_EXCEPTION_THROWN(Project::fromJson(as1Loot), std::runtime_error);
        QJsonObject unknownLootVid{{"version", "AS2R"},
                                   {"loot_boxes", QJsonArray{QJsonObject{{"vid", 601}, {"x", 1}, {"y", 1}}}}};
        QVERIFY_EXCEPTION_THROWN(Project::fromJson(unknownLootVid), std::runtime_error);
        QJsonArray overfullLines{QJsonObject{{"text", "Obj_money1"}, {"percent", 60}},
                                 QJsonObject{{"text", "Obj_money2"}, {"percent", 41}}};
        QJsonObject overfullTable{{"version", "AS2R"}, {"loot_tables", QJsonObject{{"wood", overfullLines}}}};
        QVERIFY_EXCEPTION_THROWN(Project::fromJson(overfullTable), std::runtime_error);
        QVERIFY_EXCEPTION_THROWN(Project::fromJson({{"version", "AS3"}}), std::runtime_error);
        QVERIFY_EXCEPTION_THROWN(Project::fromJson({{"as1_lab_ceiling_layer_count", 2.5}}),
                                 std::runtime_error);
        QVERIFY_EXCEPTION_THROWN(Project::fromJson({{"as1_standard_ceiling_layer_count", 51}}),
                                 std::runtime_error);
    }
    void spritePreviewDialogOptions() {
        Locale locale(LOCALE_EN_US);
        SpritePreviewDialog dialog(locale);
        auto *opacity = dialog.findChild<QSlider *>("stampSpriteOpacitySlider");
        auto *opacityValue = dialog.findChild<QLabel *>("stampSpriteOpacityValue");
        // Out-of-range opacity snaps to the limits instead of being rejected.
        dialog.setOptions(false, false, 150);
        QCOMPARE(opacity->value(), STAMP_SPRITE_OPACITY_MAX);
        dialog.setOptions(false, false, 1);
        QCOMPARE(opacity->value(), STAMP_SPRITE_OPACITY_MIN);
        QCOMPARE(opacityValue->text(), QString("10%"));
        QSignalSpy frameSpy(&dialog, &SpritePreviewDialog::alwaysShowFrameChanged);
        QSignalSpy opacitySpy(&dialog, &SpritePreviewDialog::spriteOpacityChanged);
        QSignalSpy commitSpy(&dialog, &SpritePreviewDialog::spriteOpacityCommitted);
        QSignalSpy folderSpy(&dialog, &SpritePreviewDialog::chooseGameFolderRequested);
        dialog.findChild<QCheckBox *>("stampAlwaysShowFrameCheck")->click();
        opacity->setValue(60);
        QCOMPARE(frameSpy.count(), 1);
        QCOMPARE(frameSpy.first().first().toBool(), true);
        QCOMPARE(opacitySpy.last().first().toInt(), 60);
        // Not dragging, so the value is committed right away.
        QCOMPARE(commitSpy.last().first().toInt(), 60);
        QCOMPARE(opacityValue->text(), QString("60%"));
        dialog.findChild<QPushButton *>("chooseAs1GameFolderButton")->click();
        QCOMPARE(folderSpy.count(), 1);
        QCOMPARE(folderSpy.first().first().value<GameFamily>(), GameFamily::AS1);

        // Stamp notice: red "not configured" until a game folder loads, then green "ready".
        InspectorPanel inspector(locale);
        auto *notice = inspector.findChild<QLabel *>("stampPreviewNotice");
        QCOMPARE(notice->property("ready").toBool(), false);
        QCOMPARE(notice->text(), locale.tr(TextKey::LABEL_STAMP_PREVIEW_UNSET));
        inspector.setSpritePreviewReady(true);
        QCOMPARE(notice->property("ready").toBool(), true);
        QCOMPARE(notice->text(), locale.tr(TextKey::LABEL_STAMP_PREVIEW_READY));

        // Stamp preview box: black "not configured" without a game folder, the composed image with one.
        auto *preview = inspector.findChild<QLabel *>("componentPreview");
        inspector.showStamp(23, false, QImage());
        QVERIFY(preview->isVisibleTo(&inspector));
        QCOMPARE(preview->property("notConfigured").toBool(), true);
        QCOMPARE(preview->text(), locale.tr(TextKey::LABEL_PREVIEW_NOT_CONFIGURED));
        QImage composed(40, 20, QImage::Format_ARGB32_Premultiplied);
        composed.fill(Qt::red);
        inspector.showStamp(23, true, composed);
        QCOMPARE(preview->property("notConfigured").toBool(), false);
        QVERIFY(!preview->pixmap().isNull());

        // Stamp series card: same black "not configured" box above the title, then the scaled first stamp.
        ThemeShelfPanel shelf(locale);
        ShelfCard stampCard;
        stampCard.spritePreview = true;
        stampCard.title = "01dorm";
        shelf.setCards({stampCard}, true);
        auto *cardPreview = shelf.findChild<QLabel *>("themePreview");
        QVERIFY(cardPreview != nullptr);
        shelf.setCardPreview(0, false, QImage());
        QCOMPARE(cardPreview->property("notConfigured").toBool(), true);
        QCOMPARE(cardPreview->text(), locale.tr(TextKey::LABEL_PREVIEW_NOT_CONFIGURED));
        shelf.setCardPreview(0, true, composed);
        QCOMPARE(cardPreview->property("notConfigured").toBool(), false);
        QVERIFY(!cardPreview->pixmap().isNull());

        // A wide stamp preview fills CARD_PREVIEW_WIDTH but must not widen the cards past a minimum-width shelf.
        // Several cards in a short shelf bring up the vertical scroll bar, as on the real decoration shelf.
        shelf.setCards(QList<ShelfCard>(6, stampCard), true);
        QImage wide(900, 200, QImage::Format_ARGB32_Premultiplied);
        wide.fill(Qt::red);
        for (int row = 0; row < 6; ++row)
            shelf.setCardPreview(row, true, wide);
        shelf.resize(280, 400);
        shelf.show();
        QApplication::processEvents();
        auto *scroll = shelf.findChild<QScrollArea *>("themeShelfScrollArea");
        QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
    }
    void resourcesAndCoreMetadata() {
        Locale locale(LOCALE_ZH_CN);
        Catalog catalog(locale);
        QCOMPARE(catalog.wallTypes().size(), auto_mapper::core::SUPPORTED_WALL_TYPE_COUNT);
        QVERIFY(locale.tr(TextKey::ACTION_NEW) != TextKey::ACTION_NEW);
        // Stamps are registered from assets/decoration_maps/AS1 (grid cells) and AS2 (regions) beside the exe.
        QVERIFY(catalog.stampSeries(false).contains("01dorm"));
        QVERIFY(catalog.stampSeries(true).contains("01office"));
        for (bool as1 : {true, false}) {
            for (const auto &seriesId : catalog.stampSeries(as1)) {
                auto variants = catalog.seriesVariants(as1, seriesId);
                QVERIFY2(!variants.isEmpty(), qPrintable(seriesId));
                // Every shipped library map has its own card description in both locales (key sets are compared).
                QVERIFY2(locale.contains(QString("decoration.%1.description").arg(seriesId)), qPrintable(seriesId));
                for (const auto &id : variants) {
                    const auto *stamp = catalog.stamp(as1, id);
                    QVERIFY2(stamp != nullptr, qPrintable(id));
                    QVERIFY2(!stamp->members.empty(), qPrintable(id));
                    QCOMPARE(catalog.stampFrameSize(as1, id), QSizeF(stamp->width, stamp->height));
                }
            }
        }
        QCOMPARE(catalog.seriesVariants(false, "01dorm").first(), QString("01dorm:1"));
        QCOMPARE(catalog.stampLabel("01dorm:3"), QString("01dorm 区域 3"));
        // A library map without its own description text still gets a card description.
        QCOMPARE(catalog.seriesDescription("99unlisted"), locale.tr(TextKey::LABEL_STAMP_SERIES_DESCRIPTION_DEFAULT));
        QVERIFY(catalog.stamp(false, "dorms01") == nullptr);
        // The two families keep separate libraries.
        QVERIFY(catalog.stamp(true, "01dorm:1") == nullptr);
        QVERIFY(catalog.stamp(false, "01office:1") == nullptr);
        for (bool as1 : {true, false}) {
            for (int setId : catalog.wallSetOrder(as1)) {
                WallSet wall = catalog.wallSet(setId);
                QVERIFY2(!QPixmap(catalog.imagePath(wall.preview)).isNull(), qPrintable(wall.preview));
                for (const auto &part : wall.parts) {
                    if (part.isCurveWall())
                        QVERIFY2(auto_mapper::core::is_supported_curve_wall_type(part.curveWallType), qPrintable(part.id));
                    else
                        QVERIFY2(catalog.wallTypes().contains(part.wallType), qPrintable(part.id));
                    QVERIFY2(!QPixmap(catalog.imagePath(part.preview)).isNull(), qPrintable(part.preview));
                }
            }
        }
        // AS2 Wall Set 1 variants share one shelf card.
        QCOMPARE(catalog.wallSetOf(4), 3);
        QCOMPARE(catalog.wallSetOf(5), 3);
        QCOMPARE(catalog.wallSetOf(2), 2);
    }
    void localesShareOneKeySet() {
        Locale english(LOCALE_EN_US);
        Locale chinese(LOCALE_ZH_CN);
        auto englishKeys = english.keys();
        auto chineseKeys = chinese.keys();
        englishKeys.sort();
        chineseKeys.sort();
        QCOMPARE(englishKeys, chineseKeys);
        // Every data-driven label resolves in both languages.
        for (const Locale *locale : {&english, &chinese}) {
            Catalog catalog(*locale);
            for (bool as1 : {true, false}) {
                for (int setId : catalog.wallSetOrder(as1)) {
                    QVERIFY(locale->contains(QString("wall.%1.label").arg(setId)));
                    QVERIFY(locale->contains(QString("wall.%1.short_label").arg(setId)));
                    QVERIFY(locale->contains(QString("wall.%1.description").arg(setId)));
                    for (const auto &part : catalog.wallSet(setId).parts)
                        QVERIFY2(locale->contains(part.labelKey), qPrintable(part.labelKey));
                }
            }
            for (const auto &binding : hotkeys::hotkeyBindings()) {
                QString key = hotkeys::hotkeyTextKey(binding.commandId);
                QVERIFY2(locale->contains(key), qPrintable(key));
            }
        }
        // Placeholders accept Python format specs; replacement text is inserted literally.
        QString text = english.tr(TextKey::STATUS_MAP_SIZE_APPLIED, {{"map_size_x", "600.0"}, {"map_size_y", "3000.0"}});
        QCOMPARE(text, QString("Map size applied: 600.0 x 3000.0"));
        QString windowsPath = "C:\\1maps\\a.map";
        text = english.tr(TextKey::STATUS_GENERATED_MAP, {{"output_path", windowsPath}});
        QVERIFY(text.contains(windowsPath));
        QCOMPARE(english.tr("missing.key"), QString("missing.key"));
    }
    void eraserGeometry() {
        std::vector<Segment> segments{{{0, 0}, {0, 6}, 0}, {{0, 0}, {0, 6}, 2}};
        std::vector<DoorInstance> doors{{{0, 3}, 0, 0, 1, 0, 2, 0}, {{0, 3}, 2, 0, 1, 0, 2, 0}};
        // Size 1 removes the two edges touching (0, 3) plus the door there, only for wall type 0.
        QVERIFY(eraseAround(segments, doors, {0, 3}, 1, 0));
        QCOMPARE(segments.size(), size_t(3));
        QCOMPARE(doors.size(), size_t(1));
        QCOMPARE(doors[0].wall_type, 2);
        QVERIFY(!eraseAround(segments, doors, {9, 9}, 1, 0));
    }
    void coordinatesAndWallTools() {
        Project project;
        project.width = 3000;
        project.height = 3000;
        Locale locale;
        Catalog catalog(locale);
        Viewport canvas(project, catalog);
        canvas.resize(900, 700);
        canvas.show();
        canvas.fitMap();
        for (int type : catalog.wallTypes()) {
            QPointF grid(2, 5);
            auto roundTrip = canvas.mapToGrid(canvas.gridToMap(grid, type), type);
            QVERIFY(QLineF(grid, roundTrip).length() < 0.0001);
        }
        auto click = [&](QPoint grid) {
            QTest::mouseClick(&canvas, Qt::LeftButton, {},
                              canvas.mapToScreen(canvas.gridToMap(grid, canvas.wallType)).toPoint());
        };
        canvas.setMode(DrawingMode::Rectangle);
        click({0, 0});
        click({4, 4});
        QCOMPARE(project.segments.size(), size_t(4));
        canvas.setMode(DrawingMode::Polyline);
        click({1, 1});
        click({1, 4});
        click({4, 4});
        QCOMPARE(project.segments.size(), size_t(6));
        QTest::mouseClick(&canvas, Qt::RightButton);
        click({2, 2});
        QCOMPARE(project.segments.size(), size_t(6));
        canvas.setWall(0, "active_door");
        click({1, 1});
        click({1, 2});
        QCOMPARE(project.doors.size(), size_t(1));
        QCOMPARE(project.doors[0].pos.y, 2);
        canvas.setWall(6, "as2_door_closed");
        click({1, 1});
        click({1, 2});
        QCOMPARE(project.doors.back().size, 2);
    }
    void stampPlacementSelectionAndDragging() {
        Project project;
        project.version = "AS2R";
        project.width = 3000;
        project.height = 3000;
        Locale locale;
        Catalog catalog(locale);
        Viewport canvas(project, catalog);
        canvas.resize(900, 700);
        canvas.show();
        canvas.fitMap();
        QPoint location(450, 350);
        canvas.armStamp("01dorm:1");
        QTest::mouseClick(&canvas, Qt::LeftButton, {}, location);
        QTest::mouseClick(&canvas, Qt::LeftButton, {}, location);
        QCOMPARE(project.decorations.size(), size_t(2));
        QCOMPARE(canvas.selectedDecoration, -1);
        QVERIFY(canvas.isPlacing());
        QTest::mouseClick(&canvas, Qt::RightButton, {}, location);
        QVERIFY(canvas.isEditing());
        QVERIFY(!canvas.isPlacing());
        auto original = project.decorations.back().origin;
        auto center = canvas.decorationFrame(project.decorations.back()).boundingRect().center();
        location = canvas.mapToScreen(center).toPoint();
        QTest::mousePress(&canvas, Qt::LeftButton, {}, location);
        QCOMPARE(canvas.selectedDecoration, 1);
        QTest::mouseMove(&canvas, location + QPoint(30, 20));
        QTest::mouseRelease(&canvas, Qt::LeftButton, {}, location + QPoint(30, 20));
        QVERIFY(QLineF(original, project.decorations.back().origin).length() > 1);
        canvas.deleteSelection();
        QCOMPARE(project.decorations.size(), size_t(1));
        canvas.armStamp("01dorm:1");
        canvas.setMode(DrawingMode::Polyline);
        QVERIFY(!canvas.isPlacing());
        QCOMPARE(canvas.part, QString("wall_body"));
    }
    void stampDrawOrderStacksBottomUp() {
        using auto_mapper::core::decoration_builder::DecorationMember;
        using auto_mapper::core::decoration_builder::DecorationStamp;
        // Members of AS1 library stamp 02archive:6, in SPR order.
        struct Source {
            int vid;
            float x, y, z;
        };
        const Source sources[] = {
            {413, 4346, 576, 54}, // 0: small cabinet stack, top first as MapEdit saved it
            {413, 4346, 576, 27}, // 1
            {413, 4346, 576, 0},  // 2
            {412, 4344, 594, 1},  // 3: plant in front of the stack
            {428, 4384, 551, 71}, // 4: box on the bookcase, anchor 3 units behind it
            {413, 4382, 554, 0},  // 5: bookcase
            {916, 4300, 650, 0},  // 6: blood on the ground
            {916, 4300, 500, 20}, // 7: blood raised onto something
        };
        DecorationStamp stamp;
        for (const Source &source : sources) {
            DecorationMember member;
            member.vid = source.vid;
            member.offset_x = source.x;
            member.offset_y = source.y;
            member.pos_z = source.z;
            stamp.members.push_back(member);
        }

        // Main window headers bring in stampDrawOrder() through the viewport.
        auto order = stampDrawOrder(stamp, auto_mapper::io::MapFormat::AS1);
        QList<int> drawn;
        for (const auto *member : order)
            drawn.append(int(member - stamp.members.data()));
        // Ground blood; bookcase then the box resting on it; the stack bottom-up; the plant in front of the
        // stack (not covered by the raised cabinets); raised blood last.
        QCOMPARE(drawn, QList<int>({6, 5, 4, 2, 1, 0, 3, 7}));
    }
    void as1ProjectsUseTheAs1StampLibrary() {
        MainWindow window("en_US");
        Project project;
        project.version = "AS1";
        project.width = 3000;
        project.height = 3000;
        window.replaceProject(project);
        // The decoration shelf opens for AS1 and arms the first AS1 library stamp.
        window.chooseShelf(ShelfMode::Decorations);
        QVERIFY(window.viewport->isPlacing());
        QVERIFY2(window.viewport->activeStamp.startsWith("01office:"), qPrintable(window.viewport->activeStamp));

        window.viewport->resize(900, 700);
        window.viewport->fitMap();
        QTest::mouseClick(window.viewport, Qt::LeftButton, {}, QPoint(450, 350));
        QCOMPARE(window.project.decorations.size(), size_t(1));
        QVERIFY(window.viewport->decorationFrame(window.project.decorations[0]).size() == 4);
        QString capture = qEnvironmentVariable("AUTO_MAPPER_CAPTURE_DIR");
        if (!capture.isEmpty()) {
            // AS1 shelf cards and a 1:1 close-up of the placed stamp: AS1 sprites when AutoMapper.cfg names as1_path.
            QDir().mkpath(capture);
            window.resize(1600, 1000);
            window.show();
            QPointF placed = window.project.decorations[0].origin;
            window.viewport->zoom = 1.0;
            window.viewport->pan = QPointF(window.viewport->rect().center()) - placed;
            window.viewport->update();
            QTest::qWait(250);
            QVERIFY(window.grab().save(capture + "/native-as1-stamps.png"));
            // Stacked cabinets placed, and the translucent placing ghost of the same stamp beside them.
            window.viewport->armStamp("02archive:3");
            QPoint placedAt = window.viewport->rect().center() + QPoint(-200, 0);
            QTest::mouseClick(window.viewport, Qt::LeftButton, {}, placedAt);
            QTest::mouseMove(window.viewport, placedAt + QPoint(320, 0));
            QTest::qWait(250);
            QVERIFY(window.viewport->grab().save(capture + "/native-as1-ghost.png"));
        }

        QTemporaryDir directory;
        auto path = directory.filePath("as1-stamp.map");
        QVERIFY(window.project.generate(path, false, false, false, &window.catalog.decorationLibrary(true)));
        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly));
        QVERIFY(file.size() > 280);
    }
    void arraysAndEraser() {
        Project project;
        project.width = 3000;
        project.height = 3000;
        Locale locale;
        Catalog catalog(locale);
        Viewport canvas(project, catalog);
        canvas.resize(900, 700);
        canvas.show();
        canvas.fitMap();
        canvas.armArray("incubator_array");
        QTest::mouseClick(&canvas, Qt::LeftButton, {}, {300, 300});
        QTest::mouseClick(&canvas, Qt::LeftButton, {}, {450, 450});
        QCOMPARE(project.decorations.size(), size_t(1));
        QVERIFY(project.decorations[0].rowLength > 0);
        canvas.setMode(DrawingMode::Select);
        auto center =
            canvas.mapToScreen(canvas.decorationFrame(project.decorations[0]).boundingRect().center())
                .toPoint();
        QTest::mouseClick(&canvas, Qt::LeftButton, {}, center);
        QCOMPARE(canvas.selectedDecoration, 0);
        canvas.setSpacing(1.5, 2);
        QCOMPARE(project.decorations[0].itemSpacing, 1.5);
        project.segments = {{{0, 1}, {0, 5}, 0}, {{0, 1}, {0, 5}, 2}};
        canvas.setWall(0);
        canvas.setMode(DrawingMode::Eraser);
        canvas.eraserSize = 2;
        QTest::mouseClick(&canvas, Qt::LeftButton, {},
                          canvas.mapToScreen(canvas.gridToMap({0, 3}, 0)).toPoint());
        int dark = 0;
        for (auto segment : project.segments)
            if (segment.wall_type == 2)
                ++dark;
        QCOMPARE(dark, 1);
        QCOMPARE(project.decorations.size(), size_t(1));
    }
    void curveWallFreehandAndEraser() {
        using auto_mapper::core::CURVE_WALL_TYPE_ALIEN_NEST;
        using auto_mapper::core::CURVE_WALL_TYPE_ROCK;
        MainWindow window(LOCALE_EN_US);
        window.resize(1400, 900);
        window.show();
        QApplication::processEvents();
        window.replaceProject(Project::createEmpty(PROJECT_VERSION_AS2R));
        auto *shelf = window.findChild<ThemeShelfPanel *>("setShelf");
        auto *freehand = window.findChild<QAction *>("drawingMode5");
        auto *polyline = window.findChild<QAction *>("drawingMode1");
        QVERIFY(freehand && polyline);
        QVERIFY(!freehand->isVisible());

        // The Curve Wall card is the last AS2 wall card: it swaps the grid tools for Freehand.
        int curveCard = window.catalog.wallSetOrder(false).size() - 1;
        shelf->setCurrentRow(curveCard);
        QVERIFY(window.viewport->isCurveWallCard());
        QCOMPARE(window.viewport->curveWallType, CURVE_WALL_TYPE_ROCK);
        QCOMPARE(window.viewport->mode, DrawingMode::Freehand);
        QVERIFY(freehand->isVisible());
        QVERIFY(!polyline->isVisible());
        window.activateMode(DrawingMode::Polyline);
        QCOMPARE(window.viewport->mode, DrawingMode::Freehand);

        // A loop that ends back on its start closes; a plain drag stays open.
        Viewport *canvas = window.viewport;
        QPoint center = canvas->rect().center();
        QPoint start = center + QPoint(120, 0);
        QTest::mousePress(canvas, Qt::LeftButton, {}, start);
        for (int step = 1; step < 24; ++step) {
            double angle = 2 * std::numbers::pi * step / 24;
            QTest::mouseMove(canvas, center + QPoint(int(120 * std::cos(angle)), int(80 * std::sin(angle))));
        }
        QTest::mouseMove(canvas, start + QPoint(3, -2));
        QTest::mouseRelease(canvas, Qt::LeftButton, {}, start + QPoint(3, -2));
        QCOMPARE(window.project.curveWalls.size(), size_t(1));
        QVERIFY(window.project.curveWalls[0].closed);
        QVERIFY(window.project.curveWalls[0].points.size() >= 3);
        QCOMPARE(window.project.curveWalls[0].curve_wall_type, CURVE_WALL_TYPE_ROCK);

        auto *combo = window.findChild<QComboBox *>("componentCombo");
        QCOMPARE(combo->count(), 3);
        combo->setCurrentIndex(2);
        QCOMPARE(window.viewport->curveWallType, auto_mapper::core::CURVE_WALL_TYPE_CLIFF);
        QString cliffCapture = qEnvironmentVariable("AUTO_MAPPER_CAPTURE_DIR");
        if (!cliffCapture.isEmpty()) {
            // A cliff drawn left to right, with its face ticks above the line; removed again afterwards.
            QDir().mkpath(cliffCapture);
            QPoint cliffStart = center + QPoint(-250, -220);
            QTest::mousePress(canvas, Qt::LeftButton, {}, cliffStart);
            for (int step = 1; step <= 10; ++step)
                QTest::mouseMove(canvas, cliffStart + QPoint(step * 40, int(30 * std::sin(step / 2.0))));
            QTest::mouseRelease(canvas, Qt::LeftButton, {}, cliffStart + QPoint(400, int(30 * std::sin(5.0))));
            QTest::qWait(250);
            QVERIFY(canvas->grab().save(cliffCapture + "/native-curve-wall-cliff.png"));
            window.project.curveWalls.pop_back();
        }
        combo->setCurrentIndex(1);
        QCOMPARE(window.viewport->curveWallType, CURVE_WALL_TYPE_ALIEN_NEST);
        QPoint lineStart = center + QPoint(-200, 200);
        QTest::mousePress(canvas, Qt::LeftButton, {}, lineStart);
        for (int step = 1; step <= 10; ++step)
            QTest::mouseMove(canvas, lineStart + QPoint(step * 30, 0));
        QTest::mouseRelease(canvas, Qt::LeftButton, {}, lineStart + QPoint(300, 0));
        QCOMPARE(window.project.curveWalls.size(), size_t(2));
        QVERIFY(!window.project.curveWalls[1].closed);
        // A click is not a wall; right click drops a stroke in progress.
        QTest::mouseClick(canvas, Qt::LeftButton, {}, center);
        QTest::mousePress(canvas, Qt::LeftButton, {}, center);
        QTest::mouseMove(canvas, center + QPoint(80, 40));
        QTest::mouseClick(canvas, Qt::RightButton, {}, center + QPoint(80, 40));
        QTest::mouseRelease(canvas, Qt::LeftButton, {}, center + QPoint(80, 40));
        QCOMPARE(window.project.curveWalls.size(), size_t(2));
        QString capture = qEnvironmentVariable("AUTO_MAPPER_CAPTURE_DIR");
        if (!capture.isEmpty()) {
            // Curve Walls draw as wall lines with piece dots, like grid walls.
            QDir().mkpath(capture);
            QTest::qWait(250);
            QVERIFY(window.grab().save(capture + "/native-curve-walls.png"));
            QPointF loopCenter = canvas->screenToMap(center);
            double savedZoom = canvas->zoom;
            QPointF savedPan = canvas->pan;
            canvas->zoom = 0.6;
            canvas->pan = QPointF(canvas->rect().center()) - loopCenter * canvas->zoom;
            canvas->update();
            QTest::qWait(250);
            QVERIFY(canvas->grab().save(capture + "/native-curve-walls-zoom.png"));
            canvas->zoom = savedZoom;
            canvas->pan = savedPan;
        }

        // The eraser keeps the card and cuts curves of the active type only.
        window.activateMode(DrawingMode::Eraser);
        QCOMPARE(window.viewport->mode, DrawingMode::Eraser);
        QVERIFY(window.viewport->isCurveWallCard());
        QTest::mouseClick(canvas, Qt::LeftButton, {}, start);
        QCOMPARE(window.project.curveWalls.size(), size_t(2));
        // Cutting the middle of the nest line leaves its two halves.
        QTest::mouseClick(canvas, Qt::LeftButton, {}, lineStart + QPoint(150, 0));
        QCOMPARE(window.project.curveWalls.size(), size_t(3));
        QCOMPARE(window.project.curveWalls[1].curve_wall_type, CURVE_WALL_TYPE_ALIEN_NEST);
        QCOMPARE(window.project.curveWalls[2].curve_wall_type, CURVE_WALL_TYPE_ALIEN_NEST);
        QVERIFY(window.project.curveWalls[1].points.back().x < window.project.curveWalls[2].points.front().x);
        // Dragging across the rest of the line erases it piece by piece.
        window.viewport->eraserSize = 10;
        QTest::mousePress(canvas, Qt::LeftButton, {}, lineStart);
        for (int step = 1; step <= 10; ++step)
            QTest::mouseMove(canvas, lineStart + QPoint(step * 30, 0));
        QTest::mouseRelease(canvas, Qt::LeftButton, {}, lineStart + QPoint(300, 0));
        QCOMPARE(window.project.curveWalls.size(), size_t(1));
        QVERIFY(window.project.curveWalls[0].closed);
        window.viewport->eraserSize = 1;

        // Project JSON keeps curve walls; AS1 projects cannot hold them.
        QJsonObject json = window.project.toJson();
        Project reloaded = Project::fromJson(json);
        QCOMPARE(reloaded.curveWalls.size(), size_t(1));
        QCOMPARE(reloaded.curveWalls[0].points.size(), window.project.curveWalls[0].points.size());
        QCOMPARE(reloaded.curveWalls[0].points[1].x, window.project.curveWalls[0].points[1].x);
        QVERIFY(reloaded.curveWalls[0].closed);
        json["version"] = "AS1";
        QVERIFY_EXCEPTION_THROWN(Project::fromJson(json), std::runtime_error);
        QVERIFY(!Project::createEmpty(PROJECT_VERSION_AS2R).toJson().contains("curve_walls"));
        QTemporaryDir directory;
        QVERIFY(window.project.generate(directory.filePath("curve.map"), false, false, true));

        // Back on a grid card the grid tools return and Freehand hides.
        shelf->setCurrentRow(0);
        QVERIFY(!window.viewport->isCurveWallCard());
        QCOMPARE(window.viewport->mode, DrawingMode::Polyline);
        QVERIFY(!freehand->isVisible());
        QVERIFY(polyline->isVisible());
    }
    void lootBoxBrushEraserAndTable() {
        using auto_mapper::core::LOOT_VID_EGG;
        using auto_mapper::core::LOOT_VID_WOOD_BOX;
        MainWindow window(LOCALE_EN_US);
        window.resize(1400, 900);
        window.show();
        QApplication::processEvents();
        window.replaceProject(Project::createEmpty(PROJECT_VERSION_AS2R));
        auto *freehand = window.findChild<QAction *>("drawingMode5");
        auto *select = window.findChild<QAction *>("drawingMode0");
        auto *polyline = window.findChild<QAction *>("drawingMode1");
        auto *eraser = window.findChild<QAction *>("drawingMode4");
        QVERIFY(freehand && select && polyline && eraser);

        // The Loot shelf offers the brush (Freehand) and the eraser only.
        window.chooseShelf(ShelfMode::Loot);
        QVERIFY(window.viewport->isLootCard());
        QCOMPARE(*window.viewport->lootKind, LootBoxKind::Wood);
        QCOMPARE(window.viewport->mode, DrawingMode::Freehand);
        QVERIFY(freehand->isVisible());
        QVERIFY(eraser->isVisible());
        QVERIFY(!select->isVisible());
        QVERIFY(!polyline->isVisible());
        window.activateMode(DrawingMode::Polyline);
        QCOMPARE(window.viewport->mode, DrawingMode::Freehand);
        QVERIFY(window.findChild<LootBrushPropertiesWidget *>()->isVisible());

        // A drag paints wood box piles inside the map.
        Viewport *canvas = window.viewport;
        QPoint center = canvas->rect().center();
        QTest::mousePress(canvas, Qt::LeftButton, {}, center);
        for (int step = 1; step <= 10; ++step)
            QTest::mouseMove(canvas, center + QPoint(step * 20, 0));
        QTest::mouseRelease(canvas, Qt::LeftButton, {}, center + QPoint(200, 0));
        QVERIFY(!window.project.lootBoxes.empty());
        for (const LootBox &box : window.project.lootBoxes) {
            QCOMPARE(box.vid, LOOT_VID_WOOD_BOX);
            QVERIFY(box.x >= 0 && box.y >= 0 && box.x <= window.project.width && box.y <= window.project.height);
        }
        QVERIFY(window.isDirty());

        // The egg card paints eggs; its eraser leaves the wood boxes alone.
        window.findChild<ThemeShelfPanel *>("setShelf")->setCurrentRow(2);
        QCOMPARE(*window.viewport->lootKind, LootBoxKind::Egg);
        size_t woodCount = window.project.lootBoxes.size();
        QTest::mouseClick(canvas, Qt::LeftButton, {}, center);
        QVERIFY(window.project.lootBoxes.size() > woodCount);
        window.activateMode(DrawingMode::Eraser);
        QVERIFY(window.viewport->isLootCard());
        window.viewport->eraserSize = 20;
        QTest::mouseClick(canvas, Qt::LeftButton, {}, center);
        QCOMPARE(window.project.lootBoxes.size(), woodCount);
        for (const LootBox &box : window.project.lootBoxes)
            QVERIFY(box.vid != LOOT_VID_EGG);
        window.viewport->eraserSize = 1;

        // Leaving the Loot shelf drops the card; AS1 projects hide the Loot shelf button.
        window.chooseShelf(ShelfMode::Walls);
        QVERIFY(!window.viewport->isLootCard());
        QVERIFY(polyline->isVisible());
        window.replaceProject(Project::createEmpty(PROJECT_VERSION_AS1));
        bool lootButtonVisible = false;
        for (auto *button : window.findChildren<QPushButton *>("shelfModeButton")) {
            if (button->text() == window.locale.tr(TextKey::SHELF_MODE_LOOT))
                lootButtonVisible = button->isVisible();
        }
        QVERIFY(!lootButtonVisible);
    }
    void lootTableDialogAndPresets() {
        Locale locale(LOCALE_EN_US);
        // A working-directory loot_presets.ini wins over the one beside the exe: use a temporary one.
        QTemporaryDir directory;
        QString previous = QDir::currentPath();
        // Go back even when a check fails midway, or the temporary directory stays locked.
        auto restoreCurrent = qScopeGuard([previous] {
            QDir::setCurrent(previous);
        });
        QVERIFY(QDir::setCurrent(directory.path()));
        QFile seed(directory.filePath("loot_presets.ini"));
        QVERIFY(seed.open(QIODevice::WriteOnly));
        // Hand-written: a comment, rows out of order with a gap, "%" optional, one row without a percent.
        seed.write("; shared loot presets\n"
                   "[drop_egg]\n"
                   "item3 = Bonus_health10\n"
                   "item3_p = 25\n"
                   "item1 = Ammo_shotgun Ammo_minigun EMPTY\n"
                   "item1_p = 70.5%\n"
                   "item4 = Obj_money1\n"
                   "\n"
                   "[\xe6\xb5\x8b\xe8\xaf\x95]\n"
                   "item1 = Obj_money2\n"
                   "item1_p = 10%\n");
        seed.close();
        QCOMPARE(lootPresetsPath(), directory.filePath("loot_presets.ini"));

        QList<LootPreset> loaded = loadLootPresets();
        QCOMPARE(loaded.size(), 2);
        QCOMPARE(loaded[0].name, QString("drop_egg"));
        QCOMPARE(loaded[0].lines.size(), size_t(2));
        QCOMPARE(QString::fromStdString(loaded[0].lines[0].text), QString("Ammo_shotgun Ammo_minigun EMPTY"));
        QCOMPARE(loaded[0].lines[0].percent, 70.5);
        QCOMPARE(loaded[0].lines[1].percent, 25.0);
        QCOMPARE(loaded[1].name, QString::fromUtf8("\xe6\xb5\x8b\xe8\xaf\x95"));
        QVERIFY(!isValidLootPresetName("a]b"));
        QVERIFY(!isValidLootPresetName("  "));

        LootPreset preset;
        preset.name = "mine";
        preset.lines = {{"EMPTY Ammo_shotgun Obj_money3", 40}, {"Obj_money1", 10}};
        saveLootPreset(preset);
        QList<LootPreset> saved = loadLootPresets();
        QCOMPARE(saved.size(), 3);
        QCOMPARE(saved[2].name, QString("mine"));
        QCOMPARE(saved[2].lines.size(), size_t(2));
        QCOMPARE(QString::fromStdString(saved[2].lines[0].text), QString("EMPTY Ammo_shotgun Obj_money3"));
        QCOMPARE(saved[2].lines[0].percent, 40.0);
        // Re-saving an existing preset keeps its place and the file comment; rows are renumbered with "%".
        LootPreset egg = saved[0];
        egg.lines.pop_back();
        saveLootPreset(egg);
        saved = loadLootPresets();
        QCOMPARE(saved[0].name, QString("drop_egg"));
        QCOMPARE(saved[0].lines.size(), size_t(1));
        QFile written(lootPresetsPath());
        QVERIFY(written.open(QIODevice::ReadOnly));
        QByteArray content = written.readAll();
        written.close();
        QVERIFY(content.contains("; shared loot presets"));
        QVERIFY(content.contains("item1_p = 70.5%"));

        // The dialog edits whole lines and refuses a total above 100%.
        LootTableDialog dialog(locale, "Wood", preset.lines);
        dialog.show();
        QApplication::processEvents();
        auto *ok = dialog.findChild<QPushButton *>("lootTableOk");
        QVERIFY(ok->isEnabled());
        dialog.findChild<QPushButton *>("lootAddLine")->click();
        auto texts = dialog.findChildren<QPlainTextEdit *>("lootText");
        auto percents = dialog.findChildren<QDoubleSpinBox *>("lootPercent");
        QCOMPARE(texts.size(), 3);
        texts[2]->setPlainText("Ammo_rocket Ammo_rocket EMPTY");
        percents[2]->setValue(60);
        QVERIFY(!ok->isEnabled());
        percents[2]->setValue(50);
        QVERIFY(ok->isEnabled());
        QCOMPARE(dialog.lines().size(), size_t(3));
        QCOMPARE(QString::fromStdString(dialog.lines()[2].text), QString("Ammo_rocket Ammo_rocket EMPTY"));
        // A long text wraps and its box grows downwards; a pasted line break turns into a space.
        int oneLine = texts[2]->height();
        texts[2]->setPlainText(QString("Ammo_shotgun ").repeated(40));
        QApplication::processEvents();
        QVERIFY(texts[2]->height() > oneLine);
        QCOMPARE(texts[2]->width(), texts[0]->width());
        texts[1]->setPlainText("Obj_money1\nObj_money2");
        QCOMPARE(texts[1]->toPlainText(), QString("Obj_money1 Obj_money2"));

        deleteLootPreset("mine");
        QCOMPARE(loadLootPresets().size(), 2);

        // The shipped template is user data: only check that it parses into usable tables, not what it holds.
        QString templatePath = QString(AUTO_MAPPER_SOURCE_DIR) + "/loot_presets.ini";
        QVERIFY(QFile::remove(directory.filePath("loot_presets.ini")));
        QVERIFY(QFile::copy(templatePath, directory.filePath("loot_presets.ini")));
        QList<LootPreset> shipped = loadLootPresets();
        QVERIFY(!shipped.isEmpty());
        for (const LootPreset &template_ : shipped) {
            auto_mapper::core::LootTable table;
            table.lines = template_.lines;
            QVERIFY2(!template_.lines.empty(), qPrintable(template_.name));
            QVERIFY2(auto_mapper::core::is_valid_loot_table(table), qPrintable(template_.name));
        }
    }
    void windowNavigationAndPersistence() {
        MainWindow window("zh_CN");
        window.show();
        window.activateWindow();
        QApplication::processEvents();
        Project project;
        project.version = "AS2R";
        project.width = 3000;
        project.height = 3000;
        project.segments.push_back({{0, 0}, {0, 4}, 3});
        window.replaceProject(project);
        auto *combo = window.findChild<QComboBox *>("componentCombo");
        QVERIFY(combo);
        combo->setCurrentIndex(2);
        QCOMPARE(window.viewport->wallType, 5);
        auto *shelf = window.findChild<ThemeShelfPanel *>("setShelf");
        QVERIFY(shelf);
        shelf->setCurrentRow(1);
        shelf->setCurrentRow(0);
        QCOMPARE(combo->currentIndex(), 2);
        window.chooseShelf(ShelfMode::Decorations);
        QVERIFY(window.viewport->isPlacing());
        window.activateMode(DrawingMode::Select);
        QVERIFY(window.viewport->isEditing());
        window.chooseShelf(ShelfMode::Walls);
        QCOMPARE(window.viewport->part, QString("wall_body"));
        window.findChild<QComboBox *>("as2FormatCombo")->setCurrentText("AS2OE");
        QCOMPARE(window.project.version, QString("AS2OE"));
        QCOMPARE(window.project.segments.size(), size_t(1));
        QTemporaryDir temporary;
        auto path = temporary.filePath("项目.json");
        QVERIFY(window.saveProject(path));
        QCOMPARE(Project::load(path).version, QString("AS2OE"));
        auto *width = window.findChild<QDoubleSpinBox *>("mapWidth");
        width->setFocus();
        QTest::keyClick(width, Qt::Key_4);
        QVERIFY(window.viewport->mode != DrawingMode::Eraser);
        window.saveProject(path);
    }
    void hotkeysDispatchToCommands() {
        MainWindow window(LOCALE_EN_US);
        window.show();
        window.activateWindow();
        if (!QTest::qWaitForWindowActive(&window))
            QSKIP("Window activation is unavailable on this platform.");
        window.viewport->setFocus();
        QTest::keyClick(window.viewport, Qt::Key_4);
        QCOMPARE(window.viewport->mode, DrawingMode::Eraser);
        // Select is hidden on the wall shelf, so its hotkey does nothing there.
        QTest::keyClick(window.viewport, Qt::Key_0);
        QCOMPARE(window.viewport->mode, DrawingMode::Eraser);
        QTest::keyClick(window.viewport, Qt::Key_1);
        QCOMPARE(window.viewport->mode, DrawingMode::Polyline);
        auto *combo = window.findChild<QComboBox *>("componentCombo");
        QTest::keyClick(window.viewport, Qt::Key_Plus);
        QCOMPARE(combo->currentIndex(), 1);
        QTest::keyClick(window.viewport, Qt::Key_Minus);
        QCOMPARE(combo->currentIndex(), 0);
        auto *shelf = window.findChild<ThemeShelfPanel *>("setShelf");
        QTest::keyClick(window.viewport, Qt::Key_E);
        QCOMPARE(shelf->currentRow(), 1);
        QTest::keyClick(window.viewport, Qt::Key_Q);
        QCOMPARE(shelf->currentRow(), 0);
        // F5 toggles the floor option; F7 is unavailable in AS2 projects and must stay unchecked.
        auto *floor = window.findChild<QCheckBox *>("floorCheck");
        QTest::keyClick(window.viewport, Qt::Key_F5);
        QVERIFY(floor->isChecked());
        window.replaceProject(Project::createEmpty(PROJECT_VERSION_AS2R));
        window.viewport->setFocus();
        QTest::keyClick(window.viewport, Qt::Key_F7);
        QVERIFY(!window.findChild<QCheckBox *>("isDoorOpenCheck")->isChecked());
        // F9 opens the sprite preview window; the shortcuts and language buttons have no key.
        QTest::keyClick(window.viewport, Qt::Key_F9);
        QVERIFY(window.findChild<QDialog *>("spritePreviewDialog")->isVisible());
        window.findChild<QDialog *>("spritePreviewDialog")->close();
        // The help dialog lists real commands only: no reserved placeholder keys.
        for (const auto &binding : hotkeys::hotkeyBindings())
            QVERIFY2(!binding.commandId.startsWith("reserved"), qPrintable(binding.commandId));
    }
    void generateAllFormats() {
        QTemporaryDir directory;
        for (const auto &version : {"AS1", "AS2", "AS2R", "AS2OE"}) {
            Project project;
            project.version = version;
            int type = 0;
            if (!project.isAS1()) {
                project.width = 3000;
                project.height = 3000;
                type = 3;
            }
            project.segments = {{{0, 0}, {4, 0}, type}, {{4, 0}, {4, 4}, type}, {{4, 4}, {0, 4}, type}, {{0, 4}, {0, 0}, type}};
            auto path = directory.filePath(QString::fromUtf8("地图-") + version + ".map");
            QVERIFY(project.generate(path, true, true, false));
            QFile file(path);
            QVERIFY(file.open(QIODevice::ReadOnly));
            auto data = file.readAll();
            QVERIFY(data.size() > 280);
            QVERIFY(data.contains("SPR "));
            if (project.version == "AS2OE")
                QVERIFY(data.contains("LVL "));
        }
    }
    void legacyFixturesKeepGeneratedContent() {
        QTemporaryDir directory;
        const QDir source(QString::fromUtf8(AUTO_MAPPER_SOURCE_DIR));
        const QStringList fixtures{"wall_builder.gold", "incubator_builder_gold", "floor_builder.gold"};
        for (const auto &name : fixtures) {
            Project project = Project::load(source.filePath("tests/golden/" + name + ".json"));
            auto saved = directory.filePath(name + ".json");
            project.save(saved);
            QCOMPARE(Project::load(saved).toJson(), project.toJson());
            auto generated = directory.filePath(name + ".map");
            QVERIFY(project.generate(generated, name == "floor_builder.gold", false, false));
            QFile actual(generated);
            QFile expected(source.filePath("tests/golden/" + name + ".map"));
            QVERIFY(actual.open(QIODevice::ReadOnly));
            QVERIFY(expected.open(QIODevice::ReadOnly));
            QCOMPARE(actual.readAll(), expected.readAll());
        }
    }
    void navigationExportAndUnsavedChanges() {
        QTemporaryDir directory;
        MainWindow window("en_US");
        window.show();
        Project project;
        project.segments = {{{0, 0}, {4, 0}, 0}, {{4, 0}, {4, 4}, 0}};
        window.replaceProject(project);
        QApplication::processEvents();
        auto *canvas = window.viewport;
        QPointF cursor(140, 150);
        auto before = canvas->screenToMap(cursor);
        QWheelEvent wheel(cursor, canvas->mapToGlobal(cursor.toPoint()), {}, {0, 120}, Qt::NoButton,
                          Qt::NoModifier, Qt::NoScrollPhase, false);
        QApplication::sendEvent(canvas, &wheel);
        QVERIFY(QLineF(before, canvas->screenToMap(cursor)).length() < 0.0001);
        auto pan = canvas->pan;
        QTest::mousePress(canvas, Qt::MiddleButton, {}, {150, 150});
        QTest::mouseMove(canvas, {180, 170});
        QTest::mouseRelease(canvas, Qt::MiddleButton, {}, {180, 170});
        QCOMPARE(canvas->pan, pan + QPointF(30, 20));
        auto output = directory.filePath("异步导出.map");
        QVERIFY(window.exportMap(output));
        QVERIFY(!window.isEnabled());
        QTRY_VERIFY_WITH_TIMEOUT(window.isEnabled(), 10000);
        QVERIFY(QFileInfo(output).size() > 280);
        window.findChild<QDoubleSpinBox *>("mapWidth")->setValue(800);
        window.findChild<QPushButton *>("applyMapSize")->click();
        QCOMPARE(window.project.width, 800.0);
        QTimer::singleShot(0, [] {
            auto *dialog = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
            if (dialog)
                dialog->button(QMessageBox::Cancel)->click();
        });
        QVERIFY(!window.close());
        QVERIFY(window.isVisible());
        QVERIFY(window.saveProject(directory.filePath("saved.json")));
        QVERIFY(window.close());
    }
};
QTEST_MAIN(EditorTest)
#include "test_editor.moc"
