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

ResourceTree::ResourceTree(MainWindow *mainWindow, const QString &path, QObject *parent)
    : QAbstractItemModel(parent)
    , _iconProvider(new QFileIconProvider())
    , _mainWindow(mainWindow)
{
    _rootPath = path;
    _root = new ResourceTreeItem("Filename", nullptr);
    setRootPath(path);
}

ResourceTree::~ResourceTree() {
    _archives.clear();
    _keyDataFiles.clear();
    delete _iconProvider;
    delete _root;
}

void ResourceTree::setRootPath(const QString &path) {
    _mainWindow->status()->push("Populating resource tree...");

    ResourceTreeItem *top = _root;
    // If the root path is a directory, add a top level item
    // with its name
    auto info = QFileInfo(path);
    if (info.isDir()) {
        top = new ResourceTreeItem(info.canonicalFilePath(), _root);
        _root->appendChild(top);
    }

    populate(path, top);

    _mainWindow->status()->pop();
}

void ResourceTree::populate(const QString& path, ResourceTreeItem *parent) {
    if (QFileInfo(path).isDir()) {
        _mainWindow->status()->push(tr("Recursively adding all files in ") + path + "...");
        ResourceTreeItem *curItem;

        QDir dir(path);
        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::DirsFirst | QDir::Name);

        QFileInfoList list = dir.entryInfoList();

        for (const auto &e : list) {
            curItem = new ResourceTreeItem(e.canonicalFilePath(), parent);
            parent->appendChild(curItem);
            if (e.isDir()) {
                populate(e.canonicalFilePath(), curItem);
            }
        }
    }
    else {
        _mainWindow->status()->push(tr("Adding file ") + path + "...");
        parent->appendChild(new ResourceTreeItem(path, parent));
    }
}

ResourceTreeItem *ResourceTree::getItem(const QModelIndex &index) const {
    if (index.isValid())
        return static_cast<ResourceTreeItem*>(index.internalPointer());

    return _root;
}

bool ResourceTree::canFetchMore(const QModelIndex &index) const {
    auto type = getItem(index)->getFileType();
    if (type == Aurora::kFileTypeZIP ||
        type == Aurora::kFileTypeERF ||
        type == Aurora::kFileTypeMOD ||
        type == Aurora::kFileTypeNWM ||
        type == Aurora::kFileTypeSAV ||
        type == Aurora::kFileTypeHAK ||
        type == Aurora::kFileTypeRIM ||
        type == Aurora::kFileTypeKEY)
        return true;
    return false;
}

