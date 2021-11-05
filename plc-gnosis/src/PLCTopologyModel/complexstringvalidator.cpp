#include "complexstringvalidator.h"
#include <QRegExp>
#include <QDebug>

ComplexStringValidator::ComplexStringValidator(QObject *parent) :
    QValidator(parent)
{}

QValidator::State ComplexStringValidator::validate(QString& input, int & pos) const{
    Q_UNUSED(pos)


    if(input.length() == 0){
        return Intermediate;
    }

    input.replace(QRegExp("\\s+"), " ");
    if(QRegExp("[eE]").exactMatch(input)){
        return Invalid;
    }

    QRegExp  realExp(QString("([\\dEe\\.\\+\\-]+)"));
    QRegExp  plusExp(QString("\\s([\\+\\-])\\s"));
    QRegExp  imagExp(QString("(\\d[\\dEe\\.\\+\\-]*)j"));

    realExp.exactMatch(input);
    QString real = realExp.cap(1);


    if(realExp.matchedLength() == input.length()){
        return numValidator.validate(input, pos);
    }

    if(numValidator.validate(real, pos) != Acceptable || realExp.indexIn(input) != 0){
        return Invalid;
    }

    //We have a valid first number

    QString substr = input.right(input.length() - real.length());

    plusExp.exactMatch(substr);

    if((plusExp.matchedLength() < 3) && (plusExp.matchedLength() != substr.length())){
        return Invalid;
    }

    if(plusExp.matchedLength() == substr.length()){
        return Intermediate;
    }

    substr = substr.right(substr.length() - 3);

    imagExp.exactMatch(substr);
    QString imag = imagExp.cap(1);

    if(imagExp.matchedLength() != substr.length()){
        return Invalid;
    }

    if(imagExp.indexIn(substr) != 0){
        imag = substr;
    }

    if(numValidator.validate(imag, pos) == Acceptable && imagExp.indexIn(substr) == 0){
        return Acceptable;
    }

    return Intermediate;
}

void ComplexStringValidator::fixup(QString &input) const{
    Q_UNUSED(input)
}



