#include "edgemodel.h"
#include <QDebug>

EdgeModel::EdgeModel(QString fromNode, QString toNode)
{
    twoPortParameters[0] = PLCValueString("1.0");
    twoPortParameters[1] = PLCValueString("0.0");
    twoPortParameters[2] = PLCValueString("1.0");
    twoPortParameters[3] = PLCValueString("0.0");

    this->cableType = "PLC_NAYY150SE_Cable";
    this->length = 10.0;

    this->isCable = true;

    this->fromNode = fromNode;
    this->toNode = toNode;
}

EdgeModel::EdgeModel(const QVariantMap &map){
    fromVariantMap(map);
}

void EdgeModel::fromVariantMap(const QVariantMap &map){
    this->name = map["Name"].toString();
    this->fromNode = map["FromNode"].toString();
    this->toNode = map["ToNode"].toString();

    if(map["EdgeType"].toString() == "Cable"){
        isCable = true;

        this->cableType = map["CableType"].toString();
        if(cableType.isEmpty()){
            qDebug() << "Empty cable type: Setting default value (PLC_NAYY150SE_Cable)";
            cableType = "PLC_NAYY150SE_Cable";
        }

        if(!map.contains("Length")){
            this->length = 10;
        }
        else{
            this->length = map["Length"].toDouble();
        }

    }
    else{
        isCable = false;
        length = 1;

        QVariantList parameters = map["Parameters"].toList();

        for(int i = 0; i < 4; i++){
            twoPortParameters[i] = PLCValueString(parameters.at(i).toString());
        }
    }

}

QVariantMap EdgeModel::toVariantMap(){
    QVariantMap map;

    map["Name"] = name;
    map["FromNode"] = fromNode;
    map["ToNode"] = toNode;

    if(isCable){

        map["EdgeType"] = "Cable";
        map["CableType"] = cableType;
        map["Length"] = this->length;

    } else
    {
        QVariantList paramList;

        for(int i =0; i < 4; i++){
            paramList.append(twoPortParameters[i].getValue());
        }

        map["EdgeType"] = "TwoPort";
        map["Parameters"] = paramList;
    }

    return map;
}

bool EdgeModel::connectsSameNodes(const EdgeModel &other){

    bool sameFrom = (other.fromNode == this->fromNode);
    bool sameTo = (other.toNode == this->toNode);

    if(sameFrom && sameTo){
        qDebug() << "Fail hard";
    }


    bool connectsSame = ( ((this->fromNode == other.fromNode) && (this->toNode == other.toNode)) ||
             ((this->fromNode == other.toNode) && (this->toNode == other.fromNode)) );

    return connectsSame;
}

