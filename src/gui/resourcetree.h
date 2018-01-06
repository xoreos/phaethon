#ifndef RESOURCETREE_H
#define RESOURCETREE_H

#include "verdigris/wobjectdefs.h"

#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QModelIndex>
#include <QVariant>

#include "src/aurora/keyfile.h"
#include "src/common/ptrmap.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

class MainWindow;

class ResourceTree : public QStandardItemModel {
    W_OBJECT(ResourceTree)

private:
    ResourceTreeItem *_root = nullptr;
    QFileIconProvider *_iconProvider = nullptr;
    MainWindow *_mainWindow;

    typedef Common::PtrMap<Common::UString, Aurora::Archive> ArchiveMap;
    typedef Common::PtrMap<Common::UString, Aurora::KEYDataFile> KEYDataFileMap;

    ArchiveMap _archives;
    KEYDataFileMap _keyDataFiles;

public:
    explicit ResourceTree(const QString &path = "", QObject *parent = 0);
    ~ResourceTree();

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    ResourceTreeItem *getNode(const QModelIndex &index) const;
    bool canFetchMore(const QModelIndex &index) const override;
    void fetchMore(const QModelIndex &index);
    void insertItemsFromArchive(ResourceTreeItem::ArchiveInfo &data, const QModelIndex &parent);
    void insertNodes(int position, QList<ResourceTreeItem*> &nodes, const QModelIndex &parent);
    bool hasChildren(const QModelIndex &index) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setRootPath(const QString& path);
    void populate(const QString& path, ResourceTreeItem *parentNode);
    Aurora::Archive *getArchive(const QString &path);
    Aurora::KEYDataFile *getKEYDataFile(const Common::UString &file);
    void loadKEYDataFiles(Aurora::KEYFile &key);
};

} // End of namespace GUI

#endif // RESOURCETREE_H
