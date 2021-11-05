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

#ifndef NOISESOURCEDIALOG_H
#define NOISESOURCEDIALOG_H

#include <QDialog>
#include "noisesourcemodel.h"
#include "plcdatainputwidget.h"
#include <QListWidgetItem>
#include <QComboBox>
#include <QCloseEvent>

class NoiseSourceEditor : public QWidget
{
    Q_OBJECT
public:
    explicit NoiseSourceEditor(NoiseSourceModel* source, QWidget *parent = 0);
    void populateFromModel();
    void saveChanges();
    bool isValid();

signals:

protected:
    
public slots:

private:
    QComboBox* noiseTypeSelector;
    QLineEdit* nameEdit;
    PLCDataInputWidget * noisePSDInput;

    NoiseSourceModel * noiseSrcModel;
};

#endif // NOISESOURCEDIALOG_H
