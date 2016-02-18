#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>

class ImageViewer : public QLabel
{
    Q_OBJECT

public:
    ImageViewer(QWidget* parent = 0);
    ~ImageViewer();

public slots:
    void setData(const QImage& image);

private:
    QPixmap m_data;
};

#endif // IMAGEVIEWER_H
