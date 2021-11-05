#include <QDebug>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDir>
#include <QMap>

typedef void (*argHandler)(const QString& val);

QMap<QString,  argHandler> argHandlers;
QString qMakeName("qmake");
QString scriptName;
QString execName("script");

void setQMakeName(const QString& val){
    qMakeName = val;
}

void setExecName(const QString& val){
    execName = val;
}

void initArgHandlers(){
    argHandlers["--qmake-name"] = &setQMakeName;
    argHandlers["--exec-name"] = &setExecName;
}

int main(int argc, char* argv[]){
    initArgHandlers();

    QStringList argVals;

    if(argc < 2){
        qDebug() << "Pass name of ns3 program/script to launch.";
        return 1;
    }

    for(int i = 1; i < argc; i++){
        argVals = QString(argv[i]).split("=");
        if(argVals.count() == 2){
            if(argHandlers.contains(argVals.at(0))){
                argHandlers[argVals.at(0)](argVals.at(1));
            }
        }
        else{
            scriptName = argv[i];
        }
    }

    QProcess* scriptProcess = new QProcess();
    scriptProcess->setProcessChannelMode(QProcess::ForwardedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QDir dir("./scripts");

    //Make sure we can get at the ns3 libraries
    env.insert(LD_ENV_VAR, QDir::currentPath() + ':' + PLC_DEFAULT_NS3_PATH + ':' + env.value(LD_ENV_VAR));
    scriptProcess->setProcessEnvironment(env);

    if(!dir.cd(scriptName)){
        qDebug() << "Folder for specified script" << scriptName << "doesn't exist";
        return 0;
    }

    scriptProcess->setWorkingDirectory(dir.path());

    qDebug() << "Checking build";
    scriptProcess->start(QString("qmake plc-ns3-script.pro NS3_DIR=") + QString(PLC_DEFAULT_NS3_PATH));
    scriptProcess->waitForFinished();
    qDebug() << "qMake finished, with ret code" << scriptProcess->exitCode();

    scriptProcess->start("make");
    scriptProcess->waitForFinished();
    qDebug() << "make finished, with ret code" << scriptProcess->exitCode();

    qDebug() << "running script...";
    scriptProcess->start(QString("./") + execName);
    scriptProcess->waitForFinished();
    qDebug() << "Script finished, with ret code" << scriptProcess->exitCode();
}

