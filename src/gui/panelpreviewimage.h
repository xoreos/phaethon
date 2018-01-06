#ifndef PANELPREVIEWIMAGE_H
#define PANELPREVIEWIMAGE_H

#include "ui/ui_previewimage.h"
#include "verdigris/wobjectdefs.h"

#include "src/gui/resourcetreeitem.h"

#include <QFrame>
#include <QGraphicsView>
#include <QScrollBar>
#include <QWidget>

class PanelPreviewImage : public QFrame
{
    W_OBJECT(PanelPreviewImage)

private:
    ResourceTreeItem *_currentItem;
    uint8 _color;
    QLabel *_imageLabel;
    QPixmap _originalPixmap;
    double _scaleFactor = 1.0;
    Qt::TransformationMode _mode = Qt::SmoothTransformation;

    Ui::PreviewImage _ui;

public:
    PanelPreviewImage(QWidget *parent = 0);

    void setItem(ResourceTreeItem *node);
    void loadImage();
    void convertImage(const Images::Decoder &image, byte *data, QImage::Format &format);
    void writePixel(const byte *&data, Images::PixelFormat format, byte *&data_out, QImage::Format &format_out);
    void getImageDimensions(const Images::Decoder &image, int32 &width, int32 &height);

    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateButtons();

    void darknessSliderSlot(int value);
    W_SLOT(darknessSliderSlot, W_Access::Private)

    void zoomIn();
    W_SLOT(zoomIn, W_Access::Private)

    void zoomOut();
    W_SLOT(zoomOut, W_Access::Private)

    void zoomOriginal();
    W_SLOT(zoomOriginal, W_Access::Private)

    void nearestToggled(bool checked);
    W_SLOT(nearestToggled, W_Access::Private)
};

#endif // PANELPREVIEWIMAGE_H
