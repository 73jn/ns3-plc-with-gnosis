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

#ifndef PLCDATAINPUTWIDGET_H
#define PLCDATAINPUTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

class PLCDataInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PLCDataInputWidget(const QString& paramName, bool checkable = false, QWidget *parent = 0);
    QString getValue();
    void setValue(const QString &value);
    bool isValid();
    bool isChecked(){ return (!checkable || checked); }
    bool isCheckable();

    void setCheckable(bool checkable);

signals:


public slots:
    void getFile();
    void setChecked(bool);

private:
    QLabel * parameterLabel;
    QLineEdit* lineEdit;
    QPushButton* selectFileButton;
    QCheckBox * checkBox;

    bool checkable;
    bool checked;
};

#endif // PLCDATAINPUTWIDGET_H
