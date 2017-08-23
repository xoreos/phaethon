#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <boost/scope_exit.hpp>

#include "verdigris/wobjectimpl.h"

#include "src/cline.h"
#include "src/aurora/util.h"
#include "src/common/filepath.h"
#include "src/common/strutil.h"
#include "src/common/system.h"
#include "src/common/writefile.h"
#include "src/gui/mainwindow.h"
#include "src/gui/panelpreviewempty.h"
#include "src/gui/panelpreviewimage.h"
#include "src/gui/panelpreviewsound.h"
#include "src/gui/panelpreviewtext.h"
#include "src/gui/panelresourceinfo.h"
#include "src/gui/resourcetree.h"
#include "src/gui/resourcetreeitem.h"
#include "src/images/dumptga.h"
#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent, const char *title, const QSize &size, const Common::UString &path)
    : QMainWindow(parent) {
    _centralWidget = new QWidget(this);
    _centralLayout = new QGridLayout(_centralWidget);

    _layoutVertical = new QVBoxLayout();

    _splitterTopBottom = new QSplitter(_centralWidget);
    _splitterLeftRight = new QSplitter(_splitterTopBottom);

    _actionOpenDirectory = new QAction(this);
    _actionClose = new QAction(this);
    _actionQuit = new QAction(this);
    _actionAbout = new QAction(this);
    _actionOpenFile = new QAction(this);

    _menuBar = new QMenuBar(this);
    _menuFile = new QMenu(_menuBar);
    _menuHelp = new QMenu(_menuBar);

    _panelPreviewEmpty = new PanelPreviewEmpty();
    _panelPreviewImage = new PanelPreviewImage();
    _panelPreviewSound = new PanelPreviewSound();
    _panelPreviewText  = new PanelPreviewText();
    _panelResourceInfo = new PanelResourceInfo();

    _treeView = new QTreeView(_splitterLeftRight);
    // 1:8 ratio, 8 being the (res info + preview) wrapper widget
    {
        QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sp.setHorizontalStretch(1);
        _treeView->setSizePolicy(sp);
    }

    QGroupBox *logBox = new QGroupBox(_splitterTopBottom);
    logBox->setTitle(tr("Log"));
    _log = new QTextEdit(logBox);
    {
        QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sp.setVerticalStretch(1);
        logBox->setSizePolicy(sp);

        QHBoxLayout *hl = new QHBoxLayout(logBox);
        hl->addWidget(_log);
        hl->setContentsMargins(0, 0, 0, 0);
    }

    _centralLayout->addWidget(_splitterTopBottom);

    // left/right splitter
    // 8:1 ratio, 1 being the log box
    {
        QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sp.setVerticalStretch(5);
        _splitterLeftRight->setSizePolicy(sp);
    }

    _splitterTopBottom->setOrientation(Qt::Vertical);

    /*
         _______
        |       |
        |_______|
        |_______|
    */

    _splitterTopBottom->addWidget(_splitterLeftRight);
    _splitterTopBottom->addWidget(logBox);

    /*
         _______
        | |     |
        |_|_____|
        |_______|
    */

    // can't add a layout directly to a splitter
    QWidget *wrapperWidget = new QWidget(_splitterTopBottom);
    wrapperWidget->setContentsMargins(0, 0, 0, 0);
    {
        QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sp.setHorizontalStretch(6);
        wrapperWidget->setSizePolicy(sp);
    }

    _splitterLeftRight->addWidget(_treeView);
    _splitterLeftRight->addWidget(wrapperWidget);

    wrapperWidget->setLayout(_layoutVertical);
    _layoutVertical->setParent(wrapperWidget);
    _layoutVertical->setMargin(0);

    QFrame *resInfoFrame = new QFrame(wrapperWidget);
    {
        QHBoxLayout *hl = new QHBoxLayout(resInfoFrame);
        hl->addWidget(_panelResourceInfo);
    }
    resInfoFrame->setFrameShape(QFrame::StyledPanel);
    resInfoFrame->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    resInfoFrame->setFixedHeight(140);

    _resPreviewFrame = new QFrame(wrapperWidget);
    {
        QHBoxLayout *hl = new QHBoxLayout(_resPreviewFrame);
        hl->setMargin(0);
        hl->addWidget(_panelPreviewEmpty);
    }
    _resPreviewFrame->setFrameShape(QFrame::StyledPanel);

    _layoutVertical->addWidget(resInfoFrame);
    _layoutVertical->addWidget(_resPreviewFrame);

    /*
         _______
        | |-----|
        |_|_____|
        |_______|
    */

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

    this->setMenuBar(_menuBar);

    _actionOpenDirectory->setText(tr("&Open directory"));
    _actionClose->setText(tr("&Close"));
    _actionQuit->setText(tr("MainWindow"));
    _actionAbout->setText(tr("About"));
    _actionOpenFile->setText(tr("&Open file"));

    this->setCentralWidget(_centralWidget);

    setWindowTitle(title);
    resize(size);

    // signals/slots
    QObject::connect(_actionOpenDirectory, &QAction::triggered, this, &MainWindow::slotOpenDir);
    QObject::connect(_actionOpenFile, &QAction::triggered, this, &MainWindow::slotOpenFile);
    QObject::connect(_actionClose, &QAction::triggered, this, &MainWindow::slotCloseDir);
    QObject::connect(_actionQuit, &QAction::triggered, this, &MainWindow::slotQuit);
    QObject::connect(_panelResourceInfo, &PanelResourceInfo::loadModel, this, &MainWindow::setTreeViewModel);
    QObject::connect(_panelResourceInfo, &PanelResourceInfo::log, this, &MainWindow::slotLog);
    QObject::connect(_panelResourceInfo, &PanelResourceInfo::closeDirClicked, this, &MainWindow::slotCloseDir);
    QObject::connect(_panelResourceInfo, &PanelResourceInfo::saveClicked, this, &MainWindow::saveItem);
    QObject::connect(_panelResourceInfo, &PanelResourceInfo::exportTGAClicked, this, &MainWindow::exportTGA);
    QObject::connect(_panelResourceInfo, &PanelResourceInfo::exportBMUMP3Clicked, this, &MainWindow::exportBMUMP3);
    QObject::connect(_panelResourceInfo, &PanelResourceInfo::exportWAVClicked, this, &MainWindow::exportWAV);
    QObject::connect(_actionAbout, &QAction::triggered, this, &MainWindow::slotAbout);
    QObject::connect(_panelPreviewText, &PanelPreviewText::log, this, &MainWindow::slotLog);

    _actionAbout->setShortcut(QKeySequence(tr("F1")));

    // status bar
    _status = std::make_shared<StatusBar>(this->statusBar());
    _status->setText("Idle...");

    // open the path given in command line if any
    if (path.empty()) {
        // nothing is open yet
        _actionClose->setEnabled(false);
    }
    else {
        _rootPath = path.toQString();
        setTreeViewModel(_rootPath);
    }
}

