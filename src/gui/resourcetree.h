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
 *  Phaethon's tree of game resource files.
 */

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

struct Archive;
class MainWindow;
class ResourceTreeItem;

class ResourceTree : public QAbstractItemModel {
    W_OBJECT(ResourceTree)

private:
    ResourceTreeItem *_root;
    MainWindow *_mainWindow;
    
    QFileIconProvider *_iconProvider;

    typedef Common::PtrMap<QString, Aurora::Archive> ArchiveMap;
    typedef Common::PtrMap<QString, Aurora::KEYDataFile> KEYDataFileMap;

    ArchiveMap _archives;
    KEYDataFileMap _keyDataFiles;

public:
    explicit ResourceTree(MainWindow *mainWindow, const QString &path, QObject *parent = 0);
    ~ResourceTree();

    void populate(const QString& path, ResourceTreeItem *parent);

    ResourceTreeItem *itemFromIndex(const QModelIndex &index) const;

    QModelIndex index(int row, int col, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool canFetchMore(const QModelIndex &index) const;
    void fetchMore(const QModelIndex &index);
    bool hasChildren(const QModelIndex &index) const;

    void insertItemsFromArchive(Archive &archive, const QModelIndex &parentIndex);
    void insertItems(int position, QList<ResourceTreeItem*> &items, const QModelIndex &parentIndex);
    Aurora::Archive *getArchive(const QString &path);
    Aurora::KEYDataFile *getKEYDataFile(const QString &file);
    void loadKEYDataFiles(Aurora::KEYFile &key);
};

} // End of namespace GUI

#endif // RESOURCETREE_H
