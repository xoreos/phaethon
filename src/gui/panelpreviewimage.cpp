#include "panelpreviewimage.h"
#include "verdigris/wobjectimpl.h"

#include "src/gui/panel.h"

#include <QImageReader>

W_OBJECT_IMPL(PanelPreviewImage)

PanelPreviewImage::PanelPreviewImage(QWidget *parent)
    : _imageLabel(new QLabel(this))
    , _scaleFactor(1)
{
    _ui.setupUi(this);

    // get rid of spacing between buttons
    _ui.verticalLayout->insertStretch(-1, 1);

    _imageLabel->setBackgroundRole(QPalette::Base);
    _imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    _ui.imageScrollArea->setWidget(_imageLabel);
    _imageLabel->setScaledContents(true);

    darknessSliderSlot(0);

    connect(_ui.darkness, &QSlider::valueChanged, this, &PanelPreviewImage::darknessSliderSlot);
    connect(_ui.zoomIn, &QPushButton::clicked, this, &PanelPreviewImage::zoomIn);
    connect(_ui.zoomOut, &QPushButton::clicked, this, &PanelPreviewImage::zoomOut);
    connect(_ui.zoomOriginal, &QPushButton::clicked, this, &PanelPreviewImage::zoomOriginal);
    connect(_ui.nearest, &QCheckBox::toggled, this, &PanelPreviewImage::nearestToggled);


    // TODO
    _ui.fit->setEnabled(false);
    _ui.fitWidth->setEnabled(false);
    _ui.fitShrink->setEnabled(false);
    _ui.fitShrinkWidth->setEnabled(false);
}

void PanelPreviewImage::setItem(const ResourceTreeItem *item) {
    _scaleFactor = 1.0;
    _originalPixmap = QPixmap();
    _imageLabel->setPixmap(_originalPixmap);

    if (item->getResourceType() != Aurora::kResourceImage &&
        item->getFileType() != Aurora::FileType::kFileTypeICO)
            return;

    _currentItem = item;

    try {
        loadImage();
    } catch (Common::Exception &e) {
        Common::printException(e, "WARNING: ");
    }
}

void PanelPreviewImage::loadImage() {
    if (Panel::kPreviewMoreTypes) {
        if (_currentItem->getFileType() == Aurora::FileType::kFileTypeICO) {
            QImageReader reader(_currentItem->getPath());
            reader.setAutoTransform(true);
            const QImage newImage = reader.read();

            _originalPixmap = QPixmap::fromImage(newImage);
            _imageLabel->setPixmap(_originalPixmap);
            scaleImage(1.0);

            _ui.dimensions->setText(QString("(%1x%2)").arg(_originalPixmap.width()).arg(_originalPixmap.height()));
            _ui.imageScrollArea->show();
            return;
        }
    }

    QScopedPointer<Images::Decoder> image(_currentItem->getImage());

    if ((image->getMipMapCount() == 0) || (image->getLayerCount() == 0)) {
        return;
    }

    int32 width = 0, height = 0;
    getImageDimensions(*image, width, height);
    if ((width <= 0) || (height <= 0))
        throw Common::Exception("Invalid image dimensions (%d x %d)", width, height);

    _ui.dimensions->setText(QString("(%1x%2)").arg(width).arg(height));

    Common::ScopedArray<byte, Common::DeallocatorFree> data ((byte *) malloc(width * height * 4));
    std::memset(data.get(), 0, width * height * 4);

    QImage::Format format;
    convertImage(*image, data.get(), format);

    _originalPixmap = QPixmap::fromImage(QImage(data.get(), width, height, format).mirrored());
    _imageLabel->setPixmap(_originalPixmap);
    _imageLabel->adjustSize();
    scaleImage(1.0);

    data.release();
}

void PanelPreviewImage::convertImage(const Images::Decoder &image, byte *data_out, QImage::Format &format) {
    int32 width, height;
    getImageDimensions(image, width, height);

    for (size_t i = 0; i < image.getLayerCount(); i++) {
        const Images::Decoder::MipMap &mipMap = image.getMipMap(0, i);
        const byte *data = mipMap.data.get();

        uint32 count = mipMap.width * mipMap.height;
        while (count-- > 0)
            writePixel(data, image.getFormat(), data_out, format);
    }
}

