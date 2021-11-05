#include "noisesourceeditor.h"
#include <QBoxLayout>
#include <assert.h>

NoiseSourceEditor::NoiseSourceEditor(NoiseSourceModel* source, QWidget *parent) :
    QWidget(parent)
{


    noiseSrcModel = source;
    assert(noiseSrcModel != 0);

    QLabel * nameLabel = new QLabel("Name:", this);
    nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    nameEdit = new QLineEdit(this);
    QHBoxLayout * nameLayout = new QHBoxLayout();

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);

    noiseTypeSelector = new QComboBox(this);
    noiseTypeSelector->addItem("Static");
    noiseTypeSelector->addItem("Impulsive");
    noiseTypeSelector->setFixedWidth(100);


    noisePSDInput = new PLCDataInputWidget("Noise PSD", this);
    noisePSDInput->setCheckable(false);

    QVBoxLayout * editorLayout = new QVBoxLayout();
    editorLayout->addLayout(nameLayout);
    editorLayout->addWidget(noisePSDInput);
    editorLayout->addWidget(noiseTypeSelector, 0, Qt::AlignCenter);


    this->setLayout(editorLayout);

    //deleteButton->setFixedWidth(20);

    populateFromModel();


}

void NoiseSourceEditor::populateFromModel(){
    this->nameEdit->setText(noiseSrcModel->getName());
    this->noiseTypeSelector->setCurrentIndex(noiseTypeSelector->findText(noiseSrcModel->getNoiseType()));
    this->noisePSDInput->setValue(noiseSrcModel->getNoisePSD().getValue());
}

bool NoiseSourceEditor::isValid(){
    return (nameEdit->text().length() != 0 && noisePSDInput->isValid());
}

void NoiseSourceEditor::saveChanges(){
    if(isValid()){
        this->noiseSrcModel->setName(nameEdit->text());
        this->noiseSrcModel->setNoisePSD(noisePSDInput->getValue());
        this->noiseSrcModel->setNoiseType(noiseTypeSelector->currentText());
    }
}
