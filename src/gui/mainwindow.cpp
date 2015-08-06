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
 *  Phaethon's main window.
 */

#include <deque>

#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/artprov.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>

#include <wx/generic/stattextg.h>

#include "src/common/ustring.h"
#include "src/common/version.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readfile.h"
#include "src/common/filepath.h"

#include "src/aurora/util.h"
#include "src/aurora/zipfile.h"
#include "src/aurora/erffile.h"
#include "src/aurora/rimfile.h"
#include "src/aurora/keyfile.h"
#include "src/aurora/keydatafile.h"
#include "src/aurora/biffile.h"
#include "src/aurora/bzffile.h"

#include "src/gui/eventid.h"
#include "src/gui/about.h"
#include "src/gui/mainwindow.h"
#include "src/gui/resourcetree.h"
#include "src/gui/panelresourceinfo.h"
#include "src/gui/panelpreviewempty.h"
#include "src/gui/panelpreviewsound.h"
#include "src/gui/panelpreviewimage.h"

#include "src/cline.h"

namespace GUI {

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(kEventMenuFileOpenDir , MainWindow::onOpenDir)
	EVT_MENU(kEventMenuFileOpenFile, MainWindow::onOpenFile)
	EVT_MENU(kEventMenuFileClose   , MainWindow::onClose)
	EVT_MENU(kEventMenuFileQuit    , MainWindow::onQuit)
	EVT_MENU(kEventMenuHelpAbout   , MainWindow::onAbout)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size) :
	wxFrame(NULL, wxID_ANY, title, pos, size) {

	createLayout();

	resourceSelect(0);
}

MainWindow::~MainWindow() {
}

void MainWindow::createLayout() {
	CreateStatusBar();
	GetStatusBar()->SetStatusText(wxT("Idle..."));

	wxMenu *menuFile = new wxMenu;

	wxMenuItem *menuFileOpenDir =
		new wxMenuItem(0, kEventMenuFileOpenDir, wxT("Open &directory\tCtrl-D"), Common::UString("Open directory"));
	wxMenuItem *menuFileOpenFile =
		new wxMenuItem(0, kEventMenuFileOpenFile, wxT("Open &file\tCtrl-D"), Common::UString("Open file"));
	wxMenuItem *menuFileClose =
		new wxMenuItem(0, kEventMenuFileClose, wxT("&Close\tCtrl-W"), Common::UString("Close"));

	menuFileOpenDir->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN));
	menuFileOpenFile->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	menuFileClose->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE));

	menuFile->Append(menuFileOpenDir);
	menuFile->Append(menuFileOpenFile);
	menuFile->AppendSeparator();
	menuFile->Append(menuFileClose);
	menuFile->AppendSeparator();
	menuFile->Append(kEventMenuFileQuit, wxT("&Quit\tCtrl-Q"), Common::UString("Quit ") + PHAETHON_NAME);


	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(kEventMenuHelpAbout, wxT("&About\tF1"), Common::UString("About ") + PHAETHON_NAME);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, wxT("&File"));
	menuBar->Append(menuHelp, wxT("&Help"));

	SetMenuBar(menuBar);


	wxSplitterWindow *splitterMainLog = new wxSplitterWindow(this, wxID_ANY);
	wxSplitterWindow *splitterTreeRes = new wxSplitterWindow(splitterMainLog, wxID_ANY);

	_splitterInfoPreview = new wxSplitterWindow(splitterTreeRes, wxID_ANY);

	wxPanel *panelLog  = new wxPanel( splitterMainLog    , wxID_ANY);
	wxPanel *panelTree = new wxPanel( splitterTreeRes    , wxID_ANY);

	_panelResourceInfo = new PanelResourceInfo(_splitterInfoPreview, *this, "Resource info");

	_panelPreviewEmpty = new PanelPreviewEmpty(_splitterInfoPreview, "Preview");
	_panelPreviewSound = new PanelPreviewSound(_splitterInfoPreview, "Preview");
	_panelPreviewImage = new PanelPreviewImage(_splitterInfoPreview, "Preview");

	_panelPreviewSound->Hide();
	_panelPreviewImage->Hide();

	_resourceTree = new ResourceTree(panelTree, *this);

	wxTextCtrl *log = new wxTextCtrl(panelLog, wxID_ANY, wxEmptyString, wxDefaultPosition,
	                                 wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

	wxBoxSizer *sizerWindow = new wxBoxSizer(wxVERTICAL);

	wxStaticBox *boxLog  = new wxStaticBox(panelLog , wxID_ANY,  wxT("Log"));
	wxStaticBox *boxTree = new wxStaticBox(panelTree, wxID_ANY,  wxT("Resources"));

	boxLog->Lower();
	boxTree->Lower();

	wxStaticBoxSizer *sizerLog  = new wxStaticBoxSizer(boxLog , wxHORIZONTAL);
	wxStaticBoxSizer *sizerTree = new wxStaticBoxSizer(boxTree, wxHORIZONTAL);

	sizerTree->Add(_resourceTree, 1, wxEXPAND, 0);
	panelTree->SetSizer(sizerTree);

	sizerLog->Add(log, 1, wxEXPAND, 0);
	panelLog->SetSizer(sizerLog);

	_splitterInfoPreview->SetMinimumPaneSize(20);
	splitterTreeRes->SetMinimumPaneSize(20);
	splitterMainLog->SetMinimumPaneSize(20);

	splitterMainLog->SetSashGravity(1.0);

	_splitterInfoPreview->SplitHorizontally(_panelResourceInfo, _panelPreviewEmpty);
	splitterTreeRes->SplitVertically(panelTree, _splitterInfoPreview);
	splitterMainLog->SplitHorizontally(splitterTreeRes, panelLog);

	sizerWindow->Add(splitterMainLog, 1, wxEXPAND, 0);
	SetSizer(sizerWindow);

	Layout();

	_splitterInfoPreview->SetSashPosition(150);
	splitterTreeRes->SetSashPosition(200);
	splitterMainLog->SetSashPosition(480);
}

