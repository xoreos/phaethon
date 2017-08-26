/* Phaethon - A FLOSS resource explorer for BioWare's Aurora engine games
 *
 * Phaethon is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * Phaethon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Phaethon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Phaethon's main window.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QAction>
#include <QFileSystemModel>
#include <QGraphicsView>
#include <QItemSelection>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QSplitter>
#include <QTreeView>

#include "verdigris/wobjectdefs.h"

#include "src/common/readstream.h"
#include "src/common/writestream.h"
#include "src/gui/statusbar.h"
#include "src/sound/sound.h"

namespace GUI {

class PanelPreviewEmpty;
class PanelPreviewImage;
class PanelPreviewSound;
class PanelPreviewText;
class PanelResourceInfo;
class ResourceTreeItem;
class ResourceTree;

class MainWindow : public QMainWindow {
    W_OBJECT(MainWindow)

public:
    MainWindow(QWidget *parent, const char *title, const QSize &size, const char *path);
    ~MainWindow();

    std::shared_ptr<StatusBar> status();

private /*slots*/:
    void setTreeViewModel(const QString &path);
    void slotOpenDir();
    void slotOpenFile();
    void slotCloseDir();
    void slotQuit();
    void slotLog(const QString &text);
    void saveItem();
    void exportTGA();
    void exportBMUMP3();
    void exportWAV();
    void slotAbout();
    void resourceSelect(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void setLabels();
    void showPreviewPanel(QFrame *panel);
    void showPreviewPanel();
    void showExportButtons();
    void showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA);
    void exportBMUMP3Impl(Common::SeekableReadStream &bmu, Common::WriteStream &mp3);
    void exportWAVImpl(Sound::AudioStream *sound, Common::WriteStream &wav);

    std::shared_ptr<StatusBar> _status;
    const ResourceTreeItem *_currentItem;
    std::unique_ptr<ResourceTree> _treeModel;
    QString _rootPath;

    QWidget *_centralWidget;

    QGridLayout *_centralLayout;
    QVBoxLayout *_layoutVertical;
    QSplitter *_splitterTopBottom;
    QSplitter *_splitterLeftRight;

    QFrame *_resPreviewFrame;

    QTreeView *_treeView;

    QTextEdit *_log;

    QAction *_actionOpenDirectory;
    QAction *_actionClose;
    QAction *_actionQuit;
    QAction *_actionAbout;
    QAction *_actionOpenFile;

    QMenuBar *_menuBar;
    QMenu *_menuFile;
    QMenu *_menuHelp;

    // resource preview
    PanelPreviewEmpty *_panelPreviewEmpty;
    PanelPreviewImage *_panelPreviewImage;
    PanelPreviewSound *_panelPreviewSound;
    PanelPreviewText *_panelPreviewText;
    PanelResourceInfo *_panelResourceInfo;
};

} // End of namespace GUI

#endif // MAINWINDOW_H
