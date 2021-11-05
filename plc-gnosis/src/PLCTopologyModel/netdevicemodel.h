#ifndef NETDEVICEMODEL_H
#define NETDEVICEMODEL_H

#include "plcvaluestring.h"

class NetDeviceModel
{
public:
    NetDeviceModel();
    NetDeviceModel(const QString& name);
    NetDeviceModel(const QVariantMap& map);

    void setTXImpedance(const QString& value);
    void setRXImpedance(const QString& value);
    void setShuntImpedance(const QString& value);

    PLCValueString getTXImpedance() { return txImpedance; }
    PLCValueString getRXImpedance() { return rxImpedance; }
    PLCValueString getShuntImpedance() {return shuntImpedance; }

    QString getName() const;
    void setName(const QString & name) { this->name = name;}

    void fromVariantMap(const QVariantMap& map);
    QVariantMap toVariantMap();

    bool receiverEnabled(){return isReceiver;}
    bool transmitterEnabled(){return isTransmitter;}

    void setReceiverEnabled(bool on){isReceiver = on;}
    void setTransmitterEnabled(bool on){isTransmitter = on;}

private:
    QString name;

    bool isTransmitter;
    bool isReceiver;

    PLCValueString txImpedance;
    PLCValueString rxImpedance;
    PLCValueString shuntImpedance;

    void setupDefaults();
};

#endif // NETDEVICEMODEL_H
