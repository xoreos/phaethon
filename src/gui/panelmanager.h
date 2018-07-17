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
 *  Manager class for resource preview panels.
 */

#ifndef GUI_PANELMANAGER_H
#define GUI_PANELMANAGER_H

#include <memory>
#include <map>

#include "src/aurora/types.h"

class QLayout;

namespace GUI {

class PanelBase;
class ResourceTreeItem;

class PanelManager {
public:
	~PanelManager();

	void registerPanel(PanelBase *panel, Aurora::ResourceType type);
	void setLayout(QLayout *layout);
	void setItem(const ResourceTreeItem *item);
	PanelBase *getPanelByType(Aurora::ResourceType type);

private:
	void showPanel(Aurora::ResourceType type, const ResourceTreeItem *item);

private:
	QLayout *_layout { nullptr };
	PanelBase *_currentPanel { nullptr };
	std::map<Aurora::ResourceType, PanelBase *> _panels;
};

} // End of namespace GUI

#endif // PANEL_MANAGER_H
