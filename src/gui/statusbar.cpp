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

#include "statusbar.h"

namespace GUI {

StatusBar::StatusBar(QStatusBar *statusBar) {
    _statusBar = statusBar;
}

void StatusBar::setText(const QString &text) {
    _statusBar->showMessage(text);
    _text = text;
}

void StatusBar::push(const QString &text, int timeout) {
    _statusBar->showMessage(text, timeout);
}

void StatusBar::pop() {
    _statusBar->showMessage(_text);
}

} // End of namespace GUI
