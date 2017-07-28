#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "verdigris/wobjectdefs.h"

#include <QMainWindow>
#include <QFileSystemModel>
#include <QGraphicsView>
#include <QLabel>
#include <QProgressBar>
#include <QItemSelection>
#include <QPlainTextEdit>

#include "src/common/ustring.h"
#include "src/gui/panelpreviewempty.h"
#include "src/gui/panelpreviewimage.h"
#include "src/gui/panelpreviewsound.h"
#include "src/gui/panelpreviewtext.h"
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
    void setTreeViewModel(const QString &path);
    void slotOpenDir();
    void slotCloseDir();
    void slotQuit();

    void sbLoadKotorDir();
    void sbCloseDir();
    void sbUnused1();
    void sbUnused2();

    void selection(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void setLabels();
    void closeResource();
    void showPreviewPanel(QFrame *panel);
    void showPreviewPanel();

private:
    Ui::MainWindow *ui = nullptr;
    QLabel *_statusLabel = nullptr;
    ResourceTreeItem *_currentSelection = nullptr;
    ResourceTree *_treeModel = nullptr;
    QString _rootPath = nullptr;

    // resource preview
    PanelPreviewEmpty *_panelPreviewEmpty = nullptr;
    PanelPreviewImage *_panelPreviewImage = nullptr;
    PanelPreviewSound *_panelPreviewSound = nullptr;
    PanelPreviewText *_panelPreviewText = nullptr;
};

} // End of namespace GUI

#endif // MAINWINDOW_H
