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

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>

#include "verdigris/wobjectimpl.h"

#include "src/common/encoding.h"
#include "src/common/system.h"

#include "src/gui/panelpreviewtext.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewText)

PanelPreviewText::PanelPreviewText(QWidget *parent) :
	QFrame(parent), _encodingBox(0) {
	QVBoxLayout *layoutTop = new QVBoxLayout(this);

	_textEdit = new QTextEdit(this);
	_textEdit->setFrameShape(QFrame::NoFrame);
	_textEdit->setReadOnly(true);
	_textEdit->setStyleSheet("font-family: monospace;");

	_encodingBox = new QComboBox(this);
	_encodingBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	for (int i = 0; i < Common::kEncodingMAX; i++) {
		_encodingBox->addItem(QString::fromUtf8(Common::getEncodingName(Common::Encoding(i)).c_str()));
	}

	QFormLayout *layoutEncoding = new QFormLayout();
	layoutEncoding->setSizeConstraint(QLayout::SetMinimumSize);
	layoutEncoding->addRow(tr("Encoding: "), _encodingBox);
	layoutTop->addLayout(layoutEncoding);
	layoutTop->addWidget(_textEdit);
	layoutTop->setContentsMargins(0, 0, 0, 0);

	QObject::connect(_encodingBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PanelPreviewText::slotEncodingChanged);
}

void PanelPreviewText::setItem(const ResourceTreeItem *item) {
	if (item == _currentItem)
		return;

	if (item->getResourceType() != Aurora::kResourceText)
		return;

	_currentItem = item;

	Common::Encoding defaultEncoding = Common::kEncodingCP1252;
	_encodingBox->setCurrentIndex(defaultEncoding);
	_textEdit->setText(getEncodedText(defaultEncoding));
}

void PanelPreviewText::slotEncodingChanged(int index) {
	_textEdit->setText(getEncodedText(Common::Encoding(index)));
}

QString PanelPreviewText::getEncodedText(Common::Encoding encoding) {
	Common::ScopedPtr<Common::SeekableReadStream> stream(_currentItem->getResourceData());
	Common::UString converted;

	try {
		converted = Common::readString(*stream.get(), encoding);
	} catch (const Common::Exception &e) {
		emit log("Exception: " + QString(e.what()));
	}

	return QString(converted.c_str());
}

} // End of namespace GUI
