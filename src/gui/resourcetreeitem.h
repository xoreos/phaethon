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
 *  Items that make up Phaethon's resource tree.
 */

#ifndef RESOURCETREEITEM_H
#define RESOURCETREEITEM_H

#include <QFileInfo>
#include <QString>

#include "src/aurora/archive.h"
#include "src/aurora/util.h"
#include "src/common/filetree.h"
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

class ResourceTreeItem {
private:
    ResourceTreeItem *_parent;
    QList<ResourceTreeItem*> _children;
    QString _name; // The filename. It's what the tree view displays.

    QString _path;
    qint64 _size;

    mutable bool _triedDuration;
    mutable uint64 _duration;

    Archive _archive;

    Source _source;
    Aurora::FileType _fileType;
    Aurora::ResourceType _resourceType;

public:
    /** Filesystem item. */
    ResourceTreeItem(const Common::FileTree::Entry &entry);
    /** Archive item. */
    ResourceTreeItem(Aurora::Archive *archive, const Aurora::Archive::Resource &resource);
    /** Root item. */
    ResourceTreeItem(const QString &data);
    
    ~ResourceTreeItem();

    inline bool isArchive() {
        return _fileType == Aurora::kFileTypeZIP ||
               _fileType == Aurora::kFileTypeERF ||
               _fileType == Aurora::kFileTypeMOD ||
               _fileType == Aurora::kFileTypeNWM ||
               _fileType == Aurora::kFileTypeSAV ||
               _fileType == Aurora::kFileTypeHAK ||
               _fileType == Aurora::kFileTypeRIM ||
               _fileType == Aurora::kFileTypeKEY;
    }

    /** Model structure. **/
    bool hasChildren() const;
    bool insertChild(int position, ResourceTreeItem *child);
    int childCount() const;
    int row() const;
    ResourceTreeItem *childAt(int row) const;
    ResourceTreeItem *getParent() const;
    void addChild(ResourceTreeItem *child);
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
};

} // End of namespace GUI

#endif // RESOURCETREEITEM_H
