#include "noisesourcemodel.h"

NoiseSourceModel::NoiseSourceModel()
{
    this->noiseSourceType = "Static";
    this->noisePSD.setValue("1e-7");
    this->name = "Noise";
}

NoiseSourceModel::NoiseSourceModel(const QVariantMap& map){
    fromVariantMap(map);
}


QVariantMap NoiseSourceModel::toVariantMap(){
    QVariantMap map;

    map["Name"] = this->name;
    map["Type"] = this->noiseSourceType;
    map["NoisePSD"] = this->noisePSD.getValue();

    return map;
}

void NoiseSourceModel::fromVariantMap(const QVariantMap& map){
    name = map["Name"].toString();
    noiseSourceType = map["Type"].toString();
    noisePSD.setValue(map["NoisePSD"].toString());
}

QString NoiseSourceModel::getName() const{
    return name;
}
