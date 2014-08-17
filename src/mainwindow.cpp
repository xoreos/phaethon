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

#include "common/ustring.h"
#include "common/version.h"

#include "eventid.h"
#include "mainwindow.h"

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(kEventFileQuit, MainWindow::onQuit)
wxEND_EVENT_TABLE()


MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size,
                       const Common::UString &path) :
	wxFrame(NULL, wxID_ANY, title, pos, size), _path(path) {

	CreateStatusBar();

	wxMenu *menuFile = new wxMenu;
	menuFile->Append(kEventFileQuit, wxT("&Quit\tCtrl-Q"),
	                 wxString::FromUTF8((Common::UString("Quit ") + PHAETHON_NAME).c_str()));

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(kEventHelpAbout, wxT("&About\tF1"),
	                 wxString::FromUTF8((Common::UString("About ") + PHAETHON_NAME).c_str()));

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, wxT("&File"));
	menuBar->Append(menuHelp, wxT("&Help"));

	SetMenuBar(menuBar);
}

MainWindow::~MainWindow() {
}

void MainWindow::onQuit(wxCommandEvent &event) {
	Close(true);
}
