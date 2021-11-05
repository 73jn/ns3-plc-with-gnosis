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

#ifndef DIAGRAMEDITOR_H
#define DIAGRAMEDITOR_H

#include <QGraphicsView>
#include "../DiagramSheet/diagramsheet.h"
#include "../PLCSpectrumConfiguration/plcspectrumconfiguration.h"


/*! \class DiagramEditor
 *  \brief Provides a viewport widget for DiagramSheets
 *
 *  The DiagramEditor, derived from QGraphicsView, provides a widget for displaying
 *  DiagramSheet objects. In particular this class handles modifying zoom parameters
 *  and handles most key presses relevant for editing the scene. When necessary,
 *  this class notifies the current DiagramSheet that certain editing operations
 *  are underway and that it should update itself accordingly.
 *
*/
class DiagramEditor : public QGraphicsView
{
    Q_OBJECT

public:
    DiagramEditor();
    DiagramSheet* sheet() const;

    void setScene(QGraphicsScene *scene);

signals:
    void zoomChanged(double zoom);

public slots:
    void setZoom(double zoom);

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    QPointF zoomAnchorScene;  /*!< Holds the zoom anchor in scene coordinates */
    QPoint  zoomAnchorView;   /*!< Holds the zoom anchor in the editors coordinates */

    double zoomFactor;
};

#endif // DIAGRAMEDITOR_H
