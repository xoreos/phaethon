#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfoList>
#include <QTreeView>
#include <boost/scope_exit.hpp>

#include "verdigris/wobjectimpl.h"

#include "src/aurora/biffile.h"
#include "src/aurora/bzffile.h"
#include "src/aurora/erffile.h"
#include "src/aurora/rimfile.h"
#include "src/aurora/zipfile.h"
#include "src/common/filepath.h"
#include "src/common/readfile.h"
#include "src/gui/mainwindow.h"
#include "src/gui/statusbar.h"
#include "src/gui/resourcetree.h"

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

void ResourceTree::setRootPath(QString path) {
    populate(path, _root);
    _mainWindow->status()->push("Populating resource tree...");
    _mainWindow->status()->pop();
}

void ResourceTree::populate(const QString& path, ResourceTreeItem *parentNode) {
    if (QFileInfo(path).isDir()) {
        _mainWindow->status()->push(tr("Recursively adding all files in ") + path + "...");
        ResourceTreeItem *curNode;

        QDir dir(path);
        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::DirsFirst | QDir::Name);

        QFileInfoList list = dir.entryInfoList();

        for (auto &e : list) {
            curNode = new ResourceTreeItem(e.canonicalFilePath(), parentNode);
            parentNode->appendChild(curNode);
            if (e.isDir()) {
                populate(e.canonicalFilePath(), curNode);
            }
        }
    }
    else {
        _mainWindow->status()->push(tr("Adding file ") + path + "...");
        parentNode->appendChild(new ResourceTreeItem(path, parentNode));
    }
}

ResourceTreeItem *ResourceTree::getNode(const QModelIndex &index) const {
    if (index.isValid())
        return static_cast<ResourceTreeItem*>(index.internalPointer());

    return _root;
}

bool ResourceTree::canFetchMore(const QModelIndex &index) const {
    auto type = getNode(index)->getFileType();
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
    auto type = getNode(index)->getFileType();
    if (type != Aurora::kFileTypeZIP &&
        type != Aurora::kFileTypeERF &&
        type != Aurora::kFileTypeMOD &&
        type != Aurora::kFileTypeNWM &&
        type != Aurora::kFileTypeSAV &&
        type != Aurora::kFileTypeHAK &&
        type != Aurora::kFileTypeRIM &&
        type != Aurora::kFileTypeKEY)
        return;

    ResourceTreeItem *item = getNode(index);

    // We already added the archive members. Nothing to do
    ResourceTreeItem::ArchiveInfo &archiveInfo = item->getData();
    if (archiveInfo.addedArchiveMembers)
        return;

    _mainWindow->status()->push(tr("Loading archive") + item->getData() + "...");
    BOOST_SCOPE_EXIT((&_mainWindow)) {
        _mainWindow->status()->pop();
    } BOOST_SCOPE_EXIT_END

    // Load the archive, if necessary
    if (!archiveInfo.archive) {
        try {
            archiveInfo.archive = getArchive(item->getPath());
        } catch (Common::Exception &e) {
            // If that fails, print the error and treat this archive as empty

            e.add("Failed to load archive \"%s\"", getNode(index)->getName().toStdString().c_str());
            Common::printException(e, "WARNING: ");

            return;
        }
    }

    insertItemsFromArchive(archiveInfo, index);

    archiveInfo.addedArchiveMembers = true;
}

bool ResourceTree::hasChildren(const QModelIndex &index) const {
    if (!index.isValid())
        return true;

    auto type = getNode(index)->getFileType();
    if (type == Aurora::kFileTypeZIP ||
        type == Aurora::kFileTypeERF ||
        type == Aurora::kFileTypeMOD ||
        type == Aurora::kFileTypeNWM ||
        type == Aurora::kFileTypeSAV ||
        type == Aurora::kFileTypeHAK ||
        type == Aurora::kFileTypeRIM ||
        type == Aurora::kFileTypeKEY)
        return true;

    return getNode(index)->hasChildren();
}

int ResourceTree::rowCount(const QModelIndex &parent) const {
    return getNode(parent)->childCount();
}

int ResourceTree::columnCount(const QModelIndex &parent) const {
    return 1;
}

QModelIndex ResourceTree::parent(const QModelIndex &index) const {
    ResourceTreeItem *parent = getNode(index)->getParent();
    if (parent == _root)
        return QModelIndex();

    return createIndex(parent->row(), 0, parent);
}

QModelIndex ResourceTree::index(int row, int column, const QModelIndex &parent) const {
    ResourceTreeItem *child = getNode(parent)->childAt(row);

    if (!child)
        return QModelIndex();

    return createIndex(row, column, child);
}

QVariant ResourceTree::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _root->getName();

    return QVariant();
}

QVariant ResourceTree::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    ResourceTreeItem *cur = getNode(index);

    if (role == Qt::DecorationRole)
    {
        switch (cur->getSource()) {
            case ResourceTreeItem::Source::kSourceFile:
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
            case ResourceTreeItem::Source::kSourceDirectory:
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

void ResourceTree::insertItemsFromArchive(ResourceTreeItem::ArchiveInfo &data, const QModelIndex &parent) {
    QList<ResourceTreeItem*> nodes;

    ResourceTreeItem *parentItem = getNode(parent);

    auto resources = data.archive->getResources();
    for (auto r = resources.begin(); r != resources.end(); ++r)
    {
        nodes << new ResourceTreeItem(data.archive, *r, parentItem);
    }

    insertNodes(0, nodes, parent);
}

void ResourceTree::insertNodes(int position, QList<ResourceTreeItem*> &nodes, const QModelIndex &parent) {
    ResourceTreeItem *node = getNode(parent);

    beginInsertRows(parent, position, position + nodes.count() - 1);

    for (auto &child : nodes)
        node->appendChild(child);

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
        throw Common::Exception("No such file or directory \"%s\"", (_root->getPath() + "/" + file));

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

            //            GetStatusBar()->PopStatusText(); // fixme
            Common::printException(e, "WARNING: ");
        }
    }
}

} // End of namespace GUI
