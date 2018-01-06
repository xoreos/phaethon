#include "verdigris/wobjectimpl.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include "src/aurora/util.h"
#include "src/common/filepath.h"
#include "src/common/strutil.h"
#include "src/common/writefile.h"
#include "src/gui/mainwindow.h"
#include "src/images/dumptga.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent, const char *version, const QSize &size, const Common::UString &path)
    : QMainWindow(parent)
    , _treeModel(nullptr)
{
    _ui.setupUi(this);

//    resize(size);

    _panelPreviewEmpty = new PanelPreviewEmpty();
    _panelPreviewImage = new PanelPreviewImage();
    _panelPreviewSound = new PanelPreviewSound();
    _panelPreviewText  = new PanelPreviewText();
    _resInfo = new ResourceInfoPanel();

    _ui.resBox->addWidget(_resInfo);
    _ui.resLayout->addWidget(_panelPreviewEmpty);

    // signals/slots
    QObject::connect(_ui.actionOpenDirectory, &QAction::triggered, this, &MainWindow::slotOpenDir);
    QObject::connect(_ui.actionClose, &QAction::triggered, this, &MainWindow::slotCloseDir);
    QObject::connect(_ui.actionQuit, &QAction::triggered, this, &MainWindow::slotQuit);
    QObject::connect(_resInfo, &ResourceInfoPanel::loadModel, this, &MainWindow::setTreeViewModel);
    QObject::connect(_resInfo, &ResourceInfoPanel::logAppend, this, &MainWindow::slotLogAppend);
    QObject::connect(_resInfo, &ResourceInfoPanel::closeDirClicked, this, &MainWindow::slotCloseDir);
    QObject::connect(_resInfo, &ResourceInfoPanel::saveClicked, this, &MainWindow::saveItem);
    QObject::connect(_resInfo, &ResourceInfoPanel::exportTGAClicked, this, &MainWindow::exportTGA);

    // status bar
    _statusLabel = new QLabel(this);
    _statusLabel->setText("None");
    _statusLabel->setAlignment(Qt::AlignLeft);
    _ui.statusBar->addWidget(_statusLabel, 1);

    // tree view
    _ui.treeView->setHeaderHidden(true);

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
    delete _treeModel;
}

void MainWindow::slotLogAppend(const QString& text) {
    _ui.log->append(text);
}

void MainWindow::setTreeViewModel(const QString &path) {
    if (_rootPath == path)
        return;

    _rootPath = path;

    _statusLabel->setText("Loading...");
    _statusLabel->repaint();

    if (_treeModel) {
        delete _treeModel;
        _ui.treeView->setModel(nullptr);
    }

    _treeModel = new ResourceTree(path, _ui.treeView);
    _ui.treeView->setModel(_treeModel);
    _ui.treeView->show();

    _statusLabel->setText(tr("Root: %1").arg(path));


    QObject::connect(_ui.treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::selection);


    _ui.log->append(tr("Set root: %1").arg(path));

    _resInfo->getUi().bCloseDir->setEnabled(true);
    _ui.actionClose->setEnabled(true);
}

void MainWindow::slotOpenDir() {
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open directory"),
                                                    QString(QStandardPaths::HomeLocation),
                                                    QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
        setTreeViewModel(dir);
}

void MainWindow::slotCloseDir() {
    showPreviewPanel(_panelPreviewEmpty);

    _resInfo->setButtonsForClosedDir();

    _ui.treeView->setModel(nullptr);

    _ui.log->append(tr("Closed directory: %1").arg(_rootPath));

    _rootPath = "";

    if (_currentSelection)
        _currentSelection = nullptr;

    _resInfo->clearLabels();

    _statusLabel->setText("None");

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
    switch (_currentSelection->getResourceType()) {
        case Aurora::kResourceImage:
            showPreviewPanel(_panelPreviewImage);
            break;

        case Aurora::kResourceSound:
            showPreviewPanel(_panelPreviewSound);
            break;

        default:
        {
            switch (_currentSelection->getFileType()) {
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
    _currentSelection = _treeModel->getNode(index);
    _resInfo->update(_currentSelection);
    showPreviewPanel();

    _panelPreviewImage->setItem(_currentSelection);
    _panelPreviewText->setItem(_currentSelection);
    _panelPreviewSound->setItem(_currentSelection);
}

void MainWindow::saveItem() {
    if (!_currentSelection)
        return;

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Aurora game resource file"), "",
            tr("Aurora game resource (*.*)|*.*"));

    if (fileName.isEmpty())
        return;

    try {
        QScopedPointer<Common::SeekableReadStream> res(_currentSelection->getResourceData());

        Common::WriteFile file(Common::UString(fileName.toStdString().c_str()));

        file.writeStream(*res);
        file.flush();

    } catch (Common::Exception &e) {
        Common::printException(e, "WARNING: ");
    }
}

void MainWindow::exportTGA() {
    if (!_currentSelection)
        return;

    assert(_currentSelection->getResourceType() == Aurora::kResourceImage);

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save TGA file"), "",
            tr("TGA file (*.tga)|*.tga"));

    if (fileName.isEmpty())
        return;

    try {
        QScopedPointer<Images::Decoder> image(_currentSelection->getImage());

        image->dumpTGA(Common::UString(fileName.toStdString().c_str()));

    } catch (Common::Exception &e) {
        Common::printException(e, "WARNING: ");
    }
}

} // End of namespace GUI
