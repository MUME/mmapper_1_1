/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   krejza@cbox.cz                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DEFS_H
#define DEFS_H

#include <qobject.h>
#include <qstring.h>
/**
@author Marek Krejza
*/

const QString version = "1.1";

#define MAPROOMSIZE 48
#define MAPROOMS 600

#define ROOMTOVIEWPORT(X) ( X * MAPROOMSIZE )
#define VIEWPORTTOROOM(X) ( (int)(X / MAPROOMSIZE) )

#define ISCONNECTEDTO(r,c) ( (c->room1==r) ? c->room2 : c->room1 )

#define SETDOORNAME(r,c,name) ( (c->room1==r) ? c->doorName2=name : c->doorName1=name )
#define DOORNAME(r,c) ( (c->room1==r) ? c->doorName2 : c->doorName1 )
#define SETDOORTYPE(r,c,type) ( (c->room1==r) ? c->doorType2=type : c->doorType1=type )
#define DOORTYPE(r,c) ( (c->room1==r) ? c->doorType2 : c->doorType1 )

const char ESC=27;

#define UNSET(dest,bit) (dest &=~bit)
#define SET(dest,bit) (dest |=bit)
#define ISSET(src,bit) (src & bit)
#define ISNOTSET(src,bit) (!(src & bit))

#define bit0 1
#define bit1 2
#define bit2 4
#define bit3 8
#define bit4 16
#define bit5 32
#define bit6 64
#define bit7 128
#define bit8 256
#define bit9 512
#define bit10 1024
#define bit11 2048
#define bit12 4096
#define bit13 8192
#define bit14 16384
#define bit15 32768
#define bit16 65536
#define bit17 131072
#define bit18 262144



// for room flags
#define FLAGS_VALID bit0
#define EXIT_N bit1
#define EXIT_S bit2
#define EXIT_E bit3
#define EXIT_W bit4
#define EXIT_U bit5
#define EXIT_D bit6
#define DOOR_N bit7
#define DOOR_S bit8
#define DOOR_E bit9
#define DOOR_W bit10
#define DOOR_U bit11
#define DOOR_D bit12
#define ROAD_N bit13
#define ROAD_S bit14
#define ROAD_E bit15
#define ROAD_W bit16
#define ROAD_U bit17
#define ROAD_D bit18

//for prompt flags
// 0-3 terrain type
#define PROMPT_INFO_VALID bit8
#define SUN_ROOM bit9 

typedef Q_UINT32 RoomID;
typedef Q_UINT32 ConnectionID;

typedef class QString RoomName;
typedef class QString RoomDescription;
typedef class QString DoorName;


#define WALLNORTH 1
#define WALLSOUTH 2
#define WALLEAST  4
#define WALLWEST  8
#define WALLUP    16
#define WALLDOWN  32
#define DOORNORTH 64
#define DOORSOUTH 128
#define DOOREAST  256
#define DOORWEST  512
#define DOORUP    1024
#define DOORDOWN  2048

enum DirectionType { dir_up, dir_down, dir_east, dir_west, dir_north, dir_south, dir_unknown };
enum DoorActionType { door_open, door_close, door_lock, door_unlock, door_pick, door_rock, door_bash, door_break, door_block  };

typedef Q_UINT32 ConnectionType;
#define CONNECTION_NORMAL 0
#define CONNECTION_LOOP 1
#define CONNECTION_ONEWAY 2
#define CONNECTION_DOOR 4
#define CONNECTION_CLIMB 8
#define CONNECTION_SPECIAL 16
#define CONNECTION_RANDOM 32

enum RoomTerrainType { TINDOORS, TCITY, TFIELD, TFOREST, THILLS ,TMOUNTAINS, TSHALLOW, TWATER, TRAPIDS, TUNDERWATER, TROAD, TBRUSH, TTUNNEL, TCAVERN, TDEATHTRAP, TRANDOM };

enum RoomMobType  { MNOMOB, MPEACEFULL, MAGGRESIVE, MQUEST, MSHOP, MRENT, MGUILD };

enum RoomLoadType { LNOLOAD, LTREASURE, LHERB, LKEY, LWATER, LFOOD, LHORSE, LWARG, LTOWER, LATTENTION, LBOAT };

enum RoomLocationType { INDOOR, OUTSIDE };
enum RoomPortableType { PORT, NOPORT };
enum RoomLightType { DARK, LIT };
enum RoomAlignType { GOOD, NEUTRAL, EVIL };

enum DoorType { DNORMAL, DHIDDEN, DNORMALNEEDKEY, DHIDDENNEEDKEY };

#endif