MainWindow::~MainWindow() {
    if (_panelPreviewEmpty)
        delete _panelPreviewEmpty;

    if (_panelPreviewText)
        delete _panelPreviewText;

    if (_panelPreviewSound)
        delete _panelPreviewSound;

    if (_panelPreviewImage)
        delete _panelPreviewImage;
}

void MainWindow::slotLog(const QString &text) {
    _log->append(text);
}

void MainWindow::setTreeViewModel(const QString &path) {
    if (_rootPath == path)
        return;
    _rootPath = path;

    _status->push("Populating resource tree...");
    BOOST_SCOPE_EXIT((&_status)) {
        _status->pop();
    } BOOST_SCOPE_EXIT_END

    _treeModel.reset();
    _treeView->setModel(nullptr);

    _treeModel = std::make_unique<ResourceTree>(this, path, _treeView);
    _treeView->setModel(_treeModel.get());
    _treeView->expandToDepth(0);
    _treeView->show();

    _treeView->resizeColumnToContents(0);


    QObject::connect(_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::resourceSelect);


    _log->append(tr("Set root: %1").arg(path));

    _actionClose->setEnabled(true);
}

void MainWindow::slotOpenDir() {
    QString dir = QFileDialog::getExistingDirectory(this,
        tr("Open directory"), QString(QStandardPaths::HomeLocation), QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
        setTreeViewModel(dir);
}

void MainWindow::slotOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Aurora game resource file"), QString(QStandardPaths::HomeLocation), tr("Aurora game resource (*.*)"));
    if (!fileName.isEmpty())
        setTreeViewModel(fileName);
}

