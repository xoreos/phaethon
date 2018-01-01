#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <boost/scope_exit.hpp>

#include "verdigris/wobjectimpl.h"

#include "src/cline.h"
#include "src/aurora/util.h"
#include "src/common/filepath.h"
#include "src/common/strutil.h"
#include "src/common/writefile.h"
#include "src/gui/mainwindow.h"
#include "src/images/dumptga.h"
#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent, const char *version, const QSize &size, const Common::UString &path)
    : QMainWindow(parent)
{
    _ui.setupUi(this);

//    resize(size);

    // preview
    _panelPreviewEmpty = new PanelPreviewEmpty();
    _panelPreviewImage = new PanelPreviewImage();
    _panelPreviewSound = new PanelPreviewSound();
    _panelPreviewText  = new PanelPreviewText();
    _resInfo = new ResourceInfoPanel();

    _ui.resBox->addWidget(_resInfo);
    _ui.resLayout->addWidget(_panelPreviewEmpty);

    // signals/slots
    QObject::connect(_ui.actionOpenDirectory, &QAction::triggered, this, &MainWindow::slotOpenDir);
    QObject::connect(_ui.actionOpenFile, &QAction::triggered, this, &MainWindow::slotOpenFile);
    QObject::connect(_ui.actionClose, &QAction::triggered, this, &MainWindow::slotCloseDir);
    QObject::connect(_ui.actionQuit, &QAction::triggered, this, &MainWindow::slotQuit);
    QObject::connect(_resInfo, &ResourceInfoPanel::loadModel, this, &MainWindow::setTreeViewModel);
    QObject::connect(_resInfo, &ResourceInfoPanel::logAppend, this, &MainWindow::slotLogAppend);
    QObject::connect(_resInfo, &ResourceInfoPanel::closeDirClicked, this, &MainWindow::slotCloseDir);
    QObject::connect(_resInfo, &ResourceInfoPanel::saveClicked, this, &MainWindow::saveItem);
    QObject::connect(_resInfo, &ResourceInfoPanel::exportTGAClicked, this, &MainWindow::exportTGA);
    QObject::connect(_resInfo, &ResourceInfoPanel::exportBMUMP3Clicked, this, &MainWindow::exportBMUMP3);
    QObject::connect(_resInfo, &ResourceInfoPanel::exportWAVClicked, this, &MainWindow::exportWAV);
    QObject::connect(_ui.actionAbout, &QAction::triggered, this, &MainWindow::slotAbout);

    _ui.actionAbout->setShortcut(QKeySequence(tr("F1")));

    // status bar
    _status = std::make_shared<StatusBar>(this->statusBar());
    _status->setText("Idle...");

    // open the path given in command line if any
    if (path.empty()) {
        // nothing is open yet
        _ui.actionClose->setEnabled(false);
        _resInfo->getUi().bCloseDir->setEnabled(false);
    }
    else {
        _rootPath = path.toQString();
        setTreeViewModel(_rootPath);
    }
}

MainWindow::~MainWindow() {
}

void MainWindow::slotLogAppend(const QString &text) {
    _ui.log->append(text);
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
    _ui.treeView->setModel(nullptr);

    _treeModel = std::make_unique<ResourceTree>(this, path, _ui.treeView);
    _ui.treeView->setModel(_treeModel.get());
    _ui.treeView->expandToDepth(0);
    _ui.treeView->show();

    _ui.treeView->resizeColumnToContents(0);


    QObject::connect(_ui.treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::selection);


    _ui.log->append(tr("Set root: %1").arg(path));

    _resInfo->getUi().bCloseDir->setEnabled(true);
    _ui.actionClose->setEnabled(true);
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

    _resInfo->setButtonsForClosedDir();

    _ui.treeView->setModel(nullptr);

    _ui.log->append(tr("Closed directory: %1").arg(_rootPath));

    _rootPath = "";

    _currentItem = nullptr;

    _resInfo->clearLabels();

    _ui.actionClose->setEnabled(false);
}

void MainWindow::slotQuit() {
    QCoreApplication::quit();
}

void MainWindow::showPreviewPanel(QFrame *panel) {
    if (_ui.resLayout->count()) {
        QFrame *old = static_cast<QFrame*>(_ui.resLayout->itemAt(0)->widget());
        if (old != panel) {
            _ui.resLayout->removeWidget(old);
            old->setParent(0);
            if (old == _panelPreviewSound)
                _panelPreviewSound->stop();
            _ui.resLayout->addWidget(panel);
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
            }
            break;
        }
    }
}

void MainWindow::selection(const QItemSelection &selected, const QItemSelection &deselected) {
    const QModelIndex index = selected.indexes().at(0);
    _currentItem = _treeModel->getNode(index);
    _resInfo->update(_currentItem);
    showPreviewPanel();

    _panelPreviewImage->setItem(_currentItem);
    _panelPreviewText->setItem(_currentItem);
    _panelPreviewSound->setItem(_currentItem);
}

QString constructStatus(const QString &action, const QString &name, const QString &destination) {
    return action + " \"" + name + "\" to \"" + destination + "\"...";
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
    QMessageBox::about(this, "About", msg);
}

std::shared_ptr<StatusBar> MainWindow::status() {
    return _status;
}

} // End of namespace GUI
