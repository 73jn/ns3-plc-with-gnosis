#include "plctopologyloader.h"
#include <QFile>

#include <QtCore/QVariant>

#include "../../lib/qjson/src/parser.h"

#include "ns3/plc-spectrum-helper.h"
#include "ns3/object-factory.h"
#include "ns3/plc-device-helper.h"
#include "ns3/simulator.h"

void
PLCTopologyLoader::incrementMacAddress(Mac48Address *addr)
{
    uint8_t buf[6];
    addr->CopyTo(buf);

    uint64_t address = 	(((uint64_t) buf[0] << 40) & 0xff0000000000) |
                        (((uint64_t) buf[1] << 32) & 0x00ff00000000) |
                        (((uint64_t) buf[2] << 24) & 0x0000ff000000) |
                        (((uint64_t) buf[3] << 16) & 0x000000ff0000) |
                        (((uint64_t) buf[4] << 8)  & 0x00000000ff00) |
                        (((uint64_t) buf[5] << 0)  & 0x0000000000ff);

    ++address;

    buf[0] = (address >> 40) & 0xff;
    buf[1] = (address >> 32) & 0xff;
    buf[2] = (address >> 24) & 0xff;
    buf[3] = (address >> 16) & 0xff;
    buf[4] = (address >> 8) & 0xff;
    buf[5] = (address >> 0) & 0xff;

    addr->CopyFrom(buf);
}

