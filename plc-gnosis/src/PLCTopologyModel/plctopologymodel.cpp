#include "plctopologymodel.h"

PLCTopologyModel::PLCTopologyModel()
{
}

PLCTopologyModel::PLCTopologyModel(const PLCSpectrumModel &spectrumModel, const QList<EdgeModel *> &edges, const QList<NodeModel *> &nodes){
    this->nodes = nodes;
    this->edges = edges;
    this->spectrumModel = spectrumModel;
}

PLCTopologyModel::PLCTopologyModel(const QVariantMap &map){
    fromVariantMap(map);
}

QVariantMap PLCTopologyModel::toVariantMap(){
    QVariantMap map;

    map["SpectrumModel"] = this->spectrumModel.toVariantMap();

    QVariantList nodeList;
    for(int i = 0; i < nodes.length(); i++){
        nodeList.append(nodes.at(i)->toVariantMap());
    }

    map["Nodes"] = nodeList;

    QVariantList edgeList;
    for(int i = 0; i < edges.length(); i++){
        edgeList.append(edges.at(i)->toVariantMap());
    }

    map["Edges"] = edgeList;

    return map;
}

void PLCTopologyModel::fromVariantMap(const QVariantMap& map){
    this->spectrumModel = PLCSpectrumModel(map["SpectrumModel"].toMap());

    QVariantList nodeList = map["Nodes"].toList();
    QVariantList edgeList = map["Edges"].toList();

    for(int i = 0; i < nodeList.length(); i++){
        this->nodes << new NodeModel(nodeList.at(i).toMap());
    }

    for(int i = 0; i < edgeList.length(); i++){
        this->edges << new EdgeModel(edgeList.at(i).toMap());
    }
}
