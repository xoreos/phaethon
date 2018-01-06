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

MainWindow::MainWindow(QWidget *parent, const char *version, const QSize &size, const Common::UString &path) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

//    resize(size);

    _panelPreviewEmpty = new PanelPreviewEmpty();
    _panelPreviewImage = new PanelPreviewImage();
    _panelPreviewSound = new PanelPreviewSound();
    _panelPreviewText  = new PanelPreviewText();

    ui->resLayout->addWidget(_panelPreviewEmpty);

    // signals/slots
    QObject::connect(ui->bLoadKotorDir,       &QPushButton::clicked, this, &MainWindow::sbLoadKotorDir);
    QObject::connect(ui->bCloseDir,           &QPushButton::clicked, this, &MainWindow::sbCloseDir);
    QObject::connect(ui->bSave,               &QPushButton::clicked, this, &MainWindow::sbSave);
    QObject::connect(ui->bExportTGA,          &QPushButton::clicked, this, &MainWindow::sbExportTGA);
    QObject::connect(ui->actionOpenDirectory, &QAction::triggered,   this, &MainWindow::slotOpenDir);
    QObject::connect(ui->actionClose,         &QAction::triggered,   this, &MainWindow::slotCloseDir);
    QObject::connect(ui->actionQuit,          &QAction::triggered,   this, &MainWindow::slotQuit);

    // status bar
    _statusLabel = new QLabel(this);
    _statusLabel->setText("None");
    _statusLabel->setAlignment(Qt::AlignLeft);
    ui->statusBar->addWidget(_statusLabel, 1);

    // tree view
    ui->treeView->setHeaderHidden(true);

    // resource info panel
    ui->resLabelName->setText("Resource name:");
    ui->resLabelSize->setText("Size:");
    ui->resLabelFileType->setText("File type:");
    ui->resLabelResType->setText("Resource type:");

    ui->bSave->setEnabled(false);
    ui->bExportTGA->setEnabled(false);

    // open the path given in command line if any
    if (path.empty()) {
        // nothing is open yet
        ui->actionClose->setEnabled(false);
        ui->bCloseDir->setEnabled(false);
    }
    else {
        _rootPath = path.toQString();
        setTreeViewModel(_rootPath);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setTreeViewModel(const QString &path) {
    _rootPath = path;

    _statusLabel->setText("Loading...");
    _statusLabel->repaint();

    if (_treeModel) {
        ui->treeView->setModel(nullptr);
        delete _treeModel;
    }

    _treeModel = new ResourceTree(path, ui->treeView);
    ui->treeView->setModel(_treeModel);
    ui->treeView->show();

    _statusLabel->setText(tr("Root: %1").arg(path));


    QObject::connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &MainWindow::selection);


    ui->log->append(tr("Set root: %1").arg(path));

    ui->bCloseDir->setEnabled(true);
    ui->actionClose->setEnabled(true);
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

    ui->bCloseDir->setEnabled(false);
    ui->bSave->setEnabled(false);
    ui->bExportTGA->setEnabled(false);

    ui->treeView->setModel(nullptr);

    ui->log->append(tr("Closed directory: %1").arg(_rootPath));

    _rootPath = "";

    if (_currentSelection)
        _currentSelection = nullptr;

    ui->resLabelName->setText("Resource name:");
    ui->resLabelSize->setText("Size:");
    ui->resLabelFileType->setText("File type:");
    ui->resLabelResType->setText("Resource type:");

    _statusLabel->setText("None");

    ui->actionClose->setEnabled(false);
}

void MainWindow::slotQuit() {
    QCoreApplication::quit();
}


/*                        */
/** Resource info panel. **/
/*                        */

void MainWindow::sbLoadKotorDir() {
    QString myKotorPath("/home/mike/kotor");
    QDir dir(myKotorPath);
    if (dir.exists())
        setTreeViewModel(myKotorPath);
    else
        ui->log->append("Failed: /home/mike/kotor is doesn't exist.");
}

