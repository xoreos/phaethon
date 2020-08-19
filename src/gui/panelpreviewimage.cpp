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
 *  Preview panel for image resources.
 */

#include <memory>

#include <QImageReader>
#include <QCheckBox>
#include <QFrame>
#include <QGraphicsView>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QWidget>

#include "external/verdigris/wobjectimpl.h"

#include "src/gui/panelpreviewimage.h"
#include "src/gui/resourcetreeitem.h"

// FIXME: Zooming is kind of broken.

namespace GUI {

W_OBJECT_IMPL(PanelPreviewImage)

PanelPreviewImage::PanelPreviewImage(QWidget *parent) :
	PanelBase(parent) {
	QGridLayout *layoutTop = new QGridLayout(this);
	QVBoxLayout *layoutLeft = new QVBoxLayout();

	_buttonZoomIn = new QPushButton(tr("Zoom in"), this);
	_buttonZoomOut = new QPushButton(tr("Zoom out"), this);
	_buttonZoomOriginal = new QPushButton(tr("Zoom 100%"), this);
	_buttonFit = new QPushButton(tr("Fit"), this);
	_buttonFitWidth = new QPushButton(tr("Fit width"), this);
	_buttonShrinkFit = new QPushButton(tr("Shrink fit"), this);
	_buttonShrinkFitWidth = new QPushButton(tr("Shrink fit width"), this);

	_labelDimensions = new QLabel(this);
	QLabel *labelBrightness = new QLabel(tr("Background brightness"), this);
	_labelZoomPercent = new QLabel(this);
	_labelImage = new QLabel(this);

	_scrollAreaImage = new QScrollArea(this);

	_sliderBrightness = new QSlider(this);

	_checkNearest = new QCheckBox("Nearest", this);

	layoutLeft->addWidget(_labelZoomPercent);
	layoutLeft->addWidget(_buttonZoomIn);
	layoutLeft->addWidget(_buttonZoomOut);
	layoutLeft->addWidget(_buttonZoomOriginal);
	layoutLeft->addWidget(_buttonFit);
	layoutLeft->addWidget(_buttonFitWidth);
	layoutLeft->addWidget(_buttonShrinkFit);
	layoutLeft->addWidget(_buttonShrinkFitWidth);
	layoutLeft->addWidget(_checkNearest);
	layoutLeft->insertStretch(-1, 1); // get rid of spacing between buttons
	_labelZoomPercent->setAlignment(Qt::AlignCenter);

	layoutTop->addWidget(_labelDimensions, 0, 0);
	layoutTop->addWidget(labelBrightness, 1, 0);
	layoutTop->addWidget(_sliderBrightness, 1, 1);
	layoutTop->addLayout(layoutLeft, 2, 0);
	layoutTop->addWidget(_scrollAreaImage, 2, 1);

	_labelDimensions->setText(tr("(WxH)"));
	_labelZoomPercent->setText(tr("100%"));

	_labelImage->setBackgroundRole(QPalette::Base);
	_labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	_labelImage->setScaledContents(true);

	_scrollAreaImage->setWidget(_labelImage);

	_sliderBrightness->setOrientation(Qt::Horizontal);

	slotSliderBrightness(0);

	connect(_sliderBrightness, &QSlider::valueChanged, this, &PanelPreviewImage::slotSliderBrightness);
	connect(_buttonZoomIn, &QPushButton::clicked, this, &PanelPreviewImage::slotZoomIn);
	connect(_buttonZoomOut, &QPushButton::clicked, this, &PanelPreviewImage::slotZoomOut);
	connect(_buttonZoomOriginal, &QPushButton::clicked, this, &PanelPreviewImage::slotZoomOriginal);
	connect(_buttonFit, &QPushButton::clicked, this, &PanelPreviewImage::slotFit);
	connect(_buttonFitWidth, &QPushButton::clicked, this, &PanelPreviewImage::slotFitWidth);
	connect(_buttonShrinkFit, &QPushButton::clicked, this, &PanelPreviewImage::slotShrinkFit);
	connect(_buttonShrinkFitWidth, &QPushButton::clicked, this, &PanelPreviewImage::slotShrinkFitWidth);
	connect(_checkNearest, &QCheckBox::toggled, this, &PanelPreviewImage::slotNearest);
}

void PanelPreviewImage::show(const ResourceTreeItem *item) {
	PanelBase::show(item);

	_zoomFactor = 1.0f;
	_originalPixmap = QPixmap();
	_labelImage->setPixmap(_originalPixmap);

	if (item->getResourceType() != Aurora::kResourceImage)
		return;

	_currentItem = item;

	try {
		loadImage();
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}
}

static void cleanupImage(void *info) {
	byte *image = static_cast<byte *>(info);

	delete[] image;
}

void PanelPreviewImage::loadImage() {
	std::unique_ptr<Images::Decoder> image(_currentItem->getImage());

	if ((image->getMipMapCount() == 0) || (image->getLayerCount() == 0))
		return;

	int32_t width = 0, height = 0;
	getImageDimensions(*image, width, height);
	if ((width <= 0) || (height <= 0))
		throw Common::Exception("Invalid image dimensions (%d x %d)", width, height);

	_labelDimensions->setText(QString("(%1x%2)").arg(width).arg(height));

	std::unique_ptr<byte[]> rgbaData = std::make_unique<byte[]>(width * height * 4);
	std::memset(rgbaData.get(), 0, width * height * 4);

	convertImage(*image, rgbaData.get());

	QImage qImage(rgbaData.get(), width, height, QImage::Format_RGBA8888, cleanupImage, rgbaData.get());
	rgbaData.release();

	_originalPixmap = QPixmap::fromImage(qImage.mirrored());
	_originalSize = _originalPixmap.size();

	_labelImage->setPixmap(_originalPixmap);
	_labelImage->adjustSize();
	_labelImage->setFixedSize(_originalSize);
}

void PanelPreviewImage::convertImage(const Images::Decoder &image, byte *dataOut) {
	int32_t width, height;
	getImageDimensions(image, width, height);

	for (size_t i = 0; i < image.getLayerCount(); i++) {
		const Images::Decoder::MipMap &mipMap = image.getMipMap(0, i);
		const byte *mipMapData = mipMap.data.get();

		uint32_t count = mipMap.width * mipMap.height;
		while (count-- > 0)
			writePixel(mipMapData, image.getFormat(), dataOut);
	}
}

void PanelPreviewImage::writePixel(const byte *&dataIn, Images::PixelFormat format, byte *&dataOut) {
	switch (format) {
		case Images::kPixelFormatR8G8B8:
			*dataOut++ = dataIn[0];
			*dataOut++ = dataIn[1];
			*dataOut++ = dataIn[2];
			*dataOut++ = 0xFF;
			dataIn    += 3;
			break;

		case Images::kPixelFormatB8G8R8:
			*dataOut++ = dataIn[2];
			*dataOut++ = dataIn[1];
			*dataOut++ = dataIn[0];
			*dataOut++ = 0xFF;
			dataIn    += 3;
			break;

		case Images::kPixelFormatR8G8B8A8:
			*dataOut++ = dataIn[0];
			*dataOut++ = dataIn[1];
			*dataOut++ = dataIn[2];
			*dataOut++ = dataIn[3];
			dataIn    += 4;
			break;

		case Images::kPixelFormatB8G8R8A8:
			*dataOut++ = dataIn[2];
			*dataOut++ = dataIn[1];
			*dataOut++ = dataIn[0];
			*dataOut++ = dataIn[3];
			dataIn    += 4;
			break;

		case Images::kPixelFormatR5G6B5:
			{
				const uint16_t color = READ_LE_UINT16(dataIn);

				*dataOut++ =  color & 0x001F;
				*dataOut++ = (color & 0x07E0) >>  5;
				*dataOut++ = (color & 0xF800) >> 11;
				*dataOut++ = 0xFF;
				dataIn    += 2;
			}
			break;

		case Images::kPixelFormatA1R5G5B5:
			{
				const uint16_t color = READ_LE_UINT16(dataIn);

				*dataOut++ =  color & 0x001F;
				*dataOut++ = (color & 0x03E0) >>  5;
				*dataOut++ = (color & 0x7C00) >> 10;
				*dataOut++ = (color & 0x8000) ? 0xFF : 0x00;
				dataIn    += 2;
			}
			break;

		default:
			throw Common::Exception("Unsupported pixel format: %d", (int) format);
	}
}

void PanelPreviewImage::getImageDimensions(const Images::Decoder &image, int32_t &width, int32_t &height) {
	width  = image.getMipMap(0, 0).width;
	height = 0;

	for (size_t i = 0; i < image.getLayerCount(); i++) {
		const Images::Decoder::MipMap &mipMap = image.getMipMap(0, i);

		if (mipMap.width != width)
			throw Common::Exception("Unsupported image with variable layer width");

		height += mipMap.height;
	}
}

// FIXME: This also changes the scrollbar color
void PanelPreviewImage::slotSliderBrightness(int value) {
	int rgb = static_cast<int>(((float) value / (float) _sliderBrightness->maximum()) * 255.f);
	_scrollAreaImage->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(rgb).arg(rgb).arg(rgb));
}

