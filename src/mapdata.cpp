/***************************************************************************
*   Copyright (C) 2003 by Marek Krejza                                    *
*   mkrejza@cbox.cz                                                       *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "mapdata.h"
#include "mapcanvas.h"
#include "pref.h"
#include <qfile.h>
#include <qdatastream.h>
#include <qapplication.h>
#include <qbuffer.h>

#include "..\cryptoLib\gzip.h"
#include "..\cryptoLib\files.h"
#include "commandeval.h"
#include "mmapper.h"

//#include <kfilterdev.h>


MapData& Data() {
	static MapData data;
	return data;
};


InfoItem::InfoItem()
{
	onScreen = FALSE;
	item=NULL;
	itemText="";
	itemName="";
	itemType=line;
	x1=0;y1=0;x2=0;y2=0;
	m_rectangleObject = NULL;
}



Room::Room(){
	clearRoom();
}

Room::Room(QString name){
	clearRoom();
	setRoomName(name);
}

Room::~Room(){
	CanvasImage *spr;
	while (! roomObjects.isEmpty()) {
		spr = roomObjects.take();
		delete spr;
	}
}


void Room::clearRoom(){

	d = NULL;
	u = NULL;
	w = NULL;
	e = NULL;
	n = NULL;
	s = NULL;

	//name of the room
	roomName="";
	roomDesc="";
	//parsedRoomDesc="";

	roomFlags = 0;
	roomUpdated = FALSE;
	roomCheckExits = TRUE;

	roomTerrain=TFIELD;
	roomMob=MNOMOB;
	roomLoad=LNOLOAD;
	roomLocation=OUTSIDE;
	roomPortable=PORT;
	roomLight=DARK;
	roomAlign=NEUTRAL;

	mapx=0;
	mapy=0;

	roomObjects.setAutoDelete(TRUE);
	roomLineObjects.setAutoDelete(TRUE);
	onScreen = FALSE;
}


MapData::MapData(QObject *parent, const char *name)
: QObject(parent, name) {
	fileName = "";
	m_dataChanged=FALSE;
	rX=0;rY=0;
	rX2=0;rY2=0;
	currentRoom = NULL;
	selectedRoom2 = NULL;
	selected=FALSE;
	selected2=FALSE;
}


MapData::~MapData() {
}



InfoItem* MapData::createNewInfoObject(QString name){
	InfoItem *i=new InfoItem();
	i->itemName=name;
	addInfoObject(i);
	return i;
}


Connection* MapData::createNewConnection(){
	Connection *c=new Connection();
	addConnection(c);
	m_dataChanged = TRUE;  
	return c;
}

Room* MapData::createNewRoom(unsigned int x, unsigned int y){
	Room *r;  
	if (!findRoomAt(x,y)){
		r = new Room();
		r->mapx=x;
		r->mapy=y;
		addRoom(r);
		m_dataChanged = TRUE;
		return r;
	}    
	return NULL;
}


Room *MapData::createNewRoom(Room *sr, DirectionType dir) {
	Room *dr = NULL;

	if ( sr ) {
		switch (dir) {

		case dir_north:
			if (  (dr = createNewRoom(sr->mapx,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx,sr->mapy-2))
				||(dr = createNewRoom(sr->mapx,sr->mapy-3))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy-2))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy-3))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy-2))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy-3)) ) {
				}
				break;

		case dir_south:
			if (  (dr = createNewRoom(sr->mapx,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx,sr->mapy+2))
				||(dr = createNewRoom(sr->mapx,sr->mapy+3))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy+2))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy+3))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy+2))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy+3)) ) {
				}
				break;

		case dir_east:
			if (  (dr = createNewRoom(sr->mapx+1,sr->mapy))
				||(dr = createNewRoom(sr->mapx+2,sr->mapy))
				||(dr = createNewRoom(sr->mapx+3,sr->mapy))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx+2,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx+3,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx+2,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx+3,sr->mapy+1)) ) {
				}
				break;

		case dir_west:
			if (  (dr = createNewRoom(sr->mapx-1,sr->mapy))
				||(dr = createNewRoom(sr->mapx-2,sr->mapy))
				||(dr = createNewRoom(sr->mapx-3,sr->mapy))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx-2,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx-3,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx-2,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx-3,sr->mapy+1)) ) {
				}
				break;

		case dir_up:
			if (  (dr = createNewRoom(sr->mapx-1,sr->mapy))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy))
				||(dr = createNewRoom(sr->mapx,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy+1)) ) {
				}
				break;

		case dir_down:
			if (  (dr = createNewRoom(sr->mapx+1,sr->mapy))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy))
				||(dr = createNewRoom(sr->mapx,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy-1))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx-1,sr->mapy+1))
				||(dr = createNewRoom(sr->mapx+1,sr->mapy-1)) ) {
				}
				break;
		default:
			;
		}
	}
	return dr;
}


Room* MapData::createAndConnectNewRoom(unsigned int x, unsigned int y){
	Connection *c;
	Room *r;
	Room *rt;

	if ((r = createNewRoom(x,y))){

		//add connections
		if((rt = findRoomAt(x,y-1)) && !rt->s) {
			c = new Connection();
			addConnection(c);
			r->n=c;
			rt->s=c;
			c->room1=r;
			c->room2=rt;
			emit roomChanged(rt);
		}
		if((rt = findRoomAt(x-1,y)) && !rt->e) {
			c = new Connection();
			addConnection(c);
			r->w=c;
			rt->e=c;
			c->room1=r;
			c->room2=rt;
			emit roomChanged(rt);
		}
		if((rt = findRoomAt(x+1,y)) && !rt->w) {
			c = new Connection();
			addConnection(c);
			r->e=c;
			rt->w=c;
			c->room1=r;
			c->room2=rt;
			emit roomChanged(rt);
		}
		if((rt = findRoomAt(x,y+1)) && !rt->n) {
			c = new Connection();
			addConnection(c);
			r->s=c;
			rt->n=c;
			c->room1=r;
			c->room2=rt;
			emit roomChanged(rt);
		}

		emit roomChanged(r);
		return r;
	}
	return NULL;
}


bool MapData::addInfoObject(InfoItem* item){
	InfoItem* i;

	for (i=infoItems.first(); i; i=infoItems.next()){
		if (i->itemName == item->itemName) return FALSE;
	}
	infoItems.append(item);
	m_dataChanged=TRUE;
	return TRUE;
}

bool MapData::removeInfoObject(InfoItem* item){
	InfoItem* i;

	for (i=infoItems.first(); i; i=infoItems.next()){
		if (i->itemName == item->itemName) {
			if (i->item)
				switch (i->itemType){
		case text:delete (QCanvasText *) (i->item);break;
		case line:delete (QCanvasLine *) (i->item);break;
		case arrow:delete (MyCanvasArrow *) (i->item);break;
				}
				infoItems.remove(i);
				m_dataChanged=TRUE;
				delete i;
				return TRUE;
		}
	}
	return FALSE;
}


bool MapData::addRoom(Room *room){
	if (rooms.find(room)<0) {
		rooms.append(room);
		m_dataChanged=TRUE;
		return TRUE;
	}
	return FALSE;
}

bool MapData::removeRoom(Room *room){
	if (!room) return FALSE;

	Connection* connection = connections.first();
	while (connection){
		if (connection->room1 == room || connection->room2 == room){
			removeConnection(connection);
			connection = connections.current();
		}
		else
			connection = connections.next();
	}

	rooms.remove(room);
	m_dataChanged=TRUE;
	delete room;
	return TRUE;
}

bool MapData::addConnection(Connection *connection){
	if (connections.find(connection)<0) {
		connections.append(connection);
		m_dataChanged=TRUE;
		return TRUE;
	}
	return FALSE;
}

bool MapData::removeConnection(Connection *connection){
	if (!connection) return FALSE;
	if (connection->room1 && connection->room1->u == connection) connection->room1->u = NULL;
	if (connection->room1 && connection->room1->d == connection) connection->room1->d = NULL;
	if (connection->room1 && connection->room1->e == connection) connection->room1->e = NULL;
	if (connection->room1 && connection->room1->w == connection) connection->room1->w = NULL;
	if (connection->room1 && connection->room1->n == connection) connection->room1->n = NULL;
	if (connection->room1 && connection->room1->s == connection) connection->room1->s = NULL;
	if (connection->room2 && connection->room2->u == connection) connection->room2->u = NULL;
	if (connection->room2 && connection->room2->d == connection) connection->room2->d = NULL;
	if (connection->room2 && connection->room2->e == connection) connection->room2->e = NULL;
	if (connection->room2 && connection->room2->w == connection) connection->room2->w = NULL;
	if (connection->room2 && connection->room2->n == connection) connection->room2->n = NULL;
	if (connection->room2 && connection->room2->s == connection) connection->room2->s = NULL;
	delete connection->arrowObject;
	m_dataChanged=TRUE;
	emit roomChanged( connection->room1 );
	emit roomChanged( connection->room2 );
	return connections.remove(connection);
	delete connection;

}

Room* MapData::findRoomAt(unsigned int x, unsigned int y){
	return Canvas().getRoom(ROOMTOVIEWPORT(x)+10, ROOMTOVIEWPORT(y)+10);
	/*
	Room *room;
	for ( room = rooms.first(); room; room = rooms.next() ){
	if ( (room->mapx == x) && (room->mapy == y) ) return room;
	}
	return NULL;*/

}


