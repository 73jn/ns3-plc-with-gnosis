#include <QFileInfo>

#include "plcvaluestring.h"
#include "complexstringvalidator.h"
#include <QStringList>

PLCValueString::PLCValueString(){
    this->value.clear();
}

PLCValueString::PLCValueString(const QString &value)
{
    setValue(value);
}

QString PLCValueString::getValue(){
    return this->value;
}

void PLCValueString::setValue(const QString &value){
    this->value = value;
}

bool PLCValueString::isValid(){
    PLCValueStringValidator validator;

    int errorPos;
    return (validator.validate(this->value, errorPos) == QValidator::Acceptable);
}



std::complex<double> PLCValueString::getComplex(){
    double real;
    double imag;

    if(isFile()){
        return 0;
    }

    QStringList values = this->value.split(' ');

    QString realString = values.at(0);
    real = realString.toDouble();

    QString imagString;

    if(values.length() != 1){
        imagString = values.at(1);
        imagString.append(values.at(2));
        imagString.chop(1);
        imag = imagString.toDouble();
    }
    else {
        imag = 0;
    }

    return std::complex<double>(real, imag);
}

double PLCValueString::getReal(){
    return getComplex().real();
}

double PLCValueString::getImaginary(){
    return getComplex().imag();
}


bool PLCValueString::isFile(){
    return (value.right(3) == "vdf");
}


PLCValueStringValidator::PLCValueStringValidator(QObject *parent)
    : QValidator(parent){}

QValidator::State PLCValueStringValidator::validate(QString &input, int &pos) const {
    Q_UNUSED(pos)
    if((input.right(3) == "vdf")){
        QFileInfo fileInfo(input);
        if(fileInfo.exists()){
            return QValidator::Acceptable;
        }

        return QValidator::Invalid;
    }

    ComplexStringValidator validator;

    int errorPos = 0;
    return validator.validate(input, errorPos);
}

void PLCValueStringValidator::fixup(QString &input) const {
    Q_UNUSED(input);
    return;
}


