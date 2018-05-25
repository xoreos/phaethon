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

#ifndef PANELPREVIEWTEXT_H
#define PANELPREVIEWTEXT_H

#include <QFrame>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

class QComboBox;

namespace GUI {

class ResourceTreeItem;

class PanelPreviewText : public QFrame {
	W_OBJECT(PanelPreviewText)

public:
	PanelPreviewText(QWidget *parent);

	void setItem(const ResourceTreeItem *item);

public /*signals*/:
	void log(const QString &text)
	W_SIGNAL(log, text)

private /*slots*/:
	void slotEncodingChanged(int index);
	W_SLOT(slotEncodingChanged, W_Access::Private)

private:
	QTextEdit *_textEdit;
	QComboBox *_encodingBox;
	const ResourceTreeItem *_currentItem;

	void setText(const QString &text);
	QString getEncodedText(Common::Encoding encoding);
};

} // End of namespace GUI

#endif // PANELPREVIEWTEXT_H
