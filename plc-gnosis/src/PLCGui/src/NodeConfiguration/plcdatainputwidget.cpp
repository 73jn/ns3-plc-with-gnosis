#include "plcdatainputwidget.h"
#include <QHBoxLayout>


#include <QFileDialog>
#include <QGroupBox>

#include "plcvaluestring.h"
#include "complexstringvalidator.h"

PLCDataInputWidget::PLCDataInputWidget(const QString &paramName,
                                       bool checkable, QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    checkBox = new QCheckBox(this);


    setCheckable(checkable);
    checkBox->setChecked(checkable);

    parameterLabel = new QLabel(paramName + ":");
    parameterLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    //parameterLabel->setFixedWidth(100);


    lineEdit = new QLineEdit(this);
    selectFileButton = new QPushButton("...", this);


    selectFileButton->setMaximumWidth(20);
    selectFileButton->setMaximumHeight(selectFileButton->height());
    lineEdit->setMinimumWidth(120);
    layout->addWidget(checkBox);
    layout->addWidget(parameterLabel, 0);
    layout->addWidget(lineEdit, 0);
    layout->addWidget(selectFileButton, 0);

    connect(selectFileButton, SIGNAL(clicked()), this, SLOT(getFile()));
    connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
    lineEdit->setValidator(new PLCValueStringValidator(this));

    this->setLayout(layout);
}

QString PLCDataInputWidget::getValue(){
    return this->lineEdit->text();
}

void PLCDataInputWidget::setValue(const QString& value){
    this->lineEdit->setText(value);
}

bool PLCDataInputWidget::isValid(){
    int i;

    if((!checked) && checkable){
        return true;
    }

    QString value = lineEdit->text();
    return lineEdit->validator()->validate(value, i) == QValidator::Acceptable;
}

void PLCDataInputWidget::getFile(){
    QString file = QFileDialog::getOpenFileName(this, "Select Spectrum Data File", "./data/", "Data Files (*.vdf)", 0);

    if(!file.isEmpty()){
        file = QDir().relativeFilePath(file);
        this->lineEdit->setText(file);
    }
}

void PLCDataInputWidget::setCheckable(bool checkable){
    this->checkable = checkable;
    checkBox->setVisible(checkable);
}

void PLCDataInputWidget::setChecked(bool checked){
    this->checked = checked;
    this->checkBox->setChecked(checked);

    if(checked){
        lineEdit->setEnabled(true);
        selectFileButton->setEnabled(true);
    }
    else {
        lineEdit->setDisabled(true);
        selectFileButton->setDisabled(true);
    }

}