void MainWindow::sbCloseDir() {
    slotCloseDir();
}

void MainWindow::sbSave() {
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

void MainWindow::sbExportTGA() {
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

QString getSizeLabel(size_t size) {
    if (size == Common::kFileInvalid)
        return "-";

    if (size < 1024)
        return QString("%1").arg(size);

    QString humanRead = Common::FilePath::getHumanReadableSize(size).toQString();

    return QString("%1 (%2)").arg(humanRead).arg(size);
}

QString getFileTypeLabel(Aurora::FileType type) {
    QString label = QString("%1").arg(type);

    if (type != Aurora::kFileTypeNone) {
        QString temp = TypeMan.getExtension(type).toQString();
        label += QString(" (%1)").arg(temp);
    }

    return label;
}

QString getResTypeLabel(Aurora::ResourceType type) {
    QString label = QString("%1").arg(type);

    if (type != Aurora::kResourceNone) {
        QString temp = getResourceTypeDescription(type).toQString();
        label += QString(" (%1)").arg(temp);
    }

    return label;
}

void MainWindow::setLabels() {
    QString labelName     = "Resource name: ";
    QString labelSize     = "Size: ";
    QString labelFileType = "File type: ";
    QString labelResType  = "Resource type: ";

    labelName += _currentSelection->getName();

    if (_currentSelection->getSource() == ResourceTreeItem::Source::kSourceDirectory) {

        labelSize     += "-";
        labelFileType += "Directory";
        labelResType  += "Directory";

    } else if ((_currentSelection->getSource() == ResourceTreeItem::Source::kSourceFile) ||
               (_currentSelection->getSource() == ResourceTreeItem::Source::kSourceArchiveFile)) {

        Aurora::FileType     fileType = _currentSelection->getFileType();
        Aurora::ResourceType resType  = _currentSelection->getResourceType();

        labelSize     += getSizeLabel(_currentSelection->getSize());
        labelFileType += getFileTypeLabel(fileType);
        labelResType  += getResTypeLabel(resType);
    }

    ui->resLabelName->setText(labelName);
    ui->resLabelSize->setText(labelSize);
    ui->resLabelFileType->setText(labelFileType);
    ui->resLabelResType->setText(labelResType);
}

void MainWindow::showPreviewPanel(QFrame *panel) {
    if (ui->resLayout->count()) {
        QFrame *old = static_cast<QFrame*>(ui->resLayout->itemAt(0)->widget());
        if (old != panel) {
            ui->resLayout->removeWidget(old);
            old->setParent(0);
            if (old == _panelPreviewSound)
                _panelPreviewSound->stop();
            ui->resLayout->addWidget(panel);
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
    setLabels();
    showPreviewPanel();
    showExportButtons();

    _panelPreviewImage->setItem(_currentSelection);
    _panelPreviewText->setItem(_currentSelection);
    _panelPreviewSound->setItem(_currentSelection);
}

void MainWindow::showExportButtons() {
    if (!_currentSelection || _currentSelection->getSource() == ResourceTreeItem::Source::kSourceDirectory) {
        showExportButtons(false, false, false, false);
        return;
    }

    bool isBMU   = _currentSelection->getFileType()     == Aurora::kFileTypeBMU;
    bool isSound = _currentSelection->getResourceType() == Aurora::kResourceSound;
    bool isImage = _currentSelection->getResourceType() == Aurora::kResourceImage;

    showExportButtons(true, isBMU, isSound, isImage);
}

void MainWindow::showExportButtons(bool enableRaw, bool showMP3, bool showWAV, bool showTGA) {
    ui->bSave->setEnabled(enableRaw);
    ui->bExportTGA->setEnabled(showTGA);
//    _buttonExportBMUMP3->Show(showMP3);
//    _buttonExportWAV->Show(showWAV);
}

} // End of namespace GUI
