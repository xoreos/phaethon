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

#include "src/common/filepath.h"
#include "src/common/readfile.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

// Data for items within an archive
FSData::FSData(const QString &parentPath, const QString &fileName, Aurora::Archive *archiveData, Aurora::Archive::Resource &resource) {
    _fullPath     = parentPath + "/" + fileName;
    _isDir        = false;
    _size         = archiveData->getResourceSize(resource.index);
}

// Data for items which are actually on the filesystem
FSData::FSData(const QString &fullPath, const QFileInfo &info) {
    _fullPath = fullPath;
    _isDir    = info.isDir();
    _size     = _isDir ? Common::kFileInvalid : info.size();
}

// Items within an archive
ResourceTreeItem::ResourceTreeItem(Aurora::Archive *archiveData, Aurora::Archive::Resource &resource, ResourceTreeItem *parent)
    : _parent(parent)
{
    QString fileName = QString::fromUtf8(TypeMan.setFileType(resource.name, resource.type).c_str());
    _name = fileName;

    _fsData = new FSData(parent->getPath(), fileName);

    _source       = kSourceArchiveFile;
    _fileType     = TypeMan.getFileType(fileName.toStdString().c_str());
    _resourceType = TypeMan.getResourceType(fileName.toStdString().c_str());

    _soundData.triedDuration = _resourceType != Aurora::kResourceSound;
    _soundData.duration      = Sound::RewindableAudioStream::kInvalidLength;

    _archive.data         = archiveData;
    _archive.addedMembers = false;
    _archive.index        = resource.index;
}

// Items which are actually on the filesystem
ResourceTreeItem::ResourceTreeItem(QString fullPath, ResourceTreeItem *parent)
    : _parent(parent)
{
    QFileInfo info(fullPath);
    _name = info.fileName();

    _fsData = new FSData(fullPath, info);

    _source = info.isDir() ? kSourceDirectory : kSourceFile;

    if (info.isDir()) {
        _fileType = Aurora::kFileTypeNone;
        _resourceType = Aurora::kResourceNone;
    }
    else {
        _fileType = TypeMan.getFileType(info.fileName().toStdString().c_str());
        _resourceType = TypeMan.getResourceType(info.fileName().toStdString().c_str());
    }

    _archive.data         = nullptr;
    _archive.addedMembers = false;
    _archive.index        = 0xFFFFFFFF;
}

ResourceTreeItem::~ResourceTreeItem() {
    qDeleteAll(_children);
    _children.clear();
    delete _fsData;
}

void ResourceTreeItem::appendChild(ResourceTreeItem *child) {
    _children << child;
}

bool ResourceTreeItem::insertChild(int position, ResourceTreeItem *child) {
    if (position < 0 or position >= _children.count())
        return false;

    _children.insert(position, child);

    return true;
}

ResourceTreeItem *ResourceTreeItem::childAt(int row) const {
    return _children.at(row);
}

int ResourceTreeItem::childCount() const {
    return _children.count();
}

int ResourceTreeItem::row() const {
    if (_parent)
        return _parent->_children.indexOf(const_cast<ResourceTreeItem*>(this));
    return 0;
}

ResourceTreeItem *ResourceTreeItem::getParent() const {
    return _parent;
}

void ResourceTreeItem::setParent(ResourceTreeItem *parent) {
    _parent = parent;
}

bool ResourceTreeItem::hasChildren() const {
    return _children.count();
}

const QString &ResourceTreeItem::getName() const {
    return _name;
}

/* Data. */

bool ResourceTreeItem::isDir() const {
    return _fsData->_isDir;
}

const QString &ResourceTreeItem::getPath() const {
    return _fsData->_fullPath;
}

qint64 ResourceTreeItem::getSize() const {
    return _fsData->_size;
}

Source ResourceTreeItem::getSource() const {
    return _source;
}

Aurora::FileType ResourceTreeItem::getFileType() const {
    return _fileType;
}

Aurora::ResourceType ResourceTreeItem::getResourceType() const {
    return _resourceType;
}

Common::SeekableReadStream *ResourceTreeItem::getResourceData() const {
    try {
        switch (_source) {
            case kSourceDirectory:
                throw Common::Exception("Can't get file data of a directory");

            case kSourceFile:
                return new Common::ReadFile(_fsData->_fullPath.toStdString().c_str());

            case kSourceArchiveFile:
                if (!_archive.data)
                    throw Common::Exception("No archive opened");

                return _archive.data->getResource(_archive.index);
            default:
                throw Common::Exception("kSourceArchive is not handled by getResourceData");
        }
    } catch (Common::Exception &e) {
        e.add("Failed to get resource data for resource \"%s\"", _name.toStdString().c_str());
        throw;
    }

    assert(false);
    return 0;
}

Images::Decoder *ResourceTreeItem::getImage() const {
    if (getResourceType() != Aurora::kResourceImage)
        throw Common::Exception("\"%s\" is not an image resource", getName().toStdString().c_str());

    Common::ScopedPtr<Common::SeekableReadStream> res(getResourceData());

    Images::Decoder *img = 0;
    try {
        img = getImage(*res, _fileType);
    } catch (Common::Exception &e) {
        e.add("Failed to get image from \"%s\"", getName().toStdString().c_str());
        throw;
    }

    return img;
}

Images::Decoder *ResourceTreeItem::getImage(Common::SeekableReadStream &res, Aurora::FileType type) const {
    Images::Decoder *img = 0;
    switch (type) {
        case Aurora::kFileTypeDDS:
            img = new Images::DDS(res);
            break;

        case Aurora::kFileTypeTPC:
            img = new Images::TPC(res);
            break;

        // TXB may be actually TPC
        case Aurora::kFileTypeTXB:
        case Aurora::kFileTypeTXB2:
            try {
                img = new Images::TXB(res);
            } catch (Common::Exception &e1) {

                try {
                    res.seek(0);
                    img = new Images::TPC(res);

                } catch (Common::Exception &e2) {
                    e1.add(e2);

                    throw e1;
                }
            }
            break;

        case Aurora::kFileTypeTGA:
            img = new Images::TGA(res);
            break;

        case Aurora::kFileTypeSBM:
            img = new Images::SBM(res);
            break;

        case Aurora::kFileTypeCUR:
        case Aurora::kFileTypeCURS:
            img = new Images::WinIconImage(res);
            break;

        default:
            throw Common::Exception("Unsupported image type %d", type);
    }

    return img;
}

Archive &ResourceTreeItem::getArchive() {
    return _archive;
}

uint64 ResourceTreeItem::getSoundDuration() const {
    if (_soundData.triedDuration)
        return _soundData.duration;

    _soundData.triedDuration = true;

    try {
        Common::ScopedPtr<Sound::AudioStream> sound(getAudioStream());

        Sound::RewindableAudioStream &rewSound = dynamic_cast<Sound::RewindableAudioStream &>(*sound);
        _soundData.duration = rewSound.getDuration();

    } catch (...) {
    }

    return _soundData.duration;
}

Sound::AudioStream *ResourceTreeItem::getAudioStream() const {
    if (_resourceType != Aurora::kResourceSound)
        throw Common::Exception("\"%s\" is not a sound resource", _name.toStdString().c_str());

    Common::ScopedPtr<Common::SeekableReadStream> res(getResourceData());

    Sound::AudioStream *sound = 0;
    try {
        sound = SoundMan.makeAudioStream(res.get());
    } catch (Common::Exception &e) {
        e.add("Failed to get audio stream from \"%s\"", _name.toStdString().c_str());
        throw;
    }

    res.release();
    return sound;
}

} // End of namespace GUI
