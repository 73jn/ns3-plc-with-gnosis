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

#include "diagrameditor.h"
#include "../DiagramSheet/diagramsheet.h"

#include <QMouseEvent>
#include <QDebug>
#include <QScrollBar>
#include <math.h>

#define sheet_size_x 1920
#define sheet_size_y 1080
#define grid_width 40.0

/*! Constructs a default, empty editor with the scene properly set to a \a DiagramSheet
 *  rather than the default QGraphicsScene, which is NOT compatible with this class.
 */
DiagramEditor::DiagramEditor()
{
    setResizeAnchor(QGraphicsView::NoAnchor);
    setDragMode(QGraphicsView::RubberBandDrag);
    setMouseTracking(true);
    this->viewport()->setMouseTracking(true);

    setTransformationAnchor(QGraphicsView::NoAnchor);
    setSceneRect(QRectF(-sheet_size_x/2.0, -sheet_size_y/2.0, sheet_size_x, sheet_size_y));
    scale(.5, .5);

    setFocusPolicy(Qt::StrongFocus);
    setScene(new DiagramSheet(this));

    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    zoomAnchorView = QPoint(0, 0);
    zoomAnchorScene = QRectF(mapToScene(zoomAnchorView - QPoint(1, 1)),
                             mapToScene(zoomAnchorView + QPoint(1, 1))).center();

    zoomFactor = 1.0;
}

/*! This overloaded setScene member function ensures that the set "scene" is actually a diagram
 *  sheet. Both custom QGraphicsItem subclasses and other member functions for DiagramEditor
 *  expect that the current sheet is DiagramSheet, and basic editing will fail if this is
 *  not the case.
 *
 *  \sa DiagramSheet PLCGraphicsEdgeItem PLCGraphicsNodeItem
 */
void DiagramEditor::setScene(QGraphicsScene *scene){
    if(scene == 0){
        return;
    }

    DiagramSheet * diagramSheet = dynamic_cast<DiagramSheet*>(scene);
    if(diagramSheet == 0){
        return;
    }

    QGraphicsView::setScene(scene);
}

/*! Returns the DiagramSheet associated with this editor. */
DiagramSheet * DiagramEditor::sheet() const{
   return dynamic_cast<DiagramSheet*>(this->scene());
}

/*! Handles keypresses relevant for capture by the editor rather than directly by
 *  the scene iteself. Currently only a control-modifier is captured in order to
 *  initiate zooming with the mouse wheel. Feature additions for cut/paste functionality
 *  as well as undo/redo commands will be completed as time allows.
 */
void DiagramEditor::keyPressEvent(QKeyEvent *event){

    if(event->type() == QEvent::KeyPress){
        if(event->key() == Qt::Key_Control){
            event->accept();
            zoomAnchorView = mapFromGlobal(QCursor::pos());
            zoomAnchorScene = QRectF(mapToScene(zoomAnchorView - QPoint(1, 1)),
                                     mapToScene(zoomAnchorView + QPoint(1, 1))).center();
        }

        else{
             QGraphicsView::keyPressEvent(event);
        }

        return;
    }

    QGraphicsView::keyPressEvent(event);
    return;
}

void DiagramEditor::setZoom(double zoom){
    qreal postZoomFactor = zoom;

    if(zoom != this->zoomFactor){

        postZoomFactor = (postZoomFactor < 0.25)?0.25:postZoomFactor;
        postZoomFactor = (postZoomFactor > 8.0)?8.0:postZoomFactor;

        qreal zoomAmount = postZoomFactor/transform().m11();
        scale(zoomAmount, zoomAmount);

        QPoint posDelta = zoomAnchorView - mapFromScene(zoomAnchorScene);

        QScrollBar * vBar = verticalScrollBar();
        QScrollBar * hBar = horizontalScrollBar();

        int hValue = hBar->value() - posDelta.x();
        int vValue = vBar->value() - posDelta.y();

        hBar->setValue(hValue);
        vBar->setValue(vValue);

        this->zoomFactor = postZoomFactor;
        emit zoomChanged(postZoomFactor);
    }
}

/*! Handles wheel events for zooming purposes. By taking into account the cursor position
 *  and the current scene transformation, we create the effect of anchoring zooms at
 *  the cursors position. This custom behaviour precludes the use of Qt's built-in
 *  algorithm, and seems to be a more satisfying implementation of the feature.
 */
void DiagramEditor::wheelEvent(QWheelEvent *event){

    event->accept();

    if(event->modifiers() & Qt::ControlModifier){
        qreal postZoomFactor = transform().m11() + (event->delta()/(30.0*24.0));
        setZoom(postZoomFactor);
    }

    else{
        QGraphicsView::wheelEvent(event);
    }
}