void PanelPreviewImage::updateButtons() {
	_buttonZoomOut->setEnabled(_zoomFactor <= 5.0f);
	_buttonZoomIn->setEnabled(_zoomFactor >= 0.1f);
	_buttonZoomOriginal->setEnabled(_zoomFactor <= 0.95f || _zoomFactor >= 1.05f);
}

void PanelPreviewImage::zoomTo(int width, int height, float zoom) {
	float aspect = ((float) width) / ((float) height);

	// Calculate width using the zoom level and height using the aspect ratio
	width  = MAX<int>(width * zoom  , 1);
	height = MAX<int>(width / aspect, 1);

	QSize newSize(width, height);
	_labelImage->setPixmap(_originalPixmap.scaled(newSize, Qt::IgnoreAspectRatio, _mode));
	_labelImage->setFixedSize(newSize);

	_zoomFactor = zoom;
	updateButtons();
	_labelZoomPercent->setText(QString("%1%").arg((int) (_zoomFactor * 100)));
}

void PanelPreviewImage::zoomTo(float zoom) {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return;

	zoomTo(fullWidth, fullHeight, zoom);
}

void PanelPreviewImage::zoomStep(float step) {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return;

	float zoom = ((float) currentWidth) / ((float) fullWidth);

	// Only allow zoom from 10% to 500%
	float newZoom = CLIP<float>(zoom + step, 0.1f, 5.0f);
	if (zoom == newZoom)
		return;

	zoomTo(fullWidth, fullHeight, newZoom);
}

