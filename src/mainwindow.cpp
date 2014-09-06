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

/** @file mainwindow.cpp
 *  Phaethon's main window.
 */

#include <wx/menu.h>
#include <wx/splitter.h>
#include <wx/artprov.h>
#include <wx/imaglist.h>

#include "common/ustring.h"
#include "common/version.h"
#include "common/util.h"
#include "common/error.h"
#include "common/filepath.h"
#include "common/stream.h"
#include "common/file.h"

#include "aurora/util.h"
#include "aurora/zipfile.h"
#include "aurora/erffile.h"
#include "aurora/rimfile.h"
#include "aurora/keyfile.h"
#include "aurora/keydatafile.h"
#include "aurora/biffile.h"
#include "aurora/bzffile.h"

#include "cline.h"
#include "eventid.h"
#include "about.h"
#include "mainwindow.h"


ResourceTreeItem::ResourceTreeItem(const Common::FileTree::Entry &entry) :
	_name(entry.name), _source(entry.isDirectory() ? kSourceDirectory : kSourceFile) {

	_data.path = entry.path;

	_data.archive = 0;
	_data.addedArchiveMembers = false;
	_data.archiveIndex = 0xFFFFFFFF;

	_size = Common::kFileInvalid;
	if (_source == kSourceFile)
		_size = Common::FilePath::getFileSize(entry.path.c_str());
}

ResourceTreeItem::ResourceTreeItem(Aurora::Archive *archive, const Aurora::Archive::Resource &resource) :
	_name(TypeMan.setFileType(resource.name, resource.type)), _source(kSourceArchiveFile) {

	_data.archive = archive;
	_data.addedArchiveMembers = false;
	_data.archiveIndex = resource.index;

	_size = archive->getResourceSize(resource.index);
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
	return TypeMan.getFileType(_name);
}

