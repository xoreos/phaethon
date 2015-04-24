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

#include <cstdlib>

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dc.h>

#include <wx/generic/stattextg.h>

#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"

#include "images/decoder.h"

#include "gui/panelpreviewimage.h"
#include "gui/eventid.h"
#include "gui/resourcetree.h"

namespace GUI {

ImageCanvas::ImageCanvas(wxWindow *parent) : wxScrolledCanvas(parent, wxID_ANY),
	_currentItem(0), _color(0), _scaleQuality(kScaleQualityBest), _image(0), _bitmap(0) {

	ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
}

ImageCanvas::~ImageCanvas() {
	delete _image;
	delete _bitmap;
}

void ImageCanvas::forceRedraw() {
	Refresh();
	Update();
}

void ImageCanvas::setCurrentItem(const ResourceTreeItem *item) {
	if (item == _currentItem)
		return;

	_currentItem = item;
	loadImage();
}

void ImageCanvas::setColor(uint8 color) {
	_color = color;
	forceRedraw();
}

void ImageCanvas::setScaleQuality(ScaleQuality scaleQuality) {
	_scaleQuality = scaleQuality;
	if (_image && _bitmap)
		setSize(_bitmap->GetWidth(), _bitmap->GetHeight());
}

void ImageCanvas::getSize(int &fullWidth, int &fullHeight, int &currentWidth, int &currentHeight) const {
	if (!_image || !_bitmap) {
		fullWidth = fullHeight = currentWidth = currentHeight = 0;
		return;
	}

	fullWidth     = _image->GetWidth();
	fullHeight    = _image->GetHeight();
	currentWidth  = _bitmap->GetWidth();
	currentHeight = _bitmap->GetHeight();
}

void ImageCanvas::setSize(int width, int height) {
	assert((width > 0) && (height > 0));

	if (!_image)
		return;

	delete _bitmap;

	wxImageResizeQuality quality = wxIMAGE_QUALITY_NEAREST;
	if (_scaleQuality == kScaleQualityBest)
		quality = wxIMAGE_QUALITY_HIGH;

	_bitmap = new wxBitmap(_image->Scale(width, height, quality));

	SetVirtualSize(width, height);
	SetScrollRate(1, 1);

	forceRedraw();
}

void ImageCanvas::loadImage() {
	delete _image;
	delete _bitmap;

	_image  = 0;
	_bitmap = 0;

	if (!_currentItem || (_currentItem->getResourceType() != Aurora::kResourceImage)) {
		SetVirtualSize(0, 0);
		SetScrollRate(0, 0);
		return;
	}

	Images::Decoder *image = 0;
	try {
		image = _currentItem->getImage();
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
		return;
	}

	if (image->getMipMapCount() == 0) {
		delete image;

		SetVirtualSize(0, 0);
		SetScrollRate(0, 0);
		return;
	}

	const Images::Decoder::MipMap &mipMap = image->getMipMap(0);

	byte *data_rgb   = (byte *) malloc(mipMap.width * mipMap.height * 3);
	byte *data_alpha = (byte *) malloc(mipMap.width * mipMap.height);

	memset(data_rgb  , 0, mipMap.width * mipMap.height * 3);
	memset(data_alpha, 0, mipMap.width * mipMap.height);

	try {
		convertImage(*image, data_rgb, data_alpha);
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");

		free(data_rgb);
		free(data_alpha);
		return;
	}

	wxImage *mirror = new wxImage(mipMap.width, mipMap.height, data_rgb, data_alpha);
	_image = new wxImage(mirror->Mirror(false));

	delete mirror;

	_bitmap = new wxBitmap(*_image);

	SetVirtualSize(mipMap.width, mipMap.height);
	SetScrollRate(1, 1);
}

void ImageCanvas::writePixel(const byte *&data, Images::PixelFormat format,
                                   byte *&data_rgb, byte *&data_alpha) {

	if (format == Images::kPixelFormatR8G8B8) {
		*data_rgb++   = data[0];
		*data_rgb++   = data[1];
		*data_rgb++   = data[2];
		*data_alpha++ = 0xFF;
		data += 3;
	} else if (format == Images::kPixelFormatB8G8R8) {
		*data_rgb++   = data[2];
		*data_rgb++   = data[1];
		*data_rgb++   = data[0];
		*data_alpha++ = 0xFF;
		data += 3;
	} else if (format == Images::kPixelFormatR8G8B8A8) {
		*data_rgb++   = data[0];
		*data_rgb++   = data[1];
		*data_rgb++   = data[2];
		*data_alpha++ = data[3];
		data += 4;
	} else if (format == Images::kPixelFormatB8G8R8A8) {
		*data_rgb++   = data[2];
		*data_rgb++   = data[1];
		*data_rgb++   = data[0];
		*data_alpha++ = data[3];
		data += 4;
	} else if (format == Images::kPixelFormatR5G6B5) {
		uint16 color = READ_LE_UINT16(data);
		*data_rgb++   =  color & 0x001F;
		*data_rgb++   = (color & 0x07E0) >>  5;
		*data_rgb++   = (color & 0xF800) >> 11;
		*data_alpha++ = 0xFF;
		data += 2;
	} else if (format == Images::kPixelFormatA1R5G5B5) {
		uint16 color = READ_LE_UINT16(data);
		*data_rgb++   =  color & 0x001F;
		*data_rgb++   = (color & 0x03E0) >>  5;
		*data_rgb++   = (color & 0x7C00) >> 10;
		*data_alpha++ = (color & 0x8000) ? 0xFF : 0x00;
		data += 2;
	} else
		throw Common::Exception("Unsupported pixel format: %d", (int) format);
}

void ImageCanvas::convertImage(const Images::Decoder &image, byte *data_rgb, byte *data_alpha) {
	const Images::Decoder::MipMap &mipMap = image.getMipMap(0);

	uint32 count = mipMap.width * mipMap.height;
	const byte *data = mipMap.data;
	for (uint32 i = 0; i < count; i++)
		writePixel(data, image.getFormat(), data_rgb, data_alpha);
}

void ImageCanvas::OnDraw(wxDC &dc) {
	int vw, vh, pw, ph;

	GetVirtualSize(&vw, &vh);
	GetClientSize(&pw, &ph);

	if (!_bitmap)
		return;

	// Fill the surface with the current color, then draw the image (with transparency)
	dc.SetBackground(wxBrush(wxColor(_color, _color, _color)));
	dc.Clear();
	dc.DrawBitmap(*_bitmap, 0, 0, true);
}


wxBEGIN_EVENT_TABLE(PanelPreviewImage, wxPanel)
	EVT_COMMAND_SCROLL(kEventSliderColor, PanelPreviewImage::onColorChange)

