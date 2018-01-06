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
