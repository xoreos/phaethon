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

#include "verdigris/wobjectimpl.h"

#include "src/common/system.h"
#include "src/gui/panelpreviewtext.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewText)

PanelPreviewText::PanelPreviewText(QWidget *parent)
    : QFrame(parent) {
    QHBoxLayout *layoutTop = new QHBoxLayout(this);

    _textEdit = new QTextEdit();

    _textEdit->setFrameShape(QFrame::NoFrame);

    layoutTop->addWidget(_textEdit);
    layoutTop->setContentsMargins(0, 0, 0, 0);
}

void PanelPreviewText::setText(const QString &text) {
    _textEdit->setText(text);
}

void PanelPreviewText::setItem(const ResourceTreeItem *item) {
    if (item == _currentItem)
        return;

    if (item->getFileType() != Aurora::FileType::kFileTypeTXT &&
        item->getFileType() != Aurora::FileType::kFileTypeINI)
        return;

    if (item->getSize() > 1000000) { // 1 MB
        emit log(tr("ERROR: File size too large for '%1' (> 1MB)").arg(item->getName()));
        return;
    }

    _currentItem = item;

    QFile file(item->getPath());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream textStream(&file);

    setText(textStream.readAll());
}

} // End of namespace GUI
