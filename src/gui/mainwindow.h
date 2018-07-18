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

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QFutureWatcher>

#include "src/common/filetree.h"

#include "src/gui/resourcetree.h"
#include "src/gui/proxymodel.h"
#include "src/gui/statusbar.h"

#include "verdigris/wobjectdefs.h"

class QSplitter;
class QTreeView;
class QGridLayout;
class QFrame;
class QTextEdit;

namespace GUI {

class PanelResourceInfo;
class ResourceTree;
class ResourceTreeItem;
class ProxyModel;
class PanelManager;
class PanelBase;

class MainWindow : public QMainWindow {
	W_OBJECT(MainWindow)

public:
	MainWindow(QWidget *parent, const char *title, const QSize &size, const char *path);
	~MainWindow();

private /*slots*/:
	void slotLog(const QString &text);
	W_SLOT(slotLog, W_Access::Private)

	void slotOpenDirectory();
	W_SLOT(slotOpenDirectory, W_Access::Private)

	void slotOpenFile();
	W_SLOT(slotOpenFile, W_Access::Private)

	void slotClose();
	W_SLOT(slotClose, W_Access::Private)

	void slotQuit();
	W_SLOT(slotQuit, W_Access::Private)

	void slotAbout();
	W_SLOT(slotAbout, W_Access::Private)

	void saveItem();
	W_SLOT(saveItem, W_Access::Private)

	void exportTGA();
	W_SLOT(exportTGA, W_Access::Private)

	void exportBMUMP3();
	W_SLOT(exportBMUMP3, W_Access::Private)

	void exportWAV();
	W_SLOT(exportWAV, W_Access::Private)

private:
	void open(const QString &path);
	void openFinish();

	void close();

	void statusPush(const QString &text);
	void statusPop();

	void resourceSelect(const QItemSelection &selected, const QItemSelection &deselected);

	void exportBMUMP3Impl(Common::SeekableReadStream &bmu, Common::WriteStream &mp3);
	void exportWAVImpl(Sound::AudioStream *sound, Common::WriteStream &wav);

	StatusBar _status;

	QWidget *_centralWidget { nullptr };
	QGridLayout *_centralLayout { nullptr };

	QAction *_actionOpenDirectory { nullptr };
	QAction *_actionOpenFile { nullptr };
	QAction *_actionClose { nullptr };
	QAction *_actionQuit { nullptr };
	QAction *_actionAbout { nullptr };

	QMenuBar *_menuBar { nullptr };
	QMenu *_menuFile { nullptr };
	QMenu *_menuHelp { nullptr };

	QSplitter *_splitterTopBottom { nullptr };
	QSplitter *_splitterLeftRight { nullptr };
	QTreeView *_treeView { nullptr };

	QFrame *_resPreviewFrame { nullptr };
	QTextEdit *_log { nullptr };

	Common::FileTree _files;

	std::unique_ptr<ResourceTree> _treeModel { nullptr };
	std::unique_ptr<ProxyModel> _proxyModel { nullptr };

	QString _rootPath;

	ResourceTreeItem *_currentItem { nullptr };

	PanelResourceInfo *_panelResourceInfo { nullptr };

	PanelManager *_panelManager { nullptr };

	QFutureWatcher<void> *_watcher { nullptr };

	friend class ResourceTree;
};

} // End of namespace GUI

#endif // GUI_MAINWINDOW_H