bool MapData::compareRooms(Room* r, const QString& name, const QString& parseddesc, Q_UINT32 roomFlags, Q_UINT32 promptFlags, bool debug){

	if ( !r->roomUpdated ) return FALSE;

	if ( r->roomCheckExits && (ISSET(roomFlags, FLAGS_VALID)) && 
		(r->roomFlags & 127) != (roomFlags & 127) ) {
		if (debug){
			emit logText( "[MMapper] Room exits differ!!! :" );  
		}
		return FALSE;
	}


	if (( (Q_UINT32)r->roomTerrain < 14 ) && (ISSET(promptFlags, PROMPT_INFO_VALID)) && 
		( (Q_UINT32)r->roomTerrain != (promptFlags & 15) )) {
		if (debug){
			emit logText( "[MMapper] Room terrain differ!!! :" );  
		}
		return FALSE;
	}

	if (r->roomName != name){
		if (debug){
			emit logText( "[MMapper] Room names differ!!! :" );  
			emit logText( QString("   ... [%1] [%2]").arg(name).arg(r->roomName) );
		}
		return FALSE;
	}

	if (Config().m_brief) return TRUE;

	if ((!r->roomDesc.startsWith(parseddesc))){
	const char* pd = parseddesc.ascii();
	const char* d = r->roomDesc.ascii();
	uint pdl = parseddesc.length();
	uint dl = r->roomDesc.length();
	
		if (debug){
			emit logText( "[MMapper] Room descriptions differ!!! :" );

			//qDebug("%s","\n******************************\n");
			//qDebug("%s","******** Parsed Room Desc ***********\n");
			//qDebug("%s",r->parsedRoomDesc.ascii());
			//qDebug("%s","******* Room Desc **********\n");
			//qDebug("%s",desc.ascii());
			//qDebug("%s","******************************\n\n");

			emit logDebug("n******************************\n");
			emit logDebug("******** Parsed Room Desc ***********\n");
			emit logDebug(parseddesc);
			emit logDebug("******* Room Desc **********\n");
			emit logDebug(r->roomDesc);
			emit logDebug("******************************\n\n");

			QString out = "";

			for ( uint i = 0; i < pdl; i++) {
				if ( ( i >= dl ) || pd[i] != d[i] ) out.append(pd[i]);      
				if ( pd[i]=='\n' && !out.isEmpty()){
					emit logText("   " + out); 
					out = "";
				}
			}
		}
	
	
	// compute number of changed words in string
	uint i;
	uint y = 0;
	uint error = 0;
	for ( i = 0; i < pdl; i++) {
	  if ( (y < dl) && (pd[i] == d[y]) ) {
	    y++;
	  }
	  else{
	    //wrong word
	    error++;
	    //jump to next word	    
	    while ( (i < pdl) && (pd[i]!=' ') ) i++;
	    while ( (y < dl) && (d[y]!=' ') ) y++;
	    y++;
	  }		
	}
	
	//if count of different words are max. 2, return that description match!
	if (error<3) {
	  emit logText( "[MMapper] ... probably some typo repaired, please update this room!!!" ); 
	  return TRUE;
	}
	
		return FALSE;  
	}

	return TRUE;
}

