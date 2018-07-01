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

#ifndef GUI_RESOURCETREE_H
#define GUI_RESOURCETREE_H

#include <QAbstractItemModel>
#include <QFileIconProvider>

#include "verdigris/wobjectdefs.h"

#include "src/aurora/archive.h"
#include "src/aurora/util.h"

#include "src/common/filetree.h"
#include "src/common/ptrmap.h"

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

public:
	ResourceTree(MainWindow *mainWindow, QObject *parent = 0);
	~ResourceTree();

	void populate(const Common::FileTree::Entry &rootEntry);
	void populate(const Common::FileTree::Entry &rootEntry, ResourceTreeItem *parent);

	void insertItemsFromArchive(Archive &archive, const QString &path, const QModelIndex &parentIndex);
	void insertItems(size_t position, QList<ResourceTreeItem *> &items, const QModelIndex &parentIndex);

	Aurora::Archive     *getArchive(ResourceTreeItem &item);
	Aurora::KEYDataFile *getKEYDataFile(const QString &file);
	void                loadKEYDataFiles(Aurora::KEYFile &key);

	/** Return the item in the tree structure that corresponds to the given index. */
	ResourceTreeItem *itemFromIndex(const QModelIndex &index) const;

	// Model functions

	/** Return the index if it exists, else create it. */
	QModelIndex index(int row, int col, const QModelIndex &parent) const override;

	/** Return the parent of the given index if it exists, else create it. */
	QModelIndex parent(const QModelIndex &index) const override;

	/** Return the data for the given index. */
	QVariant data(const QModelIndex &index, int role) const override;

	/** Return the header data. */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/** Define what file types should have an expand arrow in the tree. */
	bool canFetchMore(const QModelIndex &index) const;

	/** Return whether the item for index actually has children. */
	bool hasChildren(const QModelIndex &index) const;

	/** Return column count (in our case, 1 -- the filename.) */
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	/** Return row count -- how many children the given index has. */
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	/** Add children to the given index. */
	void fetchMore(const QModelIndex &index);

private:
	Common::ScopedPtr<ResourceTreeItem> _root;
	MainWindow *_mainWindow;

	Common::ScopedPtr<QFileIconProvider> _iconProvider;

	typedef Common::PtrMap<QString, Aurora::Archive>     ArchiveMap;
	typedef Common::PtrMap<QString, Aurora::KEYDataFile> KEYDataFileMap;

	ArchiveMap _archives;
	KEYDataFileMap _keyDataFiles;
};

} // End of namespace GUI

#endif // GUI_RESOURCETREE_H
