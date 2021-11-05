#ifndef ZOOMSLIDER_H
#define ZOOMSLIDER_H

#include <QSlider>

class ZoomSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ZoomSlider(QWidget *parent = 0);
    double getZoom(){ return zoomFactor; }

    void setMinZoom(double newMinZoom) {minZoom = newMinZoom; }
    void setMaxZoom(double newMaxZoom) {maxZoom = newMaxZoom; }
    void setZoomLimits(double newMinZoom, double newMaxZoom);
    void setZoomResolution(int newZoomResolution);

signals:
    void zoomChanged(double zoom);
    
public slots:
    void setZoom(double zoom);

private slots:
    void setZoomValue(int zoomValue);
    
private:
    void setupSlider();
    void adjustSlider();

    double minZoom;
    double maxZoom;
    int zoomResolution;
    double zoomFactor;
};

#endif // ZOOMSLIDER_H
