#include "netdevicemodel.h"

NetDeviceModel::NetDeviceModel()
{
    setupDefaults();
}

NetDeviceModel::NetDeviceModel(const QString &name){
    setupDefaults();
    this->name = name;
}

NetDeviceModel::NetDeviceModel(const QVariantMap &map){
    fromVariantMap(map);
}

void NetDeviceModel::setupDefaults(){
    this->name = "Dev";
    this->rxImpedance.setValue("50");
    this->txImpedance.setValue("0");

    this->isReceiver = true;
    this->isTransmitter = false;
}

void NetDeviceModel::setRXImpedance(const QString &value){
    this->rxImpedance = PLCValueString(value);
}

void NetDeviceModel::setTXImpedance(const QString &value){
    this->txImpedance = PLCValueString(value);
}

void NetDeviceModel::setShuntImpedance(const QString &value){
    this->shuntImpedance = PLCValueString(value);
}

QString NetDeviceModel::getName() const{
    return name;
}


void NetDeviceModel::fromVariantMap(const QVariantMap &map){
    this->name = map["Name"].toString();
    this->rxImpedance.setValue(map["RXImpedance"].toString());
    this->txImpedance.setValue(map["TXImpedance"].toString());
    this->shuntImpedance.setValue(map["ShuntImpedance"].toString());

    this->isTransmitter = map["TransmitterEnabled"].toBool();
    this->isReceiver = map["ReceiverEnabled"].toBool();


    //Make sure either transmitter or receiver is enabled
    //if(!(isTransmitter || isReceiver)){
    //    isReceiver = true;
    //    rxImpedance.setValue("50");
    //}
}

QVariantMap NetDeviceModel::toVariantMap(){
    QVariantMap map;

    map["Name"] = this->name;
    map["RXImpedance"] = this->rxImpedance.getValue();
    map["TXImpedance"] = this->txImpedance.getValue();
    map["ShuntImpedance"] = this->shuntImpedance.getValue();
    map["ReceiverEnabled"] = isReceiver;
    map["TransmitterEnabled"] = isTransmitter;

    return map;
}
