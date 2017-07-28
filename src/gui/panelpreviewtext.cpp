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
 *  Preview panel for text files.
 */

#include <QTextStream>

#include "verdigris/wobjectimpl.h"

#include "src/gui/panelpreviewtext.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewText)

PanelPreviewText::PanelPreviewText(QObject *parent) {
    _layout = new QHBoxLayout();
    _layout->setObjectName("previewText");
    _textBox = new QPlainTextEdit();
    _layout->addWidget(_textBox);
    setLayout(_layout);
}

void PanelPreviewText::setItem(ResourceTreeItem *item) {
    if (item == _currentItem)
        return;

    if (item->getFileType() != Aurora::FileType::kFileTypeTXT &&
        item->getFileType() != Aurora::FileType::kFileTypeINI)
        return;

    if (item->getSize() > 1000000) // 1 MB
        return; // fixme: exception

    _currentItem = item;

    QFile file(item->getFileInfo().canonicalFilePath());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream textStream(&file);

    _textBox->setPlainText(textStream.readAll());

    return; // fixme: exception
}


PanelPreviewText::~PanelPreviewText() {
    delete _layout;
}

} // End of namespace GUI
