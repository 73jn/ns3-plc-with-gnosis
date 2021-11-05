#ifndef EDGEMODEL_H
#define EDGEMODEL_H

#include "plcvaluestring.h"

class EdgeModel
{
public:
    enum EdgeType{
        TwoPort = 0,
        Cable = 1,
        UserType = 65535
    };

    EdgeModel(QString fromNode, QString toNode);
    EdgeModel(const QVariantMap& map);

    void fromVariantMap(const QVariantMap& map);
    QVariantMap toVariantMap();

    QString getFromNode(){ return fromNode; }
    QString getToNode(){ return toNode; }
    QString getName(){ return name; }

    bool connectsSameNodes(const EdgeModel& other);

    void setName(const QString& newName) {name = newName;}
    void setFromNode(const QString& node) { fromNode = node; }
    void setToNode(const QString& node) { toNode = node; }
    bool isCableModel() {return isCable;}
    void setIsCableModel(bool isCable){ this->isCable = isCable;}

    void setCableType(const QString& cableType) { this->cableType = cableType; }
    QString getCableType(){return cableType;}

    void setLength(double length){this->length = length;}
    double getLength(){return length;}

    PLCValueString* getTwoPortParameters() { return &twoPortParameters[0];}

private:
    QString name;
    QString fromNode;
    QString toNode;

    bool isCable;
    QString cableType;
    double length;
    PLCValueString twoPortParameters[4];

};

#endif // EDGEMODEL_H
