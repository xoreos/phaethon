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

#include <QFrame>
#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QTableView>

#include "verdigris/wobjectimpl.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/gdafile.h"

#include "src/common/readfile.h"
#include "src/common/scopedptr.h"

#include "src/gui/panelpreviewtable.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewTable)

PanelPreviewTable::PanelPreviewTable(QWidget *parent) :
	PanelBase(parent), _model(new QStandardItemModel(0)),
	_tableView(new QTableView(0)) {
	QVBoxLayout *layoutTop = new QVBoxLayout(this);

	layoutTop->addWidget(_tableView);
	_tableView->setModel(_model.get());

	_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	layoutTop->setContentsMargins(0, 0, 0, 0);
}

void PanelPreviewTable::show(const ResourceTreeItem *item) {
	PanelBase::show(item);

	if (item->getResourceType() != Aurora::kResourceTable)
		return;

	_currentItem = item;

	switch (item->getFileType()) {
		case Aurora::kFileType2DA:
			setTableData(false);
			break;
		case Aurora::kFileTypeGDA:
			setTableData(true);
			break;
		default:
			break;
	}
}

void PanelPreviewTable::setTableData(bool isGDA) {
	_model->clear();

	Common::ScopedPtr<Aurora::TwoDAFile> twoDA;

	Common::ScopedPtr<Common::SeekableReadStream> stream(_currentItem->getResourceData());

	if (isGDA) {
		Common::ScopedPtr<Aurora::GDAFile> gda(new Aurora::GDAFile(stream.release()));
		twoDA.reset(new Aurora::TwoDAFile(*gda));
	} else {
		twoDA.reset(new Aurora::TwoDAFile(*stream));
	}

	const std::vector<Common::UString> &headers = twoDA->getHeaders();

	for (size_t i = 0; i < headers.size(); i++) {
		_model->setHorizontalHeaderItem(i, new QStandardItem(QString(headers[i].c_str())));
	}

	for (size_t i = 0; i < twoDA->getRowCount(); i++) {
		QList<QStandardItem *> rowData;
		for (size_t j = 0; j < twoDA->getColumnCount(); j++) {
			const Common::UString &cellData = twoDA->getRow(i).getString(j);
			rowData << new QStandardItem(QString(cellData.c_str()));
		}
		_model->appendRow(rowData);
	}
}

} // End of namespace GUI
