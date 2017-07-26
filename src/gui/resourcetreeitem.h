#ifndef RESOURCETREEITEM_H
#define RESOURCETREEITEM_H

#include <QFileInfo>
#include <QString>

namespace GUI {

class ResourceTreeItem {
public:
    ResourceTreeItem(const QString name, const QString path, ResourceTreeItem *parent);
    ResourceTreeItem(const QString name, ResourceTreeItem *parent);
    ~ResourceTreeItem();

    void addChild(ResourceTreeItem *child);
    bool insertChild(int position, ResourceTreeItem *child);
    void setParent(ResourceTreeItem *parent);
    void setIsDir(bool isDir);
    int childCount();
    ResourceTreeItem *child(int row);
    int row();
    bool getIsDir();
    bool getIsTraversed();
    void setIsTraversed(bool isTraversed);
    QString &getPath();
    ResourceTreeItem *getParent();
    QString getName();
    void setFileInfo(QFileInfo info);
    QFileInfo getFileInfo();

private:
    bool _isDir;
    bool _isTraversed;
    QList<ResourceTreeItem*> _children;
    QString _name;
    ResourceTreeItem *_parent;
    QString _path;
    QFileInfo _fileInfo;
};

} // End of namespace GUI

#endif // RESOURCETREEITEM_H
