#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QPixmap>

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer(QWidget* parent = 0);
    ~ImageViewer();

public slots:
    void setData(const float* data, int dataSize);

private:
    QPixmap m_data;
};

#endif // IMAGEVIEWER_H
