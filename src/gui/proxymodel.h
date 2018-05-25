#ifndef GUI_PROXYMODEL_H
#define GUI_PROXYMODEL_H

#include <QSortFilterProxyModel>

#include "verdigris/wobjectimpl.h"

namespace GUI {

class ProxyModel : public QSortFilterProxyModel {
	W_OBJECT(ProxyModel)

public:
	using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

} // End of namespace GUI

#endif // GUI_PROXYMODEL_H
