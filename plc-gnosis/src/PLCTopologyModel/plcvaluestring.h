#ifndef PLCValueString_H
#define PLCValueString_H

#include <QString>
#include <QValidator>
#include <QObject>
#include <vector>
#include <complex>

/**
 * @brief The PLCValueString class takes a QString representaiton, either a file of the appropriate format with a .vdf
 * file extension, or a string representing a complex number in rectangular form, i.e. 10 + 2j
 */
class PLCValueString
{
public:
    PLCValueString();
    PLCValueString(const QString& value);
    bool isValid();
    bool isFile();

    QString getValue();
    void setValue(const QString& value);

    double getReal();
    double getImaginary();
    std::complex<double> getComplex();
    //std::vector<std::vector<std::complex<double> > > getData();
    //QPair<int, int> contentDimensions();

private:
    QString value;
};


class PLCValueStringValidator : public QValidator{
    Q_OBJECT

public:
    explicit PLCValueStringValidator(QObject *parent = 0);

    QValidator::State validate(QString & input, int &pos) const;
    void fixup(QString & input) const;
};


#endif // PLCValueString_H
