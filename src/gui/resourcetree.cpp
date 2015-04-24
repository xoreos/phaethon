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
 *  Phaethon's tree of game resource files.
 */

#include <wx/artprov.h>
#include <wx/imaglist.h>

#include "src/common/error.h"
#include "src/common/stream.h"
#include "src/common/file.h"
#include "src/common/filepath.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"

#include "src/images/winiconimage.h"
#include "src/images/tga.h"
#include "src/images/dds.h"
#include "src/images/tpc.h"
#include "src/images/txb.h"
#include "src/images/sbm.h"

#include "src/aurora/util.h"

#include "src/gui/resourcetree.h"
#include "src/gui/eventid.h"
#include "src/gui/mainwindow.h"

namespace GUI {

ResourceTreeItem::ResourceTreeItem(const Common::FileTree::Entry &entry) :
	_name(entry.name), _source(entry.isDirectory() ? kSourceDirectory : kSourceFile) {

	_data.path = entry.path;

	_data.archive = 0;
	_data.addedArchiveMembers = false;
	_data.archiveIndex = 0xFFFFFFFF;

	_size = Common::kFileInvalid;
	if (_source == kSourceFile)
		_size = Common::FilePath::getFileSize(entry.path.c_str());

	_triedDuration = getResourceType() != Aurora::kResourceSound;
	_duration = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::ResourceTreeItem(Aurora::Archive *archive, const Aurora::Archive::Resource &resource) :
	_name(TypeMan.setFileType(resource.name, resource.type)), _source(kSourceArchiveFile) {

	_data.archive = archive;
	_data.addedArchiveMembers = false;
	_data.archiveIndex = resource.index;

	_size = archive->getResourceSize(resource.index);

	_triedDuration = getResourceType() != Aurora::kResourceSound;
	_duration = Sound::RewindableAudioStream::kInvalidLength;
}

ResourceTreeItem::~ResourceTreeItem() {
}

const Common::UString &ResourceTreeItem::getName() const {
	return _name;
}

uint32 ResourceTreeItem::getSize() const {
	return _size;
}

ResourceTreeItem::Source ResourceTreeItem::getSource() const {
	return _source;
}

ResourceTreeItem::Data &ResourceTreeItem::getData() {
	return _data;
}

Aurora::FileType ResourceTreeItem::getFileType() const {
	if (_source == kSourceDirectory)
		return Aurora::kFileTypeNone;

	return TypeMan.getFileType(_name);
}

Aurora::ResourceType ResourceTreeItem::getResourceType() const {
	if (_source == kSourceDirectory)
		return Aurora::kResourceNone;

	return TypeMan.getResourceType(_name);
}

Common::SeekableReadStream *ResourceTreeItem::getResourceData() const {
	try {
		switch (_source) {
			case kSourceDirectory:
				throw Common::Exception("Can't get file data of a directory");

			case kSourceFile:
				return new Common::File(_data.path.c_str());

			case kSourceArchiveFile:
				if (!_data.archive)
					throw Common::Exception("No archive opened");

				return _data.archive->getResource(_data.archiveIndex);
		}
	} catch (Common::Exception &e) {
		e.add("Failed to get resource data for resource \"%s\"", _name.c_str());
		throw;
	}

	assert(false);
	return 0;
}

Sound::AudioStream *ResourceTreeItem::getAudioStream() const {
	if (getResourceType() != Aurora::kResourceSound)
		throw Common::Exception("\"%s\" is not a sound resource", getName().c_str());

	Common::SeekableReadStream *res = getResourceData();
	Sound::AudioStream *sound = 0;
	try {
		sound = SoundMan.makeAudioStream(res);
	} catch (Common::Exception &e) {
		e.add("Failed to get audio stream from \"%s\"", getName().c_str());
		delete res;
		throw;
	}

	return sound;
}

Images::Decoder *ResourceTreeItem::getImage() const {
	if (getResourceType() != Aurora::kResourceImage)
		throw Common::Exception("\"%s\" is not an image resource", getName().c_str());

	Common::SeekableReadStream *res = getResourceData();
	Images::Decoder            *img = 0;

	try {
		img = getImage(*res, getFileType());
	} catch (Common::Exception &e) {
		e.add("Failed to get image from \"%s\"", getName().c_str());
		delete res;
		throw;
	}

	delete res;
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

uint64 ResourceTreeItem::getSoundDuration() const {
	if (_triedDuration)
		return _duration;

	_triedDuration = true;

	Sound::AudioStream *sound = 0;
	try {
		sound = getAudioStream();
	} catch (...) {
		return _duration;
	}

	Sound::RewindableAudioStream *rewSound = dynamic_cast<Sound::RewindableAudioStream *>(sound);
	if (rewSound)
		_duration = rewSound->getDuration();

	delete sound;
	return _duration;
}


wxBEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(kEventResourceTree, ResourceTree::onSelChanged)
	EVT_TREE_ITEM_EXPANDING(kEventResourceTree, ResourceTree::onItemExpanding)
	EVT_TREE_ITEM_ACTIVATED(kEventResourceTree, ResourceTree::onItemActivated)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(ResourceTree, wxTreeCtrl);
ResourceTree::ResourceTree() : _mainWindow(0) {
}

ResourceTree::ResourceTree(wxWindow *parent, MainWindow &mainWindow) :
	wxTreeCtrl(parent, kEventResourceTree, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_SINGLE),
	_mainWindow(&mainWindow) {

	wxIcon icon[kImageMAX];

	icon[kImageDir]  = wxArtProvider::GetIcon(wxART_FOLDER     , wxART_LIST);
	icon[kImageFile] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_LIST);

	wxImageList *images = new wxImageList(icon[0].GetWidth(), icon[0].GetHeight(), true);

	for (int i = 0; i < kImageMAX; i++)
		images->Add(icon[i]);

	AssignImageList(images);
}

ResourceTree::~ResourceTree() {
}

void ResourceTree::populate(const Common::FileTree::Entry &root) {
	wxTreeItemId treeRoot = addRoot(new ResourceTreeItem(root));

	populate(root, treeRoot);
	Expand(treeRoot);

	_mainWindow->resourceSelect(getItemData(treeRoot));
}

void ResourceTree::populate(const Common::FileTree::Entry &e, wxTreeItemId t) {
	for (std::list<Common::FileTree::Entry>::const_iterator c = e.children.begin();
	     c != e.children.end(); ++c) {

		wxTreeItemId cT = appendItem(t, new ResourceTreeItem(*c));
		populate(*c, cT);
	}

	SortChildren(t);
}

int ResourceTree::OnCompareItems(const wxTreeItemId &item1, const wxTreeItemId &item2) {
	ResourceTreeItem *d1 = getItemData(item1);
	ResourceTreeItem *d2 = getItemData(item2);

	// No data sorts before data
	if (!d1)
		return -1;
	if (!d2)
		return 1;

	// Sort by source first
	if (d1->getSource() < d2->getSource())
		return -1;
	if (d1->getSource() > d2->getSource())
		return  1;

	// Sort case insensitively by name
	return Common::UString(d1->getName().c_str()).stricmp(Common::UString(d2->getName().c_str()));
}

void ResourceTree::onSelChanged(wxTreeEvent &event) {
	assert(_mainWindow);

	_mainWindow->resourceSelect(getItemData(event.GetItem()));
}

void ResourceTree::onItemExpanding(wxTreeEvent &event) {
	ResourceTreeItem *item = getItemData(event.GetItem());
	if (!item)
		return;

	// We only need special treatment for these archives
	if ((item->getFileType() != Aurora::kFileTypeZIP) &&
	    (item->getFileType() != Aurora::kFileTypeERF) &&
	    (item->getFileType() != Aurora::kFileTypeMOD) &&
	    (item->getFileType() != Aurora::kFileTypeNWM) &&
	    (item->getFileType() != Aurora::kFileTypeSAV) &&
	    (item->getFileType() != Aurora::kFileTypeHAK) &&
	    (item->getFileType() != Aurora::kFileTypeRIM) &&
	    (item->getFileType() != Aurora::kFileTypeKEY))
		return;

	// We already added the archive members. Nothing to do
	ResourceTreeItem::Data &data = item->getData();
	if (data.addedArchiveMembers)
		return;

	_mainWindow->pushStatus(Common::UString("Loading archive ") + item->getName() + "...");

	// Load the archive, if necessary
	if (!data.archive) {
		try {
			data.archive = _mainWindow->getArchive(data.path);
		} catch (Common::Exception &e) {
			// If that fails, print the error and treat this archive as empty

			_mainWindow->popStatus();

			e.add("Failed to load archive \"%s\"", item->getName().c_str());
			Common::printException(e, "WARNING: ");

			event.Veto();
			SetItemHasChildren(event.GetItem(), false);
			return;
		}
	}

	const Aurora::Archive::ResourceList &resources = data.archive->getResources();
	for (Aurora::Archive::ResourceList::const_iterator r = resources.begin(); r != resources.end(); ++r)
		appendItem(event.GetItem(), new ResourceTreeItem(data.archive, *r));

	_mainWindow->popStatus();
	data.addedArchiveMembers = true;
}

void ResourceTree::onItemActivated(wxTreeEvent &event) {
	assert(_mainWindow);

	const ResourceTreeItem *item = getItemData(event.GetItem());
	if (!item)
		return;

	_mainWindow->resourceActivate(*item);
}

ResourceTree::Image ResourceTree::getImage(const ResourceTreeItem &item) {
	switch (item.getSource()) {
		case ResourceTreeItem::kSourceDirectory:
			return kImageDir;

		case ResourceTreeItem::kSourceFile:
			return kImageFile;

		case ResourceTreeItem::kSourceArchiveFile:
			return kImageFile;

		default:
			break;
	}

	return kImageNone;
}

ResourceTreeItem *ResourceTree::getItemData(const wxTreeItemId &id) const {
	return dynamic_cast<ResourceTreeItem *>(GetItemData(id));
}

ResourceTreeItem *ResourceTree::getSelection() const {
	wxTreeItemId id;
	return getSelection(id);
}

ResourceTreeItem *ResourceTree::getSelection(wxTreeItemId &id) const {
	id = GetSelection();
	if (!id.IsOk())
		return 0;

	return getItemData(id);
}

void ResourceTree::forceArchiveChildren(const ResourceTreeItem &item, wxTreeItemId id) {
	// We want archive to be expandable
	if ((item.getSource() == ResourceTreeItem::kSourceFile) &&
	    ((item.getFileType() == Aurora::kFileTypeZIP) ||
	     (item.getFileType() == Aurora::kFileTypeERF) ||
	     (item.getFileType() == Aurora::kFileTypeMOD) ||
	     (item.getFileType() == Aurora::kFileTypeNWM) ||
	     (item.getFileType() == Aurora::kFileTypeSAV) ||
	     (item.getFileType() == Aurora::kFileTypeHAK) ||
	     (item.getFileType() == Aurora::kFileTypeRIM) ||
	     (item.getFileType() == Aurora::kFileTypeKEY)))
		SetItemHasChildren(id, true);
}

wxTreeItemId ResourceTree::addRoot(ResourceTreeItem *item) {
	assert(item);

	wxTreeItemId id = AddRoot(item->getName(), getImage(*item), getImage(*item), item);

	forceArchiveChildren(*item, id);

	return id;
}

wxTreeItemId ResourceTree::appendItem(wxTreeItemId parent, ResourceTreeItem *item) {
	assert(item);

	wxTreeItemId id = AppendItem(parent, item->getName(), getImage(*item), getImage(*item), item);

	forceArchiveChildren(*item, id);

	return id;
}

} // End of namespace GUI
