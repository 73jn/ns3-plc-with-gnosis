#include "nodeconfiguration.h"
#include <QHBoxLayout>

#include <QListWidget>
#include <QPushButton>

#include "plcvaluestring.h"
#include "netdevicemodel.h"
#include "noisesourcemodel.h"

#include <QDebug>
#include <QCloseEvent>

NodeConfiguration::NodeConfiguration(NodeModel *node, QDialog *parent) :
    QDialog(parent)
{
    nodeModel = node;

    this->setWindowTitle(node->getName());
    //Setup the GUI Components/GroupBoxes etc.
    outletSettings = new QGroupBox("Outlet");
    outletSettings->setCheckable(true);
    outletSettings->setChecked(nodeModel->getHasOutlet());

    QVBoxLayout* outletSettingsLayout = new QVBoxLayout();
    outletImpedanceInput = new PLCDataInputWidget("Impedance");
    outletSettingsLayout->addWidget(outletImpedanceInput);
    outletSettings->setLayout(outletSettingsLayout);



    netDeviceSettings = new QGroupBox("Net Device");
    netDeviceSettings->setCheckable(true);


    if(nodeModel->getNetDevice() == 0){
        netDeviceSettings->setChecked(false);
        nodeModel->setNetDevice(new NetDeviceModel());
    }
    else{
        netDeviceSettings->setChecked(true);
    }

    QVBoxLayout* netDevConfigLayout = new QVBoxLayout();
    netDeviceEditor = new NetDeviceEditor(node->getNetDevice(), this);

    netDevConfigLayout->addWidget(netDeviceEditor);
    netDeviceSettings->setLayout(netDevConfigLayout);

    noiseSourceSettings = new QGroupBox("Noise Source");
    noiseSourceSettings->setCheckable(true);

    //Noise sources disabled until we have a better idea of how they will be handled.
    noiseSourceSettings->setDisabled(true);

    if(nodeModel->getNoiseSource() == 0){
        noiseSourceSettings->setChecked(false);
        nodeModel->setNoiseSource(new NoiseSourceModel());
    }
    else
    {
        //If an old diagram file has a noise source specified, disable it
        noiseSourceSettings->setChecked(false);
        //noiseSourceSettings->setChecked(true);
    }

    QVBoxLayout* noiseSrcConfigLayout = new QVBoxLayout();
    noiseSourceEditor = new NoiseSourceEditor(node->getNoiseSource(), this);

    noiseSrcConfigLayout->addWidget(noiseSourceEditor);
    noiseSourceSettings->setLayout(noiseSrcConfigLayout);

    QVBoxLayout * groupBoxesLayout = new QVBoxLayout();
    groupBoxesLayout->addWidget(outletSettings);
    groupBoxesLayout->addWidget(netDeviceSettings);
    groupBoxesLayout->addWidget(noiseSourceSettings);

    QHBoxLayout * buttonsLayout = new QHBoxLayout();
    QPushButton * closeButton = new QPushButton("Close");
    buttonsLayout->addWidget(closeButton);
    groupBoxesLayout->addLayout(buttonsLayout);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));

    this->setLayout(groupBoxesLayout);

    populateFromModel();

}

void NodeConfiguration::populateFromModel(){

    outletSettings->setChecked(nodeModel->getHasOutlet());
    outletImpedanceInput->setValue(nodeModel->getOutletImpedance().getValue());
}

void NodeConfiguration::setModel(NodeModel *newModel){
    nodeModel = newModel;
    populateFromModel();
}



void NodeConfiguration::closeEvent(QCloseEvent * event){
    event->ignore();
    return;
}

void NodeConfiguration::saveAndClose(){
    if(outletSettings->isChecked()){
        if(outletImpedanceInput->isValid()){
            this->nodeModel->setHasOutlet(true);
            this->nodeModel->setOutletImpedance(outletImpedanceInput->getValue());
        }
        else{
            return;
        }
    }
    else{
        this->nodeModel->setHasOutlet(false);
    }

    if(noiseSourceSettings->isChecked()){
        if(noiseSourceEditor->isValid()){
            noiseSourceEditor->saveChanges();
        }
        else{
            return;
        }
    }
    else{
        nodeModel->setNoiseSource(0);
    }

    if(netDeviceSettings->isChecked()){
        if(netDeviceEditor->isValid()){
            netDeviceEditor->saveChanges();
        }
        else{
            return;
        }
    }
    else{
        nodeModel->setNetDevice(0);
    }

    done(0);
    return;
}
