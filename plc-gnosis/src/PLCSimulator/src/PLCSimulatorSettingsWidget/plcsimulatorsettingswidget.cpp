#include "plcsimulatorsettingswidget.h"

PLCSimulatorSettingsWidget::PLCSimulatorSettingsWidget(QString ns3Dir, QStringList rxNodes, QStringList txNodes, QWidget *parent) :
    QWidget(parent)
{

    this->rxNodes = rxNodes;
    this->txNodes = txNodes;

    QVBoxLayout* mainLayout = new QVBoxLayout();

    ctfSettings = new QGroupBox("Channel Transfer Functions", this);
    noiseSimSettings = new QGroupBox("Noise and PSD", this);

    //disabledRxNodes = new QListWidget(this);
    enabledRxNodes = new QListWidget(this);
    enabledRxNodes->setSelectionMode(QAbstractItemView::MultiSelection);

    //disabledTxNodes = new QListWidget(this);
    enabledTxNodes = new QListWidget(this);
    enabledTxNodes->setSelectionMode(QAbstractItemView::MultiSelection);

    enabledRxNodes->addItems(rxNodes);
    enabledTxNodes->addItems(txNodes);

    QVBoxLayout* ctfSettingsLayout = new QVBoxLayout();

    QHBoxLayout* listLabelsLayout = new QHBoxLayout();
    listLabelsLayout->addWidget(new QLabel("Rx Nodes"));
    listLabelsLayout->addWidget(new QLabel("Tx Nodes"));

    QHBoxLayout* listWidgetLayout = new QHBoxLayout();
    listWidgetLayout->addWidget(enabledRxNodes);
    listWidgetLayout->addWidget(enabledTxNodes);


    doCollectTransferFunctions = new QPushButton("Initialize", this);

    ctfSettingsLayout->addLayout(listLabelsLayout);
    ctfSettingsLayout->addLayout(listWidgetLayout);
    ctfSettingsLayout->addWidget(doCollectTransferFunctions);

    ctfSettings->setLayout(ctfSettingsLayout);


    //ns3DirectoryEdit = new QPlainTextEdit(ns3Dir, this);

    doCalculateRxPSDSINR = new QPushButton("Initialize", this);

    txDropDown = new QComboBox(this);
    rxDropDown = new QComboBox(this);

    QVBoxLayout* noiseSimSettingsLayout = new QVBoxLayout();
    QHBoxLayout* noisePsdCheckLayout = new QHBoxLayout();
    QHBoxLayout* noisePsdDropDownLayout = new QHBoxLayout();

    txDropDown->addItems(txNodes);
    rxDropDown->addItems(rxNodes);

    QLabel* txDDLabel = new QLabel("Tx Node:", this);
    QLabel* rxDDLabel = new QLabel("Rx Node:", this);

    noisePsdDropDownLayout->addWidget(txDDLabel);
    noisePsdDropDownLayout->addWidget(txDropDown);
    noisePsdDropDownLayout->addWidget(rxDDLabel);
    noisePsdDropDownLayout->addWidget(rxDropDown);

    doSINRCalculation = new QCheckBox("Calculate SINR", this);
    doPSDCalculation = new QCheckBox("Calculate PSD", this);

    noisePsdCheckLayout->addWidget(doPSDCalculation);
    noisePsdCheckLayout->addWidget(doSINRCalculation);

    noiseSimSettingsLayout->addLayout(noisePsdDropDownLayout);
    noiseSimSettingsLayout->addLayout(noisePsdCheckLayout);
    noiseSimSettingsLayout->addWidget(doCalculateRxPSDSINR);

    noiseSimSettings->setLayout(noiseSimSettingsLayout);

    mainLayout->addWidget(ctfSettings);
    mainLayout->addWidget(noiseSimSettings);


    this->setLayout(mainLayout);

    connect(doCollectTransferFunctions, SIGNAL(clicked()), this, SLOT(handleDoCollectTransferFunctions()));
    connect(doCalculateRxPSDSINR, SIGNAL(clicked()), this, SLOT(handleDoCalculateRxPSDSINR()));
}


void PLCSimulatorSettingsWidget::handleDoCollectTransferFunctions(){
    QStringList enRxNodes;
    QStringList enTxNodes;

    if(enabledTxNodes->selectedItems().isEmpty()){
        enTxNodes = txNodes;
    }
    else{
        QList<QListWidgetItem* > items = enabledTxNodes->selectedItems();

        foreach(QListWidgetItem* item, items){
            enTxNodes.push_back(item->text());
        }
    }

    if(enabledRxNodes->selectedItems().isEmpty()){
        enRxNodes = rxNodes;
    }
    else{
        QList<QListWidgetItem* > items = enabledRxNodes->selectedItems();

        foreach(QListWidgetItem* item, items){
            enRxNodes.push_back(item->text());
        }
    }

    emit collectGivenTransferFunctions(enTxNodes, enRxNodes);
}

void PLCSimulatorSettingsWidget::handleDoCalculateRxPSDSINR(){
    bool doPSD = doPSDCalculation->isChecked();
    bool doSINR = doSINRCalculation->isChecked();

    emit doNoiseSim(doPSD, doSINR, txDropDown->currentText(), rxDropDown->currentText());
}

