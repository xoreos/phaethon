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
 * MERCHANTABILIT	Y or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phaethon. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Phaethon's main window.
 */

#include "verdigris/wobjectimpl.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include "src/aurora/util.h"
#include "src/common/filepath.h"
#include "src/common/strutil.h"
#include "src/gui/mainwindow.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent, const char *version, const QSize &size, const Common::UString &path) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    resize(size);

    _panelPreviewEmpty = new PanelPreviewEmpty();
    _panelPreviewImage = new PanelPreviewImage();
    _panelPreviewSound = new PanelPreviewSound();
    _panelPreviewText  = new PanelPreviewText();

    ui->resLayout->addWidget(_panelPreviewEmpty);

    // signals/slots
    QObject::connect(ui->bLoadKotorDir,       &QPushButton::clicked, this, &MainWindow::sbLoadKotorDir);
    QObject::connect(ui->bCloseDir,           &QPushButton::clicked, this, &MainWindow::sbCloseDir);
    QObject::connect(ui->bUnused1,            &QPushButton::clicked, this, &MainWindow::sbUnused1);
    QObject::connect(ui->bUnused2,            &QPushButton::clicked, this, &MainWindow::sbUnused2);
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

    ui->bUnused1->setEnabled(false);
    ui->bUnused2->setEnabled(false);

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

void MainWindow::closeResource() {
    showPreviewPanel(_panelPreviewEmpty);
}

void MainWindow::slotCloseDir() {
    closeResource();

    ui->bCloseDir->setEnabled(false);

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

/* Buttons (testing). */
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

void MainWindow::sbUnused1() {
}

void MainWindow::sbUnused2() {
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

    if (_currentSelection->getSource() == Source::kSourceDirectory) {

        labelSize     += "-";
        labelFileType += "Directory";
        labelResType  += "Directory";

    } else if ((_currentSelection->getSource() == Source::kSourceFile) ||
               (_currentSelection->getSource() == Source::kSourceArchiveFile)) {

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

    _panelPreviewText->setItem(_currentSelection);
    _panelPreviewSound->setItem(_currentSelection);
}

} // End of namespace GUI
