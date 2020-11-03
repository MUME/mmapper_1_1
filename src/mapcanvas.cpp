/***************************************************************************
*   Copyright (C) 2003 by Marek Krejza                                    *
*   mkrejza@cbox.cz                                                       *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mapcanvas.h"

#include <stdlib.h>
#include "defs.h"
//#include <kstandarddirs.h>
#include "pref.h"
#include "assert.h"


MapCanvas& Canvas() {
	static MapCanvas canvas(Config().m_canvasWidth*MAPROOMSIZE, Config().m_canvasHeight*MAPROOMSIZE);
	return canvas;
};

void MapCanvas::showPath(QPtrQueue<Command> queue){

	Room *r = Data().getCurrentRoom();
	MyCanvasPathMark *mark = NULL;

	if (!m_pathMarksQueue.isEmpty()){
		m_pathMarksQueue.clear();
		if (queue.isEmpty() || !r) {
			update();
			return;
		}
	}else
		if (queue.isEmpty() || !r) return;

	int indent = MAPROOMSIZE/2;
	uint x=ROOMTOVIEWPORT(r->mapx) + indent;
	uint y=ROOMTOVIEWPORT(r->mapy) + indent;
	QPen pen;
	pen.setWidth(7);
	pen.setColor(QColor(255,255,0));
	pen.setCapStyle(Qt::RoundCap);

	Command *c;
	bool end = FALSE;

	while (!queue.isEmpty()) {
		c = queue.dequeue();

		switch (c->id){
	  case id_north: 
		  if (r->n) {	      
			  m_pathMarksQueue.enqueue( mark = new MyCanvasPathMark(this) );    
			  mark->setPen(pen);
			  mark->setX(0); mark->setY(0); mark->setZ(255);
			  r = ISCONNECTEDTO( r, r->n );
			  mark->setPoints(x,y,ROOMTOVIEWPORT(r->mapx) + indent ,ROOMTOVIEWPORT(r->mapy) + indent);
			  x = ROOMTOVIEWPORT(r->mapx) + indent;
			  y = ROOMTOVIEWPORT(r->mapy) + indent;
			  mark->show();
		  }
		  break;
	  case id_south: 
		  if (r->s) {
			  m_pathMarksQueue.enqueue( mark = new MyCanvasPathMark(this) );    
			  mark->setPen(pen);
			  mark->setX(0); mark->setY(0); mark->setZ(255);
			  r = ISCONNECTEDTO( r, r->s );
			  mark->setPoints(x,y,ROOMTOVIEWPORT(r->mapx) + indent ,ROOMTOVIEWPORT(r->mapy) + indent);
			  x = ROOMTOVIEWPORT(r->mapx) + indent;
			  y = ROOMTOVIEWPORT(r->mapy) + indent;
			  mark->show();
		  }
		  break;
	  case id_east: 
		  if (r->e) {
			  m_pathMarksQueue.enqueue( mark = new MyCanvasPathMark(this) );    
			  mark->setPen(pen);
			  mark->setX(0); mark->setY(0); mark->setZ(255);
			  r = ISCONNECTEDTO( r, r->e );
			  mark->setPoints(x,y,ROOMTOVIEWPORT(r->mapx) + indent ,ROOMTOVIEWPORT(r->mapy) + indent);
			  x = ROOMTOVIEWPORT(r->mapx) + indent;
			  y = ROOMTOVIEWPORT(r->mapy) + indent;
			  mark->show();
		  }
		  break;
	  case id_west: 
		  if (r->w) {
			  m_pathMarksQueue.enqueue( mark = new MyCanvasPathMark(this) );    
			  mark->setPen(pen);
			  mark->setX(0); mark->setY(0); mark->setZ(255);
			  r = ISCONNECTEDTO( r, r->w );
			  mark->setPoints(x,y,ROOMTOVIEWPORT(r->mapx) + indent ,ROOMTOVIEWPORT(r->mapy) + indent);
			  x = ROOMTOVIEWPORT(r->mapx) + indent;
			  y = ROOMTOVIEWPORT(r->mapy) + indent;
			  mark->show();
		  }
		  break;
	  case id_up: 
		  if (r->u) {
			  m_pathMarksQueue.enqueue( mark = new MyCanvasPathMark(this) );    
			  mark->setPen(pen);
			  mark->setX(0); mark->setY(0); mark->setZ(255);
			  r = ISCONNECTEDTO( r, r->u );
			  mark->setPoints(x,y,ROOMTOVIEWPORT(r->mapx) + indent ,ROOMTOVIEWPORT(r->mapy) + indent);
			  x = ROOMTOVIEWPORT(r->mapx) + indent;
			  y = ROOMTOVIEWPORT(r->mapy) + indent;
			  mark->show();
		  }
		  break;
	  case id_down: 
		  if (r->d) {
			  m_pathMarksQueue.enqueue( mark = new MyCanvasPathMark(this) );    
			  mark->setPen(pen);
			  mark->setX(0); mark->setY(0); mark->setZ(255);
			  r = ISCONNECTEDTO( r, r->d );
			  mark->setPoints(x,y,ROOMTOVIEWPORT(r->mapx) + indent ,ROOMTOVIEWPORT(r->mapy) + indent);
			  x = ROOMTOVIEWPORT(r->mapx) + indent;
			  y = ROOMTOVIEWPORT(r->mapy) + indent;
			  mark->show();
		  }
		  break;
	  case id_unknown:
	  case id_flee:
		  end = TRUE;
		  break;
	  case id_look:
	  case id_examine:
	  case id_scout:
		  break;
		}

		if (end) break;
	}

	update();
}



MyCanvasPathMark::MyCanvasPathMark(QCanvas * canvas) :
QCanvasLine(canvas) { 
}

int MyCanvasPathMark::rtti() const {
	return RTTI;
}
int MyCanvasPathMark::RTTI = 1030;



//
// CanvasRoom
//
int CanvasRoom::RTTI()
{
	return 1029;
}

CanvasRoom::CanvasRoom(Room* room,  const QImage& a, QCanvas * canvas) :
CanvasImage(a, canvas), r(room) {}

int CanvasRoom::rtti() const {
	return CanvasRoom::RTTI();
}

MyCanvasArrow::MyCanvasArrow(QCanvas* canvas) :
QCanvasPolygonalItem(canvas) {

	x1 = y1 = x2 = y2 = 0;
	startType = south;
	endType = south;
	arrowType = noarrow;
	arrowSize = 15;
}

MyCanvasArrow::~MyCanvasArrow() {
	hide();
}

void MyCanvasArrow::setPen(QPen p) {
	if ( pen() != p ) {
		//  removeFromChunks();
		QCanvasPolygonalItem::setPen(p);
		//  addToChunks();
	}
}

void MyCanvasArrow::setPoints(int xa, int ya, int xb, int yb) {
	if ( x1 != xa || x2 != xb || y1 != ya || y2 != yb ) {
		//    removeFromChunks();
		x1 = xa;
		y1 = ya;
		x2 = xb;
		y2 = yb;
		//    addToChunks();
	}
}

void MyCanvasArrow::drawShape(QPainter &p) {
	int xa,ya,xb,yb;
	int xao,yao;
	QPen oldPen = p.pen();
	oldPen.setWidth(1);
	oldPen.setColor(QColor(255,255,255));
	oldPen.setJoinStyle(Qt::RoundJoin);
	QPen pen = oldPen;

	xao = xa = (int)(x()+x1);
	yao = ya = (int)(y()+y1);
	xb = (int)(x()+x2);
	yb = (int)(y()+y2);

	p.setPen(pen);

	if (arrowType == noarrow){
		p.drawLine(xa,ya,xb,yb);
		return;
	}

	if (arrowType == bidirectional)
		switch (startType) {
		case up:
			p.drawLine(xa, ya, xa+arrowSize, ya-arrowSize);
			if (arrowType == bidirectional)
				for (int i=arrowSize/3; i>0; i--) {
					pen.setWidth(i);

					p.drawLine(xa + i, ya - i, xa + i -1, ya - i + 1 );
				}
				p.setPen(oldPen);
				xa = xa + arrowSize;
				ya = ya - arrowSize;
				break;
		case down:
			p.drawLine(xa, ya, xa+arrowSize, ya+arrowSize);
			if (arrowType == bidirectional)
				for (int i=arrowSize/3; i>0; i--) {
					pen.setWidth(i);
					p.setPen(pen);
					p.drawLine(xa + i, ya + i, xa + i -1, ya + i - 1 );
				}
				p.setPen(oldPen);
				xa = xa + arrowSize;
				ya = ya + arrowSize;
				break;
		case west:
			p.drawLine(xa, ya, xa-arrowSize, ya);
			if (arrowType == bidirectional)
				for (int i=arrowSize/3; i>0; i--) {
					pen.setWidth(i);
					p.setPen(pen);
					p.drawLine(xa - i, ya, xa - i +1, ya );
				}
				p.setPen(oldPen);
				xa = xa - arrowSize;
				break;
		case east:
			p.drawLine(xa, ya, xa+arrowSize, ya);
			if (arrowType == bidirectional)
				for (int i=arrowSize/3; i>0; i--) {
					pen.setWidth(i);
					p.setPen(pen);
					p.drawLine(xa + i, ya, xa + i -1, ya );
				}
				p.setPen(oldPen);
				xa = xa + arrowSize;
				break;
		case north:
			p.drawLine(xa, ya, xa, ya-arrowSize);
			if (arrowType == bidirectional)
				for (int i=arrowSize/3; i>0; i--) {
					pen.setWidth(i);
					p.setPen(pen);
					p.drawLine(xa, ya - i, xa, ya - i + 1 );
				}
				p.setPen(oldPen);
				ya = ya - arrowSize;
				break;
		case south:
			p.drawLine(xa, ya, xa, ya+arrowSize);
			if (arrowType == bidirectional)
				for (int i=arrowSize/3; i>0; i--) {
					pen.setWidth(i);
					p.setPen(pen);
					p.drawLine(xa, ya + i, xa, ya + i - 1 );
				}
				p.setPen(oldPen);
				ya = ya + arrowSize;
				break;
		}

		switch (endType) {
	case up:
		p.drawLine(xb, yb, xb+arrowSize, yb-arrowSize);
		if (arrowType != noarrow)
			for (int i=arrowSize/3; i>0; i--) {
				pen.setWidth(i);
				p.setPen(pen);
				p.drawLine(xb + i, yb - i, xb + i -1, yb - i + 1 );
			}
			p.setPen(oldPen);
			xb = xb + arrowSize;
			yb = yb - arrowSize;
			break;
	case down:
		p.drawLine(xb, yb, xb+arrowSize, yb+arrowSize);
		if (arrowType != noarrow)
			for (int i=arrowSize/3; i>0; i--) {
				pen.setWidth(i);
				p.setPen(pen);
				p.drawLine(xb + i, yb + i, xb + i -1, yb + i - 1 );
			}
			p.setPen(oldPen);
			xb = xb + arrowSize;
			yb = yb + arrowSize;
			break;
	case west:
		p.drawLine(xb, yb, xb-arrowSize, yb);
		if (arrowType != noarrow)
			for (int i=arrowSize/3; i>0; i--) {
				pen.setWidth(i);
				p.setPen(pen);
				p.drawLine(xb - i, yb, xb - i +1, yb );
			}
			p.setPen(oldPen);
			xb = xb - arrowSize;
			break;
	case east:
		p.drawLine(xb, yb, xb+arrowSize, yb);
		if (arrowType != noarrow)
			for (int i=arrowSize/3; i>0; i--) {
				pen.setWidth(i);
				p.setPen(pen);
				p.drawLine(xb + i, yb, xb + i -1, yb );
			}
			p.setPen(oldPen);
			xb = xb + arrowSize;
			break;
	case north:
		p.drawLine(xb, yb, xb, yb-arrowSize);
		if (arrowType != noarrow)
			for (int i=arrowSize/3; i>0; i--) {
				pen.setWidth(i);
				p.setPen(pen);
				p.drawLine(xb, yb - i, xb, yb - i + 1 );
			}
			p.setPen(oldPen);
			yb = yb - arrowSize;
			break;
	case south:
		p.drawLine(xb, yb, xb, yb+arrowSize);
		if (arrowType != noarrow)
			for (int i=arrowSize/3; i>0; i--) {
				pen.setWidth(i);
				p.setPen(pen);
				p.drawLine(xb, yb + i, xb, yb + i - 1 );
			}
			p.setPen(oldPen);
			yb = yb + arrowSize;
			break;
		}

		//line
		if (arrowType==bidirectional)
			p.drawLine(xa,ya,xb,yb);
		else
			p.drawLine(xao,yao,xb,yb);
}


QPointArray MyCanvasArrow::areaPoints() const {
	QPointArray p(4);
	int xa = (x1<x2) ? x1 : x2;
	int ya = (y1<y2) ? y1 : y2;
	int xb = (x1>=x2) ? x1 : x2;
	int yb = (y1>=y2) ? y1 : y2;

	int dx = QABS(xb - xa);
	int dy = QABS(yb - ya);

	p[0] = QPoint(xa-10   , ya-10);
	p[1] = QPoint(xa+dx+10, ya-10);
	p[2] = QPoint(xa+dx+10, ya+dy+10);
	p[3] = QPoint(xa-10   , ya+dy+10);

	//int pw = pen().width();
	return p;
}

int MyCanvasArrow::rtti() const {
	return RTTI;
}
int MyCanvasArrow::RTTI = 1028;



MapCanvas::MapCanvas(int w, int h)
	: QCanvas() {
	retune(MAPROOMSIZE + 5);
	resize(w, h);

	setDoubleBuffering(true);
	//    m_pathMark = NULL;
	m_pathMarksQueue.setAutoDelete(TRUE);
	m_pathMarksQueue.clear();

	QString pixname("update0.png");
	roomUpdateIcon = QImage::fromMimeSource(pixname);
	if (roomUpdateIcon.isNull())
	{qDebug("Pixmap not found !!! :%s",pixname.ascii());exit (1);}

	//Init terrain type icons
	// old : field, wood, densewood, sand, mountain, water, underwater, town, cave, road, dt
	// new : TINDOORS, TCITY, TFIELD, TFOREST, THILLS ,TMOUNTAINS, TSHALLOW, TWATTER, TRAPIDS, TUNDERWATTER, TROAD, TBRUSH, TTUNNEL, TCAVERN, TDEATHTRAP, TRANDOM
	readImageSequence("terrain", 16, roomTerrainTypeIcons);
	readImageSequence("road", 16, roomRoadTypeIcons);
	readImageSequence("river", 16, roomRiverTypeIcons);

	//Init Mob type icons
	// aggresive, peacefull, quest, shop, rent, guild
	readImageSequence("mob", 6, roomMobTypeIcons);

	//Init load type icons
	// herb, treasure, key, water, food, horse, warg, tower, attention
	readImageSequence("load", 10, roomLoadTypeIcons);

	readImageSequence("wall", 12, roomExitTypeImages);

	//Init exit type icons
	//   1        2      4      8    16   32
	// northw, southw, eastw, westw, upw, downw,    -walls
	//   64      128     256   512   1024  2048
	// northd, southd, eastd, westd, upd, downd     -doors
        roomExitTypeIcons.resize(4096);

	QImage *imageout = NULL;
	unsigned int index;
	int tmp=0;
	for(unsigned int i=0; i<4096; i++) {
		index=i;

		if ( ISSET(index,WALLNORTH) && ISSET(index,DOORNORTH))
			UNSET(index,WALLNORTH);
		if ( ISSET(index,WALLSOUTH) && ISSET(index,DOORSOUTH))
			UNSET(index,WALLSOUTH);
		if ( ISSET(index,WALLEAST)  && ISSET(index,DOOREAST))
			UNSET(index,WALLEAST);
		if ( ISSET(index,WALLWEST)  && ISSET(index,DOORWEST))
			UNSET(index,WALLWEST);
		if ( ISNOTSET(index,WALLUP)   && ISSET(index,DOORUP))
			SET(index,WALLUP);
		if ( ISNOTSET(index,WALLDOWN) && ISSET(index,DOORDOWN))
			SET(index,WALLDOWN);
		if (index<i)
			continue;

		imageout = NULL;
		if (ISSET(i,WALLNORTH))
			imageout = joinImages(imageout, roomExitTypeImages.at(0));
		if (ISSET(i,WALLSOUTH))
			imageout = joinImages(imageout, roomExitTypeImages.at(1));
		if (ISSET(i,WALLEAST))
			imageout = joinImages(imageout, roomExitTypeImages.at(2));
		if (ISSET(i,WALLWEST))
			imageout = joinImages(imageout, roomExitTypeImages.at(3));
		if (ISNOTSET(i,WALLUP))
			imageout = joinImages(imageout, roomExitTypeImages.at(4));
		if (ISNOTSET(i,WALLDOWN))
			imageout = joinImages(imageout, roomExitTypeImages.at(5));
		if (ISSET(i,DOORNORTH))
			imageout = joinImages(imageout, roomExitTypeImages.at(6));
		if (ISSET(i,DOORSOUTH))
			imageout = joinImages(imageout, roomExitTypeImages.at(7));
		if (ISSET(i,DOOREAST))
			imageout = joinImages(imageout, roomExitTypeImages.at(8));
		if (ISSET(i,DOORWEST))
			imageout = joinImages(imageout, roomExitTypeImages.at(9));
		if (ISSET(i,DOORUP))
			imageout = joinImages(imageout, roomExitTypeImages.at(10));
		if (ISSET(i,DOORDOWN))
			imageout = joinImages(imageout, roomExitTypeImages.at(11));


		if (!imageout)
			continue;
		roomExitTypeIcons[i] = *imageout;

		tmp++;
	}
	//qDebug("Pixmaps created ... %i",tmp);

	setBackgroundColor(QColor(140,140,140));
	showGrid(TRUE);
}

void MapCanvas::readImageSequence(const QString& imageFilePrefix, const int upperBound, tImageVector& vec)
{
	const QString fmt = imageFilePrefix + "%1.png";
	QString pixname;
	QImage image;
	vec.clear();

	for (int i=0; i < upperBound; i++) {
		pixname = fmt.arg(i);
		image = QImage::fromMimeSource(pixname);
		if (image.isNull())
		{
			qDebug("Pixmap not found !!! :%s",pixname.ascii());exit (1);
		}

		vec.append(image);
	}
}

MapCanvas::~MapCanvas() {}

void MapCanvas::showGrid(bool b){
	if (b){
		//pixmap to be used as background grid in mapviewcanvas
		int size = MAPROOMSIZE;
		QPixmap *pix=new QPixmap(size,size);
		QBrush brush(QColor(140,140,140));
		QPainter p;
		p.begin( pix );
		size--;
		p.setPen( QPen(QColor(150,150,150),1) );
		p.fillRect(0,0,size,size,brush);
		p.drawLine (0,size,size,size);
		p.drawLine (size,0,size,size);
		p.end();
		setBackgroundPixmap(*pix);
	}
	else {
		setBackgroundPixmap(NULL);
	}
}

QImage* MapCanvas::joinImages(QImage *dest, const QImage& src) {
	if (src.isNull())
	{
		qDebug("NULL images in join!!!");exit (1);
	}

	if (!(dest)) {
		dest = new QImage(MAPROOMSIZE,MAPROOMSIZE,32);
		*dest = src.copy();
		return dest;
	}

	QRgb color;
	QRgb *srcLine, *destLine;
	
	// avoid function calling in tight loops
	const int destWidth = dest->width();
	const int destHeight = dest->height();

	// for 32 depth we can optimalize access on scanlines, we do not need to rely on pixel()
	assert(src.depth() == 32);

	for (int y=0; y < destHeight; y++) {
		srcLine = reinterpret_cast<QRgb *>(src.scanLine(y));
		destLine = reinterpret_cast<QRgb *>(dest->scanLine(y));

		for (int x=0; x < destWidth; x++) {
			color = srcLine[x];
			if (color != 15658726) {
				destLine[x] = color;
			}
		}
	}
	return dest;
}


void MapCanvas::activate_room(int x, int y) {
	QCanvasItemList l;
	l=collisions( QPoint(x*MAPROOMSIZE, y*MAPROOMSIZE) );
	for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		if ( (*it)->rtti() == QCanvasRectangle::RTTI ) {}
	}
}


void MapCanvas::draw_connection(Connection *connection) {

	bool startUpDown = FALSE;
	bool endUpDown = FALSE;
	bool neighbours = FALSE;
	if (!connection || connection->onScreen || !connection->room1 || !connection->room2)
		return;

	bool end = FALSE;
	double r1x = ROOMTOVIEWPORT(connection->room1->mapx);
	double r1y = ROOMTOVIEWPORT(connection->room1->mapy);
	double r2x = ROOMTOVIEWPORT(connection->room2->mapx);
	double r2y = ROOMTOVIEWPORT(connection->room2->mapy);

	double xa = r1x;
	double ya = r1y;
	double xb = r2x;
	double yb = r2y;

	connection->onScreen = TRUE;

	// are rooms neighbours? if yes, we dont need draw connection
	if ( ((r1x == r2x - MAPROOMSIZE) && (r1y == r2y )) ||
		((r1x == r2x) && (r1y == r2y - MAPROOMSIZE)) ||
		((r1x == r2x + MAPROOMSIZE) && (r1y == r2y )) ||
		((r1x == r2x) && (r1y == r2y + MAPROOMSIZE ))
		)
		neighbours=TRUE;


	MyCanvasArrow* ar = new MyCanvasArrow(this);
	connection->arrowObject = ar;
	ar->setZ(255);

	//set type of start arrow
	if (connection->room1->u == connection) {
		startUpDown=TRUE;
		ar->setStartType(up);
		if(!neighbours){
			xa+=25;
			ya+=10;
		}else{
			xa+=37;
			ya+=10;
		}
	}
	if (connection->room1->d == connection) {
		startUpDown=TRUE;
		ar->setStartType(down);
		if(!neighbours){
			xa+=20;
			ya+=35;
		}else{
			xa+=12;
			ya+=35;
		}
	}
	if (connection->room1->e == connection) {
		ar->setStartType(east);
		xa+=MAPROOMSIZE-10;
		ya+=MAPROOMSIZE/2-8;
	}
	if (connection->room1->w == connection) {
		ar->setStartType(west);
		ya+=MAPROOMSIZE/2-8;
		xa+=10;
	}
	if (connection->room1->n == connection) {
		ar->setStartType(north);
		xa+=MAPROOMSIZE/2-8;
		ya+=10;
	}
	if (connection->room1->s == connection) {
		ar->setStartType(south);
		xa+=MAPROOMSIZE/2-8;
		ya+=MAPROOMSIZE-10;
	}

	//set type of end arrow
	if (connection->room2->u == connection) {
		endUpDown=TRUE;
		ar->setEndType(up);
		if(!neighbours){
			xb+=25;
			yb+=10;
		}else{
			xb+=37;
			yb+=10;
		}
		end=TRUE;
	}
	if (connection->room2->d == connection) {
		endUpDown=TRUE;
		ar->setEndType(down);
		if(!neighbours){
			xb+=20;
			yb+=35;
		}else{
			xb+=12;
			yb+=35;
		}
		end=TRUE;
	}
	if (connection->room2->e == connection) {
		ar->setEndType(east);
		xb+=MAPROOMSIZE-10;
		yb+=MAPROOMSIZE/2+8;
		end=TRUE;
	}
	if (connection->room2->w == connection) {
		ar->setEndType(west);
		yb+=MAPROOMSIZE/2+8;
		xb+=10;
		end=TRUE;
	}
	if (connection->room2->n == connection) {
		ar->setEndType(north);
		xb+=MAPROOMSIZE/2+8;
		yb+=10;
		end=TRUE;
	}
	if (connection->room2->s == connection) {
		ar->setEndType(south);
		xb+=MAPROOMSIZE/2+8;
		yb+=MAPROOMSIZE-10;
		end=TRUE;
	}


	//we will not draw this type of connection
	if (neighbours && !endUpDown && !startUpDown && end) {
		delete ar;
		connection->arrowObject = NULL;
		return;
	}

	if(neighbours && startUpDown && endUpDown){
		ar->setArrowType(noarrow);
	}
	else
		if (!end) {
			ar->setArrowType(unidirectional);
			//ar->setArrowType(onewayneighbour);
			ar->setEndType(down);
			xb+=MAPROOMSIZE/2;
			yb+=MAPROOMSIZE/2;
		} else {
			ar->setArrowType(bidirectional);
		}

		if(!end && neighbours && !startUpDown && !endUpDown){
			if (connection->room1->n == connection) {
				ar->setStartType(north);
				ar->setEndType(south);

				xa=r1x+MAPROOMSIZE/2+8;
				ya=r1y+10;
				xb=r2x+MAPROOMSIZE/2+8;
				yb=r2y+MAPROOMSIZE-10;

			}
			if (connection->room1->s == connection) {
				ar->setStartType(south);
				ar->setEndType(north);
				xa=r1x+MAPROOMSIZE/2-8;
				ya=r1y+MAPROOMSIZE-10;
				xb=+r2x+MAPROOMSIZE/2-8;
				yb=r2y+10;
			}
			if (connection->room1->e == connection) {
				ar->setStartType(east);
				ar->setEndType(west);
				xa=r1x+MAPROOMSIZE-10;
				ya=r1y+MAPROOMSIZE/2+8;
				yb=r2y+MAPROOMSIZE/2+8;
				xb=r2x+10;
			}
			if (connection->room1->w == connection) {
				ar->setStartType(west);
				ar->setEndType(east);
				ya=r1y+MAPROOMSIZE/2-8;
				xa=r1x+10;
				xb=r2x+MAPROOMSIZE-10;
				yb=r2y+MAPROOMSIZE/2-8;
			}
		}


		ar->setPoints((unsigned)xa,(unsigned)ya,(unsigned)xb,(unsigned)yb);
		ar->show();
}


Room* MapCanvas::getRoom(unsigned x, unsigned y){

	QCanvasItemList l = collisions ( QPoint (x,y) );
	for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
		if ( (*it)->rtti() == CanvasRoom::RTTI() ) 
			return ((CanvasRoom*)(*it))->getRoom();
	}    
	return NULL;
}


void MapCanvas::draw_room(Room *room) {

	unsigned int index;
	if (room->onScreen)
		return;

	QCanvasLine *line;
	QPen pen;

	if(! room->roomUpdated && Config().m_showUpdated) {
		putImage(false, room, roomUpdateIcon, 10.0);
	}

	// draw basic room
	{
		QImage img;

		switch (room->roomTerrain) {
		case TINDOORS:
			img = roomTerrainTypeIcons.at(0);
			break;
		case TCITY:
			img = roomTerrainTypeIcons.at(1);
			break;
		case TFIELD:
			img = roomTerrainTypeIcons.at(2);
			break;
		case TFOREST:
			img = roomTerrainTypeIcons.at(3);
			break;
		case THILLS:
			img = roomTerrainTypeIcons.at(4);
			break;
		case TMOUNTAINS:
			img = roomTerrainTypeIcons.at(5);
			break;
		case TSHALLOW:
			img = roomTerrainTypeIcons.at(6);
			break;
		case TWATER:
			/*index = 0;
			if (room->n && (n = ISCONNECTEDTO(room,room->n))->roomTerrain == TWATER) SET(index,bit0);
			if (room->s && (s = ISCONNECTEDTO(room,room->s))->roomTerrain == TWATER) SET(index,bit1);
			if (room->e && (e = ISCONNECTEDTO(room,room->e))->roomTerrain == TWATER) SET(index,bit2);
			if (room->w && (w = ISCONNECTEDTO(room,room->w))->roomTerrain == TWATER) SET(index,bit3);

			img = roomRiverTypeIcons.at(index);
			index = 0;

			if ((n && n->w && ISCONNECTEDTO(n,n->w)->roomTerrain == TWATER)
			|| (w && w->n && ISCONNECTEDTO(w,w->n)->roomTerrain == TWATER)) index++;

			if ((n && n->e && ISCONNECTEDTO(n,n->e)->roomTerrain == TWATER)
			|| (e && e->n && ISCONNECTEDTO(e,e->n)->roomTerrain == TWATER)) index++;

			if ((s && s->w && ISCONNECTEDTO(s,s->w)->roomTerrain == TWATER) 
			|| (w && w->s && ISCONNECTEDTO(w,w->s)->roomTerrain == TWATER)) index++;

			if ((s && s->e && ISCONNECTEDTO(s,s->e)->roomTerrain == TWATER) 
			|| (e && e->s && ISCONNECTEDTO(e,e->s)->roomTerrain == TWATER)) index++;


			if (index>2)*/ img = roomTerrainTypeIcons.at(7);
			break;
		case TRAPIDS:
			img = roomTerrainTypeIcons.at(8);
			break;
		case TUNDERWATER:
			img = roomTerrainTypeIcons.at(9);
			break;
		case TROAD:
			index = 0;
			if (ISSET(room->roomFlags,FLAGS_VALID)){
				if (ISSET(room->roomFlags,ROAD_N)) SET(index,bit0);
				if (ISSET(room->roomFlags,ROAD_S)) SET(index,bit1);
				if (ISSET(room->roomFlags,ROAD_E)) SET(index,bit2);
				if (ISSET(room->roomFlags,ROAD_W)) SET(index,bit3);
			} else {
				if (room->n && ISCONNECTEDTO(room,room->n)->roomTerrain == TROAD) SET(index,bit0);
				if (room->s && ISCONNECTEDTO(room,room->s)->roomTerrain == TROAD) SET(index,bit1);
				if (room->e && ISCONNECTEDTO(room,room->e)->roomTerrain == TROAD) SET(index,bit2);
				if (room->w && ISCONNECTEDTO(room,room->w)->roomTerrain == TROAD) SET(index,bit3);
			}
			//	if (index==0) img = roomTerrainTypeIcons.at(10);

			img = roomRoadTypeIcons.at(index); 
			break;
		case TBRUSH:
			img = roomTerrainTypeIcons.at(11);
			break;
		case TTUNNEL:
			img = roomTerrainTypeIcons.at(12);
			break;
		case TCAVERN:
			img = roomTerrainTypeIcons.at(13);
			break;
		case TDEATHTRAP:
			img = roomTerrainTypeIcons.at(14);
			break;
		case TRANDOM:
			img = roomTerrainTypeIcons.at(15);
			break;
		default:
			;
		}
		putImage(true, room, img, 1.0);
	}

	// Draw mob
	// aggressive, peaceful, quest, shop, rent, guild, no mob
	{
		QImage img;

		switch (room->roomMob) 
		{
		case MAGGRESIVE:
			img = roomMobTypeIcons.at(0);
			break;
		case MPEACEFULL:
			img = roomMobTypeIcons.at(1);
			break;
		case MQUEST    :
			img = roomMobTypeIcons.at(2);
			break;
		case MSHOP     :
			img = roomMobTypeIcons.at(3);
			break;
		case MRENT     :
			img = roomMobTypeIcons.at(4);
			break;
		case MGUILD    :
			img = roomMobTypeIcons.at(5);
			break;
		default:
			;
		}
		putImage(false, room, img, 2.0);
	}

	// Draw load
	// herb, treasure, key, no load
	{
		QImage img;

		switch (room->roomLoad) 
		{
		case LHERB     :
			img = roomLoadTypeIcons.at(0);
			break;
		case LTREASURE :
			img = roomLoadTypeIcons.at(1);
			break;
		case LKEY      :
			img = roomLoadTypeIcons.at(2);
			break;
		case LWATER      :
			img = roomLoadTypeIcons.at(3);
			break;
		case LFOOD      :
			img = roomLoadTypeIcons.at(4);
			break;
		case LHORSE      :
			img = roomLoadTypeIcons.at(5);
			break;
		case LWARG      :
			img = roomLoadTypeIcons.at(6);
			break;
		case LTOWER      :
			img = roomLoadTypeIcons.at(7);
			break;
		case LATTENTION      :
			img = roomLoadTypeIcons.at(8);
			break;
		case LBOAT      :
			img = roomLoadTypeIcons.at(9);
			break;
		default:
			;
		}
		putImage(false, room, img, 3.0);
	}


	// Draw exits
	// WALLNORTH, WALLSOUTH, WALLEAST, WALLWEST, WALLUP, WALLDOWN,      -walls
	// DOORNORTH, DOORSOUTH, DOOREAST, DOORWEST, DOORUP, DOORDOWN       -doors

	// Connection types:
	// loop, oneway, twoway, onewaydoor, twowaydoor, onewayclimb, twowayclimb, special, randomexit, // noconnection

	index = 0 | 0xFFF; // fill lower 12 bits with 1


	if (room->n) {
		if (ISSET(room->n->type,CONNECTION_DOOR))
			UNSET(index,WALLNORTH);
		else {
			UNSET(index ,WALLNORTH);
			UNSET(index ,DOORNORTH);
		}
	} else
		if (ISSET(room->roomFlags,FLAGS_VALID) && ISSET(room->roomFlags,DOOR_N)) 
			UNSET(index ,WALLNORTH); else UNSET(index ,DOORNORTH);	   

	if (room->s) {
		if (ISSET(room->s->type,CONNECTION_DOOR))
			UNSET(index,WALLSOUTH);
		else {
			UNSET(index ,WALLSOUTH);
			UNSET(index ,DOORSOUTH);
		}
	} else
		if (ISSET(room->roomFlags,FLAGS_VALID) && ISSET(room->roomFlags,DOOR_S)) 
			UNSET(index ,WALLSOUTH); else UNSET(index ,DOORSOUTH);	   

	if (room->e) {
		if (ISSET(room->e->type,CONNECTION_DOOR))
			UNSET(index,WALLEAST);
		else  {
			UNSET(index ,WALLEAST);
			UNSET(index, DOOREAST);
		}
	} else
		if (ISSET(room->roomFlags,FLAGS_VALID) && ISSET(room->roomFlags,DOOR_E)) 
			UNSET(index ,WALLEAST); else UNSET(index ,DOOREAST);	   

	if (room->w) {
		if (ISSET(room->w->type,CONNECTION_DOOR))
			UNSET(index,WALLWEST);
		else  {
			UNSET(index ,WALLWEST);
			UNSET(index, DOORWEST);
		}
	} else
		if (ISSET(room->roomFlags,FLAGS_VALID) && ISSET(room->roomFlags,DOOR_W)) 
			UNSET(index ,WALLWEST); else UNSET(index ,DOORWEST);	   

	if (room->u) {
		if (ISNOTSET(room->u->type,CONNECTION_DOOR)) {
			UNSET(index, DOORUP);
			UNSET(index, WALLUP);
		}
	} else
		if (ISSET(room->roomFlags,FLAGS_VALID) && ISSET(room->roomFlags,DOOR_U)) 
			UNSET(index ,WALLUP); else UNSET(index ,DOORUP);	   

	if (room->d) {
		if (ISNOTSET(room->d->type,CONNECTION_DOOR)) {
			UNSET(index, DOORDOWN);
			UNSET(index, WALLDOWN);
		}
	} else
		if (ISSET(room->roomFlags,FLAGS_VALID) && ISSET(room->roomFlags,DOOR_D)) 
			UNSET(index ,WALLDOWN); else UNSET(index ,DOORDOWN);	   


	// DRAW OF CLIMB EXITS
	const int x = room->mapx*MAPROOMSIZE;
	const int y = room->mapy*MAPROOMSIZE;

	pen.setWidth(2);
	pen.setStyle(Qt::DashLine);
	pen.setColor(QColor(128,128,128));

	if (room->n && ISSET(room->n->type,CONNECTION_CLIMB)) {
		line = new QCanvasLine(this);
		room->roomLineObjects.append(line);
		line->setPoints(x+2,y+2,x+MAPROOMSIZE-2,y+2);
		line->setPen(pen);
		line->setZ(4);
		line->show();
	}

	if (room->s && ISSET(room->s->type,CONNECTION_CLIMB)) {
		line = new QCanvasLine(this);
		room->roomLineObjects.append(line);
		line->setPoints(x+2,y+MAPROOMSIZE-2,x+MAPROOMSIZE-2,y+MAPROOMSIZE-2);
		line->setPen(pen);
		line->setZ(4);
		line->show();
	}

	if (room->w && ISSET(room->w->type,CONNECTION_CLIMB)) {
		line = new QCanvasLine(this);
		room->roomLineObjects.append(line);
		line->setPoints(x+2,y+2,x+2,y+MAPROOMSIZE-2);
		line->setPen(pen);
		line->setZ(4);
		line->show();
	}

	if (room->e && ISSET(room->e->type,CONNECTION_CLIMB)) {
		line = new QCanvasLine(this);
		room->roomLineObjects.append(line);
		line->setPoints(x+MAPROOMSIZE-2,y+2,x+MAPROOMSIZE-2,y+MAPROOMSIZE-2);
		line->setPen(pen);
		line->setZ(4);
		line->show();
	}


	// DRAW NOT MAPPED EXITS
	if ( Config().m_drawNotMappedExits && ISSET(room->roomFlags, FLAGS_VALID))
	{
		pen.setWidth(2);
		pen.setStyle(Qt::DotLine);
		pen.setColor(QColor(255,128,0));

		if (ISSET(room->roomFlags, EXIT_N) && !room->n) {
			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+2,y+2,x+MAPROOMSIZE-2,y+2);
			line->setPen(pen);
			line->setZ(5);
			line->show();
		}

		if (ISSET(room->roomFlags, EXIT_S) && !room->s) {
			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+2,y+MAPROOMSIZE-2,x+MAPROOMSIZE-2,y+MAPROOMSIZE-2);
			line->setPen(pen);
			line->setZ(5);
			line->show();
		}

		if (ISSET(room->roomFlags, EXIT_W) && !room->w) {
			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+2,y+2,x+2,y+MAPROOMSIZE-2);
			line->setPen(pen);
			line->setZ(5);
			line->show();
		}

		if (ISSET(room->roomFlags, EXIT_E) && !room->e) {
			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+MAPROOMSIZE-2,y+2,x+MAPROOMSIZE-2,y+MAPROOMSIZE-2);
			line->setPen(pen);
			line->setZ(5);
			line->show();
		}

		if (ISSET(room->roomFlags, EXIT_U) && !room->u) {
			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+MAPROOMSIZE-17,y+15,x+MAPROOMSIZE-7,y+5);
			line->setPen(pen);
			line->setZ(5);	     
			line->show();

			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+MAPROOMSIZE-7,y+15,x+MAPROOMSIZE-17,y+5);
			line->setPen(pen);
			line->setZ(5);	     
			line->show();
		}

		if (ISSET(room->roomFlags, EXIT_D) && !room->d) {
			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+7,y+MAPROOMSIZE-9,x+17,y+MAPROOMSIZE-19);
			line->setPen(pen);
			line->setZ(5);
			line->show();

			line = new QCanvasLine(this);
			room->roomLineObjects.append(line);
			line->setPoints(x+17,y+MAPROOMSIZE-9,x+7,y+MAPROOMSIZE-19);
			line->setPen(pen);
			line->setZ(5);
			line->show();
		}

	}

	{
		putImage(false, room, roomExitTypeIcons.at(index), 4.0);
	}

	room->onScreen = TRUE;

	// Draw connections of room
	if (room->u && !room->u->onScreen)
		draw_connection(room->u);
	if (room->d && !room->d->onScreen)
		draw_connection(room->d);
	if (room->e && !room->e->onScreen)
		draw_connection(room->e);
	if (room->w && !room->w->onScreen)
		draw_connection(room->w);
	if (room->n && !room->n->onScreen)
		draw_connection(room->n);
	if (room->s && !room->s->onScreen)
		draw_connection(room->s);
}

