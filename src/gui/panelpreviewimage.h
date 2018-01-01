#ifndef PANELPREVIEWIMAGE_H
#define PANELPREVIEWIMAGE_H

#include <QFrame>
#include <QGraphicsView>
#include <QScrollBar>
#include <QWidget>

#include "verdigris/wobjectdefs.h"

#include "src/common/types.h"
#include "src/images/decoder.h"
#include "src/images/types.h"

#include "ui/ui_previewimage.h"

namespace GUI {

class ResourceTreeItem;

class PanelPreviewImage : public QFrame
{
    W_OBJECT(PanelPreviewImage)

private:
    const GUI::ResourceTreeItem *_currentItem;
    uint8 _color;
    QLabel *_imageLabel;
    QPixmap _originalPixmap;
    double _scaleFactor = 1.0;
    Qt::TransformationMode _mode = Qt::SmoothTransformation;

    Ui::PreviewImage _ui;

public:
    PanelPreviewImage(QWidget *parent = 0);

    void setItem(const GUI::ResourceTreeItem *item);
    void loadImage();
    void convertImage(const Images::Decoder &image, byte *data, QImage::Format &format);
    void writePixel(const byte *&data, Images::PixelFormat format, byte *&data_out, QImage::Format &format_out);
    void getImageDimensions(const Images::Decoder &image, int32 &width, int32 &height);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateButtons();

/*slots:*/
    void darknessSliderSlot(int value);
    void zoomIn();
    void zoomOut();
    void zoomOriginal();
    void nearestToggled(bool checked);
};

} // End of namespace GUI

#endif // PANELPREVIEWIMAGE_H
