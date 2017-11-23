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

#include <QDir>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <boost/scope_exit.hpp>

#include "verdigris/wobjectimpl.h"

#include "src/aurora/biffile.h"
#include "src/aurora/bzffile.h"
#include "src/aurora/erffile.h"
#include "src/aurora/keyfile.h"
#include "src/aurora/rimfile.h"
#include "src/aurora/zipfile.h"
#include "src/common/filepath.h"
#include "src/common/readfile.h"
#include "src/common/system.h"
#include "src/gui/mainwindow.h"
#include "src/gui/statusbar.h"
#include "src/gui/resourcetree.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(ResourceTree)

ResourceTree::ResourceTree(MainWindow *mainWindow, QObject *parent) : QAbstractItemModel(parent),
    _mainWindow(mainWindow) {
    _root.reset(new ResourceTreeItem("Filename"));
    _iconProvider.reset(new QFileIconProvider());
}

void ResourceTree::populate(const Common::FileTree::Entry &rootEntry) {
    ResourceTreeItem *treeRoot = new ResourceTreeItem(rootEntry);
    _root->addChild(treeRoot);

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>;
    connect(watcher, &QFutureWatcher<void>::finished, _mainWindow, &MainWindow::openFinish);
    QFuture<void> future = QtConcurrent::run(this, &ResourceTree::populate, rootEntry, treeRoot);
    watcher->setFuture(future);
}

void ResourceTree::populate(const Common::FileTree::Entry &entry, ResourceTreeItem *parent) {
    for (std::list<Common::FileTree::Entry>::const_iterator childIter = entry.children.begin();
         childIter != entry.children.end(); ++childIter) {

        ResourceTreeItem *child = new ResourceTreeItem(*childIter);
        parent->addChild(child);
        populate(*childIter, child);
    }
}

ResourceTree::~ResourceTree() {
    _archives.clear();
    _keyDataFiles.clear();
}

ResourceTreeItem *ResourceTree::itemFromIndex(const QModelIndex &index) const {
    if (!index.isValid())
        return _root.get();

    return static_cast<ResourceTreeItem*>(index.internalPointer());
}

QModelIndex ResourceTree::index(int row, int col, const QModelIndex &parent) const {
    ResourceTreeItem *item = itemFromIndex(parent)->childAt(row);

    if (!item)
        return QModelIndex();

    return createIndex(row, col, item);
}

QModelIndex ResourceTree::parent(const QModelIndex &index) const {
    ResourceTreeItem *parent = itemFromIndex(index)->getParent();
    if (parent == _root.get())
        return QModelIndex();

    return createIndex(parent->row(), 0, parent);
}

int ResourceTree::rowCount(const QModelIndex &parent) const {
    return itemFromIndex(parent)->childCount();
}

int ResourceTree::columnCount(const QModelIndex &UNUSED(parent)) const {
    return 1;
}

QVariant ResourceTree::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    ResourceTreeItem *item = itemFromIndex(index);

    if (role == Qt::DecorationRole) {
        switch (item->getSource()) {
            case Source::kSourceFile:
            case Source::kSourceArchiveFile:
                switch (item->getResourceType()) {
                    case Aurora::kResourceSound:
                        return QIcon::fromTheme("audio-x-generic");
                    case Aurora::kResourceImage:
                        return QIcon::fromTheme("image");
                    case Aurora::kResourceArchive:
                        return QIcon::fromTheme("package-x-generic");
                    default:
                        return _iconProvider->icon(QFileIconProvider::File);
                }
                break;
            default:
                return _iconProvider->icon(QFileInfo(item->getPath()));
        }
    }

    if (role == Qt::DisplayRole)
        return item->getName();

    return QVariant();
}

QVariant ResourceTree::headerData(int UNUSED(section), Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _root->getName();

    return QVariant();
}

bool ResourceTree::canFetchMore(const QModelIndex &index) const {
    return itemFromIndex(index)->isArchive();
}

void ResourceTree::fetchMore(const QModelIndex &index) {
    if (!index.isValid())
        return;

    ResourceTreeItem *item = itemFromIndex(index);

    // We already added the archive members. Nothing to do
    Archive &archive = item->getArchive();
    if (archive.addedMembers)
        return;

    _mainWindow->statusPush(tr("Loading archive") + item->getName() + "...");
    BOOST_SCOPE_EXIT((&_mainWindow)) {
        _mainWindow->statusPop();
    } BOOST_SCOPE_EXIT_END

    // Load the archive, if necessary
    if (!archive.data) {
        try {
            archive.data = getArchive(item->getPath());
        } catch (Common::Exception &e) {
            // If that fails, print the error and treat this archive as empty

            e.add("Failed to load archive \"%s\"", itemFromIndex(index)->getName().toStdString().c_str());
            Common::printException(e, "WARNING: ");

            return;
        }
    }

    insertItemsFromArchive(archive, index);

    archive.addedMembers = true;
}

