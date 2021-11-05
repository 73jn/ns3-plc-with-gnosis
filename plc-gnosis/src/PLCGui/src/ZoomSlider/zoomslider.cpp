#include <QDebug>

#include "zoomslider.h"


ZoomSlider::ZoomSlider(QWidget *parent) :
    QSlider(Qt::Horizontal, parent)
{
    this->setMaximumWidth(200);
    this->setTracking(true);

    minZoom = 0.25;
    maxZoom = 8.0;
    zoomResolution = 1000;

    setupSlider();

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(setZoomValue(int)));
}

void ZoomSlider::setupSlider(){
    setMinimum(0);
    setMaximum(zoomResolution);
}

void ZoomSlider::adjustSlider(){
    double normalizedZoom =(zoomFactor - minZoom)/(maxZoom - minZoom);

    this->setValue(int(normalizedZoom*zoomResolution));
}

void ZoomSlider::setZoomLimits(double newMinZoom, double newMaxZoom){
    setMinZoom(newMinZoom);
    setMaxZoom(newMaxZoom);
}

void ZoomSlider::setZoomValue(int zoomValue){
    double normalizedZoom = double(zoomValue)/double(zoomResolution);
    double newZoom = minZoom + (normalizedZoom*(maxZoom-minZoom));

    if(newZoom != zoomFactor){
        zoomFactor = newZoom;
        emit zoomChanged(newZoom);
    }
}

void ZoomSlider::setZoomResolution(int newZoomResolution){
    if(newZoomResolution < 10){
        zoomResolution = 10;
    }
    else{
        zoomResolution = newZoomResolution;
    }

    setupSlider();
}

void ZoomSlider::setZoom(double newZoom){
    if(this->zoomFactor != newZoom){
        this->zoomFactor = newZoom;
        adjustSlider();
        emit zoomChanged(zoomFactor);
    }
}
