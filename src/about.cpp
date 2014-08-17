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

/** @file about.cpp
 *  About dialog.
 */

#include <wx/generic/stattextg.h>

#include "common/ustring.h"
#include "common/version.h"

#include "about.h"
#include "cline.h"
#include "eventid.h"

wxBEGIN_EVENT_TABLE(AboutDialog, wxDialog)
	EVT_BUTTON(kEventButtonClose, AboutDialog::onClose)
wxEND_EVENT_TABLE()


AboutDialog::AboutDialog(wxWindow *parent) :
	wxDialog(parent, wxID_ANY, Common::UString("About ") + PHAETHON_NAME) {

	wxGenericStaticText *msg = new wxGenericStaticText(this, wxID_ANY, wxEmptyString);

	msg->SetLabel(createVersionText());

	wxButton *buttonClose = new wxButton(this, kEventButtonClose, "&OK");

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(msg, 1, wxALL, 5);
	sizer->Add(buttonClose, 0, wxALL | wxCENTER, 5);

	SetSizerAndFit(sizer);
}

AboutDialog::~AboutDialog() {
}

void AboutDialog::show() {
	ShowModal();
}

void AboutDialog::onClose(wxCommandEvent &event) {
	Close(true);
}