CanvasImage *MapCanvas::putImage(const bool putRoom, Room* room, const QImage&img, const double zCoord)
{
	CanvasImage *pRetVal = NULL;
	if (room && (! img.isNull()))
	{
		pRetVal = (putRoom) ? new CanvasRoom(room, img, this) : new CanvasImage(img, this);
		room->roomObjects.append(pRetVal);
		pRetVal->move(room->mapx*MAPROOMSIZE, room->mapy*MAPROOMSIZE);
		pRetVal->setZ(zCoord);
		pRetVal->show();
	}

	return pRetVal;
}

void MapCanvas::updateRoom(Room* r) {

	if (!r) {update();return;}

	while	(!r->roomLineObjects.isEmpty()) 
		delete r->roomLineObjects.take();

	while       (!r->roomObjects.isEmpty()) 
		delete r->roomObjects.take();

	r->onScreen = FALSE;
	if (r->u && r->u->arrowObject ) {
		delete r->u->arrowObject;
		r->u->onScreen = FALSE;
	}

	if (r->s && r->s->arrowObject ) {
		delete r->s->arrowObject;
		r->s->onScreen = FALSE;
	}

	if (r->e && r->e->arrowObject ) {
		delete r->e->arrowObject;
		r->e->onScreen = FALSE;
	}

	if (r->w && r->w->arrowObject ) {
		delete r->w->arrowObject;
		r->w->onScreen = FALSE;
	}
	draw_room(r);
	update();
}


