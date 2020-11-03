/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mapcanvasview.h"
#include <stdlib.h>
#include <qwmatrix.h>
#include "mapcanvas.h"
#include "defs.h"


#define SELECTIONSIZE (MAPROOMSIZE+16)
MapCanvasView::MapCanvasView(QCanvas& c, QWidget *parent, const char *name, WFlags f)
        : QCanvasView(&c, parent, name, f) {

   scaleFactor=1.0;
   selected = FALSE;
   //room=NULL;
   selected2= FALSE;
   QPen pen;
   mark = new QCanvasRectangle(0,0,SELECTIONSIZE,SELECTIONSIZE, &c);
   mark->setZ(151);
   mark->move(MAPROOMSIZE-(SELECTIONSIZE-MAPROOMSIZE)/2,
              MAPROOMSIZE-(SELECTIONSIZE-MAPROOMSIZE)/2);
   
   pen=mark->pen();
   pen.setWidth(2);
   pen.setColor(yellow);
   mark->setPen(pen);

   selection_mark = new QCanvasRectangle(0,0,SELECTIONSIZE-2,SELECTIONSIZE-2, &c);
   selection_mark->setZ(150);
   selection_mark->move(MAPROOMSIZE-(SELECTIONSIZE-2-MAPROOMSIZE)/2,
                        MAPROOMSIZE-(SELECTIONSIZE-2-MAPROOMSIZE)/2);

   pen=selection_mark->pen();
   pen.setWidth(2);
   pen.setColor(white);
   selection_mark->setPen(pen);
}


MapCanvasView::~MapCanvasView() {}



void MapCanvasView::zoomIn(){
    setUpdatesEnabled(FALSE);
    QPoint p = QPoint (contentsX()+visibleWidth()/2    , contentsY()+visibleHeight()/2);
    p = inverseWorldMatrix().map(p);
   
    QWMatrix m;
    m=worldMatrix();
    m.scale(1/scaleFactor,1/scaleFactor);
    scaleFactor /=.8;
    if (scaleFactor >= 1.0) 
      ((MapCanvas *) canvas())->showGrid(TRUE); 
    else 
      ((MapCanvas *) canvas())->showGrid(FALSE);
    m.scale(scaleFactor,scaleFactor);
    setWorldMatrix(m);
    
    p = worldMatrix().map(p);
    center ( p.x(), p.y() );
    setUpdatesEnabled(TRUE);
    //canvas()->update();
    //canvas()->advance();
    //repaint();
}

void MapCanvasView::zoomOut(){
    
    setUpdatesEnabled(FALSE);
    QPoint p = QPoint (contentsX()+visibleWidth()/2    , contentsY()+visibleHeight()/2);
    p = inverseWorldMatrix().map(p);
    
    QWMatrix m;
    m=worldMatrix();
    m.scale(1/scaleFactor,1/scaleFactor);
    scaleFactor *=.8;
    if (scaleFactor >= 1.0) 
      ((MapCanvas *) canvas())->showGrid(TRUE); 
    else 
      ((MapCanvas *) canvas())->showGrid(FALSE);
    m.scale(scaleFactor,scaleFactor);
    setWorldMatrix(m);
    
    p = worldMatrix().map(p);
    center ( p.x(), p.y() ); ( p.x(), p.y() );
    setUpdatesEnabled(TRUE);
    //canvas()->update();
    //canvas()->advance();
    //repaint();
}

void MapCanvasView::zoomAll() {
}

void MapCanvasView::moveMark2(double xp, double yp){
   if ((xp+MAPROOMSIZE<MAPROOMSIZE) ||  (yp+MAPROOMSIZE<MAPROOMSIZE)) return;
   if ((xp>canvas()->width()-MAPROOMSIZE) ||  (yp>canvas()->height()-MAPROOMSIZE)) return;

   selection_mark->move(xp-7,yp-7);
   selection_mark->show();
   selected2=TRUE;

   canvas()->update();
}


