#ifndef COMPLEXVALIDATOR_H
#define COMPLEXVALIDATOR_H

#include <QValidator>
#include <QString>

class ComplexStringValidator : public QValidator
{
    Q_OBJECT
public:
    explicit ComplexStringValidator(QObject *parent = 0);
    QValidator::State validate(QString & input, int &pos) const;
    void fixup(QString & input) const;

private:
    QString realString;
    QString imagString;

    QDoubleValidator numValidator; 
};

#endif // COMPLEXVALIDATOR_H