Room* MapData::findRoom(const QString& name, const QString& parseddesc, Q_UINT32 roomFlags, Q_UINT32 promptFlags, bool* multiple){
	bool found=FALSE;
	Room* foundRoom = NULL;
	for ( Room *room = rooms.first(); room; room = rooms.next() ){
		if ( compareRooms(room, name, parseddesc, roomFlags, promptFlags, FALSE) ) {
			if (found) {
				*multiple = TRUE;
				return foundRoom;
			}
			found = TRUE;
			foundRoom = room;
		}
	}

	if (found) return foundRoom;
	return NULL;    
}

/*
bool MapData::reparseRoomDesc(){
QString str;
bool b=FALSE;
Room *room;
for ( room = rooms.first(); room; room = rooms.next() ){
if (room->roomDesc != "") {
str = Parser().parseDesc(room->roomDesc);
if (str!=room->parsedRoomDesc) b=TRUE;
room->parsedRoomDesc=str;
}
}
return b;
}
*/

InfoItem *MapData::getInfoItem(QString str){
	InfoItem *item;
	for ( item = infoItems.first(); item; item = infoItems.next() ){
		if ( str == item->itemName ) return item;
	}
	return NULL;
}


void MapData::deleteData(){
	Room *room = NULL;
	Connection *connection = NULL;
	InfoItem *item = NULL;

	for(room = rooms.first(); room; room = rooms.current()) {
		room = rooms.take();
		delete room;
	}
	for(connection = connections.first(); connection; connection = connections.current()){
		connection = connections.take(); 
		delete connection->arrowObject;
		delete connection;
	}
	for(item = infoItems.first(); item; item = infoItems.current()){
		item = infoItems.take(); 
		if (item->item)
			switch (item->itemType){
			case text:delete (QCanvasText *) (item->item);break;
			case line:delete (QCanvasLine *) (item->item);break;
			case arrow:delete (MyCanvasArrow *) (item->item);break;
			}
			delete item;
	}    
	m_dataChanged=TRUE;
}


