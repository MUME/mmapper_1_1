/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MAPCANVAS_H
#define MAPCANVAS_H

#include <qcanvas.h>
#include <qpainter.h>
#include <qvaluevector.h>

#include "defs.h"
#include "mapdata.h"
#include "CanvasImage.h"

/**
 * @short This class is used to draw move path cached in MMapper buffer.
 * 
 * This class draws moves, which are not yet executed by MUME server, but are in input
 * buffer of CommandEval class. This allows user to see on map, to where his char is moving.
 * @see CommandEval, Room, Canvas
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MyCanvasPathMark : public QCanvasLine {
public:
    MyCanvasPathMark( QCanvas * canvas );
    
    int rtti() const;
    static int RTTI;
};

/**
 * @short This class is used to draw room sprite on canvas.
 * 
 * Because of speed search of rooms on map by their coordinates, this sprite object 
 * contains also pointer to Room object it is representating on screen. When user clicks 
 * on this graphic object by mouse, we have fast relation betwen graphic representation 
 * of Room and Room data.
 * @see MapCanvas, Room
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class CanvasRoom : public CanvasImage {
public:
	static int RTTI();
    
	CanvasRoom(Room* room,  const QImage& a, QCanvas * canvas);
    
    virtual int rtti() const;
    
    Room* getRoom(){return r;};
    void setRoom(Room* room){r = room;};
    
protected:
    Room* r;
};


enum StartEndType {up,down,west,east,north,south};
enum ArrowTypes    {noarrow,unidirectional,bidirectional,onewayneighbour};


/**
 * @short This class represents an arrow object on map.
 * 
 * MyCanvasArrow object is used for example to display connections between rooms, or 
 * to display arrow info items on map.
 * @see MapCanvas, Room, Connection
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MyCanvasArrow : public QCanvasPolygonalItem {

public:
    MyCanvasArrow(QCanvas *canvas);
    ~MyCanvasArrow();

    void setPoints(int x1, int y1, int x2, int y2);
    QPoint startPoint() const { return QPoint(x1,y1); }
    QPoint endPoint() const { return QPoint(x2,y2); }

    int rtti() const;
    static int RTTI;

    void setPen(QPen p);
    void setArrowType(ArrowTypes at) {arrowType = at;};
    void setArrowSize(int size) {arrowSize = size;};
    void setStartType(StartEndType se) {startType = se;};
    void setEndType(StartEndType se) {endType = se;};

protected:

    void drawShape(QPainter &);
    QPointArray areaPoints() const;

private:
    int x1,y1,x2,y2;
    int arrowSize;
    StartEndType startType;
    StartEndType endType;
    ArrowTypes arrowType;
};

/**
 * @short This class serves as graphic canvas for MUME map.
 * 
 * Here all graphic items (pixmaps) are cached into canvas. To view 
 * map on screen, class MapCanvasView is used. This follows document/view schema.
 *
 * @see MapCanvasView
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MapCanvas : public QCanvas {
    Q_OBJECT
public:

    void activate_room(int x, int y);

    void draw_room(Room *room);
    void drawInfoObject(InfoItem* i);
    void draw_connection(Connection *connection);
    
    Room* getRoom(unsigned x, unsigned y);

	~MapCanvas();

public slots:
    void updateRoom(Room*);
    void updateRoomAndNeighbours(Room* r);
    void updateWorld();
    void updateInfoObject(InfoItem*);
    
    void showPath(QPtrQueue<Command> queue);
    
    void showGrid(bool);

protected:
	typedef QValueVector<QImage> tImageVector;

    QImage *joinImages(QImage *dest, const QImage& src);

    tImageVector roomTerrainTypeIcons;
    tImageVector roomRoadTypeIcons;
    tImageVector roomRiverTypeIcons;
    tImageVector roomExitTypeIcons;
    tImageVector roomMobTypeIcons;
    tImageVector roomLoadTypeIcons;
	tImageVector roomExitTypeImages;
    QImage roomUpdateIcon;

    QPtrQueue<MyCanvasPathMark> m_pathMarksQueue;
    
private:
    
    MapCanvas(int w, int h );
	
	/// reads sequence of PNG pictures into vector vec
	/// for example readImageSequence("road", 4, v); reads files road0.png, road1.png,...road3.png
	void readImageSequence(const QString& imageFilePrefix, const int upperBound, tImageVector& vec);

	/// put CanvasImage into canvas
	///\param putRoom if true, CanvasRoom item is inserted, otherwise CanvasImage
	///\param room room to which is image related
	///\param not empty item image
	///\param zCoord Z-order of the item
	///\return pointer to the inserted canvas image or NULL, if image was not inserted (empty image, NULL room pointer)
	CanvasImage *putImage(const bool putRoom, Room* room, const QImage&img, const double zCoord);

    friend MapCanvas& Canvas();
    
};

/// Returns a reference to the application configuration object.
MapCanvas& Canvas();

#endif
