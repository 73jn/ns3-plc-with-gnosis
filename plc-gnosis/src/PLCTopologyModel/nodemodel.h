#ifndef NODEMODEL_H
#define NODEMODEL_H

#include <QList>
#include <complex>
#include <QPointF>
#include "noisesourcemodel.h"
#include "plcvaluestring.h"
#include "netdevicemodel.h"

class NodeModel
{
public:
    NodeModel();
    NodeModel(const QVariantMap& map);

    ~NodeModel();

    void setHasOutlet(bool hasOutlet) {this->hasOutlet = hasOutlet;}
    bool getHasOutlet() {return hasOutlet;}
    void setOutletImpedance(PLCValueString outletImpedance){this->outletImpedance = outletImpedance;}
    PLCValueString getOutletImpedance() { return outletImpedance; }

    NetDeviceModel* getNetDevice() { return netDevice; }
    NoiseSourceModel* getNoiseSource(){ return noiseSource; }

    //QList<NetDeviceModel *> * netDevices() {return &associatedNetDevices;}
    //QList<NoiseSourceModel *> * noiseSources() {return &associatedNoiseSources;}

    QString getName(){ return name; }
    void setName(const QString& newName){name = newName;}
    void setPosition(const QPointF & pos){position = pos;}
    QPointF getPosition(){return position;}


    void fromVariantMap(const QVariantMap& map);
    QVariantMap toVariantMap();

    void setNetDevice(NetDeviceModel* device);
    void setNoiseSource(NoiseSourceModel* source);

protected:

private:
    QString name;

    bool hasOutlet;
    PLCValueString outletImpedance;

    NetDeviceModel * netDevice;
    NoiseSourceModel* noiseSource;

    QPointF position;

};

#endif // NODEMODEL_H