bool MapData::loadData(const QString& filename) {
	bool retVal = false;
	std::string stdString;
	CryptoPP::StringSink *pSink = new CryptoPP::StringSink(stdString);
	CryptoPP::Gunzip *pInflator = new CryptoPP::Gunzip(pSink);

	try 
	{
		CryptoPP::FileSource(filename, true, pInflator);
	}
	catch(CryptoPP::Exception &e)
	{
		QString msg("CryptoPP::Exception caught: %1");
		qDebug(msg.arg(e.what()));
		return false;
	}
	catch(std::exception &e)
	{
		QString msg("std::exception caught: %1");
		qDebug(msg.arg(e.what()));
		return false;
	}

	const long int stdStringLen = stdString.length();
	const char *stdStringData = stdString.data();
	QByteArray byteArray;

	// now we have to release the data with resetRawData
	byteArray.setRawData(stdStringData, stdStringLen);
	QBuffer buf(byteArray);

	if (buf.open(IO_ReadOnly)) {
		QDataStream stream (&buf);

		// Read the version and magic
		Q_UINT32 magic;

		stream >> magic;
		if ( magic == 0xFFB2AF01 ) {
			Q_INT32 version;

			stream >> version;
			if ( (version == 005) || (version == 006) || (version == 007) ) {
				Room *room = NULL;
				Connection *connection = NULL;
				InfoItem *item = NULL;

				Q_UINT32 quint32;    
				QString qstr;
				Q_UINT16 quint16;
				Q_INT8 qint8;
				unsigned index = 0;
				Q_UINT32 roomsCount = 0;
				Q_UINT32 connectionsCount = 0;
				Q_UINT32 itemsCount = 0;

				fileName=filename;

				stream >> roomsCount;
				stream >> connectionsCount;
				stream >> itemsCount; //itemsCount = 0;

				emit logText ("[MMapper] Loading data ...");

				//read selected room x,y
				stream >> quint32;rX=quint32;
				stream >> quint32;rY=quint32;

				//clear (HAVE TO DO IT BETTER!!!)
				for(room = rooms.first(); room; room = rooms.current()){
					room = rooms.take();
					delete room;
				}
				for(connection = connections.first(); connection; connection = connections.current()){
					connection = connections.take(); 
					delete connection->arrowObject;
					delete connection;
				}
				for(item = infoItems.first(); item; item = infoItems.current()){
					item = infoItems.take(); 
					if (item->item)
						switch (item->itemType){
						case text:delete (QCanvasText *) (item->item);break;
						case line:delete (QCanvasLine *) (item->item);break;
						case arrow:delete (MyCanvasArrow *) (item->item);break;
						}
						delete item;
				}

				QPtrVector<Room> indexToRooms;
				QPtrVector<Connection> indexToConnections;

				indexToRooms.resize(roomsCount);
				indexToConnections.resize(connectionsCount);

				// create all pointers to rooms
				for(index = 0; index < roomsCount; index++){
					room = new Room();
					room->u=NULL;
					room->d=NULL;
					room->e=NULL;
					room->w=NULL;
					room->n=NULL;
					room->s=NULL;
					room->roomID=index+1;
					room->onScreen=FALSE;
					rooms.append(room);
					indexToRooms.insert(index, room);
				}
				// create all pointers to connections
				for(index = 0; index < connectionsCount; index++){
					connection = new Connection();
					connection->room1=NULL;
					connection->room2=NULL;
					connection->onScreen=FALSE;
					connection->connectionID=index+1;
					connections.append(connection);
					indexToConnections.insert(index, connection);
				}
				// create all pointers to items
				for(index = 0; index<itemsCount; index++){
					item = new InfoItem();
					infoItems.append(item);
				}

				emit logText (QString("Number of rooms: %1").arg(roomsCount) );    
				for(room = rooms.first(); room; room = rooms.next()){
					stream >> qstr; room->roomName=qstr;
					stream >> qstr; room->roomDesc=qstr;

					if (version < 007){
						stream >> qstr; //room->parsedRoomDesc=qstr;
					}

					//if (room->roomDesc != "" && room->parsedRoomDesc=="") 
					//    room->parsedRoomDesc=Parser().parseDesc(room->roomDesc);

					stream >> quint16; room->roomTerrain=(RoomTerrainType)quint16;
					stream >> quint16; room->roomMob=(RoomMobType)quint16;
					stream >> quint16; room->roomLoad=(RoomLoadType)quint16;
					stream >> quint16; room->roomLocation=(RoomLocationType)quint16;
					stream >> quint16; room->roomPortable=(RoomPortableType)quint16;
					stream >> quint16; room->roomLight=(RoomLightType)quint16;
					stream >> quint16; room->roomAlign=(RoomAlignType)quint16;

					if (version == 005){
						switch ((int)room->roomMob) {
						case 0: room->roomMob = MAGGRESIVE;break;
						case 1: room->roomMob = MPEACEFULL;break;
						case 2: room->roomMob = MQUEST;break;
						case 3: room->roomMob = MSHOP;break;
						case 4: room->roomMob = MRENT;break;
						case 5: room->roomMob = MGUILD;break;
						case 6: room->roomMob = MNOMOB;break;
						}
						switch ((int)room->roomLoad) {
						case 0: room->roomLoad = LHERB;break;
						case 1: room->roomLoad = LTREASURE;break;
						case 2: room->roomLoad = LKEY;break;
						case 3: room->roomLoad = LNOLOAD;break;
						case 4: room->roomLoad = LWATER;break;
						case 5: room->roomLoad = LFOOD;break;
						case 6: room->roomLoad = LHORSE;break;
						case 7: room->roomLoad = LWARG;break;
						case 8: room->roomLoad = LTOWER;break;
						case 9: room->roomLoad = LATTENTION;break;
						case 10: room->roomLoad = LBOAT;break;
						}
					}

					stream >> quint32; room->roomFlags = quint32;
					stream >> qint8; room->roomUpdated = (bool)qint8;
					stream >> qint8; room->roomCheckExits = (bool)qint8;

					stream >> quint32; room->mapx=quint32;/*room->mapx+=5;*///if (room->mapx > 430) room->mapx=room->mapx-1;
					stream >> quint32; room->mapy=quint32;//room->mapy+=20;
					if ((room->mapx == rX) && (room->mapy == rY)) currentRoom = room;
					stream >> quint32; if (quint32!=0) room->u=indexToConnections.at(quint32-1);
					stream >> quint32; if (quint32!=0) room->d=indexToConnections.at(quint32-1);
					stream >> quint32; if (quint32!=0) room->e=indexToConnections.at(quint32-1);
					stream >> quint32; if (quint32!=0) room->w=indexToConnections.at(quint32-1);
					stream >> quint32; if (quint32!=0) room->n=indexToConnections.at(quint32-1);
					stream >> quint32; if (quint32!=0) room->s=indexToConnections.at(quint32-1);
				}

				emit logText (QString("Number of connections: %1").arg(connectionsCount) );
				for(connection = connections.first(); connection; connection = connections.next()){
					stream >> quint16; connection->type=quint16;
					stream >> quint16; connection->doorType1=(DoorType)quint16;
					stream >> quint16; connection->doorType2=(DoorType)quint16;
					stream >> qstr; connection->doorName1=qstr;
					stream >> qstr; connection->doorName2=qstr;
					stream >> quint32; if (quint32!=0 && quint32-1<roomsCount) connection->room1=indexToRooms.at(quint32-1); 
					stream >> quint32; if (quint32!=0 && quint32-1<roomsCount) connection->room2=indexToRooms.at(quint32-1); 
				}

				emit logText (QString("Number of info items: %1").arg(itemsCount) );
				for(item = infoItems.first(); item; item = infoItems.next()){
					stream >> qstr; item->itemName=qstr;
					stream >> qstr; item->itemText=qstr;
					stream >> quint16; item->itemType=(ItemType)quint16;
					stream >> quint32; item->x1=quint32;
					stream >> quint32; item->y1=quint32;
					stream >> quint32; item->x2=quint32;
					stream >> quint32; item->y2=quint32;
				}

				logText ("[MMapper] Finished loading.");
				logText ("[MMapper] Building map ...");
				//draw rooms & items
				for(room = rooms.first(); room; room = rooms.next())emit roomChanged(room);
				for(item = infoItems.first(); item; item = infoItems.next()) infoObjectChanged(item);
				logText ("[MMapper] Finished building.");
				m_dataChanged=FALSE;
				retVal = true;
			}
		}
	}

	byteArray.resetRawData(stdString.data(), stdStringLen);

	return retVal;
}

