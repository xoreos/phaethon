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
#include <wx/treectrl.h>
#include <wx/splitter.h>

#include <wx/generic/stattextg.h>

#include "common/ustring.h"
#include "common/version.h"

#include "cline.h"
#include "eventid.h"
#include "about.h"
#include "mainwindow.h"

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(kEventFileQuit , MainWindow::onQuit)
	EVT_MENU(kEventHelpAbout, MainWindow::onAbout)
wxEND_EVENT_TABLE()


MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size,
                       const Common::UString &path) :
	wxFrame(NULL, wxID_ANY, title, pos, size), _path(path) {

	CreateStatusBar();
	GetStatusBar()->SetStatusText(wxT("Idle..."));

	wxMenu *menuFile = new wxMenu;
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

	wxTreeCtrl *tree = new wxTreeCtrl(panelTree, wxID_ANY);

	wxGenericStaticText *info    = new wxGenericStaticText(panelInfo   , wxID_ANY, wxT("Info..."));
	wxGenericStaticText *preview = new wxGenericStaticText(panelPreview, wxID_ANY, wxT("Preview..."));

	wxTextCtrl *log = new wxTextCtrl(panelLog, wxID_ANY, wxEmptyString, wxDefaultPosition,
	                                 wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

	wxBoxSizer *sizerWindow = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer *sizerLog     = new wxStaticBoxSizer(wxHORIZONTAL, panelLog    , wxT("Log"));
	wxStaticBoxSizer *sizerPreview = new wxStaticBoxSizer(wxHORIZONTAL, panelPreview, wxT("Preview"));
	wxStaticBoxSizer *sizerInfo    = new wxStaticBoxSizer(wxHORIZONTAL, panelInfo   , wxT("Resource info"));
	wxStaticBoxSizer *sizerTree    = new wxStaticBoxSizer(wxHORIZONTAL, panelTree   , wxT("Resources"));

	sizerTree->Add(tree, 1, wxEXPAND, 0);
	panelTree->SetSizer(sizerTree);

	sizerInfo->Add(info, 0, 0, 0);
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
