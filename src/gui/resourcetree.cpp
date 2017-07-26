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
#include <QDirIterator>
#include <QFileInfoList>

#include "verdigris/wobjectimpl.h"

#include "src/gui/resourcetree.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(ResourceTree)

ResourceTree::ResourceTree(const QString &rootPath, QObject *parent) : QAbstractItemModel(parent) {
    _root = new ResourceTreeItem("Filename", "", nullptr);

    if (!rootPath.isEmpty())
        ResourceTree::setRootPath(rootPath);
}

ResourceTree::~ResourceTree() {
    delete _root;
}

void ResourceTree::populate(const QString& path, ResourceTreeItem *parentNode) {
    ResourceTreeItem *curNode;

    QDir dir(path);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);

    QFileInfoList list = dir.entryInfoList();

    for (auto &e : list) {
        curNode = new ResourceTreeItem(e.fileName(), parentNode);
        curNode->setFileInfo(e);
        parentNode->addChild(curNode);
        if (e.isDir()) {
            curNode->setIsDir(true);
            populate(e.canonicalFilePath(), curNode);
        }
    }
}

void ResourceTree::setRootPath(const QString& path) {
    if (_root)
        delete _root;

    populate(path, new ResourceTreeItem("", "", nullptr));
}

ResourceTreeItem *ResourceTree::getNode(const QModelIndex &index) const {
    if (index.isValid())
        return static_cast<ResourceTreeItem*>(index.internalPointer());
    else
        return _root;
}

bool ResourceTree::canFetchMore(const QModelIndex &index) {
    ResourceTreeItem *item = getNode(index);

    if (item->getIsDir() && !item->getIsTraversed())
        return true;

    return false;
}

// TODO
void ResourceTree::fetchMore(const QModelIndex &index) {
//    TreeNode *parent = getNode(index);

//    QList<TreeNode*> nodes;
//    QDirIterator it(parent->get_path(), QDirIterator::Subdirectories);
//    while (it.hasNext()) {
//        TreeNode *TreeNode = new TreeNode("", it.path(), parent);
//        if (TreeNode->is_dir())
//            TreeNode->set_is_dir();

//        nodes.append(TreeNode);
//    }
//    insertNodes(0, nodes, index);
//    parent->set_is_traversed();
}

bool ResourceTree::hasChildren(const QModelIndex &index) const {
    ResourceTreeItem *item = getNode(index);

    if (item->getIsDir())
        return true;

    return QAbstractItemModel::hasChildren(index);
}

int ResourceTree::rowCount(const QModelIndex &parent) const {
    ResourceTreeItem* item = getNode(parent);
    return item->childCount();
}

int ResourceTree::columnCount(const QModelIndex &parent) const {
    return 1;
}

QModelIndex ResourceTree::parent(const QModelIndex &index) const {
    ResourceTreeItem *node = getNode(index);

    ResourceTreeItem *parent = node->getParent();
    if (parent == _root)
        return QModelIndex();

    return createIndex(parent->row(), 0, parent);
}

QModelIndex ResourceTree::index(int row, int column, const QModelIndex &parent) const {
    ResourceTreeItem *node = getNode(parent);

    ResourceTreeItem *child = node->child(row);

    if (!child)
        return QModelIndex();

    return createIndex(row, column, child);
}

QVariant ResourceTree::headerData(int section, Qt::Orientation orientation, int role) const {
    return _root->getName();
}

QVariant ResourceTree::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    ResourceTreeItem *node = static_cast<ResourceTreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
        return node->getName();

    return QVariant();
}

Qt::ItemFlags ResourceTree::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

bool ResourceTree::insertNodes(int position, QList<ResourceTreeItem*> &nodes, QModelIndex parent) {
    ResourceTreeItem *item = getNode(parent);

    beginInsertRows(parent, position, position + nodes.count() - 1);

    bool success = false;
    for (auto &child : nodes)
        success = item->insertChild(position, child);

    endInsertRows();

    return success;
}

} // End of namespace GUI