bool MapData::saveData(const QString& filename) {
	QByteArray byteArray;
	QBuffer buf(byteArray);

	if (buf.open(IO_WriteOnly))
	{
		Room *room = NULL;
		Connection *connection = NULL;
		InfoItem *item = NULL;

		logText ("[MMapper] Writting data to file ...");

		Q_UINT32 roomsCount = 0;
		Q_UINT32 connectionsCount = 0;
		Q_UINT32 itemsCount = 0;

		for(room = rooms.first(); room; room = rooms.next())
		{roomsCount++;room->roomID=roomsCount;}
		for(connection = connections.first(); connection; connection = connections.next())
		{connectionsCount++;connection->connectionID=connectionsCount;}
		for(item = infoItems.first(); item; item = infoItems.next()) itemsCount++;

		QDataStream stream (&buf);
		fileName=filename;

		// Write a header with a "magic number" and a version
		stream << (Q_UINT32)0xFFB2AF01;
		stream << (Q_INT32)007;

		//write counters
		stream << roomsCount;
		stream << connectionsCount;
		stream << itemsCount;

		//write selected room x,y
		Q_UINT32 quint32;    
		QString qstr;
		Q_UINT16 quint16;
		Q_INT8 qint8;

		quint32=rX;stream << quint32;
		quint32=rY;stream << quint32;

		// save rooms
		for(room = rooms.first(); room; room = rooms.next()){
			qstr=room->roomName; stream << qstr;
			qstr=room->roomDesc; stream << qstr;
			//qstr=room->parsedRoomDesc; stream << qstr;
			quint16=room->roomTerrain; stream << quint16;
			quint16=room->roomMob; stream << quint16;
			quint16=room->roomLoad; stream << quint16;
			quint16=room->roomLocation; stream << quint16;
			quint16=room->roomPortable; stream << quint16;
			quint16=room->roomLight; stream << quint16;
			quint16=room->roomAlign; stream << quint16;

			quint32=room->roomFlags; stream << quint32;
			qint8=(Q_INT8)(room->roomUpdated); stream << qint8;
			qint8=(Q_INT8)(room->roomCheckExits); stream << qint8;

			quint32=room->mapx; stream << quint32;
			quint32=room->mapy; stream << quint32;

			if (room->u) quint32=room->u->connectionID; else quint32=0; stream << quint32;
			if (room->d) quint32=room->d->connectionID; else quint32=0; stream << quint32;
			if (room->e) quint32=room->e->connectionID; else quint32=0; stream << quint32;
			if (room->w) quint32=room->w->connectionID; else quint32=0; stream << quint32;
			if (room->n) quint32=room->n->connectionID; else quint32=0; stream << quint32;
			if (room->s) quint32=room->s->connectionID; else quint32=0; stream << quint32;
		}

		// save connections
		for(connection = connections.first(); connection; connection = connections.next()){
			quint16=connection->type; stream << quint16;
			quint16=connection->doorType1; stream << quint16;
			quint16=connection->doorType2; stream << quint16;
			qstr=connection->doorName1; stream << qstr;
			qstr=connection->doorName2; stream << qstr;
			if (connection->room1) quint32=connection->room1->roomID; else quint32=0; stream << quint32;
			if (connection->room2) quint32=connection->room2->roomID; else quint32=0; stream << quint32;
		}

		// save items
		for(item = infoItems.first(); item; item = infoItems.next()){
			qstr=item->itemName; stream << qstr;
			qstr=item->itemText; stream << qstr;
			quint16=item->itemType; stream << quint16;
			quint32=item->x1; stream << quint32;
			quint32=item->y1; stream << quint32;
			quint32=item->x2; stream << quint32;
			quint32=item->y2; stream << quint32;
		}

		CryptoPP::FileSink *pSink = new CryptoPP::FileSink(filename.ascii());
		CryptoPP::Gzip *pDeflator = new CryptoPP::Gzip(pSink);

		try 
		{
			CryptoPP::StringSource((const byte *) byteArray.data(), byteArray.size(), true, pDeflator);
		}
		catch(CryptoPP::Exception &e)
		{
			QString msg("CryptoPP::Exception caught: %1");
			qDebug(msg.arg(e.what()));
			return false;
		}
		catch(std::exception &e)
		{
			QString msg("std::exception caught: %1");
			qDebug(msg.arg(e.what()));
			return false;
		}

		logText (tr("[MMapper] Writing data finished."));

		m_dataChanged=FALSE;

		return TRUE;
	}
	else
	{
		logText (tr("[MMapper] Cannot open memory buffer."));
	}

	return false;
}

// ***********************************************************
// DATA Manipulation functions



void MapData::moveNorth()
{
	Room *room=getCurrentRoom();
	Room *room2=getSelectedRoom2();
	if (!room) return;
	unsigned x1=room->mapx;
	unsigned y1=room->mapy;
	unsigned x2=room->mapx+1;
	unsigned y2=room->mapy+1;

	if (!room2){
		if (!findRoomAt(room->mapx,room->mapy-1)){
			if (y1-1<=0) return;	
			room->mapy-=1;
			emit roomChanged(room);
			emit selectRoom(room);
			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2);
			y2=ROOMTOVIEWPORT(y2);
		}
	}
	else {
		x1 = room->mapx <= room2->mapx ? room->mapx : room2->mapx;
		y1 = room->mapy <= room2->mapy ? room->mapy : room2->mapy;
		x2 = room->mapx > room2->mapx ? room->mapx : room2->mapx;
		y2 = room->mapy > room2->mapy ? room->mapy : room2->mapy;
		if (y1-1<=0) return;	
		Room *r;

		for (unsigned x=x1; x<x2+1; x++){
			if (findRoomAt(x,y1-1)) return;
		}
		for (unsigned y=y1; y<y2+1; y++)
			for (unsigned x=x1; x<x2+1; x++){
				r=findRoomAt(x,y);
				if (!r) continue;
				r->mapy-=1;
				emit roomChanged(r);
			}

			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2)+MAPROOMSIZE;
			y2=ROOMTOVIEWPORT(y2)+MAPROOMSIZE;

			r = room2;
			emit selectRoom(room);
			emit selectRoom2(r);
	}

	InfoItem *i;
	for ( i = infoItems.first(); i; i = infoItems.next() ){
		if (i->x1 >= x1 && i->x1 <= x2 && i->y1 >= y1 && i->y1 <= y2){
			i->y1 -= MAPROOMSIZE;
			emit infoObjectChanged(i);
		}
		if (i->itemType != text){
			if (i->x2 >= x1 && i->x2 <= x2 && i->y2 >= y1 && i->y2 <= y2){
				i->y2 -= MAPROOMSIZE;
				emit infoObjectChanged(i);
			}
		}
	}
	m_dataChanged = TRUE;
}

