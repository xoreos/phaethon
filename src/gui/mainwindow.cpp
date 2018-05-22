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
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QTreeView>
#include <QSplitter>
#include <QGridLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QStatusBar>

#include <boost/scope_exit.hpp>

#include "verdigris/wobjectimpl.h"

#include "src/cline.h"

#include "src/common/util.h"

#include "src/gui/mainwindow.h"

#include "src/version/version.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent, const char *title, const QSize &size, const char *path) :
	QMainWindow(parent), _status(statusBar()), _treeModel(0), _proxyModel(0), _rootPath("") {
	/* Window setup. */
	setWindowTitle(title);
	resize(size);

	/* Actions. */
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

	/* Menu. */
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

	/* Slots. */
	QObject::connect(_actionOpenDirectory, &QAction::triggered, this, &MainWindow::slotOpenDirectory);
	QObject::connect(_actionOpenFile, &QAction::triggered, this, &MainWindow::slotOpenFile);
	QObject::connect(_actionClose, &QAction::triggered, this, &MainWindow::slotClose);
	QObject::connect(_actionQuit, &QAction::triggered, this, &MainWindow::slotQuit);
	QObject::connect(_actionAbout, &QAction::triggered, this, &MainWindow::slotAbout);

	/* Layout. */
	_centralWidget = new QWidget(this);
	_centralLayout = new QGridLayout(_centralWidget);
	_splitterTopBottom = new QSplitter(_centralWidget);
	_splitterLeftRight = new QSplitter(_splitterTopBottom);
	_treeView = new QTreeView(_splitterLeftRight);
	QGroupBox *logBox = new QGroupBox(_splitterTopBottom);
	QWidget *previewWrapper = new QWidget(_splitterTopBottom); // Can't add a layout directly to a splitter.
	QVBoxLayout *previewWrapperLayout = new QVBoxLayout(previewWrapper);
	QFrame *resInfoFrame = new QFrame(previewWrapper);
	_resPreviewFrame = new QFrame(previewWrapper);
	_log = new QTextEdit(logBox);

	// Tree
	// 1:8 ratio; tree:preview/log
	{
		QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		sp.setHorizontalStretch(1);
		_treeView->setSizePolicy(sp);
	}

	// Preview wrapper
	previewWrapper->setLayout(previewWrapperLayout);
	previewWrapper->setContentsMargins(0, 0, 0, 0);
	previewWrapperLayout->setParent(previewWrapper);
	previewWrapperLayout->setMargin(0);
	previewWrapperLayout->addWidget(resInfoFrame);
	previewWrapperLayout->addWidget(_resPreviewFrame);
	{
		QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		sp.setHorizontalStretch(6);
		previewWrapper->setSizePolicy(sp);
	}

	// Log
	logBox->setTitle(tr("Log"));
	{
		QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
		sp.setVerticalStretch(1);
		logBox->setSizePolicy(sp);

		QHBoxLayout *hl = new QHBoxLayout(logBox);
		hl->addWidget(_log);
		hl->setContentsMargins(0, 5, 0, 0);
	}

	// Left/right splitter
	// 8:1 ratio, preview:log
	_splitterLeftRight->addWidget(_treeView);
	_splitterLeftRight->addWidget(previewWrapper);
	{
		QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		sp.setVerticalStretch(5);
		_splitterLeftRight->setSizePolicy(sp);
	}

	// Top/bottom splitter
	_splitterTopBottom->setOrientation(Qt::Vertical);
	_splitterTopBottom->addWidget(_splitterLeftRight);
	_splitterTopBottom->addWidget(logBox);

	// Resource info frame
	resInfoFrame->setFrameShape(QFrame::StyledPanel);
	resInfoFrame->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
	resInfoFrame->setFixedHeight(140);
	{
		QHBoxLayout *hl = new QHBoxLayout(resInfoFrame);
		QLabel *info = new QLabel(tr("Resource info"), resInfoFrame);
		hl->addWidget(info);
	}

	// Resource preview frame
	_resPreviewFrame->setFrameShape(QFrame::StyledPanel);
	{
		QFrame *previewFrame = new QFrame(_resPreviewFrame);
		QLabel *label = new QLabel(tr("[No preview.]"), previewFrame);
		label->setEnabled(false);
		label->setAlignment(Qt::AlignCenter);

		QHBoxLayout *hl = new QHBoxLayout(_resPreviewFrame);
		hl->addWidget(label);
		hl->setMargin(0);
		hl->addWidget(previewFrame);
	}

	setCentralWidget(_centralWidget);
	_centralLayout->addWidget(_splitterTopBottom);

	/* Open path. */
	const QString qpath = QString::fromUtf8(path);

	_treeModel.reset(new ResourceTree(this, _treeView));
	_proxyModel.reset(new ProxyModel(this));

	_status.setText("Idle...");

	if (qpath.isEmpty())
		_actionClose->setEnabled(false);
	else
		open(qpath);
}

void MainWindow::slotOpenDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this,
		tr("Open directory"),
		QString(QStandardPaths::HomeLocation),
		QFileDialog::ShowDirsOnly);

	if (!dir.isEmpty())
		open(dir);
}

void MainWindow::slotOpenFile() {
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Aurora game resource file"),
		QString(QStandardPaths::HomeLocation),
		tr("Aurora game resource (*.*)"));

	if (!fileName.isEmpty())
		open(fileName);
}

void MainWindow::slotClose() {
	_treeView->setModel(nullptr);
	_treeModel.reset(nullptr);

	_rootPath = "";

	_actionClose->setEnabled(false);

	_status.pop();
}

void MainWindow::slotQuit() {
	QApplication::quit();
}

void MainWindow::slotAbout() {
	const QString msg = QString::fromUtf8(createVersionText().c_str());
	QMessageBox::about(this, "About Phaethon", msg);
}

void MainWindow::open(const QString &path) {
	if (_rootPath == path)
		return;

	_rootPath = path;

	// popped in openFinish
	_status.push("Populating resource tree...");

	try {
		_files.readPath(Common::UString(path.toStdString()), -1);
	} catch (Common::Exception &e) {
		_status.pop();

		Common::printException(e, "WARNING: ");
		return;
	}

	_treeModel.reset(new ResourceTree(this, _treeView));

	// Enters populate thread in here.
	_treeModel->populate(_files.getRoot());

	_actionClose->setEnabled(true);
}

void MainWindow::openFinish() {
	_proxyModel->setSourceModel(_treeModel.get());
	_proxyModel->sort(0);

	_treeView->setModel(_proxyModel.get());
	_treeView->expandToDepth(0);
	_treeView->show();
	_treeView->resizeColumnToContents(0);

	_status.pop();
}

void MainWindow::statusPush(const QString &text) {
	_status.push(text);
}

void MainWindow::statusPop() {
	_status.pop();
}

} // End of namespace GUI
