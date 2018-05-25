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
 *  Helper class to facilitate sorting of items within the resource tree.
 */

#include <QString>

#include "verdigris/wobjectdefs.h"

#include "src/gui/proxymodel.h"
#include "src/gui/resourcetree.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(ProxyModel)

bool ProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	ResourceTree *model = qobject_cast<ResourceTree *>(sourceModel());

	ResourceTreeItem *itemLeft = model->itemFromIndex(left);
	ResourceTreeItem *itemRight = model->itemFromIndex(right);

	bool compare = QString::compare(itemLeft->getName(), itemRight->getName(), Qt::CaseInsensitive) < 0;

	bool leftDir = itemLeft->isDir();
	bool rightDir = itemRight->isDir();

	if (leftDir && rightDir)
		return compare;

	else if (leftDir && !rightDir)
		return true;

	else if (!leftDir && rightDir)
		return false;

	return compare;
}

} // End of namespace GUI
