#ifndef PLCSIMULATORSETTINGSWIDGET_H
#define PLCSIMULATORSETTINGSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QCheckBox>

class PLCSimulatorSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PLCSimulatorSettingsWidget(QString ns3Dir, QStringList rxNodes, QStringList txNodes, QWidget* parent = 0);


public slots:
    void handleDoCollectTransferFunctions();
    void handleDoCalculateRxPSDSINR();

signals:
    void collectGivenTransferFunctions(QStringList enTxNodes, QStringList enRxNodes);
    void doNoiseSim(bool doPSD, bool doSINR, QString txNode, QString rxNode);

private:

    QStringList rxNodes;
    QStringList txNodes;

    QGroupBox* ctfSettings;
    QGroupBox* noiseSimSettings;

    QListWidget* disabledRxNodes;
    QListWidget* enabledRxNodes;

    QListWidget* disabledTxNodes;
    QListWidget* enabledTxNodes;

    QPlainTextEdit* ns3DirectoryEdit;

    QComboBox* txDropDown;
    QComboBox* rxDropDown;

    QPushButton* doCollectTransferFunctions;
    QPushButton* doCalculateRxPSDSINR;

    QCheckBox* doPSDCalculation;
    QCheckBox* doSINRCalculation;
};

#endif // PLCSIMULATORSETTINGSWIDGET_H
