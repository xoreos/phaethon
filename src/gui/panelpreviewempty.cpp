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
 *  Preview panel for resources we can't do anything with.
 */

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

#include "external/verdigris/wobjectimpl.h"

#include "src/common/system.h"

#include "src/gui/panelpreviewempty.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewEmpty)

PanelPreviewEmpty::PanelPreviewEmpty(QWidget *parent) :
	PanelBase(parent) {
	QHBoxLayout *layoutTop = new QHBoxLayout(this);
	layoutTop->setContentsMargins(0, 0, 0, 0);

	QLabel *label = new QLabel(tr("[No preview.]"), this);
	label->setEnabled(false);
	label->setAlignment(Qt::AlignCenter);

	layoutTop->addWidget(label);

	this->setLayout(layoutTop);
}

} // End of namespace GUI
