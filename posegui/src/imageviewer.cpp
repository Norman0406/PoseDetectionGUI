#include "imageviewer.h"
#include <QPainter>

ImageViewer::ImageViewer(QWidget* parent)
    : QLabel(parent)
{
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setScaledContents(true);
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::setData(const QImage& image)
{
    if (!image.isNull()) {
        setPixmap(QPixmap::fromImage(image));
        adjustSize();
    }
}
