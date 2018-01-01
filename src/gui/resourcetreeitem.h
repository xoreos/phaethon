#ifndef TREEITEM_H
#define TREEITEM_H

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

// can't foward declare nested types
// so it has to be out here (for treemodel.h)
struct ArchiveInfo {
    Aurora::Archive *archive;
    bool addedArchiveMembers;
    uint32 archiveIndex;
};

class ResourceTreeItem {
public:
    struct FileInfo {
        QString fullPath; // used for QFileInfo and item info e.g. logging
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

    /** Both. **/
    const QString getData() const; // doubles as filename

    /** File info. **/
    const Aurora::FileType getFileType() const;
    const Aurora::ResourceType getResourceType() const;
    const bool isDir() const;
    const QFileInfo getFileInfo() const;
    const qint64 getSize() const;
    const QString getPath() const;
    const Source getSource() const;

    /** Resource information. **/
    ArchiveInfo &getArchive();
    Common::SeekableReadStream *getResourceData() const;
    Images::Decoder *getImage() const;
    Images::Decoder *getImage(Common::SeekableReadStream &res, Aurora::FileType type) const;
    Sound::AudioStream *getAudioStream() const;
    const uint64 getSoundDuration() const;

private:
    /** Model information. **/
    ResourceTreeItem *_parent;
    QList<ResourceTreeItem*> _children;

    /** Both. **/
    QString _data; // The filename. It's what the model displays.

    /** File information. **/
    FileInfo _fileInfo;

    /** Resource information. **/
    SoundInfo _soundInfo;
    ArchiveInfo _archiveInfo;
};

} // End of namespace GUI

#endif // TREEITEM_H
