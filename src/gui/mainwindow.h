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

#include <QMainWindow>

#include "verdigris/wobjectdefs.h"

namespace GUI {

class MainWindow : public QMainWindow {
	W_OBJECT(MainWindow)

public:
	MainWindow(QWidget *parent, const char *title, const QSize &size, const char *path);

private /*slots*/:
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

private:
	QWidget *_centralWidget;

	QAction *_actionOpenDirectory;
	QAction *_actionOpenFile;
	QAction *_actionClose;
	QAction *_actionQuit;
	QAction *_actionAbout;

	QMenuBar *_menuBar;
	QMenu *_menuFile;
	QMenu *_menuHelp;
};

} // End of namespace GUI

#endif // MAINWINDOW_H
