#include "plcspectrummodel.h"

PLCSpectrumModel::PLCSpectrumModel()
{
    this->lowerBandLimit = 0.0;
    this->upperBandLimit = 100000.00;
    this->bandResolution = 1000.00;

    this->mainsFrequency = 50;
    this->samplesPerCycle = 1;
    this->symbolLength = 1.0e6/(50.0);

    this->noiseFloor = 1e-7;
}

PLCSpectrumModel::PLCSpectrumModel(const QVariantMap& map){
    fromVariantMap(map);
}

QVariantMap PLCSpectrumModel::toVariantMap(){
    QVariantMap map;

    map["LowerBandLimit"] = lowerBandLimit;
    map["UpperBandLimit"] = upperBandLimit;
    map["BandResolution"] = bandResolution;

    map["MainsFrequency"] = mainsFrequency;
    map["SamplesPerCycle"] = samplesPerCycle;
    map["SymbolLength"] = symbolLength;

    map["NoiseFloor"] = noiseFloor;

    return map;
}

void PLCSpectrumModel::fromVariantMap(const QVariantMap& map){
    lowerBandLimit = map["LowerBandLimit"].toDouble();
    upperBandLimit = map["UpperBandLimit"].toDouble();
    bandResolution = map["BandResolution"].toDouble();

    samplesPerCycle = map["SamplesPerCycle"].toInt();

    if(!map.contains("SymbolLength")){
        symbolLength = 750;
    }

    symbolLength = map["SymbolLength"].toInt();
    mainsFrequency = map["MainsFrequency"].toInt();

    noiseFloor = map["NoiseFloor"].toDouble();
}
