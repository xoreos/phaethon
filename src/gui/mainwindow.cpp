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

#include <deque>

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

#include "external/verdigris/wobjectimpl.h"

#include "src/cline.h"

#include "src/common/util.h"
#include "src/common/writefile.h"

#include "src/gui/mainwindow.h"
#include "src/gui/panelresourceinfo.h"
#include "src/gui/resourcetreeitem.h"
#include "src/gui/panelpreviewempty.h"
#include "src/gui/panelpreviewimage.h"
#include "src/gui/panelpreviewsound.h"
#include "src/gui/panelpreviewtext.h"
#include "src/gui/panelpreviewtable.h"
#include "src/gui/panelmanager.h"

#include "src/images/dumptga.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

#include "src/version/version.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent, const char *title, const QSize &size, const char *path) :
	QMainWindow(parent), _status(statusBar()), _panelManager(new PanelManager()),
	_watcher(new QFutureWatcher<void>(this)) {
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
	_actionOpenDirectory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	_actionOpenFile->setText(tr("Open &file"));
	_actionClose->setText(tr("&Close"));
	_actionClose->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	_actionQuit->setText(tr("&Quit"));
	_actionQuit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	_actionAbout->setText(tr("&About"));
	_actionAbout->setShortcut(QKeySequence(Qt::Key_F1));

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
	_log->setReadOnly(true);

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
	_panelManager->registerPanel(new PanelPreviewEmpty(nullptr), Aurora::kResourceNone);
	_panelManager->registerPanel(new PanelPreviewSound(nullptr), Aurora::kResourceSound);
	_panelManager->registerPanel(new PanelPreviewImage(nullptr), Aurora::kResourceImage);
	_panelManager->registerPanel(new PanelPreviewText(nullptr), Aurora::kResourceText);
	_panelManager->registerPanel(new PanelPreviewTable(nullptr), Aurora::kResourceTable);
	_panelManager->setItem(nullptr);

	PanelPreviewText *textPanel = static_cast<PanelPreviewText *>(_panelManager->getPanelByType(Aurora::kResourceText));
	PanelPreviewTable *tablePanel = static_cast<PanelPreviewTable *>(_panelManager->getPanelByType(Aurora::kResourceTable));
	QObject::connect(textPanel, &PanelPreviewText::log, this, &MainWindow::slotLog);
	QObject::connect(tablePanel, &PanelPreviewTable::log, this, &MainWindow::slotLog);

	_panelResourceInfo = new PanelResourceInfo(this);
	QObject::connect(_panelResourceInfo, &PanelResourceInfo::closeDirClicked, this, &MainWindow::slotClose);
	QObject::connect(_panelResourceInfo, &PanelResourceInfo::saveClicked, this, &MainWindow::saveItem);
	QObject::connect(_panelResourceInfo, &PanelResourceInfo::exportTGAClicked, this, &MainWindow::exportTGA);
	QObject::connect(_panelResourceInfo, &PanelResourceInfo::exportBMUMP3Clicked, this, &MainWindow::exportBMUMP3);
	QObject::connect(_panelResourceInfo, &PanelResourceInfo::exportWAVClicked, this, &MainWindow::exportWAV);
	QObject::connect(_panelResourceInfo, &PanelResourceInfo::log, this, &MainWindow::slotLog);
	resInfoFrame->setFrameShape(QFrame::StyledPanel);
	resInfoFrame->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
	resInfoFrame->setFixedHeight(140);
	{
		QHBoxLayout *hl = new QHBoxLayout(resInfoFrame);
		hl->addWidget(_panelResourceInfo);
	}

	// Resource preview frame
	_resPreviewFrame->setFrameShape(QFrame::StyledPanel);
	{
		QHBoxLayout *hl = new QHBoxLayout(_resPreviewFrame);

		hl->setMargin(0);
		_panelManager->setLayout(hl);
	}

	setCentralWidget(_centralWidget);
	_centralLayout->addWidget(_splitterTopBottom);

	/* Open path. */
	const QString qpath = QString::fromUtf8(path);

	_treeModel = std::make_unique<ResourceTree>(this, _treeView);
	_proxyModel = std::make_unique<ProxyModel>(this);

	_panelManager->setItem(nullptr);

	_status.setText("Idle...");

	if (qpath.isEmpty())
		_actionClose->setEnabled(false);
	else
		open(qpath);
}

