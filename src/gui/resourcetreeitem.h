#ifndef RESOURCETREEITEM_H
#define RESOURCETREEITEM_H

#include <QFileInfo>
#include <QString>

#include "src/aurora/archive.h"
#include "src/aurora/util.h"
#include "src/images/dds.h"
#include "src/images/sbm.h"
#include "src/images/tga.h"
#include "src/images/tpc.h"
#include "src/images/txb.h"
#include "src/images/winiconimage.h"
#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

namespace GUI {

class MainWindow;

enum Source {
    kSourceDirectory= 0,
    kSourceFile = 1,
    kSourceArchive = 2,
    kSourceArchiveFile = 3
};

struct Archive {
    Aurora::Archive *data;
    bool addedMembers;
    uint32 index;
};

struct ItemData {
    ItemData(const QString &parentPath, const QString &fileName, Aurora::Archive *archiveData, Aurora::Archive::Resource &resource);
    ItemData(const QString &fullPath, const QFileInfo &info);

    QString _fullPath; // used for QFileInfo and item info e.g. logging
    bool _isDir;
    Source _source;
    Aurora::FileType _fileType;
    Aurora::ResourceType _resourceType;
    qint64 _size;

    mutable bool _triedDuration;
    mutable uint64 _duration;

    Archive _archive;
};

class ResourceTreeItem {
public:
    ResourceTreeItem(Aurora::Archive *archiveData, Aurora::Archive::Resource &resource, ResourceTreeItem *parent);
    ResourceTreeItem(QString fullPath, ResourceTreeItem *parent);
    ~ResourceTreeItem();

    /** Model structure. **/
    bool hasChildren() const;
    bool insertChild(int position, ResourceTreeItem *child);
    int childCount() const;
    int row() const;
    ResourceTreeItem *childAt(int row) const;
    ResourceTreeItem *getParent() const;
    void appendChild(ResourceTreeItem *child);
    void setParent(ResourceTreeItem *parent);

    /** Both. **/
    const QString &getName() const; // doubles as filename

    /** File info. **/
    Aurora::FileType getFileType() const;
    Aurora::ResourceType getResourceType() const;
    bool isDir() const;
    qint64 getSize() const;
    const QString &getPath() const;
    Source getSource() const;

    /** Resource information. **/
    Archive &getArchive();
    Common::SeekableReadStream *getResourceData() const;
    Images::Decoder *getImage() const;
    Images::Decoder *getImage(Common::SeekableReadStream &res, Aurora::FileType type) const;
    Sound::AudioStream *getAudioStream() const;
    uint64 getSoundDuration() const;

private:
    /** Model information. **/
    ResourceTreeItem *_parent;
    QList<ResourceTreeItem*> _children;
    QString _name; // The filename. It's what the tree model displays.

    ItemData *_data;
};

} // End of namespace GUI

#endif // RESOURCETREEITEM_H
