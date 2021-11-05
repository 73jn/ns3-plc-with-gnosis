#include "simulator.h"

#include <QFile>
#include "../../lib/qjson/src/parser.h"

#include "ns3/plc.h"
#include "ns3/log.h"
#include "ns3/abort.h"

#include <ns3/simulator.h>

#include <QApplication>
#include <QSharedMemory>
#include <QTextStream>
#include <ns3/plc-net-device.h>

#include <QDebug>

#include "../../lib/QCustomPlot/qcustomplot.h"

void PLCSimulator::exportTransferData(QString fileName, Ptr<PLC_TransferBase> ctf){
    QFile file("./data/" + fileName + ".dat");
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);

    Ptr<PLC_TimeVariantTransferVector> timeVariantCtf;
    Ptr<PLC_TransferVector> freqVariantCtf;

    bool ctfIsTimeVariant = ctf->IsTimeVariant();

    unsigned int timeLoops = 1;
    unsigned int freqLoops = 1;

    if(ctfIsTimeVariant){
        timeVariantCtf = StaticCast<PLC_TimeVariantTransferVector, PLC_TransferBase>(ctf);
        timeLoops = timeVariantCtf->GetNumTimeSlots();
        freqLoops = timeVariantCtf->GetNumBands();

        //qDebug() << "Outter Dims: " << timeVariantCtf->GetValuesRef()->size();
        //qDebug() << " Inner Dims: " << timeVariantCtf->GetValuesRef()->at(0).size();
    }
    else {
        freqVariantCtf = StaticCast<PLC_TransferVector, PLC_TransferBase>(ctf);
        freqLoops = freqVariantCtf->GetNumBands();
        //qDebug() << "Freq Dims: " << freqVariantCtf->GetValuesRef()->size();
    }

    QTextStream stream(&file);
    stream.setRealNumberNotation(QTextStream::ScientificNotation);
    stream.setRealNumberPrecision(10);

    Ptr<const SpectrumModel> sm = ctf->GetSpectrumModel();
    stream << (*(sm->Begin())).fc << ',';
    stream << (*(sm->End() - 1)).fc << ',';
    stream << sm->GetNumBands() << '\n';

    stream << PLC_Time::GetPeriodS() << ',';
    stream << PLC_Time::GetNumTimeslots() << '\n';

    for(unsigned int i = 0; i < timeLoops; i++){
        PLC_ValueSpectrum* currentSpectrum = ctfIsTimeVariant ? &timeVariantCtf->GetValuesRef()->at(i):freqVariantCtf->GetValuesRef();

        for(unsigned int j = 0; j < freqLoops - 1; j++){
            stream << currentSpectrum->at(j).real() << ',';
        }
        stream << currentSpectrum->at(freqLoops - 1).real() << '\n';
    }

    for(unsigned int i = 0; i < timeLoops; i++){
        PLC_ValueSpectrum* currentSpectrum = ctfIsTimeVariant ? &timeVariantCtf->GetValuesRef()->at(i):freqVariantCtf->GetValuesRef();

        for(unsigned int j = 0; j < freqLoops - 1; j++){
            stream << currentSpectrum->at(j).imag() << ',';
        }

        stream << currentSpectrum->at(freqLoops - 1).imag() << '\n';
    }

    stream.flush();
    file.close();
}

QVector<QVector<double> > PLCSimulator::spectrumValueToPlottable(Ptr<SpectrumValue> val){

    QVector<QVector<double> > ret;
    QVector<double> xVals;
    QVector<double> yVals;

    Bands::const_iterator it = val->ConstBandsBegin();
    Values::const_iterator v_it = val->ConstValuesBegin();

    for(; it != val->ConstBandsEnd(); it++){
        xVals.push_back(it->fc/1.0e6);

        double mag = std::abs(*v_it);

        if(mag != 0){
            yVals.push_back(10.0*std::log10(mag));
        } else {
            xVals.pop_back();
        }

        v_it++;
    }

    ret.push_back(xVals);
    ret.push_back(yVals);

    return ret;
}

