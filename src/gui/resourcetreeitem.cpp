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

ResourceTreeItem::ResourceTreeItem(const Common::FileTree::Entry &entry) :
	_parent(0), _name(QString::fromUtf8(entry.name.c_str())),
	_source(entry.isDirectory() ? kSourceDirectory : kSourceFile) {

	_path = QString::fromUtf8(entry.path.string().c_str());

	_archive.data = 0;
	_archive.addedMembers = false;
	_archive.index = 0xFFFFFFFF;

	_size = Common::kFileInvalid;
	if (_source == kSourceFile)
		_size = Common::FilePath::getFileSize(entry.path.generic_string().c_str());

	if (_source == kSourceDirectory)
		_fileType = Aurora::kFileTypeNone;
	else
		_fileType = TypeMan.getFileType(_name.toStdString());

	if (_source == kSourceDirectory)
		_resourceType = Aurora::kResourceNone;
	else
		_resourceType = TypeMan.getResourceType(_name.toStdString());

	_triedDuration = getResourceType() != Aurora::kResourceSound;
	_duration = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::ResourceTreeItem(Aurora::Archive *archive, const Aurora::Archive::Resource &resource) :
	_parent(0), _name(QString::fromUtf8(TypeMan.setFileType(resource.name, resource.type).c_str())),
	_source(kSourceArchiveFile) {

	_archive.data = archive;
	_archive.addedMembers = false;
	_archive.index = resource.index;

	_size = archive->getResourceSize(resource.index);

	if (_source == kSourceDirectory)
		_fileType = Aurora::kFileTypeNone;
	else
		_fileType = TypeMan.getFileType(_name.toStdString());

	if (_source == kSourceDirectory)
		_resourceType = Aurora::kResourceNone;
	else
		_resourceType = TypeMan.getResourceType(_name.toStdString());

	_triedDuration = getResourceType() != Aurora::kResourceSound;
	_duration = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::ResourceTreeItem(const QString &data) : _parent(0) {
	_name = data;
}

ResourceTreeItem::~ResourceTreeItem() {
}

void ResourceTreeItem::addChild(ResourceTreeItem *child) {
	child->setParent(this);
	_children.push_back(std::unique_ptr<ResourceTreeItem>(child));
}

bool ResourceTreeItem::insertChild(size_t position, ResourceTreeItem *child) {
	if (position >= _children.size())
		return false;

	_children.insert(_children.begin() + position, std::unique_ptr<ResourceTreeItem>(child));

	return true;
}

ResourceTreeItem *ResourceTreeItem::childAt(int row) const {
	return _children[row].get();
}

int ResourceTreeItem::childCount() const {
	return _children.size();
}

int ResourceTreeItem::row() const {
	if (_parent) {
		for (size_t i = 0; i < _parent->_children.size(); i++) {
			if (_parent->_children[i].get() == this)
				return i;
		}
	}
	return 0;
}

ResourceTreeItem *ResourceTreeItem::getParent() const {
	return _parent;
}

void ResourceTreeItem::setParent(ResourceTreeItem *parent) {
	_parent = parent;
}

bool ResourceTreeItem::hasChildren() const {
	return _children.size();
}

const QString &ResourceTreeItem::getName() const {
	return _name;
}

bool ResourceTreeItem::isDir() const {
	return _source == kSourceDirectory;
}

const QString &ResourceTreeItem::getPath() const {
	return _path;
}

qint64 ResourceTreeItem::getSize() const {
	return _size;
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
				return new Common::ReadFile(_path.toStdString().c_str());

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