Aurora::ResourceType ResourceTreeItem::getResourceType() const {
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


wxBEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(kEventResourceTree, ResourceTree::onSelChanged)
	EVT_TREE_ITEM_EXPANDING(kEventResourceTree, ResourceTree::onItemExpanding)
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

	_mainWindow->resourceTreeSelect(getItemData(event.GetItem()));
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

	_mainWindow->GetStatusBar()->PushStatusText(Common::UString("Loading archive ") + item->getName() + "...");

	// Load the archive, if necessary
	if (!data.archive) {
		try {
			data.archive = _mainWindow->getArchive(data.path);
		} catch (Common::Exception &e) {
			// If that fails, print the error and treat this archive as empty

			_mainWindow->GetStatusBar()->PopStatusText();

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

	_mainWindow->GetStatusBar()->PopStatusText();
	data.addedArchiveMembers = true;
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

void MainWindow::populateTree(const Common::FileTree::Entry &e, wxTreeItemId t) {
	for (std::list<Common::FileTree::Entry>::const_iterator c = e.children.begin();
	     c != e.children.end(); ++c) {

		wxTreeItemId cT = _resourceTree->appendItem(t, new ResourceTreeItem(*c));
		populateTree(*c, cT);
	}

	_resourceTree->SortChildren(t);
}


wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(kEventFileOpenDir , MainWindow::onOpenDir)
	EVT_MENU(kEventFileOpenFile, MainWindow::onOpenFile)
	EVT_MENU(kEventFileClose   , MainWindow::onClose)
	EVT_MENU(kEventFileQuit    , MainWindow::onQuit)
	EVT_MENU(kEventHelpAbout   , MainWindow::onAbout)

	EVT_BUTTON(kEventButtonExportRaw, MainWindow::onExportRaw)
wxEND_EVENT_TABLE()


MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size) :
	wxFrame(NULL, wxID_ANY, title, pos, size) {

	CreateStatusBar();
	GetStatusBar()->SetStatusText(wxT("Idle..."));

	wxMenu *menuFile = new wxMenu;

	wxMenuItem *menuFileOpenDir =
		new wxMenuItem(0, kEventFileOpenDir, wxT("Open &directory\tCtrl-D"), Common::UString("Open directory"));
	wxMenuItem *menuFileOpenFile =
		new wxMenuItem(0, kEventFileOpenFile, wxT("Open &file\tCtrl-D"), Common::UString("Open file"));
	wxMenuItem *menuFileClose =
		new wxMenuItem(0, kEventFileClose, wxT("&Close\tCtrl-W"), Common::UString("Close"));

	menuFileOpenDir->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN));
	menuFileOpenFile->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	menuFileClose->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE));

	menuFile->Append(menuFileOpenDir);
	menuFile->Append(menuFileOpenFile);
	menuFile->AppendSeparator();
	menuFile->Append(menuFileClose);
	menuFile->AppendSeparator();
	menuFile->Append(kEventFileQuit, wxT("&Quit\tCtrl-Q"), Common::UString("Quit ") + PHAETHON_NAME);


	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(kEventHelpAbout, wxT("&About\tF1"), Common::UString("About ") + PHAETHON_NAME);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, wxT("&File"));
	menuBar->Append(menuHelp, wxT("&Help"));

	SetMenuBar(menuBar);


	wxSplitterWindow *splitterMainLog     = new wxSplitterWindow(this, wxID_ANY);
	wxSplitterWindow *splitterTreeRes     = new wxSplitterWindow(splitterMainLog, wxID_ANY);
	wxSplitterWindow *splitterInfoPreview = new wxSplitterWindow(splitterTreeRes, wxID_ANY);

	wxPanel *panelLog     = new wxPanel(splitterMainLog    , wxID_ANY);
	wxPanel *panelPreview = new wxPanel(splitterInfoPreview, wxID_ANY);
	wxPanel *panelInfo    = new wxPanel(splitterInfoPreview, wxID_ANY);
	wxPanel *panelTree    = new wxPanel(splitterTreeRes    , wxID_ANY);

	_resourceTree = new ResourceTree(panelTree, *this);

	_resInfoName     = new wxGenericStaticText(panelInfo, wxID_ANY, wxEmptyString);
	_resInfoSize     = new wxGenericStaticText(panelInfo, wxID_ANY, wxEmptyString);
	_resInfoFileType = new wxGenericStaticText(panelInfo, wxID_ANY, wxEmptyString);
	_resInfoResType  = new wxGenericStaticText(panelInfo, wxID_ANY, wxEmptyString);

	wxGenericStaticText *preview = new wxGenericStaticText(panelPreview, wxID_ANY, wxT("Preview..."));

	wxTextCtrl *log = new wxTextCtrl(panelLog, wxID_ANY, wxEmptyString, wxDefaultPosition,
	                                 wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

	wxBoxSizer *sizerWindow = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer *sizerLog     = new wxStaticBoxSizer(wxHORIZONTAL, panelLog    , wxT("Log"));
	wxStaticBoxSizer *sizerPreview = new wxStaticBoxSizer(wxHORIZONTAL, panelPreview, wxT("Preview"));
	wxStaticBoxSizer *sizerInfo    = new wxStaticBoxSizer(wxVERTICAL  , panelInfo   , wxT("Resource info"));
	wxStaticBoxSizer *sizerTree    = new wxStaticBoxSizer(wxHORIZONTAL, panelTree   , wxT("Resources"));

	sizerTree->Add(_resourceTree, 1, wxEXPAND, 0);
	panelTree->SetSizer(sizerTree);

	wxBoxSizer *sizerExport = new wxBoxSizer(wxHORIZONTAL);

	_buttonExportRaw = new wxButton(panelInfo, kEventButtonExportRaw, wxT("Save"));
	_buttonExportRaw->Disable();

	sizerExport->Add(_buttonExportRaw, 0, wxEXPAND, 0);

	sizerInfo->Add(_resInfoName    , 0, wxEXPAND, 0);
	sizerInfo->Add(_resInfoSize    , 0, wxEXPAND, 0);
	sizerInfo->Add(_resInfoFileType, 0, wxEXPAND, 0);
	sizerInfo->Add(_resInfoResType , 0, wxEXPAND, 0);

	sizerInfo->Add(sizerExport, 0, wxEXPAND | wxTOP, 5);

	panelInfo->SetSizer(sizerInfo);

	sizerPreview->Add(preview, 0, 0, 0);
	panelPreview->SetSizer(sizerPreview);

	sizerLog->Add(log, 1, wxEXPAND, 0);
	panelLog->SetSizer(sizerLog);

	splitterInfoPreview->SetMinimumPaneSize(20);
	splitterTreeRes->SetMinimumPaneSize(20);
	splitterMainLog->SetMinimumPaneSize(20);

	splitterMainLog->SetSashGravity(1.0);

	splitterInfoPreview->SplitHorizontally(panelInfo, panelPreview);
	splitterTreeRes->SplitVertically(panelTree, splitterInfoPreview);
	splitterMainLog->SplitHorizontally(splitterTreeRes, panelLog);

	sizerWindow->Add(splitterMainLog, 1, wxEXPAND, 0);
	SetSizer(sizerWindow);

	Layout();

	splitterInfoPreview->SetSashPosition(150);
	splitterTreeRes->SetSashPosition(200);
	splitterMainLog->SetSashPosition(480);

	resourceTreeSelect(0);
}