void MapCanvasView::moveMark(double xp, double yp){
   
   double ax,ay,bx,by;

    
   if ((xp+MAPROOMSIZE<MAPROOMSIZE) ||  (yp+MAPROOMSIZE<MAPROOMSIZE)) return;
   if ((xp>canvas()->width()-MAPROOMSIZE) ||  (yp>canvas()->height()-MAPROOMSIZE)) return;

   mark->move(xp-8,yp-8);
   mark->show();
   selection_mark->hide();
   selected2=FALSE;
   selected=TRUE;

   canvas()->update();

   if (mode != 2){
     ax=(xp-MAPROOMSIZE*3.5);
     ay=(yp-MAPROOMSIZE*3.5);
     bx=(xp+MAPROOMSIZE*4.5);
     by=(yp+MAPROOMSIZE*4.5);
   }else{
     ax=(xp+MAPROOMSIZE*0.5);
     ay=(yp+MAPROOMSIZE*0.5);
     bx=(xp+MAPROOMSIZE*0.5);
     by=(yp+MAPROOMSIZE*0.5);
   }   
   
   worldMatrix().map(ax,ay,&ax,&ay);
   worldMatrix().map(bx,by,&bx,&by);
   
   ensureVisible ( (int)ax, (int)ay);
   ensureVisible ( (int)bx, (int)ay);
   ensureVisible ( (int)bx, (int)by);
   ensureVisible ( (int)ax, (int)by);
  
}

void MapCanvasView::markMoveUp(){
  double x =  mark->x() + 8;
  double y =  mark->y() + 8;		    
  moveMark(x,y-MAPROOMSIZE);
  Data().setCurrentRoom(VIEWPORTTOROOM(x), VIEWPORTTOROOM(y)-1);
  emit roomSelected();
}

void MapCanvasView::markMoveDown(){
  double x =  mark->x() + 8;
  double y =  mark->y() + 8;		    
  moveMark(x, y + MAPROOMSIZE);
  Data().setCurrentRoom(VIEWPORTTOROOM(x), VIEWPORTTOROOM(y)+1);
  emit roomSelected();
}

void MapCanvasView::markMoveLeft(){
  double x =  mark->x() + 8;
  double y =  mark->y() + 8;		    
  moveMark(x-MAPROOMSIZE,y);
  Data().setCurrentRoom(VIEWPORTTOROOM(x)-1, VIEWPORTTOROOM(y));
  emit roomSelected();
}

void MapCanvasView::markMoveRight(){
  double x =  mark->x() + 8;
  double y =  mark->y() + 8;		    
  moveMark(x+MAPROOMSIZE,y);
  Data().setCurrentRoom(VIEWPORTTOROOM(x)+1, VIEWPORTTOROOM(y));
  emit roomSelected();
}


void MapCanvasView::contentsMousePressEvent( QMouseEvent* e ) {
    QWMatrix m;
    double xs=e->pos().x();
    double ys=e->pos().y();
    m=inverseWorldMatrix();
    m.map(xs,ys,&xs,&ys);

    setFocus();
   emit mousePressed(e,xs,ys);
}

void MapCanvasView::contentsMouseMoveEvent( QMouseEvent* e ) {
    QWMatrix m;
    double xs=e->pos().x();
    double ys=e->pos().y();
    m=inverseWorldMatrix();
    m.map(xs,ys,&xs,&ys);

    setFocus();
   emit mouseMoved(e,xs,ys);
}

void MapCanvasView::contentsMouseDoubleClickEvent( QMouseEvent* e ){
    QWMatrix m;
    double xs=e->pos().x();
    double ys=e->pos().y();
    m=inverseWorldMatrix();
    m.map(xs,ys,&xs,&ys);

    setFocus();
   emit mouseDoubleClicked(e,xs,ys);
}


void MapCanvasView::selectRoom(Room* r){
       if (!r) return;
       moveMark(ROOMTOVIEWPORT(r->mapx), ROOMTOVIEWPORT(r->mapy));
       
       Data().setCurrentRoom(r);
       emit roomSelected();
}

void MapCanvasView::selectRoom(unsigned x,unsigned y){
    moveMark(ROOMTOVIEWPORT(x), ROOMTOVIEWPORT(y));
    
    Data().setCurrentRoom(x, y);
    emit roomSelected();
}

void MapCanvasView::selectRoom2(Room *r){
       if (!r) return;
       moveMark2(ROOMTOVIEWPORT(r->mapx), ROOMTOVIEWPORT(r->mapy));
       
       Data().setSelectedRoom2(r);
       emit roomSelected2();
}

void MapCanvasView::selectRoom2(unsigned x,unsigned y){
    moveMark2(ROOMTOVIEWPORT(x), ROOMTOVIEWPORT(y));
    
    Data().setSelectedRoom2(x,y);
    emit roomSelected2();
}