PLCTopologyLoader::PLCTopologyLoader(PLCTopologyModel topologyModel){

    validInputValues = true;

    PLC_SpectrumModelHelper smHelper;

    double numBands = (topologyModel.getSpectrumModel().getUpperBandLimit() - topologyModel.getSpectrumModel().getLowerBandLimit());
    numBands /= topologyModel.getSpectrumModel().getBandResolution();

    Ptr<const SpectrumModel> sm = smHelper.GetSpectrumModel(topologyModel.getSpectrumModel().getLowerBandLimit(),
                                                      topologyModel.getSpectrumModel().getUpperBandLimit(),
                                                      numBands);

    PLC_Time::SetTimeModel(topologyModel.getSpectrumModel().getMainsFrequency(),
                           topologyModel.getSpectrumModel().getSamplesPerCycle(),
                           MicroSeconds(topologyModel.getSpectrumModel().getSymbolLength()));

    plcGraph = Create<PLC_Graph>();

    // Create channel
    channel = CreateObject<PLC_Channel> ();
    channel->SetGraph(plcGraph);

    QList<NodeModel*>* topologyNodes = topologyModel.getNodes();
    QList<EdgeModel*>* topologyEdges = topologyModel.getEdges();

    Mac48Address addr("00:00:00:00:00:00");

    //Loop through the nodes, and create as necessary.
    for(int i = 0; i < topologyNodes->length(); i++){
        NodeModel* currentNode = topologyNodes->at(i);
        Ptr<PLC_Node> newNode = fromNodeModel(currentNode, sm, validInputValues);

        //Add the node to our tracking variables
        plcGraph->AddNode(newNode);
        nodesByName[currentNode->getName()] = newNode;

        //Loop through nodes net devices and create as necessary
        if(currentNode->getNetDevice() != 0)
        {
            Ptr<PLC_NetDevice> newNetDevice = fromNetDeviceModel(currentNode->getNetDevice(), sm, newNode, addr, validInputValues);
            PLCTopologyLoader::incrementMacAddress(&addr);

            QString netDeviceName = currentNode->getNetDevice()->getName();

            this->netDevices.push_back(newNetDevice);
            this->netDeviceNames.push_back(netDeviceName);

            if(currentNode->getNetDevice()->receiverEnabled()){
                this->receiverNameToIndexMap[netDeviceName] = receivers.size();
                this->receivers.push_back(newNetDevice);
                this->receiverNames.push_back(netDeviceName);
            }

            if(currentNode->getNetDevice()->transmitterEnabled()){
                this->transmitterNameToIndexMap[netDeviceName] = transmitters.size();
                this->transmitters.push_back(newNetDevice);
                this->transmitterNames.push_back(netDeviceName);
            }
        }

        if(this->netDevices.size() > 0){
            this->netDeviceNodes.push_back(newNode);
        }

        //Loop Through Node noise sources and create as necessary.
        if(currentNode->getNoiseSource() != 0){
            Ptr<PLC_NoiseSource> newNoiseSource = fromNoiseSourceModel(currentNode->getNoiseSource(), sm, newNode, validInputValues);
            this->noiseSourceNodes.push_back(newNode);
            this->noiseSources.push_back(newNoiseSource);
        }
    }

    ObjectFactory edgeFactory;
    //Loop through edges, create lines/two-ports and link to appropriate nodes
    for(int i = 0; i < topologyEdges->length(); i++){

        EdgeModel* currentEdge = topologyEdges->at(i);
        Ptr<PLC_Edge> newEdge;


        Ptr<PLC_Node> fromNode = nodesByName[currentEdge->getFromNode()];
        Ptr<PLC_Node> toNode = nodesByName[currentEdge->getToNode()];

        //Fake the node positions to avoid the user having to accurately depict the topology... (Hopefully this works!)
        toNode->SetPosition( fromNode->GetPosition().x + currentEdge->getLength(), 0, 0);
\
        //qDebug() << "Set Pos: " << fromNode->GetPosition().x + currentEdge->getLength();
        if(currentEdge->isCableModel()){
            edgeFactory.SetTypeId(currentEdge->getCableType().toStdString());
            Ptr<PLC_Cable> cable = edgeFactory.Create()->GetObject<PLC_Cable>();
            cable->SetSpectrumModel(sm);
            cable->Calculate();

            //fromCableFile(currentEdge->getCableType(), sm);
            newEdge = Create<PLC_Line>(cable, fromNode, toNode);
        }
        else{
            newEdge = Create<PLC_TwoPort>(sm, toNode, fromNode,
                                          fromValueString(currentEdge->getTwoPortParameters()[0], sm, validInputValues),
                                          fromValueString(currentEdge->getTwoPortParameters()[1], sm, validInputValues),
                                          fromValueString(currentEdge->getTwoPortParameters()[2], sm, validInputValues),
                                          fromValueString(currentEdge->getTwoPortParameters()[3], sm, validInputValues));
        }

        //Reset the 'to' nodes original position.
        edges.push_back(newEdge);
        //toNode->SetPosition(0, 0, 0);


    }

    channel->InitTransmissionChannels();
    channel->CalcTransmissionChannels();

    //qDebug() << plcGraph->GetDistance(transmitters.at(0)->GetPlcNode(), receivers.at(0)->GetPlcNode());

    //And.... that should be everything...
}

Ptr<PLC_ValueBase> PLCTopologyLoader::infinity(Ptr<const SpectrumModel> spectrumModel){
    std::complex<double> infinity(std::numeric_limits<double>::infinity());
    return Create<PLC_ConstValue>(spectrumModel, infinity);
}

//TODO: Require as PLCValueString conversion to a SpectrumValue

