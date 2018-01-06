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

#include <QFileDialog>
#include <QStandardPaths>

#include "verdigris/wobjectimpl.h"

#include "mainwindow.h"
#include "ui/ui_mainwindow.h"

namespace GUI {

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    statusLabel = new QLabel(this);
    statusLabel->setText("None");
    statusLabel->setAlignment(Qt::AlignLeft);
    statusProgressBar = new QProgressBar(this);
    statusProgressBar->setVisible(false);

    ui->statusBar->addWidget(statusLabel, 2);
    ui->statusBar->addWidget(statusProgressBar, 1);

    ui->treeView->setHeaderHidden(true);

    // remove close button from Files tab
    ui->filesTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0,0);

    ui->filesTabWidget->removeTab(1);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setTreeViewModel(const QString &path) {
    QString cPath;

    cPath = QDir(path).canonicalPath();
    fsModel.setRootPath(cPath);

    ui->treeView->setModel(&fsModel);

    for (int i = 1; i < fsModel.columnCount(); i++)
        ui->treeView->hideColumn(i);

    if (!path.isEmpty()) {
        const QModelIndex rootIndex = fsModel.index(QDir::cleanPath(cPath));
        if (rootIndex.isValid())
            ui->treeView->setRootIndex(rootIndex);
    }

    statusLabel->setText(cPath);
    ui->treeView->show();
}

void MainWindow::on_actionOpen_directory_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open directory"),
                                                    QString(QStandardPaths::HomeLocation),
                                                    QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
        emit open(dir);
}

void MainWindow::on_actionOpen_file_triggered() {
}

void MainWindow::on_actionClose_triggered() {
    ui->treeView->setModel(nullptr);
    statusLabel->setText("None");
}

void MainWindow::on_actionQuit_triggered() {
    QCoreApplication::quit();
}

// testing
void MainWindow::on_pushButton_clicked() {
    emit open("/home/mike/kotor");
}

void MainWindow::on_pushButton_2_clicked() {
    on_actionClose_triggered();
}

void MainWindow::on_pushButton_3_clicked() {
    ui->filesTabWidget->insertTab(1, ui->ftwTab_2, "Archive");
}

// can only be 1 == ftwTab_2 (Archive tab)
void MainWindow::on_filesTabWidget_tabCloseRequested(int index) {
    ui->filesTabWidget->removeTab(index);
}

} // End of namespace GUI