MainWindow::~MainWindow() {
	delete _panelManager;
}

void MainWindow::slotLog(const QString &text) {
	_log->append(text);
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
	_log->append(tr("Closed: %1").arg(_rootPath));
	close();
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

	close();

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

	_treeModel = std::make_unique<ResourceTree>(this, _treeView);

	// Enters populate thread in here.
	_treeModel->populate(_files.getRoot());

	_log->append(tr("Set root: %1").arg(path));

	_actionClose->setEnabled(true);
}

void MainWindow::openFinish() {
	_proxyModel->setSourceModel(_treeModel.get());
	_proxyModel->sort(0);

	_treeView->setModel(_proxyModel.get());
	_treeView->expandToDepth(0);
	_treeView->show();
	_treeView->resizeColumnToContents(0);

	QObject::connect(_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
		this, &MainWindow::resourceSelect);

	_status.pop();
}

void MainWindow::close() {
	_panelManager->setItem(nullptr);

	_panelResourceInfo->setButtonsForClosedDir();
	_panelResourceInfo->clearLabels();
	_treeView->setModel(nullptr);
	_treeModel.reset(nullptr);
	_currentItem = nullptr;

	_rootPath = "";

	_actionClose->setEnabled(false);

	_status.pop();
}

void MainWindow::statusPush(const QString &text) {
	_status.push(text);
}

void MainWindow::statusPop() {
	_status.pop();
}

void MainWindow::resourceSelect(const QItemSelection &selected, const QItemSelection &UNUSED(deselected)) {
	const QModelIndexList index = _proxyModel->mapSelectionToSource(selected).indexes();
	_currentItem = _treeModel->itemFromIndex(index.at(0));

	_panelResourceInfo->update(_currentItem);

	if (!_currentItem->isDir() && !_currentItem->isArchive())
		_panelManager->setItem(_currentItem);
	else
		_panelManager->setItem(nullptr);
}

QString constructStatus(const QString &_action, const QString &name, const QString &destination) {
	return _action + " \"" + name + "\" to \"" + destination + "\"...";
}

void MainWindow::saveItem() {
	if (!_currentItem)
		return;

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Aurora game resource file"),
		_currentItem->getName(),
		tr("Aurora game resource (*.*)"));

	if (fileName.isEmpty())
		return;

	_status.push(constructStatus("Saving", _currentItem->getName(), fileName));
	BOOST_SCOPE_EXIT((&_status)) {
		_status.pop();
	} BOOST_SCOPE_EXIT_END

	try {
		std::unique_ptr<Common::SeekableReadStream> res(_currentItem->getResourceData());

		Common::WriteFile file(fileName.toStdString());

		file.writeStream(*res);
		file.flush();

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}
}

QString replaceExtension(const QString &fileName, Aurora::FileType type) {
	Common::UString replaced = TypeMan.setFileType(fileName.toStdString(), type);
	return QString::fromUtf8(replaced.c_str());
}

void MainWindow::exportTGA() {
	if (!_currentItem)
		return;

	assert(_currentItem->getResourceType() == Aurora::kResourceImage);

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save TGA file"),
		replaceExtension(_currentItem->getName(), Aurora::kFileTypeTGA),
		tr("TGA file (*.tga)"));

	if (fileName.isEmpty())
		return;

	_status.push(constructStatus("Exporting", _currentItem->getName(), fileName));
	BOOST_SCOPE_EXIT((&_status)) {
		_status.pop();
	} BOOST_SCOPE_EXIT_END

	try {
		std::unique_ptr<Images::Decoder> image(_currentItem->getImage());

		image->dumpTGA(fileName.toStdString());

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}
}

struct SoundBuffer {
	static const size_t kBufferSize = 4096;

	int16 buffer[kBufferSize];
	int samples;

	SoundBuffer() : samples(0) {
	}
};

void MainWindow::exportBMUMP3Impl(Common::SeekableReadStream &bmu, Common::WriteStream &mp3) {
	if ((bmu.size() <= 8) ||
		(bmu.readUint32BE() != MKTAG('B', 'M', 'U', ' ')) ||
		(bmu.readUint32BE() != MKTAG('V', '1', '.', '0')))
		throw Common::Exception("Not a valid BMU file");

	mp3.writeStream(bmu);
}

