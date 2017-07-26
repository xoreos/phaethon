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
#include <QDebug>

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

    connect(this, &MainWindow::open, this, &MainWindow::setTreeViewModel);
    connect(ui->actionOpen_directory, &QAction::triggered, this, &MainWindow::on_actionOpen_directory_triggered);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::on_actionClose_triggered);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::on_actionQuit_triggered);
    connect(ui->pushButton_1, &QPushButton::clicked, this, &MainWindow::on_pushButton_1_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_pushButton_2_clicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::on_pushButton_3_clicked);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::on_pushButton_4_clicked);

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

    resLabels << ui->resLabel1;
    resLabels << ui->resLabel2;
    resLabels << ui->resLabel3;
    resLabels << ui->resLabel4;
    resLabels[0]->setText("Resource name: -");
    resLabels[1]->setText("Size: -");
    resLabels[2]->setText("File type: -");
    resLabels[3]->setText("Resource type: -");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setTreeViewModel(const QString &path) {
    QString cPath;

    cPath = QDir(path).canonicalPath();
    fsModel.setRootPath(cPath);

    ui->treeView->setModel(&fsModel);

    connect(ui->treeView->selectionModel(),
    		&QItemSelectionModel::selectionChanged,
    		this,
    		&MainWindow::selection);

    for (int i = 1; i < fsModel.columnCount(); i++)
        ui->treeView->hideColumn(i);

    if (!path.isEmpty()) {
        const QModelIndex rootIndex = fsModel.index(QDir::cleanPath(cPath));
        if (rootIndex.isValid())
            ui->treeView->setRootIndex(rootIndex);
    }

    statusLabel->setText(tr("Root: %1").arg(cPath));
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

void MainWindow::on_actionClose_triggered() {
    ui->treeView->setModel(nullptr);

	resLabels[0]->setText("Resource name: -");
	resLabels[1]->setText("Size: -");
	resLabels[2]->setText("File type: -");
	resLabels[3]->setText("Resource type: -");

    statusLabel->setText("None");
}

void MainWindow::on_actionQuit_triggered() {
    QCoreApplication::quit();
}

// testing
void MainWindow::on_pushButton_1_clicked() {
	QString myKotorPath("/home/mike/kotor");
	QDir dir(myKotorPath);
	if (dir.exists())
		emit open(myKotorPath);
}

void MainWindow::on_pushButton_2_clicked() {
    on_actionClose_triggered();
}

void MainWindow::on_pushButton_3_clicked() {
}

void MainWindow::on_pushButton_4_clicked() {
}

// ty stackoverflow
QString size_human(qint64 size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

void MainWindow::selection(const QItemSelection &selected) {
    QModelIndex index = selected.indexes().at(0);

    resLabels[0]->setText(tr("Resource name: %1").arg(fsModel.fileName(index)));

    if (fsModel.isDir(index)) {
        resLabels[1]->setText("Size: -");
        resLabels[2]->setText("File type: Directory");
        resLabels[3]->setText("Resource type: Directory");
    }
    else {
        resLabels[1]->setText(tr("Size: %1").arg(size_human(fsModel.size(index))));
        resLabels[2]->setText("File type: -"); // TODO
        resLabels[3]->setText("Resource type: -"); // TODO
    }
}

} // End of namespace GUI
