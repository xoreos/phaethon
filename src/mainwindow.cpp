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

#include "aurora/util.h"

#include "cline.h"
#include "eventid.h"
#include "about.h"
#include "mainwindow.h"


ResourceTreeItem::ResourceTreeItem(const Common::FileTree::Entry &entry) :
	_name(entry.name), _source(entry.isDirectory() ? kSourceDirectory : kSourceFile), _path(entry.path) {

}

ResourceTreeItem::~ResourceTreeItem() {
}

const Common::UString &ResourceTreeItem::getName() const {
	return _name;
}

ResourceTreeItem::Source ResourceTreeItem::getSource() const {
	return _source;
}

const boost::filesystem::path &ResourceTreeItem::getPath() const {
	return _path;
}

Aurora::FileType ResourceTreeItem::getFileType() const {
	return TypeMan.getFileType(_name);
}

Aurora::ResourceType ResourceTreeItem::getResourceType() const {
	return TypeMan.getResourceType(_name);
}


wxBEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(kEventResourceTree, ResourceTree::onSelChanged)
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
	ResourceTreeItem *d1 = dynamic_cast<ResourceTreeItem *>(GetItemData(item1));
	ResourceTreeItem *d2 = dynamic_cast<ResourceTreeItem *>(GetItemData(item2));

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

	_mainWindow->resourceTreeSelect(dynamic_cast<ResourceTreeItem *>(GetItemData(event.GetItem())));
}


wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(kEventFileOpenDir , MainWindow::onOpenDir)
	EVT_MENU(kEventFileOpenFile, MainWindow::onOpenFile)
	EVT_MENU(kEventFileClose   , MainWindow::onClose)
	EVT_MENU(kEventFileQuit    , MainWindow::onQuit)
	EVT_MENU(kEventHelpAbout   , MainWindow::onAbout)
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

	sizerInfo->Add(_resInfoName    , 0, wxEXPAND, 0);
	sizerInfo->Add(_resInfoFileType, 0, wxEXPAND, 0);
	sizerInfo->Add(_resInfoResType , 0, wxEXPAND, 0);
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
}

void MainWindow::populateTree(const Common::FileTree::Entry &e, wxTreeItemId t) {
	for (std::list<Common::FileTree::Entry>::const_iterator c = e.children.begin();
	     c != e.children.end(); ++c) {

		int image = c->isDirectory() ? ResourceTree::kImageDir : ResourceTree::kImageFile;

		wxTreeItemId cT = _resourceTree->AppendItem(t, c->name, image, image, new ResourceTreeItem(*c));
		populateTree(*c, cT);
	}

	_resourceTree->SortChildren(t);
}

void MainWindow::populateTree() {
	const Common::FileTree::Entry &fileRoot = _files.getRoot();

	int image = fileRoot.isDirectory() ? ResourceTree::kImageDir : ResourceTree::kImageFile;

	wxTreeItemId treeRoot = _resourceTree->AddRoot(fileRoot.name, image, image, new ResourceTreeItem(fileRoot));

	populateTree(fileRoot, treeRoot);
	_resourceTree->Expand(treeRoot);

	resourceTreeSelect(dynamic_cast<ResourceTreeItem *>(_resourceTree->GetItemData(treeRoot)));
}

void MainWindow::resourceTreeSelect(const ResourceTreeItem *item) {
	Common::UString labelInfoName     = "Resource name: ";
	Common::UString labelInfoFileType = "File type: ";
	Common::UString labelInfoResType  = "Resource type: ";

	if (item) {
		labelInfoName += item->getName();

		if (item->getSource() == ResourceTreeItem::kSourceDirectory) {
			labelInfoFileType += "Directory";
			labelInfoResType  += "Directory";
		} else {
			Aurora::FileType     fileType = item->getFileType();
			Aurora::ResourceType resType  = item->getResourceType();

			labelInfoFileType += Common::UString::sprintf("%d", fileType);
			labelInfoResType  += Common::UString::sprintf("%d", resType);

			if (fileType != Aurora::kFileTypeNone)
				labelInfoFileType += Common::UString::sprintf(" (%s)", TypeMan.getExtension(fileType).c_str());
			if (resType  != Aurora::kResourceNone)
				labelInfoResType  += Common::UString::sprintf(" (%s)", getResourceTypeDescription(resType).c_str());
		}
	}

	_resInfoName->SetLabel(labelInfoName);
	_resInfoFileType->SetLabel(labelInfoFileType);
	_resInfoResType->SetLabel(labelInfoResType);
}
