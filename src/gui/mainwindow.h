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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QLabel>
#include <QProgressBar>
#include <QItemSelection>

#include "verdigris/wobjectdefs.h"

#include "src/gui/selection.h"

namespace Ui {
    class MainWindow;
}

namespace GUI {

class MainWindow : public QMainWindow {
    W_OBJECT(MainWindow)

public:
    explicit MainWindow(QWidget *parent = 0, const char *version = "", QSize size = QSize(), QString path = QString());
    ~MainWindow();

// signals:
    void openDir(const QString &path)
    W_SIGNAL(openDir, path)

// public slots:
    void setTreeViewModel(const QString &path);

// private slots:
    void on_actionOpen_directory_triggered();
    void on_actionClose_triggered();
    void on_actionQuit_triggered();

    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

    void selection(const QItemSelection &selected);

    void setLabels();
    void clearLabels();

private:
    Ui::MainWindow *ui;
    QFileSystemModel fsModel;
    QLabel *statusLabel;
    QProgressBar *statusProgressBar;
    QList<QLabel*> resLabels;
    Selection _currentSelection;
};

} // End of namespace GUI

#endif // MAINWINDOW_H