MainWindow::~MainWindow() {
}

void MainWindow::onQuit(wxCommandEvent &event) {
	close();
	Close(true);
}

void MainWindow::onAbout(wxCommandEvent &event) {
	AboutDialog *about = new AboutDialog(this);
	about->show();
}

void MainWindow::onOpenDir(wxCommandEvent &event) {
	wxDirDialog dialog(this, wxT("Open Aurora game directory"), wxEmptyString,
	                   wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dialog.ShowModal() != wxID_OK)
		return;

	open(dialog.GetPath());
}

void MainWindow::onOpenFile(wxCommandEvent &event) {
	wxFileDialog dialog(this, wxT("Open Aurora game resource file"), wxEmptyString, wxEmptyString,
	                    wxT("Aurora game resource (*.*)|*.*"), wxFD_DEFAULT_STYLE | wxFD_FILE_MUST_EXIST);

	if (dialog.ShowModal() != wxID_OK)
		return;

	open(dialog.GetPath());
}

void MainWindow::onClose(wxCommandEvent &event) {
	close();
}

void MainWindow::onExportRaw(wxCommandEvent &event) {
	ResourceTreeItem *item = _resourceTree->getSelection();
	if (!item)
		return;

	wxFileDialog dialog(this, wxT("Save Aurora game resource file"), wxEmptyString, item->getName(),
	                    wxT("Aurora game resource (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (dialog.ShowModal() != wxID_OK)
		return;

	exportRaw(*item, dialog.GetPath());
}

void MainWindow::forceRedraw() {
	Refresh();
	Update();
}

bool MainWindow::open(Common::UString path) {
	close();

	if (!Common::FilePath::isDirectory(path) && !Common::FilePath::isRegularFile(path)) {
		warning("Path \"%s\" is neither a directory nor a regular file", path.c_str());
		return false;
	}

	path = Common::FilePath::normalize(path);

	if (Common::FilePath::isDirectory(path))
		GetStatusBar()->PushStatusText(Common::UString("Recursively adding all files in ") + path + "...");
	else
		GetStatusBar()->PushStatusText(Common::UString("Adding file ") + path + "...");

	forceRedraw();

	try {
		_files.readPath(path, -1);
	} catch (Common::Exception &e) {
		GetStatusBar()->PopStatusText();

		Common::printException(e, "WARNING: ");
		return false;
	}

	_path = path;

	GetStatusBar()->PopStatusText();
	GetStatusBar()->PushStatusText(Common::UString("Populating resource tree..."));
	populateTree();
	GetStatusBar()->PopStatusText();

	return true;
}

void MainWindow::close() {
	_resourceTree->DeleteAllItems();

	_files.clear();
	_path.clear();

	for (ArchiveMap::iterator a = _archives.begin(); a != _archives.end(); ++a)
		delete a->second;

	_archives.clear();

	for (KEYDataFileMap::iterator d = _keyDataFiles.begin(); d != _keyDataFiles.end(); ++d)
		delete d->second;

	_keyDataFiles.clear();

	resourceTreeSelect(0);
}

void MainWindow::populateTree() {
	const Common::FileTree::Entry &fileRoot = _files.getRoot();

	wxTreeItemId treeRoot = _resourceTree->addRoot(new ResourceTreeItem(fileRoot));

	populateTree(fileRoot, treeRoot);
	_resourceTree->Expand(treeRoot);

	resourceTreeSelect(_resourceTree->getItemData(treeRoot));
}

void MainWindow::resourceTreeSelect(const ResourceTreeItem *item) {
	Common::UString labelInfoName     = "Resource name: ";
	Common::UString labelInfoSize     = "Size: ";
	Common::UString labelInfoFileType = "File type: ";
	Common::UString labelInfoResType  = "Resource type: ";

	if (item) {
		labelInfoName += item->getName();

		if (item->getSource() == ResourceTreeItem::kSourceDirectory) {

			_buttonExportRaw->Disable();

			labelInfoSize     += "-";
			labelInfoFileType += "Directory";
			labelInfoResType  += "Directory";

		} else if ((item->getSource() == ResourceTreeItem::kSourceFile) ||
		           (item->getSource() == ResourceTreeItem::kSourceArchiveFile)) {

			_buttonExportRaw->Enable();

			if (item->getSize() != Common::kFileInvalid) {

				if (item->getSize() >= 1024)
					labelInfoSize += Common::UString::sprintf("%s (%u)",
							Common::FilePath::getHumanReadableSize(item->getSize()).c_str(), item->getSize());
				else
					labelInfoSize += Common::UString::sprintf("%u", item->getSize());

			} else
				labelInfoSize += "-";

			Aurora::FileType     fileType = item->getFileType();
			Aurora::ResourceType resType  = item->getResourceType();

			labelInfoFileType += Common::UString::sprintf("%d", fileType);
			labelInfoResType  += Common::UString::sprintf("%d", resType);

			if (fileType != Aurora::kFileTypeNone)
				labelInfoFileType += Common::UString::sprintf(" (%s)", TypeMan.getExtension(fileType).c_str());
			if (resType  != Aurora::kResourceNone)
				labelInfoResType  += Common::UString::sprintf(" (%s)", getResourceTypeDescription(resType).c_str());
		}
	} else {
		_buttonExportRaw->Disable();
	}

	_resInfoName->SetLabel(labelInfoName);
	_resInfoSize->SetLabel(labelInfoSize);
	_resInfoFileType->SetLabel(labelInfoFileType);
	_resInfoResType->SetLabel(labelInfoResType);
}

bool MainWindow::exportRaw(const ResourceTreeItem &item, const Common::UString &path) {
	Common::UString msg = Common::UString("Saving \"") + item.getName() + "\" to \"" + path + "\"...";
	GetStatusBar()->PushStatusText(msg);

	Common::SeekableReadStream *res = 0;
	try {
		res = item.getResourceData();

		Common::DumpFile file(path);

		file.writeStream(*res);

		if (!file.flush() || file.err())
			throw Common::Exception(Common::kWriteError);

		delete res;

	} catch (Common::Exception &e) {
		delete res;

		GetStatusBar()->PopStatusText();
		Common::printException(e, "WARNING: ");
		return false;
	}

	GetStatusBar()->PopStatusText();
	return true;
}

Aurora::Archive *MainWindow::getArchive(const boost::filesystem::path &path) {
	ArchiveMap::iterator a = _archives.find(path.c_str());
	if (a != _archives.end())
		return a->second;

	Aurora::Archive *arch = 0;
	switch (TypeMan.getFileType(path.c_str())) {
		case Aurora::kFileTypeZIP:
			arch = new Aurora::ZIPFile(path.c_str());
			break;

		case Aurora::kFileTypeERF:
		case Aurora::kFileTypeMOD:
		case Aurora::kFileTypeNWM:
		case Aurora::kFileTypeSAV:
		case Aurora::kFileTypeHAK:
			arch = new Aurora::ERFFile(path.c_str());
			break;

		case Aurora::kFileTypeRIM:
			arch = new Aurora::RIMFile(path.c_str());
			break;

		case Aurora::kFileTypeKEY: {
				Aurora::KEYFile *key = new Aurora::KEYFile(path.c_str());
				loadKEYDataFiles(*key);

				arch = key;
				break;
			}

		default:
			throw Common::Exception("Invalid archive file \"%s\"", path.c_str());
	}

	_archives.insert(std::make_pair(path.c_str(), arch));
	return arch;
}

Aurora::KEYDataFile *MainWindow::getKEYDataFile(const Common::UString &file) {
	KEYDataFileMap::iterator d = _keyDataFiles.find(file);
	if (d != _keyDataFiles.end())
		return d->second;

	Common::UString path = Common::FilePath::normalize(_path + "/" + file);
	if (path.empty())
		throw Common::Exception("No such file or directory \"%s\"", (_path + "/" + file).c_str());

	Aurora::FileType type = TypeMan.getFileType(file);

	Aurora::KEYDataFile *dataFile = 0;
	switch (type) {
		case Aurora::kFileTypeBIF:
			dataFile = new Aurora::BIFFile(path);
			break;

		case Aurora::kFileTypeBZF:
			dataFile = new Aurora::BZFFile(path);
			break;

		default:
			throw Common::Exception("Unknown KEY data file type %d\n", type);
	}

	_keyDataFiles.insert(std::make_pair(file, dataFile));
	return dataFile;
}

void MainWindow::loadKEYDataFiles(Aurora::KEYFile &key) {
	const std::vector<Common::UString> dataFiles = key.getDataFileList();
	for (uint i = 0; i < dataFiles.size(); i++) {
		try {

			GetStatusBar()->PushStatusText(Common::UString("Loading data file") + dataFiles[i] + "...");

			Aurora::KEYDataFile *dataFile = getKEYDataFile(dataFiles[i]);
			key.addDataFile(i, dataFile);

			GetStatusBar()->PopStatusText();

		} catch (Common::Exception &e) {
			e.add("Failed to load KEY data file \"%s\"", dataFiles[i].c_str());

			GetStatusBar()->PopStatusText();
			Common::printException(e, "WARNING: ");
		}
	}
}
