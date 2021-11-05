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

#ifndef NETDEVICEDIALOG_H
#define NETDEVICEDIALOG_H

#include "netdevicemodel.h"
#include "plcdatainputwidget.h"
#include <QListWidgetItem>
#include <QCloseEvent>

class NetDeviceEditor : public QWidget
{
    Q_OBJECT
public:
    explicit NetDeviceEditor(NetDeviceModel* device, QWidget *parent = 0);


    void saveChanges();
    bool isValid();

    void clearModel();
    NetDeviceModel* takeNetDevice();


signals:
    
public slots:

protected:

private:
    void populateFromModel();

    QLineEdit * nameEdit;

    PLCDataInputWidget * shuntImpedanceInput;
    PLCDataInputWidget * txImpedanceInput;
    PLCDataInputWidget * rxImpedanceInput;

    NetDeviceModel* netDevModel;
};

#endif // NETDEVICEDIALOG_H