void MainWindow::onQuit(wxCommandEvent &UNUSED(event)) {
	close();
	Close(true);
}

void MainWindow::onAbout(wxCommandEvent &UNUSED(event)) {
	AboutDialog *about = new AboutDialog(this);
	about->show();
}

void MainWindow::onOpenDir(wxCommandEvent &UNUSED(event)) {
	Common::UString path = dialogOpenDir("Open Aurora game directory");
	if (path.empty())
		return;

	open(path);
}

void MainWindow::onOpenFile(wxCommandEvent &UNUSED(event)) {
	Common::UString path = dialogOpenFile("Open Aurora game resource file",
	                                      "Aurora game resource (*.*)|*.*");
	if (path.empty())
		return;

	open(path);
}

void MainWindow::onClose(wxCommandEvent &UNUSED(event)) {
	close();
}

void MainWindow::forceRedraw() {
	Refresh();
	Update();
}

void MainWindow::pushStatus(const Common::UString &text) {
	GetStatusBar()->PushStatusText(text);
}

void MainWindow::popStatus() {
	GetStatusBar()->PopStatusText();
}

Common::UString MainWindow::dialogOpenDir(const Common::UString &title) {
	wxDirDialog dialog(this, title, wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dialog.ShowModal() == wxID_OK)
		return dialog.GetPath();

	return "";
}

Common::UString MainWindow::dialogOpenFile(const Common::UString &title,
                                           const Common::UString &mask) {

	wxFileDialog dialog(this, title, wxEmptyString, wxEmptyString, mask,
	                    wxFD_DEFAULT_STYLE | wxFD_FILE_MUST_EXIST);
	if (dialog.ShowModal() == wxID_OK)
		return dialog.GetPath();

	return "";
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
	_resourceTree->populate(_files.getRoot());
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

	resourceSelect(0);
}

void MainWindow::showPreviewPanel(const ResourceTreeItem *item) {
	showPreviewPanel(item ? item->getResourceType() : Aurora::kResourceNone);
}

void MainWindow::showPreviewPanel(Aurora::ResourceType type) {
	switch (type) {
		case Aurora::kResourceSound:
			showPreviewPanel(_panelPreviewSound);
			break;

		case Aurora::kResourceImage:
			showPreviewPanel(_panelPreviewImage);
			break;

		default:
			showPreviewPanel(_panelPreviewEmpty);
			break;
	}
}

void MainWindow::showPreviewPanel(wxPanel *panel) {
	wxWindow *old = _splitterInfoPreview->GetWindow2();

	Freeze();
	old->Hide();
	_splitterInfoPreview->ReplaceWindow(old, panel);
	panel->Show();
	Thaw();
}

void MainWindow::resourceSelect(const ResourceTreeItem *item) {
	showPreviewPanel(item);

	_panelResourceInfo->setCurrentItem(item);
	_panelPreviewSound->setCurrentItem(item);
	_panelPreviewImage->setCurrentItem(item);
}

void MainWindow::resourceActivate(const ResourceTreeItem &item) {
	if (item.getResourceType() == Aurora::kResourceSound) {
		_panelPreviewSound->setCurrentItem(&item);
		_panelPreviewSound->play();
	}
}

Aurora::Archive *MainWindow::getArchive(const boost::filesystem::path &path) {
	ArchiveMap::iterator a = _archives.find(path.generic_string().c_str());
	if (a != _archives.end())
		return a->second;

	Aurora::Archive *arch = 0;
	switch (TypeMan.getFileType(path.generic_string().c_str())) {
		case Aurora::kFileTypeZIP:
			arch = new Aurora::ZIPFile(new Common::ReadFile(path.generic_string().c_str()));
			break;

		case Aurora::kFileTypeERF:
		case Aurora::kFileTypeMOD:
		case Aurora::kFileTypeNWM:
		case Aurora::kFileTypeSAV:
		case Aurora::kFileTypeHAK:
			arch = new Aurora::ERFFile(new Common::ReadFile(path.generic_string().c_str()));
			break;

		case Aurora::kFileTypeRIM:
			arch = new Aurora::RIMFile(new Common::ReadFile(path.generic_string().c_str()));
			break;

		case Aurora::kFileTypeKEY: {
				Aurora::KEYFile *key = new Aurora::KEYFile(new Common::ReadFile(path.generic_string().c_str()));
				loadKEYDataFiles(*key);

				arch = key;
				break;
			}

		default:
			throw Common::Exception("Invalid archive file \"%s\"", path.generic_string().c_str());
	}

	_archives.insert(std::make_pair(path.generic_string().c_str(), arch));
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
			dataFile = new Aurora::BIFFile(new Common::ReadFile(path));
			break;

		case Aurora::kFileTypeBZF:
			dataFile = new Aurora::BZFFile(new Common::ReadFile(path));
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

} // End of namespace GUI