QVector<QVector<double> > PLCSimulator::ctfToPlottable(Ptr<PLC_TransferVector> ctf){
    QVector<double> xValues;
    QVector<double> abs;
    QVector<double> arg;

    Ptr<const SpectrumModel> sm = ctf->GetSpectrumModel();

    QVector<QVector<double> > ret;

    Bands::const_iterator it = sm->Begin();

    qDebug() << "Collecting frequency axis values";

    for(it = it; it != sm->End(); it++){
        xValues.push_back(it->fc/1.0e6);
    }

    qDebug() << "Added: " << xValues.size() << "X-Coordinates";

    PLC_ValueSpectrum vals = ctf->GetValues();

    qDebug() << "Num y vals in ValueSpectrum.... " << vals.size();
    PLC_ValueSpectrum::iterator v_it;

    qDebug() << "Collecting y-axis Values";
    for(unsigned int i = 0; i < vals.size(); i++){
        double mag = std::abs(vals.at(i));

        if(mag != 0){
            abs.push_back(20.0*std::log10(mag));
            arg.push_back(std::arg(vals.at(i)));
        } else {
            xValues.remove(i);
        }
    }

    ret.push_back(xValues);
    ret.push_back(abs);
    ret.push_back(arg);

    return ret;
}

PLCSimulator::PLCSimulator(): QObject(0){
    QSharedMemory *sharedMemory = new QSharedMemory("PLC_TOPOLOGY_JSON_DATA");

    if(!sharedMemory->attach()){
        canSimulate = false;
        return;
    };

    sharedMemory->lock();
    QByteArray jsonData((const char*)sharedMemory->constData(), sharedMemory->size());
    sharedMemory->unlock();
    sharedMemory->detach();
    delete(sharedMemory);

    QJson::Parser parser;

    QVariantMap modelData = parser.parse(jsonData).toMap();
    PLCTopologyModel* topologyModel = new PLCTopologyModel(modelData);
    this->loader = new PLCTopologyLoader(*topologyModel);

    canSimulate = loader->topologyIsValid();

    setupWidgets();
}

PLCSimulator::PLCSimulator(QString modelFileName): QObject(0){
    QFile modelFile(modelFileName);
    modelFile.open(QIODevice::ReadOnly);

    QByteArray jsonData = modelFile.readAll();
    QJson::Parser parser;

    QVariantMap modelData = parser.parse(jsonData).toMap();
    PLCTopologyModel* topologyModel = new PLCTopologyModel(modelData);
    this->loader = new PLCTopologyLoader(*topologyModel);

    canSimulate = loader->topologyIsValid();

    setupWidgets();
}

void PLCSimulator::setupWidgets(){
    bodeWindow = new BodeWidgetWindow();

    bodeWindow->setWindowTitle("Channel Transfer Functions");
    bodeWindow->resize(600, 400);

    psdGraphWidget = new GraphWidget();
    psdGraphWidget->setWindowTitle("Rx Power Spectral Density (dB)");
    psdGraphWidget->resize(600, 400);

    sinrGraphWidget = new GraphWidget();
    sinrGraphWidget->setWindowTitle("Rx SINR (dB)");
    sinrGraphWidget->resize(600, 400);
}

void PLCSimulator::showBodeWindow(){
    bodeWindow->getPlot()->replot();
    bodeWindow->show();
}

void PLCSimulator::showPsdPlotWindow(){
    psdGraphWidget->getPlot()->rescaleAxes();
    psdGraphWidget->getPlot()->replot();
    psdGraphWidget->show();
}

void PLCSimulator::showSinrPlotWindow(){
    sinrGraphWidget->getPlot()->rescaleAxes();
    sinrGraphWidget->getPlot()->replot();
    sinrGraphWidget->show();
}

int PLCSimulator::numberOfPlots(){
    return bodeWindow->getPlot()->getNumberPlots();
}

