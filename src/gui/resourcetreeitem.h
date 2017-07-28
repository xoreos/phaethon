#ifndef RESOURCETREEITEM_H
#define RESOURCETREEITEM_H

#include <QFileInfo>
#include <QString>

#include "src/aurora/archive.h"
#include "src/aurora/util.h"
#include "src/images/decoder.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

namespace GUI {

enum Source {
    kSourceDirectory= 0,
    kSourceFile = 1,
    kSourceArchive = 2,
    kSourceArchiveFile = 3
};

class ResourceTreeItem {
private:
    QList<ResourceTreeItem*> _children;
    QString _name = "DEFAULTNAME";
    ResourceTreeItem *_parent = nullptr;
    QString _fullPath = "DEFAULTPATH";
    bool _isDir;
    Source _source;
    Aurora::FileType _fileType;
    Aurora::ResourceType _resourceType;
    qint64 _size;

    mutable bool _triedDuration;
    mutable uint64 _duration;

    /** Internal resource information. */
public:
    struct Data {
        QString path;

        Aurora::Archive *archive = nullptr;
        bool addedArchiveMembers = false;
        uint32 archiveIndex;
    };

private:
    Data _data;

public:
    ResourceTreeItem(const QString fullPath, Aurora::Archive *archive, Aurora::Archive::Resource &resource, ResourceTreeItem *parent);
    ResourceTreeItem(const QString fullPath, ResourceTreeItem *parent);
    ~ResourceTreeItem();

    void addChild(ResourceTreeItem *child);
    bool insertChild(int position, ResourceTreeItem *child);
    void setParent(ResourceTreeItem *parent);
    int childCount();
    ResourceTreeItem *child(int row);
    int row();
    bool isDir();
    QString &getPath();
    ResourceTreeItem *getParent();
    QString getName();
    void setFileInfo(const QFileInfo info);
    QFileInfo getFileInfo();
    Source getSource();
    Aurora::FileType getFileType();
    Aurora::ResourceType getResourceType();
    qint64 getSize();
    bool hasChildren();
    Images::Decoder *getImage();
    Images::Decoder *getImage(Common::SeekableReadStream &res, Aurora::FileType type);
    Common::SeekableReadStream *getResourceData() const;
    Data &getData();
    uint64 getSoundDuration() const;
    Sound::AudioStream *getAudioStream() const;
};

} // End of namespace GUI

#endif // RESOURCETREEITEM_H
