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
    QGridLayout *_layoutGrid;
    QVBoxLayout *_layoutVertical;
    QHBoxLayout *_layoutHorizontal;

    QPushButton *_buttonZoomIn;
    QPushButton *_buttonZoomOut;
    QPushButton *_buttonZoomOriginal;
    QPushButton *_buttonFit;
    QPushButton *_buttonFitWidth;
    QPushButton *_buttonShrinkFit;
    QPushButton *_buttonShrinkFitWidth;

    QLabel *_labelDimensions;
    QLabel *_labelBrightness;
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

public:
    PanelPreviewImage(QWidget *parent = 0);

    void setItem(const GUI::ResourceTreeItem *item);
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

/*slots:*/
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
