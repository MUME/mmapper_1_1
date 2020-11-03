/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MAPDATA_H
#define MAPDATA_H

#include <qobject.h>
#include "defs.h"
#include <qptrlist.h>
#include <qcanvas.h>
#include "commandeval.h"
#include "CanvasImage.h"

// other objects on map (without direct connection to rooms or connections)
enum ItemType { text, line, arrow };


/**
 * @short  This class represents info item on map.
 *
 * Info items can be three different types - text, arrow, line. Items are created on map using
 * RoomEditDialog (in edit mode).
 * @see RoomEditDialog
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class InfoItem {
public:
    InfoItem();
    ~InfoItem(){};


    Q_UINT32 x1,y1,x2,y2;
    bool onScreen;

    ItemType itemType;
    QString itemText;
    QString itemName;

    class QCanvasItem *item;
    
    QCanvasRectangle *m_rectangleObject;
};


/**
 * @short  This class represents connection betwen two rooms (arrow on map).
 *
 * There are different types of connections between rooms on map 
 * (oneway, twoway, withdoor, climb ...).
 * In case of connection with doors, there are different door-type attributes available
 * (normal, hidden, need key ....) 
 * @see RoomEditDialog
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class Connection {

public:
    Connection(){onScreen = FALSE;
	         arrowObject=NULL;
                 doorType1=DNORMAL;
                 doorType2=DNORMAL;
                 doorName1="";
                 doorName2="";
                 type=CONNECTION_NORMAL;
		 room1=NULL;
		 room2=NULL;
	        };

    ~Connection(){};

    ConnectionID connectionID;

    class Room *room1;
    class Room *room2;

    //types of connections
    ConnectionType type;

    //doors (in case of connection type with doors)
    DoorType doorType1;
    DoorType doorType2;
    DoorName doorName1;
    DoorName doorName2;

    //used to issue if connection was already created on map or no
    bool onScreen; // TRUE/FALSE
    class MyCanvasArrow *arrowObject; // Graphic representation of connection
};

/**
 * @short  This class represents room on map.
 *
 * Rooms connected together with Connection class make MUD world map.
 * There are lots of different attributes for rooms. 
 * Room has it's own terrain type (field, forest, water, city ...), 
 * mobtype (agressive, peacefull, shop, rent ...), 
 * loadtype (treasure, key, herb ...) and many other.
 * @see RoomEditDialog
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class Room {

public:
    Room();

    Room(RoomName name);

    ~Room();

    void clearRoom();
    void setRoomName(RoomName name){this->roomName = name;};

    RoomName getRoomName() {return roomName;};

public:

    //ID of the room
    RoomID roomID;

    //ID to other rooms in mud
    Connection *u;
    Connection *d;
    Connection *w;
    Connection *e;
    Connection *n;
    Connection *s;

    //name of the room
    RoomName roomName;

    //description of the room
    RoomDescription roomDesc;
    //RoomDescription parsedRoomDesc;
    
    Q_UINT32 roomFlags;
    bool roomUpdated;
    bool roomCheckExits;

    //type of room
    RoomTerrainType  roomTerrain;
    RoomMobType      roomMob;
    RoomLoadType     roomLoad;

    RoomLocationType roomLocation;
    RoomPortableType roomPortable;
    RoomLightType    roomLight;
    RoomAlignType    roomAlign;

////////////////////////////
// Map related structures

    //coordinates on the map
    Q_UINT32 mapx, mapy;

    bool onScreen; // TRUE/FALSE
    QPtrList<CanvasImage> roomObjects;
    QPtrList<QCanvasLine> roomLineObjects;

protected:

};

/**
 * @short  This class represents whole world map.
 *
 * Rooms connected together with Connection class make MUD world map.
 * This class works with whole world map. It can store map data, load them into memory,
 * and make operations with parts of world (create, delete, move, connect, disconnect ...).
 * Also this class is used to search rooms in whole world map.
 * @see Room, Connection, InfoItem
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MapData : public QObject {
    Q_OBJECT
public:

    bool dataChanged() const { return m_dataChanged; };
   
   // room description functions
    void updateRoom(Room* dr, const QString&, const QString&, Q_UINT32, Q_UINT32);
    void resetRoom(Room*); 

    // create data objects
    Room* createNewRoom(unsigned int x, unsigned int y);
    Room* createNewRoom(Room *sr, DirectionType dir);
    Room* createAndConnectNewRoom(unsigned int x, unsigned int y);
    Connection* createNewConnection();
    Connection *connectRooms(Room*,Room*, DirectionType, Q_UINT32);
    InfoItem* createNewInfoObject(QString name);
    InfoItem *getInfoItem(QString);
    
    // functions for use with lists
    bool addRoom(Room *room);
    bool removeRoom(Room *room);
    bool addInfoObject(InfoItem* item);
    bool removeInfoObject(InfoItem* item);
    bool addConnection(Connection *connection);
    bool removeConnection(Connection *connection);
    
    // searching functions    
    Room* findRoomAt(unsigned int x, unsigned int y);
    Room* findRoom(const QString&, const QString&, Q_UINT32, Q_UINT32, bool*);
    
    // compare function
    bool compareRooms(Room*, const QString&, const QString&, Q_UINT32, Q_UINT32, bool debug = FALSE);

    // load, save, delete data
    void deleteData();
    bool loadData(const QString& filename);
    bool saveData(const QString& filename);
    
    // room Descriptions (re)filter
    //bool reparseRoomDesc();

    //move rooms    
    void moveNorth();
    void moveSouth();
    void moveEast();
    void moveWest();
    
    // operations on current room selected
    void removeCurrentRoom();
    
    // Selected room and room2 functions and data types
    Room *getCurrentRoom() {if (selected) return currentRoom; else return NULL;};
    void setCurrentRoom(Room* r) {if (r){ selected = TRUE; currentRoom = r; rX=r->mapx; rY=r->mapy; }};
    void setCurrentRoom(unsigned int x, unsigned int y){selected = TRUE; rX = x; rY=y; currentRoom=findRoomAt(x,y);};
    bool isSelectedRoom(){return selected;};
    Room *getSelectedRoom2() {if (selected2) return selectedRoom2; else return NULL;};
    void setSelectedRoom2(Room* r) {selected2 = TRUE; selectedRoom2 = r; if (r){rX2=r->mapx; rY2=r->mapy;} else {rX2=0; rY2=0;}};
    void setSelectedRoom2(unsigned int x, unsigned int y){selected2 = TRUE; rX2 = x; rY2=y; selectedRoom2=findRoomAt(x,y); };
    bool isSelectedRoom2(){return selected2;};

    int getSelectedRoom1x(){return rX;};
    int getSelectedRoom1y(){return rY;};
    int getSelectedRoom2x(){return rX2;};
    int getSelectedRoom2y(){return rY2;};
    
    void setFileName(QString name){fileName=name;};
    QString getFileName(){return fileName;};

	virtual ~MapData();
    
signals:
    void logText( const QString& );
    void logDebug( const QString& );
    
    void roomChanged(Room*);
    void infoObjectChanged(InfoItem*);
    void selectRoom(Room*);
    void selectRoom2(Room*);
    void selectRoom(unsigned, unsigned);
    
protected:
    // data
    QPtrList<InfoItem> infoItems;
    QPtrList<Room> rooms;
    QPtrList<Connection> connections;

    QPtrList<QCanvasItem> items;

    // changed data?
    bool m_dataChanged;

    QString fileName;

    //selected rooms 1 and 2
    Room *currentRoom;
    bool selected;
    Room *selectedRoom2;
    bool selected2;
    //selected room coordinates
    unsigned int rX,rY; 
    unsigned int rX2, rY2;

    
private:
    MapData(QObject *parent = 0, const char *name = 0);

    friend MapData& Data();
    friend class RoomEditDialogImpl;
    friend class MapCanvas;
};

/// Returns a reference to the application configuration object.
MapData& Data();


#endif
