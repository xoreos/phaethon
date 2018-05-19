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

#include <QAction>
#include <QCoreApplication>
#include <QMenuBar>
#include <QMenu>

#include "verdigris/wobjectimpl.h"

#include "src/gui/mainwindow.h"

#include "src/common/util.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent, const char *title, const QSize &size, const char *UNUSED(path)) : QMainWindow(parent) {
	_centralWidget = new QWidget(this);
	setCentralWidget(_centralWidget);
	setWindowTitle(title);
	resize(size);

	_actionOpenDirectory = new QAction(this);
	_actionOpenFile = new QAction(this);
	_actionClose = new QAction(this);
	_actionQuit = new QAction(this);
	_actionAbout = new QAction(this);

	_actionOpenDirectory->setText(tr("&Open directory"));
	_actionOpenFile->setText(tr("&Open file"));
	_actionClose->setText(tr("&Close"));
	_actionQuit->setText(tr("Quit"));
	_actionAbout->setText(tr("About"));

	_menuBar = new QMenuBar(this);
	_menuFile = new QMenu(_menuBar);
	_menuHelp = new QMenu(_menuBar);

	_menuBar->addAction(_menuFile->menuAction());
	_menuBar->addAction(_menuHelp->menuAction());
	_menuFile->addAction(_actionOpenDirectory);
	_menuFile->addAction(_actionOpenFile);
	_menuFile->addSeparator();
	_menuFile->addAction(_actionClose);
	_menuFile->addSeparator();
	_menuFile->addAction(_actionQuit);
	_menuFile->setTitle("&File");
	_menuHelp->addAction(_actionAbout);
	_menuHelp->setTitle("&Help");

	setMenuBar(_menuBar);
}

} // End of namespace GUI
