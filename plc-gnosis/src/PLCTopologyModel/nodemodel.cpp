#include "nodemodel.h"

NodeModel::NodeModel()
{
    hasOutlet = false;

    netDevice = 0;
    noiseSource = 0;
}

NodeModel::NodeModel(const QVariantMap &map){
    hasOutlet = false;

    netDevice = 0;
    noiseSource = 0;

    fromVariantMap(map);
}

NodeModel::~NodeModel(){
    if(netDevice != 0){
        delete netDevice;
    }

    if(noiseSource != 0){
        delete noiseSource;
    }
}

QVariantMap NodeModel::toVariantMap(){
    QVariantMap map;

    QVariantList pos;
    QVariantList netDevices;
    QVariantList noiseSources;


    pos << this->position.x() << this->position.y();

    map["Name"] = this->name;
    map["HasOutlet"] = this->hasOutlet;
    map["Position"] = pos;

    if(netDevice != 0){
        map["NetDevice"] = netDevice->toVariantMap();
    }

    if(noiseSource != 0){
        map["NoiseSource"] = noiseSource->toVariantMap();
    }

    map["OutletImpedance"] = outletImpedance.getValue();

    return map;
}

void NodeModel::fromVariantMap(const QVariantMap& map){

    QVariantList pos = map["Position"].toList();
    QVariantMap netDeviceMap = map["NetDevice"].toMap();
    QVariantMap noiseSourceMap = map["NoiseSource"].toMap();

    //Migration for older versions of the dgm files.
    if(netDeviceMap.isEmpty()){
        QVariantList netDevicesList = map["NetDevices"].toList();
        if(!netDevicesList.isEmpty()){
            netDeviceMap = netDevicesList.at(0).toMap();
        }
    }

    if(noiseSourceMap.isEmpty()){
        QVariantList noiseSourcesList = map["NoiseSources"].toList();
        if(!noiseSourcesList.isEmpty()){
            noiseSourceMap = noiseSourcesList.at(0).toMap();
        }
    }

    name = map["Name"].toString();
    position = QPointF(pos.at(0).toDouble(), pos.at(1).toDouble());

    if (!netDeviceMap.isEmpty()){
        netDevice = new NetDeviceModel(netDeviceMap);
    }

    if(!noiseSourceMap.isEmpty()){
        noiseSource = new NoiseSourceModel(noiseSourceMap);
    }

    this->hasOutlet = map["HasOutlet"].toBool();
    this->outletImpedance.setValue(map["OutletImpedance"].toString());
}

void NodeModel::setNetDevice(NetDeviceModel *device){
    if(this->netDevice != 0){
        delete (netDevice);
    }

    this->netDevice = device;
}

void NodeModel::setNoiseSource(NoiseSourceModel *source){
    if(this->noiseSource != 0){
        delete(noiseSource);
    }

    this->noiseSource = source;
}

