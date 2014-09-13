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

/** @file gui/eventid.h
 *  GUI event IDs.
 */

#ifndef EVENTID_H
#define EVENTID_H

#include <wx/defs.h>

namespace GUI {

enum {
	// Main window - menu bar
	kEventMenuFileQuit        = wxID_EXIT,
	kEventMenuHelpAbout       = wxID_ABOUT,
	kEventMenuFileOpenDir     = 0,
	kEventMenuFileOpenFile       ,
	kEventMenuFileClose          ,
	// Main window - resource tree
	kEventResourceTree,

	// About dialog
	kEventButtonAboutClose,

	// Resource info panel
	kEventButtonExportRaw   ,
	kEventButtonExportBMUMP3,
	kEventButtonExportWAV   ,
	kEventButtonExportTGA   ,

	// Preview panel for sound resources - play buttons
	kEventButtonPlay ,
	kEventButtonPause,
	kEventButtonStop ,
	// Preview panel for sound resources - volume slider
	kEventSliderVolume,

	// Preview panel for image resource - zoom buttons
	kEventButton                  ,
	kEventButtonZoomIn            ,
	kEventButtonZoomOut           ,
	kEventButtonZoom100           ,
	kEventButtonZoomFit           ,
	kEventButtonZoomFitWidth      ,
	kEventButtonZoomShrinkFit     ,
	kEventButtonZoomShrinkFitWidth,
	// Preview panel for image resource - zoom check button
	kEventCheckZoomNearest,
	// Preview panel for image resource - background color slider
	kEventSliderColor
};

} // End of namespace GUI

#endif // EVENTID_H
