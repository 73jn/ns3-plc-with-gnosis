#include "edgeconfiguration.h"
#include <QBoxLayout>
#include <QCloseEvent>
#include <QDoubleValidator>
#include <QDebug>
#include <QDir>

EdgeConfiguration::EdgeConfiguration(EdgeModel* edge, QDialog *parent) :
    QDialog(parent)
{
    this->edgeModel = edge;
    cableModels = new QComboBox(this);
    lengthInput = new QLineEdit(this);
    nameInput = new QLineEdit(this);

    lengthInput->setValidator(new QDoubleValidator(this));


    QVBoxLayout * twoPortSettingsLayout = new QVBoxLayout();
    QVBoxLayout * cableSettingsLayout = new QVBoxLayout();
    QVBoxLayout * edgeSettingsLayout = new QVBoxLayout();

    QLabel* lengthLabel = new QLabel("Length:", this);
    lengthLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel* nameLabel = new QLabel("Name:", this);
    nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);


    QHBoxLayout * lengthLayout = new QHBoxLayout();
    lengthLayout->addWidget(lengthLabel);
    lengthLayout->addWidget(lengthInput);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameInput);

    cableSettingsLayout->addWidget(cableModels);
    cableSettingsLayout->addLayout(lengthLayout);

    for(int i = 0; i < 4; i++){
        this->parametersInput[i] = new PLCDataInputWidget(QString('A' + i), false, this);
        twoPortSettingsLayout->addWidget(parametersInput[i]);
    }

    QFile cableTypes("./cables/typelist.lst");
    cableTypes.open(QIODevice::ReadOnly);

    QString typesData(cableTypes.readAll());
    typesData.remove('\r');
    QStringList cablesList = typesData.split('\n', QString::SkipEmptyParts);


    for(int i = 0; i < cablesList.length(); i++){
        cableModels->addItem(cablesList.at(i));
    }


    acceptButton = new QPushButton("Accept");

    this->twoPortSettings = new QGroupBox("Two-Port Settings", this);
    this->cableSettings = new QGroupBox("Cable Settings", this);

    twoPortSettings->setCheckable(true);
    twoPortSettings->setChecked(false);
    cableSettings->setCheckable(true);


    twoPortSettings->setLayout(twoPortSettingsLayout);
    cableSettings->setLayout(cableSettingsLayout);

    edgeSettingsLayout->addLayout(nameLayout);
    edgeSettingsLayout->addWidget(cableSettings);
    edgeSettingsLayout->addWidget(twoPortSettings);
    edgeSettingsLayout->addWidget(acceptButton);

    this->setLayout(edgeSettingsLayout);

    connect(cableSettings, SIGNAL(toggled(bool)), this, SLOT(cableSettingsToggled(bool)));
    connect(twoPortSettings, SIGNAL(toggled(bool)), this, SLOT(twoPortSettingsToggled(bool)));
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));

    populateFromModel();
}

void EdgeConfiguration::cableSettingsToggled(bool checked){
    if(checked){
        twoPortSettings->setChecked(false);
    }

    if((!twoPortSettings->isChecked()) && (!cableSettings->isChecked())){
        cableSettings->setChecked(true);
    }
}

void EdgeConfiguration::twoPortSettingsToggled(bool checked){
    if(checked){
        cableSettings->setChecked(false);
    }

    if((!twoPortSettings->isChecked()) && (!cableSettings->isChecked())){
        cableSettings->setChecked(true);
    }
}

void EdgeConfiguration::populateFromModel(){
    if(edgeModel->isCableModel()){
        cableSettings->setChecked(true);
        twoPortSettings->setChecked(false);
    }
    else{
        cableSettings->setChecked(false);
        twoPortSettings->setChecked(true);
    }

    PLCValueString* params = edgeModel->getTwoPortParameters();
    for(int i = 0; i < 4; i++){
        parametersInput[i]->setValue(params[i].getValue());
    }

    cableModels->setCurrentIndex(cableModels->findText(edgeModel->getCableType()));
    lengthInput->setText(QString::number(edgeModel->getLength()));
    nameInput->setText(edgeModel->getName());
}

void EdgeConfiguration::saveAndClose(){

    edgeModel->setName(nameInput->text());

    if(cableSettings->isChecked()){
        QString length = lengthInput->text();
        int start = 0;
        if(lengthInput->validator()->validate(length, start) != QValidator::Acceptable){
            qDebug() << "Bad Length Input";
            return;
        }
        edgeModel->setIsCableModel(true);
        edgeModel->setCableType(cableModels->currentText());
        edgeModel->setLength(lengthInput->text().toDouble());
    }

    if(twoPortSettings->isChecked()){
        edgeModel->setIsCableModel(false);
        PLCValueString* params = edgeModel->getTwoPortParameters();

        for(int i = 0; i < 4; i++){
            if(!parametersInput[i]->isValid()){
                return;
            }
            params[i] = parametersInput[i]->getValue();
        }
    }

    done(0);
    return;
}

void EdgeConfiguration::closeEvent(QCloseEvent *event){
    event->ignore();
}
