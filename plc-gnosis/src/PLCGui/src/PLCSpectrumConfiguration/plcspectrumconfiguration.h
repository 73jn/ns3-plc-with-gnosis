/*

Copyright (C) 2013 Donald Harris (dharris.ubc@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

*/

#ifndef PLC_SPECTRUM_MANAGER_H_
#define PLC_SPECTRUM_MANAGER_H_

#include <QDialog>
#include <QDoubleSpinBox>
#include <QPushButton>

#include "plcspectrummodel.h"
class PLCSpectrumConfiguration : public QDialog
{
    Q_OBJECT

public:
    PLCSpectrumConfiguration(PLCSpectrumModel* spectrumModel, QWidget *parent = 0);

    QDoubleSpinBox *freqLimitLowBox, *freqResBox, *noiseFloorBox;
    QSpinBox       *freqNumBandsBox, *mainsFreqBox, *samplesPerCycleBox, *symbolLengthBox;

    PLCSpectrumModel* spectrum;

public slots:
    void saveValues(void);

private:
    QPushButton *okButton;
};


#endif /* PLC_SPECTRUM_MANAGER_H_ */