	EVT_BUTTON(kEventButtonZoomIn            , PanelPreviewImage::onZoomIn)
	EVT_BUTTON(kEventButtonZoomOut           , PanelPreviewImage::onZoomOut)
	EVT_BUTTON(kEventButtonZoom100           , PanelPreviewImage::onZoom100)
	EVT_BUTTON(kEventButtonZoomFit           , PanelPreviewImage::onZoomFit)
	EVT_BUTTON(kEventButtonZoomFitWidth      , PanelPreviewImage::onZoomFitWidth)
	EVT_BUTTON(kEventButtonZoomShrinkFit     , PanelPreviewImage::onZoomShrinkFit)
	EVT_BUTTON(kEventButtonZoomShrinkFitWidth, PanelPreviewImage::onZoomShrinkFitWidth)

	EVT_CHECKBOX(kEventCheckZoomNearest, PanelPreviewImage::onZoomNearest)
wxEND_EVENT_TABLE()

PanelPreviewImage::PanelPreviewImage(wxWindow *parent, const Common::UString &title) :
	wxPanel(parent, wxID_ANY), _lastZoomOperation(kZoomOpLevel), _lastZoomLevel(1.0) {

	wxStaticBox *boxPreviewImage = new wxStaticBox(this, wxID_ANY, title);
	boxPreviewImage->Lower();

	wxStaticBoxSizer *sizerPreviewImage = new wxStaticBoxSizer(boxPreviewImage, wxVERTICAL);

	_sliderColor = new wxSlider(this, kEventSliderColor, 0, 0, 255,
	                            wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);

	_canvas = new ImageCanvas(this);

	_textZoomLevel = new wxGenericStaticText(this, wxID_ANY, wxT("100%"));

	_buttonZoomIn             = new wxButton(this, kEventButtonZoomIn            , wxT("Zoom in"));
	_buttonZoomOut            = new wxButton(this, kEventButtonZoomOut           , wxT("Zoom out"));
	_buttonZoom100            = new wxButton(this, kEventButtonZoom100           , wxT("Zoom 100%"));
	_buttonZoomFit            = new wxButton(this, kEventButtonZoomFit           , wxT("Fit"));
	_buttonZoomFitWidth       = new wxButton(this, kEventButtonZoomFitWidth      , wxT("Fit width"));
	_buttonZoomShrinkFit      = new wxButton(this, kEventButtonZoomShrinkFit     , wxT("Shrink fit"));
	_buttonZoomShrinkFitWidth = new wxButton(this, kEventButtonZoomShrinkFitWidth, wxT("Shrink fit width"));

	_checkZoomNearest = new wxCheckBox(this, kEventCheckZoomNearest, wxT("Nearest"));
	_checkZoomNearest->SetValue(false);

	wxBoxSizer *sizerZoom = new wxBoxSizer(wxVERTICAL);

	sizerZoom->Add(_textZoomLevel, 0, wxALIGN_CENTER_HORIZONTAL, 0);

	sizerZoom->Add(_buttonZoomIn            , 0, wxEXPAND, 0);
	sizerZoom->Add(_buttonZoomOut           , 0, wxEXPAND, 0);
	sizerZoom->Add(_buttonZoom100           , 0, wxEXPAND, 0);
	sizerZoom->Add(_buttonZoomFit           , 0, wxEXPAND, 0);
	sizerZoom->Add(_buttonZoomFitWidth      , 0, wxEXPAND, 0);
	sizerZoom->Add(_buttonZoomShrinkFit     , 0, wxEXPAND, 0);
	sizerZoom->Add(_buttonZoomShrinkFitWidth, 0, wxEXPAND, 0);
	sizerZoom->Add(_checkZoomNearest        , 0, wxEXPAND, 0);

	wxBoxSizer *sizerImage = new wxBoxSizer(wxHORIZONTAL);

	sizerImage->Add(sizerZoom, 0, wxEXPAND, 0);
	sizerImage->Add(_canvas  , 1, wxEXPAND, 0);

	sizerPreviewImage->Add(_sliderColor, 0, wxEXPAND, 0);
	sizerPreviewImage->Add(sizerImage  , 1, wxEXPAND, 0);

	SetSizer(sizerPreviewImage);
}

PanelPreviewImage::~PanelPreviewImage() {
}

void PanelPreviewImage::setCurrentItem(const ResourceTreeItem *item) {
	_canvas->setCurrentItem(item);

	if (item) {
		assertZoomOp();
		updateZoomLevelText();
	}
}

void PanelPreviewImage::onColorChange(wxScrollEvent &event) {
	_canvas->setColor(_sliderColor->GetValue());
}

void PanelPreviewImage::onZoomIn(wxCommandEvent &event) {
	zoomStep(0.1);

	updateZoomOp(kZoomOpLevel);
	updateZoomLevelText();
}

void PanelPreviewImage::onZoomOut(wxCommandEvent &event) {
	zoomStep(-0.1);

	updateZoomOp(kZoomOpLevel);
	updateZoomLevelText();
}

void PanelPreviewImage::onZoom100(wxCommandEvent &event) {
	zoomTo(1.0);

	updateZoomOp(kZoomOpLevel);
	updateZoomLevelText();
}

void PanelPreviewImage::onZoomFit(wxCommandEvent &event) {
	zoomFit(false, true);

	updateZoomOp(kZoomOpFit);
	updateZoomLevelText();
}

void PanelPreviewImage::onZoomFitWidth(wxCommandEvent &event) {
	zoomFit(true, true);

	updateZoomOp(kZoomOpFitWidth);
	updateZoomLevelText();
}

void PanelPreviewImage::onZoomShrinkFit(wxCommandEvent &event) {
	zoomFit(false, false);

	updateZoomOp(kZoomOpShrinkFit);
	updateZoomLevelText();
}

void PanelPreviewImage::onZoomShrinkFitWidth(wxCommandEvent &event) {
	zoomFit(true, false);

	_lastZoomOperation = kZoomOpShrinkFitWidth;
	_lastZoomLevel     = getCurrentZoomLevel();

	updateZoomLevelText();
}

void PanelPreviewImage::onZoomNearest(wxCommandEvent &event) {
	_canvas->setScaleQuality(_checkZoomNearest->GetValue() ?
			ImageCanvas::kScaleQualityNearest : ImageCanvas::kScaleQualityBest);
}

void PanelPreviewImage::zoomTo(int width, int height, double zoom) {
	double aspect = ((double) width) / ((double) height);

	// Calculate width using the zoom level and height using the aspect ratio
	width  = MAX<int>(width * zoom  , 1);
	height = MAX<int>(width / aspect, 1);

	_canvas->setSize(width, height);
}

void PanelPreviewImage::zoomStep(double step) {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	_canvas->getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return;

	double zoom = ((double) currentWidth) / ((double) fullWidth);

	// Only allow zoom from 10% to 500%
	double newZoom = CLIP<double>(zoom + step, 0.1, 5.0);
	if (zoom == newZoom)
		return;

	zoomTo(fullWidth, fullHeight, newZoom);
}

void PanelPreviewImage::zoomTo(double zoom) {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	_canvas->getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return;

	zoomTo(fullWidth, fullHeight, zoom);
}

void PanelPreviewImage::zoomFit(bool onlyWidth, bool grow) {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	_canvas->getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return;

	double aspect = ((double) fullWidth) / ((double) fullHeight);

	int canvasWidth, canvasHeight;
	_canvas->GetClientSize(&canvasWidth, &canvasHeight);

	// The size we want to scale to, depending on whether to grow the image
	int toWidth  = grow ? canvasWidth  : MIN<int>(canvasWidth , fullWidth);
	int toHeight = grow ? canvasHeight : MIN<int>(canvasHeight, fullHeight);

	// Try to fit by width
	int newWidth  = toWidth;
	int newHeight = MAX<int>(newWidth / aspect, 1);

	// If the height overflows, fit by height instead (if requested)
	if (!onlyWidth && (newHeight > toHeight)) {
		newHeight = toHeight;
		newWidth  = MAX<int>(newHeight * aspect, 1);
	}

	_canvas->setSize(newWidth, newHeight);
}

void PanelPreviewImage::updateZoomLevelText() {
	Common::UString percent = Common::UString::sprintf("%d%%", (int) (getCurrentZoomLevel() * 100));

	_textZoomLevel->SetLabel(percent);
	_textZoomLevel->Fit();
}

double PanelPreviewImage::getCurrentZoomLevel() const {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	_canvas->getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return 1.0;

	return ((double) currentWidth) / ((double) fullWidth);
}

void PanelPreviewImage::updateZoomOp(ZoomOperation op) {
	_lastZoomOperation = op;
	_lastZoomLevel     = getCurrentZoomLevel();
}

void PanelPreviewImage::assertZoomOp() {
	switch (_lastZoomOperation) {
		case kZoomOpLevel:
			zoomTo(_lastZoomLevel);
			break;

		case kZoomOpFit:
			zoomFit(false, true);
			_lastZoomLevel = getCurrentZoomLevel();
			break;

		case kZoomOpFitWidth:
			zoomFit(true, true);
			_lastZoomLevel = getCurrentZoomLevel();
			break;

		case kZoomOpShrinkFit:
			zoomFit(false, false);
			_lastZoomLevel = getCurrentZoomLevel();
			break;

		case kZoomOpShrinkFitWidth:
			zoomFit(true, false);
			_lastZoomLevel = getCurrentZoomLevel();
			break;
	}
}

} // End of namespace GUI
