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
 *  Preview panel for resources consisting of tabular data such as
 *  2DA and GDA.
 */

#ifndef GUI_PANELPREVIEWTABLE_H
#define GUI_PANELPREVIEWTABLE_H

#include <QStandardItemModel>

#include "src/common/scopedptr.h"

#include "src/gui/panelbase.h"

class QComboBox;
class QTableView;

namespace GUI {

class ResourceTreeItem;

class PanelPreviewTable : public PanelBase {
	W_OBJECT(PanelPreviewTable)

public:
	PanelPreviewTable(QWidget *parent);

	virtual void show(const ResourceTreeItem *item);

public /*signals*/:
	void log(const QString &text)
	W_SIGNAL(log, text)

private:
	const ResourceTreeItem *_currentItem { nullptr };
	Common::ScopedPtr<QStandardItemModel> _model { nullptr };
	QTableView *_tableView { nullptr };

	void setTableData(bool isGDA);
};

} // End of namespace GUI

#endif // GUI_PANELPREVIEWTABLE_H
