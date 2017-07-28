#ifndef TREEITEM_H
#define TREEITEM_H

#include <QFileInfo>
#include <QString>

#include "src/aurora/archive.h"
#include "src/aurora/util.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

#include "src/images/decoder.h"


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
        QString fileName; // this is what is displayed in the tree view
        QString fullPath; // used for QFileInfo and item info such as logging
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
    ResourceTreeItem(QString fullPath, ResourceTreeItem *parent);
    ~ResourceTreeItem();

    /** Model structure. **/
    const bool hasChildren() const;
    bool insertChild(int position, ResourceTreeItem *child);
    const int childCount() const;
    const int row() const;
    ResourceTreeItem *childAt(int row) const;
    ResourceTreeItem *getParent() const;
    void appendChild(ResourceTreeItem *child);
    void setParent(ResourceTreeItem *parent);

    /** File info. **/
    const Aurora::FileType getFileType() const;
    const Aurora::ResourceType getResourceType() const;
    const bool isDir() const;
    const QFileInfo getFileInfo() const;
    const qint64 getSize() const;
    const QString getPath() const;
    const QString getName() const;
    const Source getSource() const;

    /** Resource information. **/
    ArchiveInfo &getData();
    Common::SeekableReadStream *getResourceData() const;
    Images::Decoder *getImage() const;
    Images::Decoder *getImage(Common::SeekableReadStream &res, Aurora::FileType type) const;
    Sound::AudioStream *getAudioStream() const;
    const uint64 getSoundDuration() const;

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

#endif // TREEITEM_H
