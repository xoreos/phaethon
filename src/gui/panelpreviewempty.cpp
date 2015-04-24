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
 *  Preview panel for resources we can't do anything with.
 */

#include <wx/sizer.h>
#include <wx/statbox.h>

#include "common/ustring.h"

#include "gui/panelpreviewempty.h"

namespace GUI {

PanelPreviewEmpty::PanelPreviewEmpty(wxWindow *parent, const Common::UString &title) :
	wxPanel(parent, wxID_ANY) {

	wxStaticBox *boxPreviewEmpty = new wxStaticBox(this, wxID_ANY, title);
	boxPreviewEmpty->Lower();

	wxStaticBoxSizer *sizerPreviewEmpty = new wxStaticBoxSizer(boxPreviewEmpty, wxVERTICAL);

	SetSizer(sizerPreviewEmpty);
}

PanelPreviewEmpty::~PanelPreviewEmpty() {
}

} // End of namespace GUI