void PLCSimulator::collectTransferFunctions(QStringList enTxNodes, QStringList enRxNodes){

    delete(bodeWindow);
    bodeWindow = new BodeWidgetWindow();

    bodeWindow->setWindowTitle("Channel Transfer Functions");
    bodeWindow->resize(600, 400);

    transferFunctions.clear();

    PLC_NetdeviceList transmitterDevices;
    PLC_NetdeviceList receiverDevices;

    QStringList receiverNames = enRxNodes;
    QStringList transmitterNames = enTxNodes;

    //Pretty bad if the named devices don't exist......

    for(int i = 0; i < enTxNodes.length(); i++){
        transmitterDevices.push_back(loader->getTransmitterByName(enTxNodes.at(i)));
    }

    for(int i = 0; i < enRxNodes.length(); i++){
        receiverDevices.push_back(loader->getReceiverByName(enRxNodes.at(i)));
    }

    Ptr<PLC_HalfDuplexOfdmPhy> txPhy;
    Ptr<PLC_HalfDuplexOfdmPhy> rxPhy;

    Ptr<PLC_TxInterface> txIf;
    Ptr<PLC_RxInterface> rxIf;

    BodeWidget * const bodePlot = bodeWindow->getPlot();

    for(unsigned int i = 0; i < transmitterDevices.size(); i++){

        for(unsigned int j = 0; j< receiverDevices.size(); j++){
            if(transmitterDevices.at(i) != receiverDevices.at(j)){


                txPhy = transmitterDevices.at(i)->GetPhy()->GetObject<PLC_HalfDuplexOfdmPhy>();

                txIf = txPhy->GetTxInterface();

                rxPhy = receiverDevices.at(j)->GetPhy()->GetObject<PLC_HalfDuplexOfdmPhy>();
                rxIf = rxPhy->GetRxInterface();

                transmitterDevices.at(i)->GetChannelTransferImpl((receiverDevices.at(j)))->CalculateChannelTransferVector();

                PLC_ChannelTransferImpl *chImpl = txIf->GetChannelTransferImpl(PeekPointer(rxIf));
                NS_ABORT_MSG_IF(chImpl == NULL, "no channel from rxDev to txDev");

                Ptr<PLC_TransferBase> chTransFunc = chImpl->GetChannelTransferVector();

                transferFunctions[transmitterNames.at(i)][receiverNames.at(j)] = chTransFunc;

                QString name;
                name += transmitterNames.at(i) + "-to-" + receiverNames.at(j);

                NS_ASSERT(chTransFunc->GetValueType() == PLC_ValueBase::FREQ_SELECTIVE);

                if(chTransFunc->IsTimeVariant()){
                    qDebug() << "Cannot plot time variant functions. Exporting to" << (name + ".dat");
                    exportTransferData(name, chTransFunc);
                }
                else{
                    //Plot the channel transfer functionnew BodeWidget();
                    Ptr<PLC_TransferVector> data = StaticCast<PLC_TransferVector, PLC_TransferBase> (chTransFunc);
                    BodeData plotData = ctfToPlottable(data);

                    qDebug() << "Adding: " << name << "to Bode Widget";
                    bodePlot->addBodePlot(&plotData, name);

                    qDebug() << "Exporting ctf to" << (name + ".dat");
                    exportTransferData(name, chTransFunc);
                }
            }
        }
    }

    qDebug() << "Diagram has" << transmitterDevices.size() << "Tx device(s) and" << receiverDevices.size() << "Rx device(s)";
    qDebug() << "Finished collecting all channel transfer functions!";

    this->showBodeWindow();
}

