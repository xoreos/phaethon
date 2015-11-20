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
 *  Preview panel for images resources.
 */

#ifndef PANELPREVIEWIMAGE_H
#define PANELPREVIEWIMAGE_H

#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/panel.h>

#include "src/images/types.h"

namespace Common {
	class UString;
}

class wxDC;
class wxImage;
class wxBitmap;
class wxSlider;
class wxButton;
class wxCheckBox;

namespace GUI {

class ResourceTreeItem;

class ImageCanvas : public wxScrolledCanvas {
public:
	enum ScaleQuality {
		kScaleQualityNearest,
		kScaleQualityBest
	};

	ImageCanvas(wxWindow *parent);
	~ImageCanvas();

	void OnDraw(wxDC &dc);

	void setCurrentItem(const ResourceTreeItem *item);
	void setColor(uint8 color);

	void setScaleQuality(ScaleQuality scaleQuality);

	void getSize(int &fullWidth, int &fullHeight, int &currentWidth, int &currentHeight) const;
	void setSize(int width, int height);

private:
	const ResourceTreeItem *_currentItem;

	uint8 _color;

	ScaleQuality _scaleQuality;

	wxImage  *_image;
	wxBitmap *_bitmap;

	void forceRedraw();

	void loadImage();

	static void getImageDimensions(const Images::Decoder &image, int32 &width, int32 &height);
	static void convertImage(const Images::Decoder &image, byte *data_rgb, byte *data_alpha);
	static void writePixel(const byte *&data, Images::PixelFormat format,
			byte *&data_rgb, byte *&data_alpha);
};

class PanelPreviewImage : public wxPanel {
public:
	PanelPreviewImage(wxWindow *parent, const Common::UString &title);
	~PanelPreviewImage();

	void setCurrentItem(const ResourceTreeItem *item);

	void getImageSize(int &width, int &height);


private:
	enum ZoomOperation {
		kZoomOpLevel,
		kZoomOpFit,
		kZoomOpFitWidth,
		kZoomOpShrinkFit,
		kZoomOpShrinkFitWidth
	};

	Common::UString _title;

	ImageCanvas *_canvas;

	wxStaticBox *_boxPreviewImage;

	wxSlider *_sliderColor;

	wxGenericStaticText *_textZoomLevel;

	wxButton *_buttonZoomIn;
	wxButton *_buttonZoomOut;
	wxButton *_buttonZoom100;
	wxButton *_buttonZoomFit;
	wxButton *_buttonZoomFitWidth;
	wxButton *_buttonZoomShrinkFit;
	wxButton *_buttonZoomShrinkFitWidth;

	wxCheckBox *_checkZoomNearest;

	ZoomOperation _lastZoomOperation;
	double _lastZoomLevel;

	void onColorChange(wxScrollEvent &event);

	void onZoomIn(wxCommandEvent &event);
	void onZoomOut(wxCommandEvent &event);
	void onZoom100(wxCommandEvent &event);
	void onZoomFit(wxCommandEvent &event);
	void onZoomFitWidth(wxCommandEvent &event);
	void onZoomShrinkFit(wxCommandEvent &event);
	void onZoomShrinkFitWidth(wxCommandEvent &event);

	void onZoomNearest(wxCommandEvent &event);

	void zoomStep(double step);
	void zoomTo(double zoom);
	void zoomFit(bool onlyWidth, bool grow);
	void zoomFitWidth(bool grow);

	void zoomTo(int width, int height, double zoom);

	double getCurrentZoomLevel() const;

	void updateZoomLevelText();
	void updateZoomOp(ZoomOperation op);
	void assertZoomOp();

	wxDECLARE_EVENT_TABLE();
};

} // End of namespace GUI

#endif // PANELPREVIEWIMAGE_H