void PanelPreviewImage::slotZoomIn() {
	zoomStep(0.1f);
}

void PanelPreviewImage::slotZoomOut() {
	zoomStep(-0.1f);
}

void PanelPreviewImage::slotZoomOriginal() {
	_labelImage->setPixmap(_originalPixmap.scaled(_originalSize, Qt::IgnoreAspectRatio, _mode));
	_labelImage->setFixedSize(_originalSize);
	_zoomFactor = 1.0f;
	updateButtons();
	_labelZoomPercent->setText("100%");
	// fixme: there's probably a better way
}

void PanelPreviewImage::slotFit() {
	fit(false, true);
}

void PanelPreviewImage::slotFitWidth() {
	fit(true, true);
}

void PanelPreviewImage::slotShrinkFit() {
	fit(false, false);
}

void PanelPreviewImage::slotShrinkFitWidth() {
	fit(true, false);
}

// FIXME: zoom to 100% and shrink fit width are equivalent, and also don't cause a
// re-draw after this is toggled.
void PanelPreviewImage::slotNearest(bool checked) {
	if (checked)
		_mode = Qt::FastTransformation;
	else
		_mode = Qt::SmoothTransformation;

	_labelImage->setPixmap(_originalPixmap.scaled(_labelImage->size(), Qt::IgnoreAspectRatio, _mode));
	// fixme: there's probably a better way
}

void PanelPreviewImage::getSize(int &fullWidth, int &fullHeight, int &currentWidth, int &currentHeight) const {
	if (!_labelImage) {
		fullWidth = fullHeight = currentWidth = currentHeight = 0;
		return;
	}

	fullWidth     = _originalPixmap.width();
	fullHeight    = _originalPixmap.height();
	currentWidth  = _labelImage->width();
	currentHeight = _labelImage->height();
}

float PanelPreviewImage::getCurrentZoomLevel() const {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return 1.0f;

	return ((float) currentWidth) / ((float) fullWidth);
}

void PanelPreviewImage::fit(bool onlyWidth, bool grow) {
	int fullWidth, fullHeight, currentWidth, currentHeight;

	getSize(fullWidth, fullHeight, currentWidth, currentHeight);
	if ((fullWidth <= 0) || (fullHeight <= 0) || (currentWidth <= 0) || (currentHeight <= 0))
		return;

	float aspect = ((float) fullWidth) / ((float) fullHeight);

	QRect rect = _scrollAreaImage->contentsRect();
	int areaWidth = rect.width();
	int areaHeight = rect.height();

	// The size we want to scale to, depending on whether to grow the image
	int toWidth  = grow ? areaWidth  : MIN<int>(areaWidth , fullWidth);
	int toHeight = grow ? areaHeight : MIN<int>(areaHeight, fullHeight);

	// Try to fit by width
	int newWidth  = toWidth;
	int newHeight = MAX<int>(newWidth / aspect, 1);

	// If the height overflows, fit by height instead (if requested)
	if (!onlyWidth && (newHeight > toHeight)) {
		newHeight = toHeight;
		newWidth  = MAX<int>(newHeight * aspect, 1);
	}

	QSize newSize(newWidth, newHeight);
	_labelImage->setPixmap(_originalPixmap.scaled(newSize, Qt::IgnoreAspectRatio, _mode));
	_labelImage->setFixedSize(newSize);

	float zoomLevel = getCurrentZoomLevel() * 100;
	_zoomFactor = 100 / zoomLevel;
	_labelZoomPercent->setText(QString("%1%").arg((int) zoomLevel));

	_buttonZoomOriginal->setEnabled(true);
}

} // End of namespace GUI