void MapData::moveSouth()
{
	Room *room=getCurrentRoom();  
	Room *room2=getSelectedRoom2();
	if (!room) return;
	unsigned x1=room->mapx;
	unsigned y1=room->mapy;
	unsigned x2=room->mapx+1;
	unsigned y2=room->mapy+1;

	if (!room2){  
		if (!findRoomAt(room->mapx,room->mapy+1)){
			room->mapy+=1;
			emit roomChanged(room);
			emit selectRoom(room);
			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2);
			y2=ROOMTOVIEWPORT(y2);
		}
	}
	else {
		x1 = room->mapx <= room2->mapx ? room->mapx : room2->mapx;
		y1 = room->mapy <= room2->mapy ? room->mapy : room2->mapy;
		x2 = room->mapx > room2->mapx ? room->mapx : room2->mapx;
		y2 = room->mapy > room2->mapy ? room->mapy : room2->mapy;
		Room *r;

		for (unsigned x=x1; x<x2+1; x++){
			if (findRoomAt(x,y2+1)) return;
		}
		for (unsigned y=y2; y>y1-1; y--)
			for (unsigned x=x1; x<x2+1; x++){
				r=findRoomAt(x,y);
				if (!r) continue;
				r->mapy+=1;
				emit roomChanged(r);
			}

			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2)+MAPROOMSIZE;
			y2=ROOMTOVIEWPORT(y2)+MAPROOMSIZE;

			r = room2;
			emit selectRoom(room);
			emit selectRoom2(r);
	}
	InfoItem *i;

	for ( i = infoItems.first(); i; i = infoItems.next() ){
		if (i->x1 >= x1 && i->x1 <= x2 && i->y1 >= y1 && i->y1 <= y2){
			i->y1 += MAPROOMSIZE;
			emit infoObjectChanged(i);
		}
		if (i->itemType != text){
			if (i->x2 >= x1 && i->x2 <= x2 && i->y2 >= y1 && i->y2 <= y2){
				i->y2 += MAPROOMSIZE;
				emit infoObjectChanged(i);
			}
		}
	}
	m_dataChanged = TRUE;

}

void MapData::moveEast()
{
	Room *room=getCurrentRoom();  
	Room *room2=getSelectedRoom2();
	if (!room) return;
	unsigned x1=room->mapx;
	unsigned y1=room->mapy;
	unsigned x2=room->mapx+1;
	unsigned y2=room->mapy+1;

	if (!room2){
		if (!findRoomAt(room->mapx+1,room->mapy)){
			room->mapx+=1;
			emit roomChanged(room);
			emit selectRoom(room);
			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2);
			y2=ROOMTOVIEWPORT(y2);
		}
	}
	else {
		x1 = room->mapx <= room2->mapx ? room->mapx : room2->mapx;
		y1 = room->mapy <= room2->mapy ? room->mapy : room2->mapy;
		x2 = room->mapx > room2->mapx ? room->mapx : room2->mapx;
		y2 = room->mapy > room2->mapy ? room->mapy : room2->mapy;
		Room *r;

		for (unsigned y=y1; y<y2+1; y++){
			if (findRoomAt(x2+1,y)) return;
		}
		for (unsigned x=x2; x>x1-1; x--)
			for (unsigned y=y1; y<y2+1; y++){
				r=findRoomAt(x,y);
				if (!r) continue;
				r->mapx+=1;
				emit roomChanged(r);
			}

			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2)+MAPROOMSIZE;
			y2=ROOMTOVIEWPORT(y2)+MAPROOMSIZE;

			r = room2;
			emit selectRoom(room);
			emit selectRoom2(r);
	}
	InfoItem *i;

	for ( i = infoItems.first(); i; i = infoItems.next() ){
		if (i->x1 >= x1 && i->x1 <= x2 && i->y1 >= y1 && i->y1 <= y2){
			i->x1 += MAPROOMSIZE;
			emit infoObjectChanged(i);
		}
		if (i->itemType != text){
			if (i->x2 >= x1 && i->x2 <= x2 && i->y2 >= y1 && i->y2 <= y2){
				i->x2 += MAPROOMSIZE;
				emit infoObjectChanged(i);
			}
		}
	}
	m_dataChanged = TRUE;

}

void MapData::moveWest()
{
	Room *room=getCurrentRoom();  
	Room *room2=getSelectedRoom2();
	if (!room) return;
	unsigned x1=room->mapx;
	unsigned y1=room->mapy;
	unsigned x2=room->mapx+1;
	unsigned y2=room->mapy+1;

	if (!room2){
		if (!findRoomAt(room->mapx-1,room->mapy)){
			if (x1-1<=0) return;	
			room->mapx-=1;
			emit roomChanged(room);
			emit selectRoom(room);
			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2);
			y2=ROOMTOVIEWPORT(y2);
		}
	}
	else {
		x1 = room->mapx <= room2->mapx ? room->mapx : room2->mapx;
		y1 = room->mapy <= room2->mapy ? room->mapy : room2->mapy;
		x2 = room->mapx > room2->mapx ? room->mapx : room2->mapx;
		y2 = room->mapy > room2->mapy ? room->mapy : room2->mapy;
		if (x1-1<=0) return;	
		Room *r;

		for (unsigned y=y1; y<y2+1; y++){
			if (findRoomAt(x1-1,y)) return;
		}
		for (unsigned x=x1; x<x2+1; x++)
			for (unsigned y=y1; y<y2+1; y++){
				r=findRoomAt(x,y);
				if (!r) continue;
				r->mapx-=1;
				emit roomChanged(r);
			}

			x1=ROOMTOVIEWPORT(x1);
			y1=ROOMTOVIEWPORT(y1);
			x2=ROOMTOVIEWPORT(x2)+MAPROOMSIZE;
			y2=ROOMTOVIEWPORT(y2)+MAPROOMSIZE;

			r = room2;
			emit selectRoom(room);
			emit selectRoom2(r);
	}

	InfoItem *i;
	for ( i = infoItems.first(); i; i = infoItems.next() ){
		if (i->x1 >= x1 && i->x1 <= x2 && i->y1 >= y1 && i->y1 <= y2){
			i->x1 -= MAPROOMSIZE;
			emit infoObjectChanged(i);
		}
		if (i->itemType != text){
			if (i->x2 >= x1 && i->x2 <= x2 && i->y2 >= y1 && i->y2 <= y2){
				i->x2 -= MAPROOMSIZE;
				emit infoObjectChanged(i);
			}
		}
	}
	m_dataChanged = TRUE;

}


