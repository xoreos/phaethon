#include "src/gui/resourcetreeitem.h"

namespace GUI {
ResourceTreeItem::ResourceTreeItem(const QString name, const QString path, ResourceTreeItem *parent) {
    _name = name;
    _parent = parent;
    _isDir = false;
    if (path.isEmpty())
        _path = "";
    else
        _path = path;
    _isTraversed = false;
}

ResourceTreeItem::ResourceTreeItem(const QString name, ResourceTreeItem *parent) {
    _name = name;
    _parent = parent;
    _isDir = false;
    _path = "";
    _isTraversed = false;
}

ResourceTreeItem::~ResourceTreeItem() {
    qDeleteAll(_children);
}

void ResourceTreeItem::addChild(ResourceTreeItem *child) {
    _children << child;
}

bool ResourceTreeItem::insertChild(int position, ResourceTreeItem *child) {
    if (position < 0 or position >= _children.count())
        return false;

    _children.insert(position, child);

    return true;
}

ResourceTreeItem *ResourceTreeItem::child(int row) {
    return _children.at(row);
}

int ResourceTreeItem::childCount() {
    return _children.count();
}

int ResourceTreeItem::row() {
    if (_parent)
        return _parent->_children.indexOf(const_cast<ResourceTreeItem*>(this));
    return 0;
}

bool ResourceTreeItem::getIsDir() {
    return _isDir;
}

void ResourceTreeItem::setIsDir(bool isDir) {
    _isDir = isDir;
}

bool ResourceTreeItem::getIsTraversed() {
    return _isTraversed;
}

void ResourceTreeItem::setIsTraversed(bool isTraversed) {
    _isTraversed = isTraversed;
}

QString &ResourceTreeItem::getPath() {
    return _path;
}

ResourceTreeItem *ResourceTreeItem::getParent() {
    return _parent;
}

void ResourceTreeItem::setParent(ResourceTreeItem *parent) {
    _parent = parent;
}

QString ResourceTreeItem::getName() {
    return _name;
}

void ResourceTreeItem::setFileInfo(QFileInfo info) {
    _fileInfo = info;
}

QFileInfo ResourceTreeItem::getFileInfo() {
    return _fileInfo;
}

} // End of namespace GUI