//This function loads a csv file containing data (constant, time varying, frequency varying, or both) from a data file.
//The data file is just a CSV dump of two matrices, [A;B] where A is the REAL part of the data, and B is the IMAGINARY part
//of the data
Ptr<PLC_ValueBase> PLCTopologyLoader::valueFromFile(const QString& file, Ptr<const SpectrumModel> spectrumModel, bool& ok){
    QFile dataFile(file);

    dataFile.open(QIODevice::ReadOnly);
    QList<QByteArray> lines = dataFile.readAll().split('\n');

    if((lines.length() % 2) == 1){
        lines.pop_back();
    }

    int rows = lines.length();
    int cols = lines.at(0).split(',').length();

    int timeSlots = PLC_Time::GetNumTimeslots();
    int freqBands = spectrumModel->GetNumBands();

    qDebug() << "Loading file: " << file;
    qDebug() << "  Dimensions: " << '(' << (rows/2) << "Time Slots," << cols << "Freq. Slots)";

    if((freqBands != cols) && (cols != 1)){
        qDebug() << "The specified number of frequency bands (" << freqBands << ") does not match the number of columns in" << file << '(' << cols << ')';
        ok = false;
        return Create<PLC_ConstValue>(spectrumModel, 0);
    }

    if((timeSlots != (rows/2)) && (rows/2 != 1)){
        qDebug() << "The specified number of time samples (" << timeSlots << ") does not match the number of rows in" << file << '(' << rows/2 << ')';
        ok = false;
        return Create<PLC_ConstValue>(spectrumModel, 0);
    }

    QList<QList<double> > data;
    QList<double> currentRow;


    //Load data into doubles
    for(int i = 0; i < rows; i++){
        QList<QByteArray> values = lines.at(i).split(',');
        currentRow.clear();
        for(int j = 0; j < cols; j++){
            currentRow.push_back(values.at(j).toDouble());
        }
        data.push_back(currentRow);
    }

    //Load data into complex values
    std::vector< std::vector<std::complex<double> > >  complexValues;
    std::vector<std::complex<double> > currentComplexRow;

    for(int i = 0; i < (rows/2); i++){
        currentComplexRow.clear();
        for(int j = 0; j < cols; j++){
            currentComplexRow.push_back(std::complex<double>(data.at(i).at(j), data.at(i + (rows/2)).at(j)));
        }
        complexValues.push_back(currentComplexRow);
    }

    //We have half as many rows because we folded the second half into the imaginary part of complex values
    rows /= 2;


    if(rows == 1 || cols == 1){
        std::vector<complex<double> > valueVector;

        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                valueVector.push_back(complexValues.at(i).at(j));
            }
        }

        //Return corresponding type
        if(rows == 1 && cols == 1){
            return Create<PLC_ConstValue>(spectrumModel, valueVector.at(0));
        }

        if(rows == 1){
            return Create<PLC_FreqSelectiveValue>(spectrumModel, valueVector);
        }

        if(cols == 1){
            return Create<PLC_TimeVariantConstValue>(spectrumModel, valueVector);
        }
    }


    return Create<PLC_TimeVariantFreqSelectiveValue>(spectrumModel, complexValues);

}

Ptr<PLC_ConstValue> PLCTopologyLoader::constFromValueString(PLCValueString value, Ptr<const SpectrumModel> spectrumModel){
    return Create<PLC_ConstValue>(spectrumModel, value.getComplex());
}

Ptr<PLC_ValueBase> PLCTopologyLoader::fromValueString(PLCValueString value, Ptr<const SpectrumModel> spectrumModel, bool &ok){
    if(value.isFile()){
        return valueFromFile(value.getValue(), spectrumModel, ok);
    }
    else{
        return constFromValueString(value.getValue(), spectrumModel);
    }
}

Ptr<PLC_NoiseSource> PLCTopologyLoader::fromNoiseSourceModel(NoiseSourceModel *noiseSource, Ptr<const SpectrumModel> spectrumModel, Ptr<PLC_Node> sourceNode, bool& valid){

    Q_UNUSED(valid)

    //LogComponentEnable("PLC_Noise", LOG_LEVEL_FUNCTION);

    Ptr<PLC_NoiseSource> newNoiseSource;

    Ptr<SpectrumValue> noisePSD = Create<SpectrumValue>(spectrumModel);
    (*noisePSD) = noiseSource->getNoisePSD().getReal();

    if(noiseSource->getNoiseType() == "Static"){
        Ptr<PLC_StaticNoiseSource> staticNoise = Create<PLC_StaticNoiseSource>(sourceNode, noisePSD);
        newNoiseSource = staticNoise;
        staticNoise->Init();
        newNoiseSource->Enable();
        //Simulator::Schedule(Seconds(0), &PLC_StaticNoiseSource::Initialize, PeekPointer(staticNoise), Seconds(30));

    }

    if(noiseSource->getNoiseType() == "Impulsive"){
        newNoiseSource = Create<PLC_ImpulsiveNoiseSource>(sourceNode, noisePSD);
        newNoiseSource->Init();
        newNoiseSource->Enable();
    }



    //TODO: Adding on/off timing for the impulse noise source?

    return newNoiseSource;
}

