#ifndef NOISEMODEL_H
#define NOISEMODEL_H
#include <QString>
#include "plcvaluestring.h"

class NoiseSourceModel
{
public:
    NoiseSourceModel();
    NoiseSourceModel(const QVariantMap& map);

    QString getName() const;
    void setName(const QString& name) {this->name = name;}

    void setNoiseType(const QString& noiseType) {this->noiseSourceType = noiseType;}
    QString getNoiseType() const { return noiseSourceType;}

    PLCValueString getNoisePSD() const {return this->noisePSD;}
    void setNoisePSD(const QString& value) { this->noisePSD.setValue(value);}

    void fromVariantMap(const QVariantMap& map);
    QVariantMap toVariantMap();

private:
    QString name;
    QString noiseSourceType;

    PLCValueString noisePSD;
};

#endif // NOISEMODEL_H
