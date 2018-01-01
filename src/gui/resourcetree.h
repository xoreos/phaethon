#ifndef RESOURCETREE_H
#define RESOURCETREE_H

#include <QAbstractItemModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QModelIndex>
#include <QVariant>

#include "verdigris/wobjectdefs.h"

#include "src/aurora/archive.h"
#include "src/aurora/util.h"
#include "src/common/ptrmap.h"
#include "src/gui/statusbar.h"
#include "src/images/decoder.h"

namespace Common {
    class SeekableReadStream;
}

namespace Sound {
    class AudioStream;
}

namespace Images {
    class Decoder;
}

namespace Aurora {
    class KEYFile;
    class KEYDataFile;
}

namespace GUI {

struct ArchiveInfo;
class MainWindow;
class ResourceTreeItem;

class ResourceTree : public QAbstractItemModel {
    W_OBJECT(ResourceTree)

private:
    QString _rootPath;
    ResourceTreeItem *_root;
    QFileIconProvider *_iconProvider;
    MainWindow *_mainWindow; // to access status bar and log

    typedef Common::PtrMap<QString, Aurora::Archive> ArchiveMap;
    typedef Common::PtrMap<QString, Aurora::KEYDataFile> KEYDataFileMap;

    ArchiveMap _archives;
    KEYDataFileMap _keyDataFiles;

public:
    explicit ResourceTree(MainWindow *mainWindow, const QString &path, QObject *parent = 0);
    ~ResourceTree();

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    ResourceTreeItem *getItem(const QModelIndex &index) const;
    bool canFetchMore(const QModelIndex &index) const override;
    void fetchMore(const QModelIndex &index);
    void insertItemsFromArchive(ArchiveInfo &data, const QModelIndex &parent);
    void insertItems(int position, QList<ResourceTreeItem*> &items, const QModelIndex &parent);
    bool hasChildren(const QModelIndex &index) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setRootPath(const QString &path);
    void populate(const QString& path, ResourceTreeItem *parent);
    Aurora::Archive *getArchive(const QString &path);
    Aurora::KEYDataFile *getKEYDataFile(const QString &file);
    void loadKEYDataFiles(Aurora::KEYFile &key);
};

} // End of namespace GUI

#endif // RESOURCETREE_H
