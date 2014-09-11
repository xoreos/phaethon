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

/** @file gui/panelpreviewimage.h
 *  Preview panel for images resources.
 */

#ifndef PANELPREVIEWIMAGE_H
#define PANELPREVIEWIMAGE_H

#include <wx/scrolwin.h>
#include <wx/panel.h>

#include "images/types.h"

namespace Common {
	class UString;
}

class wxDC;
class wxImage;
class wxBitmap;
class wxSlider;

namespace GUI {

class ResourceTreeItem;

class ImageCanvas : public wxScrolledCanvas {
public:
	ImageCanvas(wxWindow *parent);
	~ImageCanvas();

	void OnDraw(wxDC &dc);

	void setCurrentItem(const ResourceTreeItem *item);
	void setColor(uint8 color);

private:
	const ResourceTreeItem *_currentItem;

	uint8 _color;

	wxImage  *_image;
	wxBitmap *_bitmap;

	void loadImage();

	static void convertImage(const Images::Decoder &image, byte *data_rgb, byte *data_alpha);
	static void writePixel(const byte *&data, Images::PixelFormat format,
			byte *&data_rgb, byte *&data_alpha);
};

class PanelPreviewImage : public wxPanel {
public:
	PanelPreviewImage(wxWindow *parent, const Common::UString &title);
	~PanelPreviewImage();

	void setCurrentItem(const ResourceTreeItem *item);

private:
	ImageCanvas *_canvas;

	wxSlider *_sliderColor;

	void onColorChange(wxScrollEvent &event);

	wxDECLARE_EVENT_TABLE();
};

} // End of namespace GUI

#endif // PANELPREVIEWIMAGE_H
