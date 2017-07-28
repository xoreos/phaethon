#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QFileSystemModel>
#include <QGraphicsView>
#include <QLabel>
#include <QProgressBar>
#include <QItemSelection>
#include <QPlainTextEdit>

#include "verdigris/wobjectdefs.h"

#include "src/common/ustring.h"
#include "src/gui/panelpreviewempty.h"
#include "src/gui/panelpreviewimage.h"
#include "src/gui/panelpreviewsound.h"
#include "src/gui/panelpreviewtext.h"
#include "src/gui/panelresourceinfo.h"
#include "src/gui/statusbar.h"
#include "src/gui/resourcetree.h"
#include "src/gui/resourcetreeitem.h"

#include "ui/ui_mainwindow.h"

namespace Ui {
    class MainWindow;
}

namespace GUI {

class MainWindow : public QMainWindow {
    W_OBJECT(MainWindow)

public:
    MainWindow(QWidget *parent = 0, const char *version = "", const QSize &size = QSize(800, 600), const Common::UString &path = "");
    ~MainWindow();

private /*slots*/:
    void setTreeViewModel(QString path);
    W_SLOT(setTreeViewModel)

    void slotOpenDir();
    W_SLOT(slotOpenDir, W_Access::Private)

    void slotOpenFile();
    W_SLOT(slotOpenFile, W_Access::Private)

    void slotCloseDir();
    W_SLOT(slotCloseDir, W_Access::Private)

    void slotQuit();
    W_SLOT(slotQuit, W_Access::Private)

    void slotLogAppend(QString text);
    W_SLOT(slotLogAppend, W_Access::Private)

    void saveItem();
    W_SLOT(saveItem, W_Access::Private)

    void exportTGA();
    W_SLOT(exportTGA, W_Access::Private)

    void exportBMUMP3();
    W_SLOT(exportBMUMP3, W_Access::Private)

    void exportWAV();
    W_SLOT(exportWAV, W_Access::Private)

    void slotAbout();
    W_SLOT(slotAbout, W_Access::Private)

    std::shared_ptr<StatusBar> getStatusBar();
    void selection(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void setLabels();
    void showPreviewPanel(QFrame *panel);
    void showPreviewPanel();
    void showExportButtons();
    void showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA);
    void exportBMUMP3Impl(Common::SeekableReadStream &bmu, Common::WriteStream &mp3);
    void exportWAVImpl(Sound::AudioStream *sound, Common::WriteStream &wav);

    Ui::MainWindow _ui;
    std::shared_ptr<StatusBar> _status;
    const ResourceTreeItem *_currentItem;
    std::unique_ptr<ResourceTree> _treeModel;
    QString _rootPath;

    // resource preview
    PanelPreviewEmpty *_panelPreviewEmpty;
    PanelPreviewImage *_panelPreviewImage;
    PanelPreviewSound *_panelPreviewSound;
    PanelPreviewText *_panelPreviewText;
    ResourceInfoPanel *_resInfo;
};

} // End of namespace GUI

#endif // MAINWINDOW_H