bool ResourceTree::hasChildren(const QModelIndex &index) const {
    if (!index.isValid())
        return true;

    if (itemFromIndex(index)->isArchive())
        return true;

    return itemFromIndex(index)->hasChildren();
}

void ResourceTree::insertItemsFromArchive(Archive &archive, const QModelIndex &parentIndex) {
    QList<ResourceTreeItem*> items;

    auto resources = archive.data->getResources();
    for (auto r = resources.begin(); r != resources.end(); ++r)
    {
        items.push_back(new ResourceTreeItem(archive.data, *r));
    }

    insertItems(0, items, parent);
}

void ResourceTree::insertItems(size_t position, QList<ResourceTreeItem*> &items, const QModelIndex &parent) {
    ResourceTreeItem *parentItem = itemFromIndex(parent);

    beginInsertRows(parent, position, position + items.size() - 1);

    for (const auto &item : items)
    {
        parentItem->addChild(item);
    }

    endInsertRows();
}

Aurora::Archive *ResourceTree::getArchive(const QString &path) {
    ArchiveMap::iterator a = _archives.find(path);
    if (a != _archives.end())
        return a->second;

    Aurora::Archive *arch = 0;
    switch (TypeMan.getFileType(path.toStdString().c_str())) {
        case Aurora::kFileTypeZIP:
            arch = new Aurora::ZIPFile(new Common::ReadFile(path.toStdString().c_str()));
            break;

        case Aurora::kFileTypeERF:
        case Aurora::kFileTypeMOD:
        case Aurora::kFileTypeNWM:
        case Aurora::kFileTypeSAV:
        case Aurora::kFileTypeHAK:
            arch = new Aurora::ERFFile(new Common::ReadFile(path.toStdString().c_str()));
            break;

        case Aurora::kFileTypeRIM:
            arch = new Aurora::RIMFile(new Common::ReadFile(path.toStdString().c_str()));
            break;

        case Aurora::kFileTypeKEY: {
            Aurora::KEYFile *key = new Aurora::KEYFile(new Common::ReadFile(path.toStdString().c_str()));
            loadKEYDataFiles(*key);

            arch = key;
            break;
        }

        default:
            throw Common::Exception("Invalid archive file \"%s\"", path.toStdString().c_str());
    }

    _archives.insert(std::make_pair(path.toStdString().c_str(), arch));
    return arch;
}

#define USTR(x) (Common::UString((x).toStdString()))

Aurora::KEYDataFile *ResourceTree::getKEYDataFile(const QString &file) {
    KEYDataFileMap::iterator d = _keyDataFiles.find(file);
    if (d != _keyDataFiles.end())
        return d->second;

    Common::UString path = Common::FilePath::normalize(USTR(_root->childAt(0)->getPath() + "/" + file));
    if (path.empty())
        throw Common::Exception("No such file or directory \"%s\"", (_root->getPath() + "/" + file).toStdString().c_str());

    Aurora::FileType type = TypeMan.getFileType(file.toStdString().c_str());

    Aurora::KEYDataFile *dataFile = 0;
    switch (type) {
        case Aurora::kFileTypeBIF:
            dataFile = new Aurora::BIFFile(new Common::ReadFile(path));
            break;

        case Aurora::kFileTypeBZF:
            dataFile = new Aurora::BZFFile(new Common::ReadFile(path));
            break;

        default:
            throw Common::Exception("Unknown KEY data file type %d\n", type);
    }

    _keyDataFiles.insert(std::make_pair(file, dataFile));
    return dataFile;
}

void ResourceTree::loadKEYDataFiles(Aurora::KEYFile &key) {
    const std::vector<Common::UString> dataFiles = key.getDataFileList();
    for (uint i = 0; i < dataFiles.size(); i++) {
        try {

            _mainWindow->statusPush(tr("Loading data file") + QString::fromUtf8(dataFiles[i].c_str()) + "...");

            Aurora::KEYDataFile *dataFile = getKEYDataFile(QString::fromUtf8(dataFiles[i].c_str()));
            key.addDataFile(i, dataFile);

            _mainWindow->statusPop();

        } catch (Common::Exception &e) {
            e.add("Failed to load KEY data file \"%s\"", dataFiles[i].c_str());

            _mainWindow->statusPop();
            Common::printException(e, "WARNING: ");
        }
    }
}

} // End of namespace GUI
