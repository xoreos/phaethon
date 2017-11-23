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

#ifndef PANELPREVIEWIMAGE_H
#define PANELPREVIEWIMAGE_H

#include <QCheckBox>
#include <QFrame>
#include <QGraphicsView>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

#include "src/common/types.h"
#include "src/images/decoder.h"
#include "src/images/types.h"

namespace GUI {

class ResourceTreeItem;

class PanelPreviewImage : public QFrame {
    W_OBJECT(PanelPreviewImage)

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
    QLabel *_labelImage; // this label is used to display the image

    QCheckBox *_checkNearest;

    QSlider *_sliderBrightness;

    QScrollArea *_scrollAreaImage;

    const ResourceTreeItem *_currentItem;

    QPixmap _originalPixmap;
    QSize _originalSize;

    float _zoomFactor;

    Qt::TransformationMode _mode;

    void loadImage();
    void convertImage(const Images::Decoder &image, byte *data, QImage::Format &format);
    void writePixel(const byte *&data, Images::PixelFormat format, byte *&data_out, QImage::Format &format_out);
    void getImageDimensions(const Images::Decoder &image, int32 &width, int32 &height);
    void getSize(int &fullWidth, int &fullHeight, int &currentWidth, int &currentHeight) const;
    void fit(bool onlyWidth, bool grow);
    float getCurrentZoomLevel() const;
    void zoomTo(int width, int height, float zoom);
    void zoomTo(float zoom);
    void zoomStep(float step);
    void updateButtons();

public:
    PanelPreviewImage();

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
};

} // End of namespace GUI

#endif // PANELPREVIEWIMAGE_H
