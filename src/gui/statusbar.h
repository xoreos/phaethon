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
 *  Wrapper class for status bar in order to
 *  have identical functionality to Phaethon (wx version.)
 */

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QMainWindow>
#include <QStatusBar>
#include <QWidget>

namespace GUI {

class StatusBar {
public:
    StatusBar(QStatusBar *statusBar);
    void setText(const QString &text);
    void push(const QString &text, int timeout = 0);
    void pop();

private:
    QStatusBar *_statusBar;
    QString _text;
};

} // End of namespace GUI

#endif // STATUSBAR_H