void MainWindow::exportBMUMP3() {
	if (!_currentItem)
		return;

	assert(_currentItem->getFileType() == Aurora::kFileTypeBMU);

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save MP3 file"),
		replaceExtension(_currentItem->getName(), Aurora::kFileTypeMP3),
		tr("MP3 file (*.mp3)"));

	if (fileName.isEmpty())
		return;

	_status.push(constructStatus("Exporting", _currentItem->getName(), fileName));
	BOOST_SCOPE_EXIT((&_status)) {
		_status.pop();
	} BOOST_SCOPE_EXIT_END

	try {
		std::unique_ptr<Common::SeekableReadStream> res(_currentItem->getResourceData());

		Common::WriteFile file(fileName.toStdString());

		exportBMUMP3Impl(*res, file);
		file.flush();

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
		return;
	}
}

uint64 getSoundLength(Sound::AudioStream *sound) {
	Sound::RewindableAudioStream *rewSound = dynamic_cast<Sound::RewindableAudioStream *>(sound);
	if (!rewSound)
		return Sound::RewindableAudioStream::kInvalidLength;

	return rewSound->getLength();
}

void MainWindow::exportWAVImpl(Sound::AudioStream *sound, Common::WriteStream &wav) {
	assert(sound);

	const uint16 channels = sound->getChannels();
	const uint32 rate     = sound->getRate();

	std::deque<SoundBuffer> buffers;

	uint64 length = getSoundLength(sound);
	if (length != Sound::RewindableAudioStream::kInvalidLength)
		buffers.resize((length / (SoundBuffer::kBufferSize / channels)) + 1);

	uint32 samples = 0;
	std::deque<SoundBuffer>::iterator buffer = buffers.begin();
	while (!sound->endOfStream()) {
		if (buffer == buffers.end()) {
			buffers.push_back(SoundBuffer());
			buffer = --buffers.end();
		}

		buffer->samples = sound->readBuffer(buffer->buffer, SoundBuffer::kBufferSize);

		if (buffer->samples > 0)
			samples += buffer->samples;

		++buffer;
	}

	samples /= channels;

	const uint32 dataSize   = samples * channels * 2;
	const uint32 byteRate   = rate * channels * 2;
	const uint16 blockAlign = channels * 2;

	wav.writeUint32BE(MKTAG('R', 'I', 'F', 'F'));
	wav.writeUint32LE(36 + dataSize);
	wav.writeUint32BE(MKTAG('W', 'A', 'V', 'E'));

	wav.writeUint32BE(MKTAG('f', 'm', 't', ' '));
	wav.writeUint32LE(16);
	wav.writeUint16LE(1);
	wav.writeUint16LE(channels);
	wav.writeUint32LE(rate);
	wav.writeUint32LE(byteRate);
	wav.writeUint16LE(blockAlign);
	wav.writeUint16LE(16);

	wav.writeUint32BE(MKTAG('d', 'a', 't', 'a'));
	wav.writeUint32LE(dataSize);

	for (std::deque<SoundBuffer>::const_iterator b = buffers.begin(); b != buffers.end(); ++b)
		for (int i = 0; i < b->samples; i++)
			wav.writeUint16LE(b->buffer[i]);
}

void MainWindow::exportWAV() {
	if (!_currentItem)
		return;

	assert(_currentItem->getResourceType() == Aurora::kResourceSound);

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save PCM WAV file"),
		replaceExtension(_currentItem->getName(), Aurora::kFileTypeWAV),
		tr("WAV file (*.wav)"));

	if (fileName.isEmpty())
		return;

	_status.push(constructStatus("Exporting", _currentItem->getName(), fileName));
	BOOST_SCOPE_EXIT((&_status)) {
		_status.pop();
	} BOOST_SCOPE_EXIT_END

	try {
		std::unique_ptr<Sound::AudioStream> sound(_currentItem->getAudioStream());

		Common::WriteFile file(fileName.toStdString());

		exportWAVImpl(sound.get(), file);
		file.flush();

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
		return;
	}
}

} // End of namespace GUI
