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

#include <QLayout>

#include "src/common/error.h"

#include "src/gui/panelbase.h"
#include "src/gui/panelmanager.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

PanelManager::PanelManager() :
	_layout(nullptr), _currentPanel(nullptr) {
}

PanelManager::~PanelManager() {
	for (auto pair : _panels) {
		delete pair.second;
	}
}

void PanelManager::registerPanel(PanelBase *panel, Aurora::ResourceType type) {
	auto result = _panels.find(type);
	if (result != _panels.end()) {
		throw Common::Exception("Panel type already exists");
	}

	if (_layout) {
		panel->setParent(_layout);
	}

	_panels.emplace(type, panel);
}

void PanelManager::setLayout(QLayout *layout) {
	_layout = layout;

	for (auto pair : _panels) {
		pair.second->setParent(layout);
	}
}

void PanelManager::setItem(const ResourceTreeItem *item) {
	if (!_layout)
		return;

	Aurora::ResourceType type;

	if (!item)
		type = Aurora::kResourceNone;
	else
		type = item->getResourceType();

	showPanel(type, item);
}

void PanelManager::showPanel(Aurora::ResourceType type, const ResourceTreeItem *item) {
	auto result = _panels.find(type);
	if (result != _panels.end()) {
		if (!_currentPanel) {
			_currentPanel = result->second;
		}
		else {
			_currentPanel->hide();
			Common::ScopedPtr<QLayoutItem> layoutItem(_layout->replaceWidget(
				static_cast<QWidget *>(_currentPanel),
				static_cast<QWidget *>(result->second)
			));
			_currentPanel = result->second;
		}
		_currentPanel->show(item);
	} else {
		throw Common::Exception("Panel doesn't exist");
	}
}

PanelBase *PanelManager::getPanelByType(Aurora::ResourceType type) {
	auto result = _panels.find(type);
	if (result != _panels.end()) {
		return result->second;
	} else {
		throw Common::Exception("Panel doesn't exist");
	}
}

} // End of namespace GUI