void MapData::removeCurrentRoom(){
	Room *room=getCurrentRoom();  
	Room *room2=getSelectedRoom2();
	if (!room) return;

	if (!room2){
		unsigned x=room->mapx;
		unsigned y=room->mapy;

		removeRoom(room);
		setCurrentRoom(x,y);
	}     
	else {
		unsigned x1 = room->mapx <= room2->mapx ? room->mapx : room2->mapx;
		unsigned y1 = room->mapy <= room2->mapy ? room->mapy : room2->mapy;
		unsigned x2 = room->mapx > room2->mapx ? room->mapx : room2->mapx;
		unsigned y2 = room->mapy > room2->mapy ? room->mapy : room2->mapy;

		Room* r=NULL;
		for (unsigned x=x1; x<x2+1; x++)
			for (unsigned y=y1; y<y2+1; y++){
				r=findRoomAt(x,y);
				if (!r) continue;
				removeRoom(r);
			}
	}
	emit roomChanged(NULL);
}



Connection *MapData::connectRooms(Room *sr,Room* dr, DirectionType dir, Q_UINT32 roomFlags) {

	Connection *c = NULL;

	if ( sr && dr ) { // to create connection we need to have start and end room
		switch (dir) {

		case dir_north:
			if (!dr->s /*&& Config().m_connect_rooms_in_both_dirs*/) {
				c = sr->n = dr->s = createNewConnection();
				if (ISSET(roomFlags,FLAGS_VALID) && ISNOTSET(roomFlags,EXIT_S) && ISNOTSET(roomFlags,DOOR_S)) {
					dr->s = NULL;
					SET(c->type,CONNECTION_ONEWAY);
				}
			} else if( dr->s && (ISCONNECTEDTO(dr,dr->s)==sr) && ISSET(dr->s->type,CONNECTION_ONEWAY)) {
				c = sr->n = dr->s;
				if (ISSET(roomFlags,FLAGS_VALID)) { // valid exit info
					if (ISNOTSET(roomFlags,EXIT_S) && ISNOTSET(roomFlags,DOOR_S))
						dr->s = NULL;
					else
						UNSET(c->type,CONNECTION_ONEWAY);
				}
			} else {
				c = sr->n = createNewConnection();
				SET(c->type,CONNECTION_ONEWAY);
			}
			break;

		case dir_south:
			if (!dr->n /*&& Config().m_connect_rooms_in_both_dirs*/) {
				c = sr->s = dr->n = createNewConnection();
				if (ISSET(roomFlags,FLAGS_VALID) && ISNOTSET(roomFlags,EXIT_N) && ISNOTSET(roomFlags,DOOR_N)) {
					dr->n = NULL;
					SET(c->type,CONNECTION_ONEWAY);
				}
			} else if( dr->n && (ISCONNECTEDTO(dr,dr->n)==sr) && ISSET(dr->n->type,CONNECTION_ONEWAY)) {
				c = sr->s = dr->n;
				if (ISSET(roomFlags,FLAGS_VALID)) { // valid exit info
					if (ISNOTSET(roomFlags,EXIT_N) && ISNOTSET(roomFlags,DOOR_N))
						dr->n = NULL;
					else
						UNSET(c->type,CONNECTION_ONEWAY);
				}
			} else {
				c = sr->s = createNewConnection();
				SET(c->type,CONNECTION_ONEWAY);
			}
			break;

		case dir_east:
			if (!dr->w /*&& Config().m_connect_rooms_in_both_dirs*/) {
				c = sr->e = dr->w = createNewConnection();
				if (ISSET(roomFlags,FLAGS_VALID) && ISNOTSET(roomFlags,EXIT_W) && ISNOTSET(roomFlags,DOOR_W)) {
					dr->w = NULL;
					SET(c->type,CONNECTION_ONEWAY);
				}
			} else if( dr->w && (ISCONNECTEDTO(dr,dr->w)==sr) && ISSET(dr->w->type,CONNECTION_ONEWAY)) {
				c = sr->e = dr->w;
				if (ISSET(roomFlags,FLAGS_VALID)) { // valid exit info
					if (ISNOTSET(roomFlags,EXIT_W) && ISNOTSET(roomFlags,DOOR_W))
						dr->w = NULL;
					else
						UNSET(c->type,CONNECTION_ONEWAY);
				}
			} else {
				c = sr->e = createNewConnection();
				SET(c->type,CONNECTION_ONEWAY);
			}
			break;

		case dir_west:
			if (!dr->e /*&& Config().m_connect_rooms_in_both_dirs*/) {
				c = sr->w = dr->e = createNewConnection();
				if (ISSET(roomFlags,FLAGS_VALID) && ISNOTSET(roomFlags,EXIT_E) && ISNOTSET(roomFlags,DOOR_E)) {
					dr->e = NULL;
					SET(c->type,CONNECTION_ONEWAY);
				}
			} else if( dr->e && (ISCONNECTEDTO(dr,dr->e)==sr) && ISSET(dr->e->type,CONNECTION_ONEWAY)) {
				c = sr->w = dr->e;
				if (ISSET(roomFlags,FLAGS_VALID)) { // valid exit info
					if (ISNOTSET(roomFlags,EXIT_E) && ISNOTSET(roomFlags,DOOR_E))
						dr->e = NULL;
					else
						UNSET(c->type,CONNECTION_ONEWAY);
				}
			} else {
				c = sr->w = createNewConnection();
				SET(c->type,CONNECTION_ONEWAY);
			}
			break;

		case dir_up:
			if (!dr->d /*&& Config().m_connect_rooms_in_both_dirs*/) {
				c = sr->u = dr->d = createNewConnection();
				if (ISSET(roomFlags,FLAGS_VALID) && ISNOTSET(roomFlags,EXIT_D) && ISNOTSET(roomFlags,DOOR_D)) {
					dr->d = NULL;
					SET(c->type,CONNECTION_ONEWAY);
				}
			} else if( dr->d && (ISCONNECTEDTO(dr,dr->d)==sr) && ISSET(dr->d->type,CONNECTION_ONEWAY)) {
				c = sr->u = dr->d;
				if (ISSET(roomFlags,FLAGS_VALID)) { // valid exit info
					if (ISNOTSET(roomFlags,EXIT_D) && ISNOTSET(roomFlags,DOOR_D))
						dr->d = NULL;
					else
						UNSET(c->type,CONNECTION_ONEWAY);
				}
			} else {
				c = sr->u = createNewConnection();
				SET(c->type,CONNECTION_ONEWAY);
			}
			break;

		case dir_down:
			if (!dr->u /*&& Config().m_connect_rooms_in_both_dirs*/) {
				c = sr->d = dr->u = createNewConnection();
				if (ISSET(roomFlags,FLAGS_VALID) && ISNOTSET(roomFlags,EXIT_U) && ISNOTSET(roomFlags,DOOR_U)) {
					dr->u = NULL;
					SET(c->type,CONNECTION_ONEWAY);
				}
			} else if( dr->u && (ISCONNECTEDTO(dr,dr->u)==sr) && ISSET(dr->u->type,CONNECTION_ONEWAY)) {
				c = sr->d = dr->u;
				if (ISSET(roomFlags,FLAGS_VALID)) { // valid exit info
					if (ISNOTSET(roomFlags,EXIT_U) && ISNOTSET(roomFlags,DOOR_U))
						dr->u = NULL;
					else
						UNSET(c->type,CONNECTION_ONEWAY);
				}
			} else {
				c = sr->u = createNewConnection();
				SET(c->type,CONNECTION_ONEWAY);
			}
			break;
		default:
			;
		}

		c->room1 = sr;
		c->room2 = dr;
		if (sr == dr)
			SET(c->type,CONNECTION_LOOP);
		m_dataChanged = TRUE;
	}
	return c;
}

