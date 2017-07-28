#include "src/aurora/util.h"
#include "src/common/filepath.h"
#include "src/common/readfile.h"
#include "src/gui/resourcetreeitem.h"
#include "src/images/winiconimage.h"
#include "src/images/tga.h"
#include "src/images/dds.h"
#include "src/images/tpc.h"
#include "src/images/txb.h"
#include "src/images/sbm.h"

namespace GUI {

#define USTR(x) (Common::UString((x).toStdString()))

ResourceTreeItem::ResourceTreeItem(const QString fullPath, Aurora::Archive *archive, Aurora::Archive::Resource &resource, ResourceTreeItem *parent) {
    _name = TypeMan.setFileType(resource.name, resource.type).toQString();
    _source = kSourceArchiveFile;
    _parent = parent;

    _data.path = fullPath;
    _data.archive = archive;
    _data.addedArchiveMembers = false;
    _data.archiveIndex = resource.index;

    _size = archive->getResourceSize(resource.index);
    _fullPath = fullPath;
    _isDir = false;
    _fileType = TypeMan.getFileType(USTR(_name));
    _resourceType = TypeMan.getResourceType(USTR(_name));

   _triedDuration = getResourceType() != Aurora::kResourceSound;
   _duration = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::ResourceTreeItem(const QString fullPath, ResourceTreeItem *parent) :
    _children(QList<ResourceTreeItem*>()), _parent(parent), _fullPath(fullPath) {

    _data.archive = 0;
    _data.addedArchiveMembers = false;
    _data.archiveIndex = 0xFFFFFFFF;

    if (!fullPath.isEmpty())
        _fullPath = fullPath;

    QFileInfo info(_fullPath);

    _isDir = info.isDir();
    _size = Common::kFileInvalid;
    if (_isDir)
        _size = info.size();
    _source = _isDir ? kSourceDirectory : kSourceFile;

    _name = info.fileName();

    if (_source == kSourceDirectory) {
        _fileType = Aurora::kFileTypeNone;
        _resourceType = Aurora::kResourceNone;
    }
    else {
        _fileType = TypeMan.getFileType(USTR(_name));
        _resourceType = TypeMan.getResourceType(USTR(_name));
    }
    _triedDuration = getResourceType() != Aurora::kResourceSound;
    _duration = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::~ResourceTreeItem() {
    if (_children.count())
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

bool ResourceTreeItem::isDir() {
    return _isDir;
}

QString &ResourceTreeItem::getPath() {
    return _fullPath;
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

QFileInfo ResourceTreeItem::getFileInfo() {
    return QFileInfo(_fullPath);
}

qint64 ResourceTreeItem::getSize() {
    return _size;
}

Source ResourceTreeItem::getSource() {
    return _source;
}

Aurora::FileType ResourceTreeItem::getFileType() {
    return _fileType;
}

Aurora::ResourceType ResourceTreeItem::getResourceType() {
    return _resourceType;
}

bool ResourceTreeItem::hasChildren() {
    return _children.count();
}

Common::SeekableReadStream *ResourceTreeItem::getResourceData() const {
    try {
        switch (_source) {
            case kSourceDirectory:
                throw Common::Exception("Can't get file data of a directory");

            case kSourceFile:
                return new Common::ReadFile(_fullPath.toStdString().c_str());

            case kSourceArchiveFile:
                if (!_data.archive)
                    throw Common::Exception("No archive opened");

                return _data.archive->getResource(_data.archiveIndex);
        }
    } catch (Common::Exception &e) {
        e.add("Failed to get resource data for resource \"%s\"", _name.toStdString().c_str());
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
        img = getImage(*res, getFileType());
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

ResourceTreeItem::Data &ResourceTreeItem::getData() {
    return _data;
}

uint64 ResourceTreeItem::getSoundDuration() const {
    if (_triedDuration)
        return _duration;

    _triedDuration = true;

    try {
        Common::ScopedPtr<Sound::AudioStream> sound(getAudioStream());

        Sound::RewindableAudioStream &rewSound = dynamic_cast<Sound::RewindableAudioStream &>(*sound);
        _duration = rewSound.getDuration();

    } catch (...) {
    }

    return _duration;
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