void PLCSimulator::collectTransferFunctions(){

    transferFunctions.clear();

    PLC_NetdeviceList receiverDevices = loader->getReceivers();
    QStringList receiverNames = loader->getReceiverNames();

    PLC_NetdeviceList transmitterDevices = loader->getTransmitters();
    QStringList transmitterNames = loader->getTransmitterNames();

    Ptr<PLC_HalfDuplexOfdmPhy> txPhy;
    Ptr<PLC_HalfDuplexOfdmPhy> rxPhy;

    Ptr<PLC_TxInterface> txIf;
    Ptr<PLC_RxInterface> rxIf;

    BodeWidget * const bodePlot = bodeWindow->getPlot();

    for(unsigned int i = 0; i < transmitterDevices.size(); i++){

        for(unsigned int j = 0; j< receiverDevices.size(); j++){
            if(transmitterDevices.at(i) != receiverDevices.at(j)){

                txPhy = transmitterDevices.at(i)->GetPhy()->GetObject<PLC_HalfDuplexOfdmPhy>();
                txIf = txPhy->GetTxInterface();

                rxPhy = receiverDevices.at(j)->GetPhy()->GetObject<PLC_HalfDuplexOfdmPhy>();
                rxIf = rxPhy->GetRxInterface();

                transmitterDevices.at(i)->GetChannelTransferImpl((receiverDevices.at(j)))->CalculateChannelTransferVector();

                PLC_ChannelTransferImpl *chImpl = txIf->GetChannelTransferImpl(PeekPointer(rxIf));
                NS_ABORT_MSG_IF(chImpl == NULL, "no channel from rxDev to txDev");

                Ptr<PLC_TransferBase> chTransFunc = chImpl->GetChannelTransferVector();

                transferFunctions[transmitterNames.at(i)][receiverNames.at(j)] = chTransFunc;

                QString name;
                name += transmitterNames.at(i) + "-to-" + receiverNames.at(j);

                NS_ASSERT(chTransFunc->GetValueType() == PLC_ValueBase::FREQ_SELECTIVE);

                if(chTransFunc->IsTimeVariant()){
                    qDebug() << "Cannot plot time variant functions. Exporting to" << (name + ".dat");
                    exportTransferData(name, chTransFunc);
                }
                else{
                    //Plot the channel transfer functionnew BodeWidget();
                    Ptr<PLC_TransferVector> data = StaticCast<PLC_TransferVector, PLC_TransferBase> (chTransFunc);
                    BodeData plotData = ctfToPlottable(data);

                    qDebug() << "Adding: " << name << "to Bode Widget";
                    bodePlot->addBodePlot(&plotData, name);

                    qDebug() << "Exporting ctf to" << (name + ".dat");
                    exportTransferData(name, chTransFunc);
                }
            }
        }
    }

    qDebug() << "Diagram has" << transmitterDevices.size() << "Tx device(s) and" << receiverDevices.size() << "Rx device(s)";
    qDebug() << "Finished collecting all channel transfer functions!";
}

void PLCSimulator::simulateSINRAtReceiver(bool doPSD, bool doSINR, QString txName, QString rxName){

    //LogComponentEnable("PLC_Phy", LOG_LEVEL_FUNCTION);
    //LogComponentEnable("PLC_Mac", LOG_LEVEL_FUNCTION);

    delete(psdGraphWidget);
    delete(sinrGraphWidget);

    psdGraphWidget = new GraphWidget();
    psdGraphWidget->setWindowTitle("Rx Power Spectral Density (dB)");
    psdGraphWidget->resize(600, 400);

    sinrGraphWidget = new GraphWidget();
    sinrGraphWidget->setWindowTitle("Rx SINR (dB)");
    sinrGraphWidget->resize(600, 400);

    Ptr<PLC_NetDevice> txDev = loader->getTransmitterByName(txName);
    Ptr<PLC_NetDevice> rxDev = loader->getReceiverByName(rxName);

    Ptr<PLC_HalfDuplexOfdmPhy> txPhy = txDev->GetPhy()->GetObject<PLC_HalfDuplexOfdmPhy>();
    Ptr<PLC_HalfDuplexOfdmPhy> rxPhy = rxDev->GetPhy()->GetObject<PLC_HalfDuplexOfdmPhy>();

    Ptr<PLC_TxInterface> txIf = txPhy->GetTxInterface();
    Ptr<PLC_RxInterface> rxIf = rxPhy->GetRxInterface();

    //Ptr<Packet> p = Create<Packet> (10000);
    //ns3::Simulator::Schedule(Seconds(1), &PLC_NetDevice::Send, txDev, p, rxDev->GetAddress(), 0);
    //ns3::Simulator::Stop(Seconds(30));
    //ns3::Simulator::Run();

    // The receive power spectral density computation is done by the channel
    // transfer implementation from TX interface to RX interface
    Ptr<PLC_ChannelTransferImpl> chImpl = txIf->GetChannelTransferImpl(PeekPointer(rxIf));
    assert(chImpl);

    Ptr<SpectrumValue> rxPSD = chImpl->CalculateRxPowerSpectralDensity(txPhy->GetTxPowerSpectralDensity());

    // SINR is calculated by PLC_Interference (member of PLC_Phy)
    PLC_Interference interference;
    Ptr<SpectrumValue> noiseFloor = CreateWorstCaseBgNoise(txDev->GetSpectrumModel())->GetNoisePsd();
    interference.SetNoiseFloor(noiseFloor);
    interference.StartRx(rxPSD);

    //ns3::Simulator::Destroy();

    //Ptr<SpectrumValue> sinr = interference.GetSINR();

    psdGraphWidget->addPlot(PLCSimulator::spectrumValueToPlottable(rxPSD), rxName + QString(" rxPSD"));
    //sinrGraphWidget->addPlot(PLCSimulator::spectrumValueToPlottable(sinr), txName + QString(" SINR"));

    if(doPSD){
        showPsdPlotWindow();
    }

    if(doSINR){
        showSinrPlotWindow();
    }
}


