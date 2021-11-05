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

#ifndef PLCEDGECONFIGURATION_H
#define PLCEDGECONFIGURATION_H

#include <QDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>

#include "edgemodel.h"
#include "../NodeConfiguration/plcdatainputwidget.h"

/*! \class EdgeConfiguration
 *  \brief Configuration dialog which handles modifying edge settings
 *
 *  This class handles the creation and display of a dialog which can modify the settings
 *  and parameters for edges. */
class EdgeConfiguration : public QDialog
{
    Q_OBJECT
public:

    explicit EdgeConfiguration(EdgeModel* node, QDialog *parent = 0);
    void setModel(EdgeModel * newModel);

signals:

protected:
    virtual void closeEvent(QCloseEvent * event);

public slots:
    void saveAndClose();
    void cableSettingsToggled(bool checked);
    void twoPortSettingsToggled(bool checked);

private:

    EdgeModel* edgeModel;           /*!< The current EdgeModel being edited by this dialog. */

    QLineEdit* nameInput;

    QGroupBox* twoPortSettings;
    QGroupBox* cableSettings;

    QComboBox* cableModels;
    QLineEdit* lengthInput;

    QPushButton* acceptButton;

    PLCDataInputWidget* parametersInput[4];

    void populateFromModel();
};

#endif // PLCEDGECONFIGURATION_H