void MainWindow::slotCloseDir() {
    showPreviewPanel(_panelPreviewEmpty);

    _panelResourceInfo->setButtonsForClosedDir();

    _treeView->setModel(nullptr);

    _log->append(tr("Closed directory: %1").arg(_rootPath));

    _rootPath = "";

    _currentItem = nullptr;

    _panelResourceInfo->clearLabels();

    _actionClose->setEnabled(false);
}

void MainWindow::slotQuit() {
    QCoreApplication::quit();
}

void MainWindow::showPreviewPanel(QFrame *panel) {
    if (_resPreviewFrame->layout()->count()) {
        QFrame *old = static_cast<QFrame*>(_resPreviewFrame->layout()->itemAt(0)->widget());
        if (old != panel) {
            _resPreviewFrame->layout()->removeWidget(old);
            old->setParent(0);
            if (old == _panelPreviewSound)
                _panelPreviewSound->stop();
            _resPreviewFrame->layout()->addWidget(panel);
        }
    }
}

void MainWindow::showPreviewPanel() {
    switch (_currentItem->getResourceType()) {
        case Aurora::kResourceImage:
            showPreviewPanel(_panelPreviewImage);
            break;

        case Aurora::kResourceSound:
            showPreviewPanel(_panelPreviewSound);
            break;

        default:
        {
            switch (_currentItem->getFileType()) {
                case Aurora::FileType::kFileTypeICO:
                    showPreviewPanel(_panelPreviewImage);
                    break;

                case Aurora::FileType::kFileTypeINI:
                case Aurora::FileType::kFileTypeTXT:
                    showPreviewPanel(_panelPreviewText);
                    break;

                default:
                    showPreviewPanel(_panelPreviewEmpty);
                    break;
            }
        }
    }
}

void MainWindow::resourceSelect(const QItemSelection &selected, const QItemSelection &UNUSED(deselected)) {
    const QModelIndex index = selected.indexes().at(0);
    _currentItem = _treeModel->getItem(index);
    _panelResourceInfo->update(_currentItem);
    showPreviewPanel();

    _panelPreviewImage->setItem(_currentItem);
    _panelPreviewText->setItem(_currentItem);
    _panelPreviewSound->setItem(_currentItem);
}

QString constructStatus(const QString &_action, const QString &name, const QString &destination) {
    return _action + " \"" + name + "\" to \"" + destination + "\"...";
}

void MainWindow::saveItem() {
    if (!_currentItem)
        return;

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Aurora game resource file"), "",
            tr("Aurora game resource (*.*)|*.*"));

    if (fileName.isEmpty())
        return;

    _status->push(constructStatus("Saving", _currentItem->getName(), fileName));
    BOOST_SCOPE_EXIT((&_status)) {
        _status->pop();
    } BOOST_SCOPE_EXIT_END

    try {
        QScopedPointer<Common::SeekableReadStream> res(_currentItem->getResourceData());

        Common::WriteFile file(Common::UString(fileName.toStdString().c_str()));

        file.writeStream(*res);
        file.flush();

    } catch (Common::Exception &e) {
        Common::printException(e, "WARNING: ");
    }
}

