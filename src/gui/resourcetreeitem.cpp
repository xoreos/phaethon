#include "src/gui/resourcetreeitem.h"

#include "src/aurora/util.h"

#include "src/common/filepath.h"
#include "src/common/readfile.h"

#include "src/images/dds.h"
#include "src/images/sbm.h"
#include "src/images/tga.h"
#include "src/images/tpc.h"
#include "src/images/txb.h"
#include "src/images/winiconimage.h"

namespace GUI {

ResourceTreeItem::ResourceTreeItem(Aurora::Archive *archive, Aurora::Archive::Resource &resource, ResourceTreeItem *parent)
    : _parent(parent)
{
    _fileInfo.fileName     = TypeMan.setFileType(resource.name, resource.type).toQString();
    _fileInfo.fullPath     = parent->getPath() + "/" + _fileInfo.fileName;
    _fileInfo.isDir        = false;
    _fileInfo.source       = kSourceArchiveFile;
    _fileInfo.fileType     = TypeMan.getFileType(Common::UString(_fileInfo.fileName.toStdString()));
    _fileInfo.resourceType = TypeMan.getResourceType(Common::UString(_fileInfo.fileName.toStdString()));
    _fileInfo.size         = archive->getResourceSize(resource.index);

    _archiveInfo.archive             = archive;
    _archiveInfo.addedArchiveMembers = false;
    _archiveInfo.archiveIndex        = resource.index;

    _soundInfo.triedDuration = _fileInfo.resourceType != Aurora::kResourceSound;
    _soundInfo.duration      = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::ResourceTreeItem(const QString fullPath, ResourceTreeItem *parent)
    : _parent(parent)
{
    QFileInfo info(fullPath);

    _fileInfo.fileName = info.fileName();
    _fileInfo.fullPath = fullPath;
    _fileInfo.source   = info.isDir() ? kSourceDirectory : kSourceFile;
    _fileInfo.isDir    = info.isDir();
    _fileInfo.size     = _fileInfo.isDir ? Common::kFileInvalid : info.size();
    if (_fileInfo.isDir) {
        _fileInfo.fileType = Aurora::kFileTypeNone;
        _fileInfo.resourceType = Aurora::kResourceNone;
    }
    else {
        _fileInfo.fileType = TypeMan.getFileType(Common::UString(_fileInfo.fileName.toStdString()));
        _fileInfo.resourceType = TypeMan.getResourceType(Common::UString(_fileInfo.fileName.toStdString()));
    }

    _archiveInfo.archive = 0;
    _archiveInfo.addedArchiveMembers = false;
    _archiveInfo.archiveIndex = 0xFFFFFFFF;

    _soundInfo.triedDuration = _fileInfo.resourceType != Aurora::kResourceSound;
    _soundInfo.duration = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::~ResourceTreeItem() {
    if (_children.count())
        qDeleteAll(_children);
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

ResourceTreeItem *ResourceTreeItem::childAt(int row) {
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

bool ResourceTreeItem::isDir() {
    return _fileInfo.isDir;
}

QString &ResourceTreeItem::getPath() {
    return _fileInfo.fullPath;
}

ResourceTreeItem *ResourceTreeItem::parent() {
    return _parent;
}

void ResourceTreeItem::setParent(ResourceTreeItem *parent) {
    _parent = parent;
}

QString ResourceTreeItem::getName() {
    return _fileInfo.fileName;
}

QFileInfo ResourceTreeItem::getFileInfo() {
    return QFileInfo(_fileInfo.fullPath);
}

qint64 ResourceTreeItem::getSize() {
    return _fileInfo.size;
}

ResourceTreeItem::Source ResourceTreeItem::getSource() {
    return _fileInfo.source;
}

Aurora::FileType ResourceTreeItem::getFileType() {
    return _fileInfo.fileType;
}

Aurora::ResourceType ResourceTreeItem::getResourceType() {
    return _fileInfo.resourceType;
}

bool ResourceTreeItem::hasChildren() {
    return _children.count();
}

Common::SeekableReadStream *ResourceTreeItem::getResourceData() const {
    try {
        switch (_fileInfo.source) {
            case kSourceDirectory:
                throw Common::Exception("Can't get file data of a directory");

            case kSourceFile:
                return new Common::ReadFile(_fileInfo.fullPath.toStdString().c_str());

            case kSourceArchiveFile:
                if (!_archiveInfo.archive)
                    throw Common::Exception("No archive opened");

                return _archiveInfo.archive->getResource(_archiveInfo.archiveIndex);
        }
    } catch (Common::Exception &e) {
        e.add("Failed to get resource data for resource \"%s\"", _fileInfo.fileName.toStdString().c_str());
        throw;
    }

    assert(false);
    return 0;
}

Images::Decoder *ResourceTreeItem::getImage() {
    if (getResourceType() != Aurora::kResourceImage)
        throw Common::Exception("\"%s\" is not an image resource", getName().toStdString().c_str());

    Common::ScopedPtr<Common::SeekableReadStream> res(getResourceData());

    Images::Decoder *img = 0;
    try {
        img = getImage(*res, _fileInfo.fileType);
    } catch (Common::Exception &e) {
        e.add("Failed to get image from \"%s\"", getName().toStdString().c_str());
        throw;
    }

    return img;
}

Images::Decoder *ResourceTreeItem::getImage(Common::SeekableReadStream &res, Aurora::FileType type) {
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

ResourceTreeItem::ArchiveInfo &ResourceTreeItem::getData() {
    return _archiveInfo;
}

uint64 ResourceTreeItem::getSoundDuration() const {
    if (_soundInfo.triedDuration)
        return _soundInfo.duration;

    _soundInfo.triedDuration = true;

    try {
        Common::ScopedPtr<Sound::AudioStream> sound(getAudioStream());

        Sound::RewindableAudioStream &rewSound = dynamic_cast<Sound::RewindableAudioStream &>(*sound);
        _soundInfo.duration = rewSound.getDuration();

    } catch (...) {
    }

    return _soundInfo.duration;
}

Sound::AudioStream *ResourceTreeItem::getAudioStream() const {
    if (_fileInfo.resourceType != Aurora::kResourceSound)
        throw Common::Exception("\"%s\" is not a sound resource", _fileInfo.fileName.toStdString().c_str());

    Common::ScopedPtr<Common::SeekableReadStream> res(getResourceData());

    Sound::AudioStream *sound = 0;
    try {
        sound = SoundMan.makeAudioStream(res.get());
    } catch (Common::Exception &e) {
        e.add("Failed to get audio stream from \"%s\"", _fileInfo.fileName.toStdString().c_str());
        throw;
    }

    res.release();
    return sound;
}

} // End of namespace GUI
