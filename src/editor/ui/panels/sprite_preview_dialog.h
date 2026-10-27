// Small modeless window behind the toolbar "Sprite Preview" button: AS1 / AS2 game folders and the
// decoration display options. It only shows values and emits requests; MainWindow saves them and updates the canvas.
#pragma once
#include "editor/config.h"
#include "editor/i18n/locale.h"
#include <QDialog>

class QCheckBox;
class QLabel;
class QSlider;

namespace editor {

class SpritePreviewDialog : public QDialog {
    Q_OBJECT
  public:
    SpritePreviewDialog(const Locale &locale, QWidget *parent = nullptr);
    // Empty path shows the "not set" hint.
    void setGameFolder(GameFamily family, const QString &path);
    // Show saved values without emitting the change signals; the slider clamps the opacity to its limits.
    void setOptions(bool crossesWithSprites, bool alwaysShowFrame, int spriteOpacity);

  signals:
    void chooseGameFolderRequested(GameFamily family);
    void crossesWithSpritesChanged(bool enabled);
    void alwaysShowFrameChanged(bool enabled);
    // Every slider value, for a live preview while dragging.
    void spriteOpacityChanged(int percent);
    // The value the slider settled on (drag released, click or key step): the one worth saving.
    void spriteOpacityCommitted(int percent);

  private:
    // Title, path label and "Choose..." button of one game folder.
    QLabel *addGameFolderRow(GameFamily family, const char *titleKey);
    void syncOpacityLabel();

    const Locale &locale_;
    QLabel *as1FolderLabel_ = nullptr;
    QLabel *as2FolderLabel_ = nullptr;
    QCheckBox *crossesWithSprites_ = nullptr;
    QCheckBox *alwaysShowFrame_ = nullptr;
    QSlider *spriteOpacity_ = nullptr;
    QLabel *spriteOpacityValue_ = nullptr;
};

} // namespace editor