void MainWindow::exportTGA() {
    if (!_currentItem)
        return;

    assert(_currentItem->getResourceType() == Aurora::kResourceImage);

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save TGA file"), "",
            tr("TGA file (*.tga)|*.tga"));

    if (fileName.isEmpty())
        return;

    _status->push(constructStatus("Exporting", _currentItem->getName(), fileName));
    BOOST_SCOPE_EXIT((&_status)) {
        _status->pop();
    } BOOST_SCOPE_EXIT_END

    try {
        QScopedPointer<Images::Decoder> image(_currentItem->getImage());

        image->dumpTGA(Common::UString(fileName.toStdString().c_str()));

    } catch (Common::Exception &e) {
        Common::printException(e, "WARNING: ");
    }
}

/* EXPORT MP3 : UNTESTED! */
void MainWindow::exportBMUMP3Impl(Common::SeekableReadStream &bmu, Common::WriteStream &mp3) {
    if ((bmu.size() <= 8) ||
        (bmu.readUint32BE() != MKTAG('B', 'M', 'U', ' ')) ||
        (bmu.readUint32BE() != MKTAG('V', '1', '.', '0')))
        throw Common::Exception("Not a valid BMU file");

    mp3.writeStream(bmu);
}

#define USTR(x) (Common::UString((x).toStdString()))

void MainWindow::exportBMUMP3() {
    if (!_currentItem)
        return;

    assert(_currentItem->getFileType() == Aurora::kFileTypeBMU);

    const QString title = "Save MP3 file";
    const QString mask  = "MP3 file (*.mp3)|*.mp3";
    const QString def   = TypeMan.setFileType(USTR(_currentItem->getName()), Aurora::kFileTypeMP3).toQString();

    QString fileName = QFileDialog::getSaveFileName(this,
            title, def,
            mask);

    if (fileName.isEmpty())
        return;

    _status->push(constructStatus("Exporting", _currentItem->getName(), fileName));
    BOOST_SCOPE_EXIT((&_status)) {
        _status->pop();
    } BOOST_SCOPE_EXIT_END

    try {
        Common::ScopedPtr<Common::SeekableReadStream> res(_currentItem->getResourceData());

        Common::WriteFile file(USTR(fileName));

        exportBMUMP3Impl(*res, file);
        file.flush();

    } catch (Common::Exception &e) {
        Common::printException(e, "WARNING: ");
        return;
    }

//    return true;
}

uint64 getLength(Sound::AudioStream *sound) {
    Sound::RewindableAudioStream *rewSound = dynamic_cast<Sound::RewindableAudioStream *>(sound);
    if (!rewSound)
        return Sound::RewindableAudioStream::kInvalidLength;

    return rewSound->getLength();
}

struct SoundBuffer {
    static const size_t kBufferSize = 4096;

    int16 buffer[kBufferSize];
    int samples;

    SoundBuffer() : samples(0) {
    }
};

void MainWindow::exportWAVImpl(Sound::AudioStream *sound, Common::WriteStream &wav) {
    assert(sound);

    const uint16 channels = sound->getChannels();
    const uint32 rate     = sound->getRate();

    std::deque<SoundBuffer> buffers;

    uint64 length = getLength(sound);
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

    const QString title = "Save PCM WAV file";
    const QString mask  = "WAV file (*.wav)|*.wav";
    const QString def   = TypeMan.setFileType(USTR(_currentItem->getName()), Aurora::kFileTypeWAV).toQString();

    QString fileName = QFileDialog::getSaveFileName(this,
            title, def,
            mask);

    if (fileName.isEmpty())
        return;

    _status->push(constructStatus("Exporting", _currentItem->getName(), fileName));
    BOOST_SCOPE_EXIT((&_status)) {
        _status->pop();
    } BOOST_SCOPE_EXIT_END

    try {
        Common::ScopedPtr<Sound::AudioStream> sound(_currentItem->getAudioStream());

        Common::WriteFile file(USTR(fileName));

        exportWAVImpl(sound.get(), file);
        file.flush();

    } catch (Common::Exception &e) {
        Common::printException(e, "WARNING: ");
        return;
    }

//    return true;
}

void MainWindow::slotAbout() {
    QString msg = createVersionText().toQString();
    QMessageBox::about(this, "About Phaethon", msg);
}

std::shared_ptr<StatusBar> MainWindow::status() {
    return _status;
}

} // End of namespace GUI
