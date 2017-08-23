#include <QImageReader>

#include "verdigris/wobjectimpl.h"

#include "src/gui/panel.h"
#include "src/gui/panelpreviewimage.h"
#include "src/gui/resourcetreeitem.h"

namespace GUI {

W_OBJECT_IMPL(PanelPreviewImage)

PanelPreviewImage::PanelPreviewImage(QWidget *parent)
    : QFrame(parent) {
    _zoomFactor = 1.0f;

    _mode = Qt::SmoothTransformation;

    _layoutGrid = new QGridLayout(this);
    _layoutVertical = new QVBoxLayout();
    _layoutHorizontal = new QHBoxLayout();

    _buttonZoomIn = new QPushButton(tr("Zoom in"), this);
    _buttonZoomOut = new QPushButton(tr("Zoom out"), this);
    _buttonZoomOriginal = new QPushButton(tr("Zoom 100%"), this);
    _buttonFit = new QPushButton(tr("Fit"), this);
    _buttonFitWidth = new QPushButton(tr("Fit width"), this);
    _buttonShrinkFit = new QPushButton(tr("Shrink fit"), this);
    _buttonShrinkFitWidth = new QPushButton(tr("Shrink fit width"), this);

    _labelDimensions = new QLabel(this);
    _labelBrightness = new QLabel(this);
    _labelZoomPercent = new QLabel(this);
    _labelImage = new QLabel(this);

    _scrollAreaImage = new QScrollArea(this);

    _sliderBrightness = new QSlider(this);

    _checkNearest = new QCheckBox("Nearest", this);

    _layoutVertical->addWidget(_labelZoomPercent);
    _layoutVertical->addWidget(_buttonZoomIn);
    _layoutVertical->addWidget(_buttonZoomOut);
    _layoutVertical->addWidget(_buttonZoomOriginal);
    _layoutVertical->addWidget(_buttonFit);
    _layoutVertical->addWidget(_buttonFitWidth);
    _layoutVertical->addWidget(_buttonShrinkFit);
    _layoutVertical->addWidget(_buttonShrinkFitWidth);
    _layoutVertical->addWidget(_checkNearest);
    _layoutVertical->insertStretch(-1, 1); // get rid of spacing between buttons
    _labelZoomPercent->setAlignment(Qt::AlignCenter);

    _layoutGrid->addWidget(_labelDimensions, 0, 0);
    _layoutGrid->addWidget(_labelBrightness, 1, 0);
    _layoutGrid->addWidget(_sliderBrightness, 1, 1);
    _layoutGrid->addLayout(_layoutVertical, 2, 0);
    _layoutGrid->addWidget(_scrollAreaImage, 2, 1);

    _labelDimensions->setText(tr("(WxH)"));
    _labelBrightness->setText(tr("Background brightness"));
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

void PanelPreviewImage::setItem(const ResourceTreeItem *item) {
    _zoomFactor = 1.0f;
    _originalPixmap = QPixmap();
    _labelImage->setPixmap(_originalPixmap);

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
            _originalSize = _originalPixmap.size();
            _labelImage->setPixmap(_originalPixmap);
            _labelImage->setFixedSize(_originalSize);

            _labelDimensions->setText(QString("(%1x%2)").arg(_originalPixmap.width()).arg(_originalPixmap.height()));
            _scrollAreaImage->show();
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

    _labelDimensions->setText(QString("(%1x%2)").arg(width).arg(height));

    Common::ScopedArray<byte, Common::DeallocatorFree> data ((byte *) malloc(width * height * 4));
    std::memset(data.get(), 0, width * height * 4);

    QImage::Format format;
    convertImage(*image, data.get(), format);

    _originalPixmap = QPixmap::fromImage(QImage(data.get(), width, height, format).mirrored());
    _originalSize = _originalPixmap.size();
    _labelImage->setPixmap(_originalPixmap);
    _labelImage->adjustSize();
    _labelImage->setFixedSize(_originalSize);

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

void PanelPreviewImage::slotSliderBrightness(int value) {
    int rgb = static_cast<int>(((float)value / (float)_sliderBrightness->maximum()) * 255.f);
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
    _labelZoomPercent->setText(QString("%1%").arg((int)(_zoomFactor*100)));
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

// fixme: zoom to 100% and shrink fit width are equal, and don't cause a
// re-draw after this is toggled
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
    _labelZoomPercent->setText(QString("%1%").arg((int)zoomLevel));

    _buttonZoomOriginal->setEnabled(true);
}

} // End of namespace GUI