Ptr<PLC_NetDevice> PLCTopologyLoader::fromNetDeviceModel(NetDeviceModel *netDevice, Ptr<const SpectrumModel> spectrumModel, Ptr<PLC_Node> sourceNode, Mac48Address addr, bool& valid){
    LogComponentEnable("PLC_NetDevice", LOG_LEVEL_FUNCTION);

    ObjectFactory netDeviceFactory;
    netDeviceFactory.SetTypeId(PLC_NetDevice::GetTypeId());

    Ptr<PLC_NetDevice> newNetDevice = netDeviceFactory.Create<PLC_NetDevice>(); //TODO: More than just the simple net device type?

    newNetDevice->SetSpectrumModel(spectrumModel);
    //ObjectFactory errorModelFactory;
    //errorModelFactory.SetTypeId(PLC_ChannelCapacityErrorModel::GetTypeId());

    Ptr<SpectrumValue> txPsd = Create<SpectrumValue> (spectrumModel);
    (*txPsd) = 1e-7; // -40dBm

    /*Defaults for now -> Add to Net Device Configuration!!!*/

    //newNetDevice->SetModulationAndCodingScheme(QPSK_1_2);
    newNetDevice->SetNoiseFloor(CreateBestCaseBgNoise(spectrumModel)->GetNoisePsd());
    //newNetDevice->SetErrorModel(errorModelFactory.Create<PLC_ErrorModel>());
    newNetDevice->SetTxPowerSpectralDensity(txPsd);

    newNetDevice->SetMac(CreateObject<PLC_ArqMac>());
    newNetDevice->SetAddress(addr);



    /*This stuff actually included from the diagram*/
    newNetDevice->SetPlcNode(sourceNode);

    if(!netDevice->getShuntImpedance().getValue().isEmpty()){
        newNetDevice->SetShuntImpedance(fromValueString(netDevice->getShuntImpedance(), spectrumModel, valid));
    }





   // newNetDevice->CompleteConfig();

    //TODO: TX PSD, Device type, Address, Bridge, ErrorModel, Mod/Coding scheme.... maybe more??
    //Seems like shunt impedance gets thrown in as an outlet on the node.

    //TODO: See what 'creating a node' does. Doing it for now.... they are not saved anywhere however.
    Ptr<Node> newNetNode = CreateObject<Node>();
    newNetNode->AddDevice(newNetDevice);


    newNetDevice->SetRxImpedance(fromValueString(netDevice->getRXImpedance(), spectrumModel, valid));
    newNetDevice->SetTxImpedance(fromValueString(netDevice->getTXImpedance(), spectrumModel, valid));

    return newNetDevice;
}

Ptr<PLC_Node> PLCTopologyLoader::fromNodeModel(NodeModel *node, Ptr<const SpectrumModel> spectrumModel, bool &valid){
    Ptr<PLC_Node> newNode = Create<PLC_Node>();

    newNode->SetName(node->getName().toStdString());
    //Creating outlet might need to be done last or else net devices will overwrite it??
    if(node->getHasOutlet()){
        Ptr<PLC_Outlet> newOutlet = Create<PLC_Outlet>(newNode, fromValueString(node->getOutletImpedance(), spectrumModel, valid));
    }

    //TODO: Double check that the position isn't used anywhere else other than when calculate is called on an edge...
    newNode->SetPosition(0, 0, 0);
    return newNode;
}

