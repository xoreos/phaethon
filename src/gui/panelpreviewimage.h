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

#ifndef GUI_PANELPREVIEWIMAGE_H
#define GUI_PANELPREVIEWIMAGE_H

#include "verdigris/wobjectdefs.h"

#include "src/common/types.h"

#include "src/images/decoder.h"
#include "src/images/types.h"

class QScrollArea;

namespace GUI {

class ResourceTreeItem;

class PanelPreviewImage : public QFrame {
	W_OBJECT(PanelPreviewImage)

public:
	PanelPreviewImage(QWidget *parent);

	void setItem(const GUI::ResourceTreeItem *item);

	// public slots:
	void slotSliderBrightness(int value);
	void slotZoomIn();
	void slotZoomOut();
	void slotZoomOriginal();
	void slotFit();
	void slotFitWidth();
	void slotShrinkFit();
	void slotShrinkFitWidth();
	void slotNearest(bool checked);

private:
	QPushButton *_buttonZoomIn;
	QPushButton *_buttonZoomOut;
	QPushButton *_buttonZoomOriginal;
	QPushButton *_buttonFit;
	QPushButton *_buttonFitWidth;
	QPushButton *_buttonShrinkFit;
	QPushButton *_buttonShrinkFitWidth;

	QLabel *_labelDimensions;
	QLabel *_labelZoomPercent;
	QLabel *_labelImage; ///< Label is used to display the image.

	QCheckBox *_checkNearest;

	QSlider *_sliderBrightness;

	QScrollArea *_scrollAreaImage;

	const ResourceTreeItem *_currentItem;

	// Necessary because the way zooming is implemented modifies the pixmap.
	QPixmap _originalPixmap; ///< To reset to default zoom level.
	QSize _originalSize; ///< To reset to default zoom level.

	float _zoomFactor;

	Qt::TransformationMode _mode; ///< Linear/nearest.

	/** Opens the image path contained in _currentItem and displays it. */
	void  loadImage();

	void  convertImage(const Images::Decoder &image, byte *dataOut);
	void  writePixel(const byte *&dataIn, Images::PixelFormat format, byte *&dataOut);
	void  getImageDimensions(const Images::Decoder &image, int32 &width, int32 &height);
	void  getSize(int &fullWidth, int &fullHeight, int &currentWidth, int &currentHeight) const;
	void  fit(bool onlyWidth, bool grow);
	float getCurrentZoomLevel() const;
	void  zoomTo(int width, int height, float zoom);
	void  zoomTo(float zoom);
	void  zoomStep(float step);
	void  updateButtons();
};

} // End of namespace GUI

#endif // GUI_PANELPREVIEWIMAGE_H