void MapCanvas::updateRoomAndNeighbours(Room* r) {
	updateRoom(r);
	if (r->n) updateRoom(ISCONNECTEDTO(r,r->n));
	if (r->s) updateRoom(ISCONNECTEDTO(r,r->s));
	if (r->e) updateRoom(ISCONNECTEDTO(r,r->e));
	if (r->w) updateRoom(ISCONNECTEDTO(r,r->w));
}

void MapCanvas::updateWorld() {

	for ( Connection *c = Data().connections.first(); c; c = Data().connections.next() ){
		if (c->arrowObject) 
			delete c->arrowObject;
		c->onScreen = FALSE;
	}

	for ( Room *r = Data().rooms.first(); r; r = Data().rooms.next() ){    
		while	(!r->roomLineObjects.isEmpty())
			delete r->roomLineObjects.take();

		while	(!r->roomObjects.isEmpty())
			delete r->roomObjects.take();

		r->onScreen = FALSE;
	}

	for ( Room *r = Data().rooms.first(); r; r = Data().rooms.next() )
		draw_room(r);

	update();
}

void MapCanvas::drawInfoObject(InfoItem* i) {
	if (!i || i->onScreen)
		return;

	QRect r;
	QBrush brush;
	QCanvasLine *l;
	QCanvasText *t;
	MyCanvasArrow *a;

	i->onScreen = TRUE;
	switch (i->itemType) {
	case text:
		i->item = new QCanvasText(i->itemText, this);
		t=(QCanvasText*)i->item;
		t->move(i->x1,i->y1);
		t->setZ(201);
		t->setColor(QColor(255,255,255));
		i->m_rectangleObject = new QCanvasRectangle(this);
		r = t->boundingRect();
		i->m_rectangleObject->setSize(r.width()+4, r.height()+4);
		i->m_rectangleObject->move(i->x1-2,i->y1-2);    
		i->m_rectangleObject->setPen(QPen(QColor(180,180,128)));    
		brush.setColor(QColor(20,20,20));
		brush.setStyle(Qt::Dense4Pattern);
		i->m_rectangleObject->setBrush(brush); 
		i->m_rectangleObject->setZ(200);
		i->m_rectangleObject->show();
		break;
	case line:
		i->item = new QCanvasLine (this);
		l=(QCanvasLine*)i->item;
		l->setPoints(i->x1,i->y1,i->x2,i->y2);
		l->setZ(200);
		break;
	case arrow:
		i->item = new MyCanvasArrow (this);
		a=(MyCanvasArrow*)i->item;
		a->setPoints(i->x1,i->y1,i->x2,i->y2);
		a->setArrowType(unidirectional);
		a->setStartType(east);
		a->setEndType(west);
		a->setZ(200);
		break;
	}
	i->item->show();
}

void MapCanvas::updateInfoObject(InfoItem* i) {
	if (!i) {
		update();
		return;
	}
	i->onScreen=FALSE;

	if (i->item)
		switch (i->itemType) {
		case text:
			delete (QCanvasText *) (i->item);
			break;
		case line:
			delete (QCanvasLine *) (i->item);
			break;
		case arrow:
			delete (MyCanvasArrow *) (i->item);
			break;
		}
		drawInfoObject(i);
		update();
		//    m_mapdata->dataChanged=TRUE;
}