void PanelPreviewImage::writePixel(const byte *&data, Images::PixelFormat format,
                                   byte *&data_out, QImage::Format &format_out) {
    if (format == Images::kPixelFormatR8G8B8) {
        format_out = QImage::Format_RGB888;
        *data_out++   = data[0];
        *data_out++   = data[1];
        *data_out++   = data[2];
        *data_out++ = 0xFF;
        data += 3;
    } else if (format == Images::kPixelFormatB8G8R8) {
        format_out = QImage::Format_RGB888;
        *data_out++   = data[2];
        *data_out++   = data[1];
        *data_out++   = data[0];
        *data_out++ = 0xFF;
        data += 3;
    } else if (format == Images::kPixelFormatR8G8B8A8) {
        format_out = QImage::Format_RGBA8888;
        *data_out++   = data[0];
        *data_out++   = data[1];
        *data_out++   = data[2];
        *data_out++ = data[3];
        data += 4;
    } else if (format == Images::kPixelFormatB8G8R8A8) {
        format_out = QImage::Format_RGBA8888;
        *data_out++   = data[2];
        *data_out++   = data[1];
        *data_out++   = data[0];
        *data_out++ = data[3];
        data += 4;
    } else if (format == Images::kPixelFormatR5G6B5) {
        format_out = QImage::Format_RGB555;
        uint16 color = READ_LE_UINT16(data);
        *data_out++   =  color & 0x001F;
        *data_out++   = (color & 0x07E0) >>  5;
        *data_out++   = (color & 0xF800) >> 11;
        *data_out++ = 0xFF;
        data += 2;
    } else if (format == Images::kPixelFormatA1R5G5B5) {
        format_out = QImage::Format_ARGB8555_Premultiplied;
        uint16 color = READ_LE_UINT16(data);
        *data_out++   =  color & 0x001F;
        *data_out++   = (color & 0x03E0) >>  5;
        *data_out++   = (color & 0x7C00) >> 10;
        *data_out++ = (color & 0x8000) ? 0xFF : 0x00;
        data += 2;
    } else
        throw Common::Exception("Unsupported pixel format: %d", (int) format);
}

void PanelPreviewImage::getImageDimensions(const Images::Decoder &image, int32 &width, int32 &height) {
    width  = image.getMipMap(0, 0).width;
    height = 0;

    for (size_t i = 0; i < image.getLayerCount(); i++) {
        const Images::Decoder::MipMap &mipMap = image.getMipMap(0, i);

        if (mipMap.width != width)
            throw Common::Exception("Unsupported image with variable layer width");

        height += mipMap.height;
    }
}

void PanelPreviewImage::darknessSliderSlot(int value) {
    int rgb = static_cast<int>(((float)value / (float)_ui.darkness->maximum()) * 255.f);
    _ui.imageScrollArea->setStyleSheet(QString("background-color: rgb(%1,%2,%3)").arg(rgb).arg(rgb).arg(rgb));
}

#define MAX_ZOOM 2

void PanelPreviewImage::updateButtons() {
    _ui.zoomOut->setEnabled(_scaleFactor >= 1.0 / MAX_ZOOM);
    _ui.zoomIn->setEnabled(_scaleFactor <= 1.0 * MAX_ZOOM);
    _ui.zoomOriginal->setEnabled(_scaleFactor <= 0.95 || _scaleFactor >= 1.05);
}

void PanelPreviewImage::scaleImage(double factor) {
    _scaleFactor *= factor;

    QSize size = _originalPixmap.size() * _scaleFactor;

    _imageLabel->setPixmap(_originalPixmap.scaled(size, Qt::IgnoreAspectRatio, _mode));
    _imageLabel->setFixedSize(size);

    _ui.zoomPercent->setText(QString("%1%").arg(_scaleFactor * 100));

    updateButtons();
}

void PanelPreviewImage::zoomIn() {
    scaleImage(1.1);
}

void PanelPreviewImage::zoomOut() {
    scaleImage(1.0/1.1);
}

void PanelPreviewImage::zoomOriginal() {
    scaleImage(1.0/_scaleFactor);
    _scaleFactor = 1.0;
}

void PanelPreviewImage::nearestToggled(bool checked) {
    if (checked)
        _mode = Qt::FastTransformation;
    else
        _mode = Qt::SmoothTransformation;

    scaleImage(1.0);
}
