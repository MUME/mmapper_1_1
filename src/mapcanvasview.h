/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MAPCANVASVIEW_H
#define MAPCANVASVIEW_H

#include <qcanvas.h>
#include <qevent.h>
#include "defs.h"
#include "mapdata.h"

extern int mode;

/**
 * @short This class is used as viewer for MapCanvas class (views whole world).
 * 
 * Except view functionality, this class accepts user input (keyboard and mouse) of
 * map view, 
 * transforms them by the current view matrix (Specialy points selected by mouse: scale, rotate ...) and
 * passes it to MMapper class, where corresponding actions are done. 
 *
 * @see MapCanvasView
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MapCanvasView : public QCanvasView {
    Q_OBJECT
public:
    MapCanvasView(QCanvas&, QWidget *parent = 0, const char *name = 0, WFlags f=0);

    ~MapCanvasView();


    void markMoveUp();
    void markMoveDown();
    void markMoveLeft();
    void markMoveRight();
    
    void zoomIn();
	void zoomOut();
	void zoomAll();

signals:
    void roomSelected();
    void roomSelected2();
    
    void mousePressed( QMouseEvent*, double, double );
    void mouseMoved( QMouseEvent*, double, double );
    void mouseDoubleClicked( QMouseEvent*, double, double );


public slots:
    void selectRoom(Room*);
    void selectRoom(unsigned,unsigned);
    void selectRoom2(Room*);
    void selectRoom2(unsigned,unsigned);
    
protected:

    void moveMark(double xp, double yp);
    void moveMark2(double xp, double yp);

    void contentsMousePressEvent( QMouseEvent* e );
    void contentsMouseMoveEvent( QMouseEvent* e );
    void contentsMouseDoubleClickEvent( QMouseEvent* e );

    //Room* room;
    bool selected, selected2;
    QCanvasRectangle *mark, *selection_mark;
    
    double scaleFactor;
};

#endif
