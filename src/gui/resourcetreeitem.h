#ifndef RESOURCETREEITEM_H
#define RESOURCETREEITEM_H

#include <QFileInfo>
#include <QString>

#include "src/aurora/archive.h"
#include "src/aurora/util.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

#include "src/images/decoder.h"

namespace GUI {

class ResourceTreeItem {
public:
    enum Source {
        kSourceDirectory= 0,
        kSourceFile = 1,
        kSourceArchive = 2,
        kSourceArchiveFile = 3
    };

    struct ArchiveInfo {
        Aurora::Archive *archive;
        bool addedArchiveMembers;
        uint32 archiveIndex;
    };

    struct FileInfo {
        QString fileName;
        QString fullPath;
        bool isDir;
        Source source;
        Aurora::FileType fileType;
        Aurora::ResourceType resourceType;
        qint64 size;
    };

    struct SoundInfo {
        mutable bool triedDuration;
        mutable uint64 duration;
    };


    ResourceTreeItem(Aurora::Archive *archive, Aurora::Archive::Resource &resource, ResourceTreeItem *parent);
    ResourceTreeItem(const QString fullPath, ResourceTreeItem *parent);
    ~ResourceTreeItem();

    /** Model structure. **/
    bool hasChildren();
    bool insertChild(int position, ResourceTreeItem *child);
    int childCount();
    int row();
    ResourceTreeItem *childAt(int row);
    ResourceTreeItem *parent();
    void appendChild(ResourceTreeItem *child);
    void setParent(ResourceTreeItem *parent);

    /** File info. **/
    Aurora::FileType getFileType();
    Aurora::ResourceType getResourceType();
    bool isDir();
    QFileInfo getFileInfo();
    qint64 getSize();
    QString &getPath();
    QString getName();
    Source getSource();

    /** Resource information. **/
    ArchiveInfo &getData();
    Common::SeekableReadStream *getResourceData() const;
    Images::Decoder *getImage();
    Images::Decoder *getImage(Common::SeekableReadStream &res, Aurora::FileType type);
    Sound::AudioStream *getAudioStream() const;
    uint64 getSoundDuration() const;

private:
    /** Model information. **/
    ResourceTreeItem *_parent;
    QList<ResourceTreeItem*> _children;

    /** File information. **/
    FileInfo _fileInfo;

    /** Resource information. **/
    SoundInfo _soundInfo;
    ArchiveInfo _archiveInfo;
};

} // End of namespace GUI

#endif // RESOURCETREEITEM_H