void MapData::updateRoom(Room* dr, const QString& name, const QString& desc, Q_UINT32 roomFlags, Q_UINT32 promptFlags) {

	if (!Config().m_autoRoomUpdate)
		return;

	if (!dr->roomUpdated){
		if ((ISNOTSET(promptFlags,PROMPT_INFO_VALID)) || (ISNOTSET(roomFlags,FLAGS_VALID))) 
			return;
	}

	int tt=promptFlags & 15;

	if (!Config().m_brief)
		dr->roomUpdated = true;

	if (ISSET( roomFlags, FLAGS_VALID))
		dr->roomFlags |= roomFlags;

	dr->roomName = name;
	if (!Config().m_brief) {
		dr->roomDesc = desc;
		//dr->parsedRoomDesc = parseddesc;//Parser().parseDesc(desc);
	}

	if (ISSET(promptFlags,PROMPT_INFO_VALID)) {
		if (tt<14)
			dr->roomTerrain = (RoomTerrainType)tt;
		if (ISSET(promptFlags,SUN_ROOM))
			dr->roomLight = LIT;
	}

	if (ISSET(roomFlags,FLAGS_VALID)) { // valid exit info

		if (dr->n) {
			if (ISSET(roomFlags,DOOR_N))
				SET(dr->n->type,CONNECTION_DOOR);
			if (ISNOTSET(roomFlags,EXIT_N)) {
				SET(dr->n->type,CONNECTION_DOOR);
				SETDOORTYPE(dr,dr->n,DHIDDEN);
			}
		}

		if (dr->s) {
			if (ISSET(roomFlags,DOOR_S))
				SET(dr->s->type,CONNECTION_DOOR);
			if (ISNOTSET(roomFlags,EXIT_S)) {
				SET(dr->s->type,CONNECTION_DOOR);
				SETDOORTYPE(dr,dr->s,DHIDDEN);
			}
		}

		if (dr->e) {
			if (ISSET(roomFlags,DOOR_E))
				SET(dr->e->type,CONNECTION_DOOR);
			if (ISNOTSET(roomFlags,EXIT_E)) {
				SET(dr->e->type,CONNECTION_DOOR);
				SETDOORTYPE(dr,dr->e,DHIDDEN);
			}
		}

		if (dr->w) {
			if (ISSET(roomFlags,DOOR_W))
				SET(dr->w->type,CONNECTION_DOOR);
			if (ISNOTSET(roomFlags,EXIT_W)) {
				SET(dr->w->type,CONNECTION_DOOR);
				SETDOORTYPE(dr,dr->w,DHIDDEN);
			}
		}

		if (dr->u) {
			if (ISSET(roomFlags,DOOR_U))
				SET(dr->u->type,CONNECTION_DOOR);
			if (ISNOTSET(roomFlags,EXIT_U)) {
				SET(dr->u->type,CONNECTION_DOOR);
				SETDOORTYPE(dr,dr->u,DHIDDEN);
			}
		}

		if (dr->d) {
			if (ISSET(roomFlags,DOOR_D))
				SET(dr->d->type,CONNECTION_DOOR);
			if (ISNOTSET(roomFlags,EXIT_D)) {
				SET(dr->d->type,CONNECTION_DOOR);
				SETDOORTYPE(dr,dr->d,DHIDDEN);
			}
		}
	}

	if (dr->n)
		Canvas().updateRoom(ISCONNECTEDTO(dr,dr->n));
	if (dr->s)
		Canvas().updateRoom(ISCONNECTEDTO(dr,dr->s));
	if (dr->e)
		Canvas().updateRoom(ISCONNECTEDTO(dr,dr->e));
	if (dr->w)
		Canvas().updateRoom(ISCONNECTEDTO(dr,dr->w));

	m_dataChanged = TRUE;
}

void MapData::resetRoom(Room* r){
	r->roomName="";
	r->roomDesc="";
	//r->parsedRoomDesc="";
	r->roomFlags = 0;
	r->roomUpdated = FALSE;

	m_dataChanged=true;
}


