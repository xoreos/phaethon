#include "verdigris/wobjectdefs.h"

#include "src/gui/proxymodel.h"
#include "src/gui/resourcetree.h"
#include "src/gui/resourcetreeitem.h"

#include <QString>

namespace GUI {

W_OBJECT_IMPL(ProxyModel)

bool ProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
    QString lname = sourceModel()->data(left).toString();
    QString rname = sourceModel()->data(right).toString();

    bool ldir = qobject_cast<ResourceTree*>(sourceModel())->getItem(left)->isDir();
    bool rdir = qobject_cast<ResourceTree*>(sourceModel())->getItem(right)->isDir();

    if (ldir && rdir)
        return !QString::compare(lname, rname, Qt::CaseInsensitive);

    else if (ldir && !rdir)
        return true;

    return !QString::compare(lname, rname, Qt::CaseInsensitive);
}

} // End of namespace GUI