void ResourceTree::fetchMore(const QModelIndex &index) {
    if (!index.isValid())
        return;

    // We only need special treatment for these archives
    auto type = getItem(index)->getFileType();
    if (type != Aurora::kFileTypeZIP &&
        type != Aurora::kFileTypeERF &&
        type != Aurora::kFileTypeMOD &&
        type != Aurora::kFileTypeNWM &&
        type != Aurora::kFileTypeSAV &&
        type != Aurora::kFileTypeHAK &&
        type != Aurora::kFileTypeRIM &&
        type != Aurora::kFileTypeKEY)
        return;

    ResourceTreeItem *item = getItem(index);

    // We already added the archive members. Nothing to do
    Archive &archive = item->getArchive();
    if (archive.addedMembers)
        return;

    _mainWindow->status()->push(tr("Loading archive") + item->getName() + "...");
    BOOST_SCOPE_EXIT((&_mainWindow)) {
        _mainWindow->status()->pop();
    } BOOST_SCOPE_EXIT_END

    // Load the archive, if necessary
    if (!archive.data) {
        try {
            archive.data = getArchive(item->getPath());
        } catch (Common::Exception &e) {
            // If that fails, print the error and treat this archive as empty

            e.add("Failed to load archive \"%s\"", getItem(index)->getName().toStdString().c_str());
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

    auto type = getItem(index)->getFileType();
    if (type == Aurora::kFileTypeZIP ||
        type == Aurora::kFileTypeERF ||
        type == Aurora::kFileTypeMOD ||
        type == Aurora::kFileTypeNWM ||
        type == Aurora::kFileTypeSAV ||
        type == Aurora::kFileTypeHAK ||
        type == Aurora::kFileTypeRIM ||
        type == Aurora::kFileTypeKEY)
        return true;

    return getItem(index)->hasChildren();
}

int ResourceTree::rowCount(const QModelIndex &parent) const {
    return getItem(parent)->childCount();
}

int ResourceTree::columnCount(const QModelIndex &UNUSED(parent)) const {
    return 1;
}

QModelIndex ResourceTree::parent(const QModelIndex &index) const {
    ResourceTreeItem *parent = getItem(index)->getParent();
    if (parent == _root)
        return QModelIndex();

    return createIndex(parent->row(), 0, parent);
}

QModelIndex ResourceTree::index(int row, int column, const QModelIndex &parent) const {
    ResourceTreeItem *child = getItem(parent)->childAt(row);

    if (!child)
        return QModelIndex();

    return createIndex(row, column, child);
}

QVariant ResourceTree::headerData(int UNUSED(section), Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _root->getName();

    return QVariant();
}

QVariant ResourceTree::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    ResourceTreeItem *cur = getItem(index);

    if (role == Qt::DecorationRole)
    {
        switch (cur->getSource()) {
            case Source::kSourceFile:
                switch (cur->getFileType()) {
                    case Aurora::kFileTypeZIP:
                    case Aurora::kFileTypeERF:
                    case Aurora::kFileTypeMOD:
                    case Aurora::kFileTypeNWM:
                    case Aurora::kFileTypeSAV:
                    case Aurora::kFileTypeHAK:
                    case Aurora::kFileTypeRIM:
                    case Aurora::kFileTypeKEY:
                        // TODO: special icon for archives?
                        //return _iconProvider->icon(QFileIconProvider::File);
                    default:
                        return _iconProvider->icon(QFileIconProvider::File);
                }
            case Source::kSourceDirectory:
                return _iconProvider->icon(QFileIconProvider::Folder);
            default:
                return _iconProvider->icon(QFileIconProvider::File);
        }
    }

    if (role == Qt::DisplayRole)
        return cur->getName();

    return QVariant();
}

Qt::ItemFlags ResourceTree::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

void ResourceTree::insertItemsFromArchive(Archive &archive, const QModelIndex &parentIndex) {
    QList<ResourceTreeItem*> items;

    ResourceTreeItem *parentItem = getItem(parent);

    auto resources = archive.data->getResources();
    for (auto r = resources.begin(); r != resources.end(); ++r)
    {
        items << new ResourceTreeItem(archive.data, *r, parentItem);
    }

    insertItems(0, items, parent);
}

void ResourceTree::insertItems(int position, QList<ResourceTreeItem*> &items, const QModelIndex &parent) {
    ResourceTreeItem *parentItem = getItem(parent);

    beginInsertRows(parent, position, position + items.count() - 1);

    for (const auto &child : items)
        parentItem->appendChild(child);

    endInsertRows();
}

Aurora::Archive *ResourceTree::getArchive(const QString &path) {
    ArchiveMap::iterator a = _archives.find(path);
    if (a != _archives.end())
        return a->second;

    Aurora::Archive *arch = 0;
    switch (TypeMan.getFileType(path)) {
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

    Common::UString path = Common::FilePath::normalize(USTR(_rootPath + "/" + file));
    if (path.empty())
        throw Common::Exception("No such file or directory \"%s\"", (_root->getPath() + "/" + file).toStdString().c_str());

    Aurora::FileType type = TypeMan.getFileType(file);

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

            _mainWindow->status()->push(tr("Loading data file") + dataFiles[i].toQString() + "...");

            Aurora::KEYDataFile *dataFile = getKEYDataFile(dataFiles[i].toQString());
            key.addDataFile(i, dataFile);

            _mainWindow->status()->pop();

        } catch (Common::Exception &e) {
            e.add("Failed to load KEY data file \"%s\"", dataFiles[i].c_str());

            _mainWindow->status()->pop();
            Common::printException(e, "WARNING: ");
        }
    }
}

} // End of namespace GUI