void PLCSimulator::psdTest(){
    // Define spectrum model
    PLC_SpectrumModelHelper smHelper;
    Ptr<const SpectrumModel> sm;
    sm = smHelper.GetSpectrumModel(0, 10e6, 100);

    // Create cable types
    Ptr<PLC_Cable> cable = CreateObject<PLC_NAYY150SE_Cable> (sm);

    // Create nodes
    Ptr<PLC_Node> n1 = CreateObject<PLC_Node> ();
    Ptr<PLC_Node> n2 = CreateObject<PLC_Node> ();
    n1->SetPosition(0,0,0);
    n2->SetPosition(1000,0,0);

    PLC_NodeList nodes;
    nodes.push_back(n1);
    nodes.push_back(n2);

    // Link nodes
    CreateObject<PLC_Line> (cable, n1, n2);

    // Set up channel
    PLC_ChannelHelper channelHelper(sm);
    channelHelper.Install(nodes);
    Ptr<PLC_Channel> channel = channelHelper.GetChannel();

    // Create interfaces (usually done by the device helper)
    Ptr<PLC_TxInterface> txIf = CreateObject<PLC_TxInterface> (n1, sm);
    Ptr<PLC_RxInterface> rxIf = CreateObject<PLC_RxInterface> (n2, sm);

    // Add interfaces to the channel (usually done by the device helper)
    channel->AddTxInterface(txIf);
    channel->AddRxInterface(rxIf);

    // Since we do not run an ns-3 simulation, the channel computation has to be triggered manually
    channel->InitTransmissionChannels();
    channel->CalcTransmissionChannels();

    // Define transmit power spectral density
    Ptr<SpectrumValue> txPsd = Create<SpectrumValue> (sm);
    (*txPsd) = 1e-8; // -50dBm/Hz

    // The receive power spectral density computation is done by the channel
    // transfer implementation from TX interface to RX interface
    Ptr<PLC_ChannelTransferImpl> chImpl = txIf->GetChannelTransferImpl(PeekPointer(rxIf));
    NS_ASSERT(chImpl);
    Ptr<SpectrumValue> rxPsd = chImpl->CalculateRxPowerSpectralDensity(txPsd);

    // SINR is calculated by PLC_Interference (member of PLC_Phy)
    PLC_Interference interference;
    Ptr<SpectrumValue> noiseFloor = CreateWorstCaseBgNoise(sm)->GetNoisePsd();
    interference.SetNoiseFloor(noiseFloor);
    interference.StartRx(rxPsd);

    //Ptr<SpectrumValue> sinr = interference.GetSINR();

    //graphWidget->addPlot(PLCSimulator::spectrumValueToPlottable(rxPsd), QString(" rxPSD"));

}
