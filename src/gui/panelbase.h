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
 *  Base class for panels which show resource previews.
 */

#ifndef GUI_PANELBASE_H
#define GUI_PANELBASE_H

#include <QFrame>
#include <QString>

#include "external/verdigris/wobjectdefs.h"

class QLayout;
class QObject;
class QWidget;

namespace GUI {

class ResourceTreeItem;

class PanelBase : public QFrame {
public:
	PanelBase(QWidget *parent);

	virtual void show(const ResourceTreeItem *item);
	virtual void hide();

	void setParent(QLayout *layout);
};

} // End of namespace GUI

#endif // PANEL_BASE_H
