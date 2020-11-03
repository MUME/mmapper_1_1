/***************************************************************************
*   Copyright (C) 2003 by Marek Krejza                                    *
*   mkrejza@cbox.cz                                                       *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "roomeditdialogimpl.h"
#include <qmainwindow.h>
//#include <kled.h>
#include <qmainwindow.h>
#include <qcheckbox.h>
#include "mmapper.h"

RoomEditDialogImpl::RoomEditDialogImpl(QWidget *parent, const char *name, WFlags f)
	: RoomEditDialog(parent, name, f)
{
	emitChangedAllowed = true;
	pointToSelect=1;

	lastConnectClicked=0;

	//connect signals and slots
	connect(connectUpButton, SIGNAL( clicked() ), SLOT( connectUpClicked() ));
	connect(connectDownButton, SIGNAL( clicked() ), SLOT( connectDownClicked() ));
	connect(connectNorthButton, SIGNAL( clicked() ), SLOT( connectNorthClicked() ));
	connect(connectSouthButton, SIGNAL( clicked() ), SLOT( connectSouthClicked() ));
	connect(connectEastButton, SIGNAL( clicked() ), SLOT( connectEastClicked() ));
	connect(connectWestButton, SIGNAL( clicked() ), SLOT( connectWestClicked() ));

	connect(disconnectUpButton, SIGNAL( clicked() ), SLOT( disconnectUpClicked() ));
	connect(disconnectDownButton, SIGNAL( clicked() ), SLOT( disconnectDownClicked() ));
	connect(disconnectNorthButton, SIGNAL( clicked() ), SLOT( disconnectNorthClicked() ));
	connect(disconnectSouthButton, SIGNAL( clicked() ), SLOT( disconnectSouthClicked() ));
	connect(disconnectEastButton, SIGNAL( clicked() ), SLOT( disconnectEastClicked() ));
	connect(disconnectWestButton, SIGNAL( clicked() ), SLOT( disconnectWestClicked() ));

	connect(connectToButton, SIGNAL( clicked() ), SLOT( connectToClicked() ));
	connect(connectToUpButton, SIGNAL( clicked() ), SLOT( connectToUpClicked() ));
	connect(connectToDownButton, SIGNAL( clicked() ), SLOT( connectToDownClicked() ));
	connect(connectToNorthButton, SIGNAL( clicked() ), SLOT( connectToNorthClicked() ));
	connect(connectToSouthButton, SIGNAL( clicked() ), SLOT( connectToSouthClicked() ));
	connect(connectToEastButton, SIGNAL( clicked() ), SLOT( connectToEastClicked() ));
	connect(connectToWestButton, SIGNAL( clicked() ), SLOT( connectToWestClicked() ));

	connect(newNorthButton, SIGNAL( clicked() ), SLOT( newRoomNorthClicked() ));
	connect(newSouthButton, SIGNAL( clicked() ), SLOT( newRoomSouthClicked() ));
	connect(newEastButton, SIGNAL( clicked() ), SLOT( newRoomEastClicked() ));
	connect(newWestButton, SIGNAL( clicked() ), SLOT( newRoomWestClicked() ));

	connect( roomDeleteButton, SIGNAL( clicked() ), SLOT( deleteClicked() ));
	connect( roomCreateButton, SIGNAL( clicked() ), SLOT( createClicked() ));

	connect( checkBoxDark, SIGNAL( stateChanged(int) ), SLOT( DarkChanged(int) ));
	connect( checkBoxIndoor, SIGNAL( stateChanged(int) ), SLOT( IndoorChanged(int) ));
	connect( checkBoxPortable, SIGNAL( stateChanged(int) ), SLOT( PortableChanged(int) ));

	connect( checkExits, SIGNAL( stateChanged(int) ), SLOT( CheckExitsChanged(int) ));

	connect( alignType, SIGNAL( activated(int) ), SLOT( alignTypeActivated(int) ));
	connect( terrainType, SIGNAL( activated(int) ), SLOT( terrainTypeActivated(int) ));
	connect( mobType, SIGNAL( activated(int) ), SLOT( mobTypeActivated(int) ));
	connect( loadType, SIGNAL( activated(int) ), SLOT( loadTypeActivated(int) ));

	connect( roomNameEdit, SIGNAL( textChanged(const QString&) ), SLOT( roomNameEditChanged(const QString&) ));
	connect( roomDescEdit, SIGNAL( textChanged() ), SLOT( roomDescEditChanged() ));

	connect( doorDownType, SIGNAL( activated(int) ), SLOT( doorDownTypeChanged(int) ));
	connect( doorUpType, SIGNAL( activated(int) ), SLOT( doorUpTypeChanged(int) ));
	connect( doorWestType, SIGNAL( activated(int) ), SLOT( doorWestTypeChanged(int) ));
	connect( doorEastType, SIGNAL( activated(int) ), SLOT( doorEastTypeChanged(int) ));
	connect( doorSouthType, SIGNAL( activated(int) ), SLOT( doorSouthTypeChanged(int) ));
	connect( doorNorthType, SIGNAL( activated(int) ), SLOT( doorNorthTypeChanged(int) ));

	connect( doorDownEdit, SIGNAL( textChanged(const QString&) ), SLOT( doorDownEditChanged(const QString&) ));
	connect( doorUpEdit, SIGNAL( textChanged(const QString&) ), SLOT( doorUpEditChanged(const QString&) ));
	connect( doorWestEdit, SIGNAL( textChanged(const QString&) ), SLOT( doorWestEditChanged(const QString&) ));
	connect( doorEastEdit, SIGNAL( textChanged(const QString&) ), SLOT( doorEastEditChanged(const QString&) ));
	connect( doorSouthEdit, SIGNAL( textChanged(const QString&) ), SLOT( doorSouthEditChanged(const QString&) ));
	connect( doorNorthEdit, SIGNAL( textChanged(const QString&) ), SLOT( doorNorthEditChanged(const QString&) ));

	connect( connectionDownType, SIGNAL( selectionChanged() ), SLOT( connectionDownTypeChanged() ));
	connect( connectionUpType, SIGNAL( selectionChanged() ), SLOT( connectionUpTypeChanged() ));
	connect( connectionWestType, SIGNAL( selectionChanged() ), SLOT( connectionWestTypeChanged() ));
	connect( connectionEastType, SIGNAL( selectionChanged() ), SLOT( connectionEastTypeChanged() ));
	connect( connectionSouthType, SIGNAL( selectionChanged() ), SLOT( connectionSouthTypeChanged() ));
	connect( connectionNorthType, SIGNAL( selectionChanged() ), SLOT( connectionNorthTypeChanged() ));



	connect ( objects, SIGNAL(activated(int)), SLOT(objectsActivated(int)));
	connect ( objectType, SIGNAL(activated(int)), SLOT(objectTypeActivated(int)));
	connect ( objectName, SIGNAL(textChanged(const QString&)), SLOT(objectNameTextChanged(const QString&)));
	connect ( objectText, SIGNAL(textChanged(const QString&)), SLOT(objectTextChanged(const QString&)));
	connect ( objectX1, SIGNAL(valueChanged(int)), SLOT(objectX1ValueChanged(int)));
	connect ( objectY1, SIGNAL(valueChanged(int)), SLOT(objectY1ValueChanged(int)));
	connect ( objectX2, SIGNAL(valueChanged(int)), SLOT(objectX2ValueChanged(int)));
	connect ( objectY2, SIGNAL(valueChanged(int)), SLOT(objectY2ValueChanged(int)));
	connect ( objectCreate, SIGNAL(clicked()), SLOT(objectCreateClicked()));
	connect ( objectModify, SIGNAL(clicked()), SLOT(objectModifyClicked()));
	connect ( objectDelete, SIGNAL(clicked()), SLOT(objectDeleteClicked()));

}


RoomEditDialogImpl::~RoomEditDialogImpl() {}


void RoomEditDialogImpl::keyPressEvent( QKeyEvent* e ){
	MMapper *m=static_cast<MMapper *>(parent());  
	switch(e->key()) {
	case 4144: 	//m->pressedMode1ButtsetChecked(true);
		m->m1->setOn(TRUE);
		break;
	case 4146: 	//m->pressedMode3ButtsetChecked(true);
		m->m3->setOn(TRUE);
		break;
	case Qt::Key_Escape:
		hide();
		break;

	default:
		;
	}
}




// Actions defined here
// *************************************************************

void RoomEditDialogImpl::CheckExitsChanged(int i){
	switch (i)
	{
	case QButton::On:
		Data().getCurrentRoom()->roomCheckExits=TRUE;
		break;
	case QButton::Off:
		Data().getCurrentRoom()->roomCheckExits=FALSE;
		break;
	default:
		return;
	}
}


void RoomEditDialogImpl::DarkChanged(int i)
{
	switch (i)
	{
	case QButton::On:
		Data().getCurrentRoom()->roomLight=DARK;
		break;
	case QButton::Off:
		Data().getCurrentRoom()->roomLight=LIT;
		break;
	default:
		return;
	}
	emit roomChanged(Data().getCurrentRoom());
}

void RoomEditDialogImpl::IndoorChanged(int i)
{
	switch (i)
	{
	case QButton::On:
		Data().getCurrentRoom()->roomLocation=INDOOR;
		break;
	case QButton::Off:
		Data().getCurrentRoom()->roomLocation=OUTSIDE;
		break;
	default:
		return;
	}
	emit roomChanged(Data().getCurrentRoom());
}

void RoomEditDialogImpl::PortableChanged(int i)
{
	switch (i)
	{
	case QButton::On:
		Data().getCurrentRoom()->roomPortable=PORT;
		break;
	case QButton::Off:
		Data().getCurrentRoom()->roomPortable=NOPORT;
		break;
	default:
		return;
	}
	emit roomChanged(Data().getCurrentRoom());
}

void RoomEditDialogImpl::alignTypeActivated(int i)
{
	Room *r =Data().getCurrentRoom();
	r->roomAlign=(RoomAlignType) i;

	emit roomChanged(r);
}

//field, wood, dense wood, sand, mountain, water, underwater, town, cave, road, dt
void RoomEditDialogImpl::terrainTypeActivated(int i)
{
	Room *r = Data().getCurrentRoom();
	r->roomTerrain=(RoomTerrainType) i;

	emit roomChanged(r);
	if (r->n) emit roomChanged(ISCONNECTEDTO(r,r->n));
	if (r->s) emit roomChanged(ISCONNECTEDTO(r,r->s));
	if (r->e) emit roomChanged(ISCONNECTEDTO(r,r->e));
	if (r->w) emit roomChanged(ISCONNECTEDTO(r,r->w));
}

void RoomEditDialogImpl::mobTypeActivated(int i)
{
	Room *r = Data().getCurrentRoom();
	r->roomMob=(RoomMobType) i;

	emit roomChanged(r);    
}

void RoomEditDialogImpl::loadTypeActivated(int i)
{
	Room *r = Data().getCurrentRoom();
	r->roomLoad=(RoomLoadType) i;

	emit roomChanged(r);    
}


void RoomEditDialogImpl::roomNameEditChanged(const QString& str)
{
	Data().getCurrentRoom()->roomName=str;
	emit roomChanged(Data().getCurrentRoom());
}

void RoomEditDialogImpl::roomDescEditChanged()
{
	if (!Data().getCurrentRoom())
		return;
	Data().getCurrentRoom()->roomDesc=roomDescEdit->text();
}


void RoomEditDialogImpl::doorNorthTypeChanged(int i)
{
	Room *room;
	if ((room = Data().getCurrentRoom())->n){
		SETDOORTYPE(room,room->n,(DoorType) i);
		emit roomChanged(room);
	}
}

void RoomEditDialogImpl::doorSouthTypeChanged(int i)
{
	Room *room;
	if ((room = Data().getCurrentRoom())->s){
		SETDOORTYPE(room,room->s,(DoorType) i);
		emit roomChanged(room);
	}
}

void RoomEditDialogImpl::doorEastTypeChanged(int i)
{
	Room *room;
	if ((room = Data().getCurrentRoom())->e){
		SETDOORTYPE(room,room->e,(DoorType) i);
		emit roomChanged(room);
	}
}

void RoomEditDialogImpl::doorWestTypeChanged(int i)
{
	Room *room;
	if ((room = Data().getCurrentRoom())->w){
		SETDOORTYPE(room,room->w,(DoorType) i);
		emit roomChanged(room);
	}
}

void RoomEditDialogImpl::doorUpTypeChanged(int i)
{
	Room *room;
	if ((room = Data().getCurrentRoom())->u){
		SETDOORTYPE(room,room->u,(DoorType) i);
		emit roomChanged(room);
	}
}

void RoomEditDialogImpl::doorDownTypeChanged(int i)
{
	Room *room;
	if ((room = Data().getCurrentRoom())->d){
		SETDOORTYPE(room,room->d,(DoorType) i);
		emit roomChanged(room);
	}
}


void RoomEditDialogImpl::doorDownEditChanged(const QString& str)
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->d)
		return;
	SETDOORNAME(room,room->d,str);
	emit roomChanged(room);
}

void RoomEditDialogImpl::doorUpEditChanged(const QString& str)
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->u)
		return;
	SETDOORNAME(room,room->u,str);
	emit roomChanged(room);
}

void RoomEditDialogImpl::doorWestEditChanged(const QString& str)
{
	Room *room = Data().getCurrentRoom();  
	if (!room || !room->w)
		return;
	SETDOORNAME(room,room->w,str);
	emit roomChanged(room);
}

void RoomEditDialogImpl::doorEastEditChanged(const QString& str)
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->e)
		return;
	SETDOORNAME(room,room->e,str);
	emit roomChanged(room);
}

void RoomEditDialogImpl::doorSouthEditChanged(const QString& str)
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->s)
		return;
	SETDOORNAME(room,room->s,str);
	emit roomChanged(room);
}

void RoomEditDialogImpl::doorNorthEditChanged(const QString& str)
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->n)
		return;
	SETDOORNAME(room,room->n,str);
	emit roomChanged(room);
}


void RoomEditDialogImpl::connectionDownTypeChanged()
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->d)
		return;
	room->d->type=CONNECTION_NORMAL;
	if ( connectionDownType->isSelected(0) )
		SET(room->d->type,CONNECTION_DOOR);
	if ( connectionDownType->isSelected(1) )
		SET(room->d->type,CONNECTION_CLIMB);
	disableDoorTypes();
	emit roomChanged(ISCONNECTEDTO(room,room->d));  
	updateDialog();
	emit roomChanged(room);
}


void RoomEditDialogImpl::connectionUpTypeChanged()
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->u)
		return;
	room->u->type=CONNECTION_NORMAL;
	if ( connectionUpType->isSelected(0) )
		SET(room->u->type,CONNECTION_DOOR);
	if ( connectionUpType->isSelected(1) )
		SET(room->u->type,CONNECTION_CLIMB);
	disableDoorTypes();
	emit roomChanged(ISCONNECTEDTO(room,room->u));  
	updateDialog();
	emit roomChanged(room);
}


void RoomEditDialogImpl::connectionWestTypeChanged()
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->w)
		return;
	room->w->type=CONNECTION_NORMAL;
	if ( connectionWestType->isSelected(0) )
		SET(room->w->type,CONNECTION_DOOR);
	if ( connectionWestType->isSelected(1) )
		SET(room->w->type,CONNECTION_CLIMB);
	disableDoorTypes();
	emit roomChanged(ISCONNECTEDTO(room,room->w));
	updateDialog();
	emit roomChanged(room);
}


void RoomEditDialogImpl::connectionEastTypeChanged()
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->e)
		return;
	room->e->type=CONNECTION_NORMAL;
	if ( connectionEastType->isSelected(0) )
		SET(room->e->type,CONNECTION_DOOR);
	if ( connectionEastType->isSelected(1) )
		SET(room->e->type,CONNECTION_CLIMB);
	disableDoorTypes();
	emit roomChanged(ISCONNECTEDTO(room,room->e));
	updateDialog();
	emit roomChanged(room);
}


void RoomEditDialogImpl::connectionSouthTypeChanged()
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->s)
		return;
	room->s->type=CONNECTION_NORMAL;
	if ( connectionSouthType->isSelected(0) )
		SET(room->s->type,CONNECTION_DOOR);
	if ( connectionSouthType->isSelected(1) )
		SET(room->s->type,CONNECTION_CLIMB);
	disableDoorTypes();
	emit roomChanged(ISCONNECTEDTO(room,room->s));
	updateDialog();
	emit roomChanged(room);
}


void RoomEditDialogImpl::connectionNorthTypeChanged()
{
	Room *room = Data().getCurrentRoom();
	if (!room || !room->n)
		return;
	room->n->type=CONNECTION_NORMAL;
	if ( connectionNorthType->isSelected(0) )
		SET(room->n->type,CONNECTION_DOOR);
	if ( connectionNorthType->isSelected(1) )
		SET(room->n->type,CONNECTION_CLIMB);
	disableDoorTypes();
	emit roomChanged(ISCONNECTEDTO(room,room->n));  
	updateDialog();
	emit roomChanged(room);

}


// *************************************************************

void RoomEditDialogImpl::createClicked(){
	if (Data().getCurrentRoom()) return;
	emit selectRoom(Data().createAndConnectNewRoom(Data().rX,Data().rY));
}

void RoomEditDialogImpl::deleteClicked()
{
	Data().removeCurrentRoom();
}

void RoomEditDialogImpl::moveNorthClicked()
{
	Data().moveNorth();
}

void RoomEditDialogImpl::moveSouthClicked()
{
	Data().moveSouth();
}

void RoomEditDialogImpl::moveEastClicked()
{
	Data().moveEast();
}

void RoomEditDialogImpl::moveWestClicked()
{
	Data().moveWest();
}

void RoomEditDialogImpl::newRoomNorthClicked()
{
	Room *room=Data().getCurrentRoom();  
	if (!room) {createClicked();return;}
	Room *r;
	if ((r=Data().createAndConnectNewRoom(room->mapx,room->mapy-1)) ){
		emit selectRoom(r);
	}
}

void RoomEditDialogImpl::newRoomSouthClicked()
{
	Room *room=Data().getCurrentRoom();  
	if (!room) {createClicked();return;}
	Room *r;
	if ((r=Data().createAndConnectNewRoom(room->mapx,room->mapy+1)) ){
		emit selectRoom(r);
	}
}

void RoomEditDialogImpl::newRoomEastClicked()
{
	Room *room=Data().getCurrentRoom();
	if (!room) {createClicked();return;}
	Room *r;
	if ((r=Data().createAndConnectNewRoom(room->mapx+1,room->mapy)) ){
		emit selectRoom(r);
	}
}

void RoomEditDialogImpl::newRoomWestClicked()
{
	Room *room=Data().getCurrentRoom();
	if (!room) {createClicked();return;}
	Room *r;
	if ((r=Data().createAndConnectNewRoom(room->mapx-1,room->mapy)) ){
		emit selectRoom(r);
	}
}

void RoomEditDialogImpl::noRoom2Error()
{
	QMessageBox::critical( this, tr("MMapper error"),
		tr("You must select source and destination\n"
		"rooms, to be able to connect them together.\n"
		"To do so, select source room with left mouse button\n"
		"and destination room with right mouse button."));
}

void RoomEditDialogImpl::connectDownClicked()
{
	Room *room=Data().getCurrentRoom();  
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}
	disableTab(1);
	disableTab(2);
	disableTab(3);
	disableTab(4);
	roomDeleteButton->setEnabled(FALSE);
	disableNewRoomButtons();
	disableConnectButtons();
	disableDisconnectButtons();
	enableConnectToButtons();
	lastConnectClicked=1;
}

void RoomEditDialogImpl::connectUpClicked()
{
	Room *room=Data().getCurrentRoom();  
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}
	disableTab(1);
	disableTab(2);
	disableTab(3);
	disableTab(4);
	roomDeleteButton->setEnabled(FALSE);
	disableNewRoomButtons();
	disableConnectButtons();
	disableDisconnectButtons();
	enableConnectToButtons();
	lastConnectClicked=2;
}

void RoomEditDialogImpl::connectNorthClicked()
{
	Room *room=Data().getCurrentRoom();  
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}
	disableTab(1);
	disableTab(2);
	disableTab(3);
	disableTab(4);
	roomDeleteButton->setEnabled(FALSE);
	disableNewRoomButtons();
	disableConnectButtons();
	disableDisconnectButtons();
	enableConnectToButtons();
	lastConnectClicked=3;
}

void RoomEditDialogImpl::connectSouthClicked()
{
	Room *room=Data().getCurrentRoom();  
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}
	disableTab(1);
	disableTab(2);
	disableTab(3);
	disableTab(4);
	roomDeleteButton->setEnabled(FALSE);
	disableNewRoomButtons();
	disableConnectButtons();
	disableDisconnectButtons();
	enableConnectToButtons();
	lastConnectClicked=4;
}

void RoomEditDialogImpl::connectEastClicked()
{
	Room *room=Data().getCurrentRoom();  
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}
	disableTab(1);
	disableTab(2);
	disableTab(3);
	disableTab(4);
	roomDeleteButton->setEnabled(FALSE);
	disableNewRoomButtons();
	disableConnectButtons();
	disableDisconnectButtons();
	enableConnectToButtons();
	lastConnectClicked=5;
}

void RoomEditDialogImpl::connectWestClicked()
{
	Room *room=Data().getCurrentRoom();  
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}
	disableTab(1);
	disableTab(2);
	disableTab(3);
	disableTab(4);
	roomDeleteButton->setEnabled(FALSE);
	disableNewRoomButtons();
	disableConnectButtons();
	disableDisconnectButtons();
	enableConnectToButtons();
	lastConnectClicked=6;
}


void RoomEditDialogImpl::disconnectDownClicked()
{
	Room *room=Data().getCurrentRoom();  
	if (!room) return;
	Data().removeConnection(room->d);
	emit selectRoom(room);
}

void RoomEditDialogImpl::disconnectUpClicked()
{
	Room *room=Data().getCurrentRoom();
	if (!room) return;
	Data().removeConnection(room->u);
	emit selectRoom(room);
}

void RoomEditDialogImpl::disconnectNorthClicked()
{
	Room *room=Data().getCurrentRoom();
	if (!room) return;
	Data().removeConnection(room->n);
	emit selectRoom(room);
}

void RoomEditDialogImpl::disconnectSouthClicked()
{
	Room *room=Data().getCurrentRoom();
	if (!room) return;
	Data().removeConnection(room->s);
	emit selectRoom(room);
}

void RoomEditDialogImpl::disconnectEastClicked()
{
	Room *room=Data().getCurrentRoom();
	if (!room) return;
	Data().removeConnection(room->e);
	emit selectRoom(room);
}

void RoomEditDialogImpl::disconnectWestClicked()
{
	Room *room=Data().getCurrentRoom();
	if (!room) return;
	Data().removeConnection(room->w);
	emit selectRoom(room);
}


void RoomEditDialogImpl::connectToClicked()
{
	Room *room=Data().getCurrentRoom();
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}

	Connection *c=Data().createNewConnection();
	c->room1=room;
	c->room2=room2;
	c->type=CONNECTION_NORMAL | CONNECTION_ONEWAY;
	if (room==room2)
		c->type |= CONNECTION_LOOP;

	switch (lastConnectClicked)
	{
	case 1:
		room->d=c;
		break;
	case 2:
		room->u=c;
		break;
	case 3:
		room->n=c;
		break;
	case 4:
		room->s=c;
		break;
	case 5:
		room->e=c;
		break;
	case 6:
		room->w=c;
		break;
	default:
		;
	};

	emit roomChanged(room2);
	emit roomChanged(room);
	emit selectRoom(room);
}

void RoomEditDialogImpl::connectToDownClicked()
{
	Room *room=Data().getCurrentRoom();
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}

	Connection *c=Data().createNewConnection();
	c->room1=room;
	c->room2=room2;
	if (room==room2)
		c->type = CONNECTION_NORMAL | CONNECTION_LOOP;

	room2->d=c;
	switch (lastConnectClicked)
	{
	case 1:
		room->d=c;
		break;
	case 2:
		room->u=c;
		break;
	case 3:
		room->n=c;
		break;
	case 4:
		room->s=c;
		break;
	case 5:
		room->e=c;
		break;
	case 6:
		room->w=c;
		break;
	default:
		;
	};

	emit roomChanged(room2);
	emit roomChanged(room);
	emit selectRoom(room);
}

void RoomEditDialogImpl::connectToUpClicked()
{
	Room *room=Data().getCurrentRoom();
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}

	Connection *c=Data().createNewConnection();
	c->room1=room;
	c->room2=room2;
	if (room==room2)
		c->type = CONNECTION_NORMAL | CONNECTION_LOOP;

	room2->u=c;
	switch (lastConnectClicked)
	{
	case 1:
		room->d=c;
		break;
	case 2:
		room->u=c;
		break;
	case 3:
		room->n=c;
		break;
	case 4:
		room->s=c;
		break;
	case 5:
		room->e=c;
		break;
	case 6:
		room->w=c;
		break;
	default:
		;
	};

	emit roomChanged(room2);
	emit roomChanged(room);
	emit selectRoom(room);
}

void RoomEditDialogImpl::connectToNorthClicked()
{
	Room *room=Data().getCurrentRoom();
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}

	Connection *c=Data().createNewConnection();
	c->room1=room;
	c->room2=room2;
	if (room==room2)
		c->type = CONNECTION_NORMAL | CONNECTION_LOOP;

	room2->n=c;
	switch (lastConnectClicked)
	{
	case 1:
		room->d=c;
		break;
	case 2:
		room->u=c;
		break;
	case 3:
		room->n=c;
		break;
	case 4:
		room->s=c;
		break;
	case 5:
		room->e=c;
		break;
	case 6:
		room->w=c;
		break;
	default:
		;
	};

	emit roomChanged(room2);
	emit roomChanged(room);
	emit selectRoom(room);
}

void RoomEditDialogImpl::connectToSouthClicked()
{
	Room *room=Data().getCurrentRoom();
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}

	Connection *c=Data().createNewConnection();
	c->room1=room;
	c->room2=room2;
	if (room==room2)
		c->type = CONNECTION_NORMAL | CONNECTION_LOOP;

	room2->s=c;
	switch (lastConnectClicked)
	{
	case 1:
		room->d=c;
		break;
	case 2:
		room->u=c;
		break;
	case 3:
		room->n=c;
		break;
	case 4:
		room->s=c;
		break;
	case 5:
		room->e=c;
		break;
	case 6:
		room->w=c;
		break;
	default:
		;
	};

	emit roomChanged(room2);
	emit roomChanged(room);
	emit selectRoom(room);
}

void RoomEditDialogImpl::connectToEastClicked()
{
	Room *room=Data().getCurrentRoom();
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}

	Connection *c=Data().createNewConnection();
	c->room1=room;
	c->room2=room2;
	if (room==room2)
		c->type = CONNECTION_NORMAL | CONNECTION_LOOP;

	room2->e=c;
	switch (lastConnectClicked)
	{
	case 1:
		room->d=c;
		break;
	case 2:
		room->u=c;
		break;
	case 3:
		room->n=c;
		break;
	case 4:
		room->s=c;
		break;
	case 5:
		room->e=c;
		break;
	case 6:
		room->w=c;
		break;
	default:
		;
	};

	emit roomChanged(room2);
	emit roomChanged(room);
	emit selectRoom(room);
}

void RoomEditDialogImpl::connectToWestClicked()
{
	Room *room=Data().getCurrentRoom();
	Room *room2=Data().getSelectedRoom2();
	if (!room)
		return;
	if (!room2)
	{
		noRoom2Error();
		return;
	}

	Connection *c=Data().createNewConnection();
	c->room1=room;
	c->room2=room2;
	if (room==room2)
		c->type = CONNECTION_NORMAL | CONNECTION_LOOP;

	room2->w=c;
	switch (lastConnectClicked)
	{
	case 1:
		room->d=c;
		break;
	case 2:
		room->u=c;
		break;
	case 3:
		room->n=c;
		break;
	case 4:
		room->s=c;
		break;
	case 5:
		room->e=c;
		break;
	case 6:
		room->w=c;
		break;
	default:
		;
	};

	emit roomChanged(room2);
	emit roomChanged(room);
	emit selectRoom(room);
}




// Enable or Disable routines
// ***********************************************************


void RoomEditDialogImpl::enableTab(int num)
{
	tab->setTabEnabled( tab->page(num), TRUE);
}

void RoomEditDialogImpl::disableTab(int num)
{
	tab->setTabEnabled( tab->page(num), FALSE);
}

void RoomEditDialogImpl::disableConnectButtons()
{
	setConnectionButtonsEnabled(false);
}

void RoomEditDialogImpl::enableConnectButtons()
{
	setConnectionButtonsEnabled(true);
}

void RoomEditDialogImpl::setConnectionButtonsEnabled(const bool enabled)
{
	QWidget *buttons[] = { connectUpButton, connectDownButton, connectEastButton, 
		connectWestButton, connectNorthButton, connectSouthButton };

	enableWidgets(buttons, sizeof(buttons) / sizeof(QWidget *), enabled);
}

void RoomEditDialogImpl::disableDisconnectButtons()
{
	disconnectUpButton->setEnabled(FALSE);
	disconnectDownButton->setEnabled(FALSE);
	disconnectEastButton->setEnabled(FALSE);
	disconnectWestButton->setEnabled(FALSE);
	disconnectNorthButton->setEnabled(FALSE);
	disconnectSouthButton->setEnabled(FALSE);
}

void RoomEditDialogImpl::enableDisconnectButtons()
{
	disconnectUpButton->setEnabled(TRUE);
	disconnectDownButton->setEnabled(TRUE);
	disconnectEastButton->setEnabled(TRUE);
	disconnectWestButton->setEnabled(TRUE);
	disconnectNorthButton->setEnabled(TRUE);
	disconnectSouthButton->setEnabled(TRUE);
}

void RoomEditDialogImpl::disableConnectToButtons()
{
	labConnectTo->setPaletteBackgroundColor(colorGroup().background());
	setConnectionToButtonsEnabled(false);
}

void RoomEditDialogImpl::enableConnectToButtons()
{
	labConnectTo->setPaletteBackgroundColor(Qt::green);
	setConnectionToButtonsEnabled(true);
}

void RoomEditDialogImpl::setConnectionToButtonsEnabled(const bool enable)
{
	QWidget * widgets[] = { connectToUpButton, connectToDownButton,
		connectToEastButton, connectToWestButton,
		connectToNorthButton, connectToSouthButton, connectToButton};

	enableWidgets(widgets, sizeof(widgets) / sizeof(QWidget *), enable);
}

void RoomEditDialogImpl::disableNewRoomButtons()
{
	newNorthButton->setEnabled(FALSE);
	newSouthButton->setEnabled(FALSE);
	newWestButton->setEnabled(FALSE);
	newEastButton->setEnabled(FALSE);
}

void RoomEditDialogImpl::enableNewRoomButtons()
{
	newNorthButton->setEnabled(TRUE);
	newSouthButton->setEnabled(TRUE);
	newWestButton->setEnabled(TRUE);
	newEastButton->setEnabled(TRUE);
}

void RoomEditDialogImpl::disableDoorTypes()
{
	doorNorthType->setEnabled(FALSE);
	doorSouthType->setEnabled(FALSE);
	doorEastType->setEnabled(FALSE);
	doorWestType->setEnabled(FALSE);
	doorUpType->setEnabled(FALSE);
	doorDownType->setEnabled(FALSE);
	doorNorthEdit->setEnabled(FALSE);
	doorSouthEdit->setEnabled(FALSE);
	doorEastEdit->setEnabled(FALSE);
	doorWestEdit->setEnabled(FALSE);
	doorUpEdit->setEnabled(FALSE);
	doorDownEdit->setEnabled(FALSE);
}

void RoomEditDialogImpl::enableWidgets(QWidget **widgets, const uint widgetsSize, const bool enable)
{
	for (uint i = 0; i < widgetsSize; ++i)
	{
		widgets[i]->setEnabled(enable);
	}
}



// Update routines
// ***********************************************************

void RoomEditDialogImpl::updateDialog()
{
	Data().setSelectedRoom2(NULL);
	if (!isVisible()) return;

	Room *room = Data().getCurrentRoom();

	updateInfoObjects();

	if (!room)
	{
		roomCreateButton->setEnabled(TRUE);
		disableTab(3);
		disableTab(2);
		disableTab(1);
		roomDeleteButton->setEnabled(FALSE);
		disableNewRoomButtons();
		disableDisconnectButtons();
		disableConnectButtons();
		return;
	}
	else
	{
		roomCreateButton->setEnabled(FALSE);
		enableTab(4);
		enableTab(3);
		enableTab(2);
		enableTab(1);
		enableTab(0);

		// 0 portable, noport
		switch (comboBoxPortable->currentItem())
		{
		case 1:
			if (room->roomPortable!=PORT)
			{
				room->roomPortable=PORT;
				emit roomChanged(room);
				return;
			}
			break;
		case 2:
			if (room->roomPortable!=NOPORT)
			{
				room->roomPortable=NOPORT;
				emit roomChanged(room);
				return;
			}
			break;
		}

		// 0 inndoor, outdoor
		switch (comboBoxIndoor->currentItem())
		{
		case 1:
			if (room->roomLocation!=INDOOR)
			{
				room->roomLocation=INDOOR;
				emit roomChanged(room);
				return;
			}
			break;
		case 2:
			if (room->roomLocation!=OUTSIDE)
			{
				room->roomLocation=OUTSIDE;
				emit roomChanged(room);
				return;
			}
			break;
		}


		// 0 dark, lit
		switch (comboBoxDark->currentItem())
		{
		case 1:
			if (room->roomLight!=DARK)
			{
				room->roomLight=DARK;
				emit roomChanged(room);
				return;
			}
			break;
		case 2:
			if (room->roomLight!=LIT)
			{
				room->roomLight=LIT;
				emit roomChanged(room);
				return;
			}
			break;
		}

		// 0 good, neutral, evil
		switch (comboBoxAlign->currentItem())
		{
		case 1:
			if (room->roomAlign!=GOOD)
			{
				room->roomAlign=GOOD;
				emit roomChanged(room);
				return;
			}
			break;
		case 2:
			if (room->roomAlign!=NEUTRAL)
			{
				room->roomAlign=NEUTRAL;
				emit roomChanged(room);
				return;
			}
			break;
		case 3:
			if (room->roomAlign!=EVIL)
			{
				room->roomAlign=EVIL;
				emit roomChanged(room);
				return;
			}
			break;
		}


		// 0 nomob, peacefull, aggresive, quest, shop, rent, guild
		if ( (comboBoxMob->currentItem()-1 >= 0) && ((int)room->roomMob != comboBoxMob->currentItem()-1) ){
			room->roomMob = (RoomMobType) (comboBoxMob->currentItem()-1);
			emit roomChanged(room);
		}

		// 0 none, treasure, herb, key
		if ( (comboBoxLoad->currentItem()-1 >= 0) && ((int)room->roomLoad != comboBoxLoad->currentItem()-1) ){
			room->roomLoad = (RoomLoadType) (comboBoxLoad->currentItem()-1);
			emit roomChanged(room);
		}

		// terrain
		if ( (comboBoxTerrain->currentItem()-1 >= 0) && ((int)room->roomTerrain != comboBoxTerrain->currentItem()-1) ){
			room->roomTerrain = (RoomTerrainType) (comboBoxTerrain->currentItem()-1);
			if (room->n) emit roomChanged(ISCONNECTEDTO(room,room->n));
			if (room->s) emit roomChanged(ISCONNECTEDTO(room,room->s));
			if (room->e) emit roomChanged(ISCONNECTEDTO(room,room->e));
			if (room->w) emit roomChanged(ISCONNECTEDTO(room,room->w));
			emit roomChanged(room);
		}

		// update emits inhibited
		emitChangedAllowed = false;

		disableDoorTypes();
		disableConnectToButtons();
		enableNewRoomButtons();
		roomDeleteButton->setEnabled(TRUE);

		roomNameEdit->setText(room->roomName);
		roomDescEdit->setText(room->roomDesc);

		loadType->setCurrentItem((int)room->roomLoad);
		mobType->setCurrentItem((int)room->roomMob);
		terrainType->setCurrentItem((int)room->roomTerrain);
		alignType->setCurrentItem((int)room->roomAlign);


		if (room->roomCheckExits)
			checkExits->setChecked(TRUE);
		else
			checkExits->setChecked(FALSE);


		if (room->roomPortable == PORT)
			checkBoxPortable->setChecked(TRUE);
		else
			checkBoxPortable->setChecked(FALSE);

		if (room->roomLocation == INDOOR)
			checkBoxIndoor->setChecked(TRUE);
		else
			checkBoxIndoor->setChecked(FALSE);

		if (room->roomLight == DARK)
			checkBoxDark->setChecked(TRUE);
		else
			checkBoxDark->setChecked(FALSE);

		if(room->n)
		{
			connectNorthButton->setEnabled(FALSE);
			disconnectNorthButton->setEnabled(TRUE);
			connectionNorthType->setEnabled(TRUE);
			loopLedNt->setEnabled(TRUE);
			onewayLedNt->setEnabled(TRUE);

			if(ISSET(room->n->type,CONNECTION_DOOR))
			{
				connectionNorthType->setSelected(0,TRUE);
				doorNorthEdit->setText(DOORNAME(room,room->n));
				doorNorthEdit->setEnabled(TRUE);
				doorNorthType->setCurrentItem((int)DOORTYPE(room,room->n));
				doorNorthType->setEnabled(TRUE);
			}
			else
			{
				connectionNorthType->setSelected(0,FALSE);
			}

			if(ISSET(room->n->type,CONNECTION_CLIMB))
				connectionNorthType->setSelected(1,TRUE);
			else
				connectionNorthType->setSelected(1,FALSE);

			if(ISSET(room->n->type,CONNECTION_LOOP))
				loopLedN->setChecked(true);
			else
				loopLedN->setChecked(false);

			if(ISSET(room->n->type,CONNECTION_ONEWAY))
				onewayLedN->setChecked(true);
			else
				onewayLedN->setChecked(false);

		}
		else
		{
			connectNorthButton->setEnabled(TRUE);
			disconnectNorthButton->setEnabled(FALSE);
			connectionNorthType->setEnabled(FALSE);
			connectionNorthType->clearSelection();
			loopLedNt->setEnabled(FALSE);
			onewayLedNt->setEnabled(FALSE);
		}

		if(room->s)
		{
			connectSouthButton->setEnabled(FALSE);
			disconnectSouthButton->setEnabled(TRUE);
			connectionSouthType->setEnabled(TRUE);
			loopLedSt->setEnabled(TRUE);
			onewayLedSt->setEnabled(TRUE);

			if(ISSET(room->s->type,CONNECTION_DOOR))
			{
				connectionSouthType->setSelected(0,TRUE);
				doorSouthEdit->setText(DOORNAME(room,room->s));
				doorSouthEdit->setEnabled(TRUE);
				doorNorthType->setCurrentItem((int)DOORTYPE(room,room->s));
				doorSouthType->setEnabled(TRUE);
			}
			else
			{
				connectionSouthType->setSelected(0,FALSE);
			}

			if(ISSET(room->s->type,CONNECTION_CLIMB))
				connectionSouthType->setSelected(1,TRUE);
			else
				connectionSouthType->setSelected(1,FALSE);

			if(ISSET(room->s->type,CONNECTION_LOOP))
				loopLedS->setChecked(true);
			else
				loopLedS->setChecked(false);

			if(ISSET(room->s->type,CONNECTION_ONEWAY))
				onewayLedS->setChecked(true);
			else
				onewayLedS->setChecked(false);
		}
		else
		{
			connectSouthButton->setEnabled(TRUE);
			disconnectSouthButton->setEnabled(FALSE);
			connectionSouthType->setEnabled(FALSE);
			connectionSouthType->clearSelection();
			loopLedSt->setEnabled(FALSE);
			onewayLedSt->setEnabled(FALSE);
		}

		if(room->e)
		{
			connectEastButton->setEnabled(FALSE);
			disconnectEastButton->setEnabled(TRUE);
			connectionEastType->setEnabled(TRUE);
			loopLedEt->setEnabled(TRUE);
			onewayLedEt->setEnabled(TRUE);

			if(ISSET(room->e->type,CONNECTION_DOOR))
			{
				connectionEastType->setSelected(0,TRUE);
				doorEastEdit->setText(DOORNAME(room,room->e));
				doorEastEdit->setEnabled(TRUE);
				doorNorthType->setCurrentItem((int)DOORTYPE(room,room->e));
				doorEastType->setEnabled(TRUE);
			}
			else
			{
				connectionEastType->setSelected(0,FALSE);
			}

			if(ISSET(room->e->type,CONNECTION_CLIMB))
				connectionEastType->setSelected(1,TRUE);
			else
				connectionEastType->setSelected(1,FALSE);

			if(ISSET(room->e->type,CONNECTION_LOOP))
				loopLedE->setChecked(true);
			else
				loopLedE->setChecked(false);

			if(ISSET(room->e->type,CONNECTION_ONEWAY))
				onewayLedE->setChecked(true);
			else
				onewayLedE->setChecked(false);
		}
		else
		{
			connectEastButton->setEnabled(TRUE);
			disconnectEastButton->setEnabled(FALSE);
			connectionEastType->setEnabled(FALSE);
			connectionEastType->clearSelection();
			loopLedEt->setEnabled(FALSE);
			onewayLedEt->setEnabled(FALSE);
		}

		if(room->w)
		{
			connectWestButton->setEnabled(FALSE);
			disconnectWestButton->setEnabled(TRUE);
			connectionWestType->setEnabled(TRUE);
			loopLedWt->setEnabled(TRUE);
			onewayLedWt->setEnabled(TRUE);

			if(ISSET(room->w->type,CONNECTION_DOOR))
			{
				connectionWestType->setSelected(0,TRUE);
				doorWestEdit->setText(DOORNAME(room,room->w));
				doorWestEdit->setEnabled(TRUE);
				doorNorthType->setCurrentItem((int)DOORTYPE(room,room->w));
				doorWestType->setEnabled(TRUE);
			}
			else
			{
				connectionWestType->setSelected(0,FALSE);
			}

			if(ISSET(room->w->type,CONNECTION_CLIMB))
				connectionWestType->setSelected(1,TRUE);
			else
				connectionWestType->setSelected(1,FALSE);

			if(ISSET(room->w->type,CONNECTION_LOOP))
				loopLedW->setChecked(true);
			else
				loopLedW->setChecked(false);

			if(ISSET(room->w->type,CONNECTION_ONEWAY))
				onewayLedW->setChecked(true);
			else
				onewayLedW->setChecked(false);
		}
		else
		{
			connectWestButton->setEnabled(TRUE);
			disconnectWestButton->setEnabled(FALSE);
			connectionWestType->setEnabled(FALSE);
			connectionWestType->clearSelection();
			loopLedWt->setEnabled(FALSE);
			onewayLedWt->setEnabled(FALSE);
		}

		if(room->u)
		{
			connectUpButton->setEnabled(FALSE);
			disconnectUpButton->setEnabled(TRUE);
			connectionUpType->setEnabled(TRUE);
			loopLedUt->setEnabled(TRUE);
			onewayLedUt->setEnabled(TRUE);

			if(ISSET(room->u->type,CONNECTION_DOOR))
			{
				connectionUpType->setSelected(0,TRUE);
				doorUpEdit->setEnabled(TRUE);
				doorUpEdit->setText(DOORNAME(room,room->u));
				doorNorthType->setCurrentItem((int)DOORTYPE(room,room->u));
				doorUpType->setEnabled(TRUE);
			}
			else
			{
				connectionUpType->setSelected(0,FALSE);
			}

			if(ISSET(room->u->type,CONNECTION_CLIMB))
				connectionUpType->setSelected(1,TRUE);
			else
				connectionUpType->setSelected(1,FALSE);

			if(ISSET(room->u->type,CONNECTION_LOOP))
				loopLedU->setChecked(true);
			else
				loopLedU->setChecked(false);

			if(ISSET(room->u->type,CONNECTION_ONEWAY))
				onewayLedU->setChecked(true);
			else
				onewayLedU->setChecked(false);
		}
		else
		{
			connectUpButton->setEnabled(TRUE);
			disconnectUpButton->setEnabled(FALSE);
			connectionUpType->setEnabled(FALSE);
			connectionUpType->clearSelection();
			loopLedUt->setEnabled(FALSE);
			onewayLedUt->setEnabled(FALSE);
		}

		if(room->d)
		{
			connectDownButton->setEnabled(FALSE);
			disconnectDownButton->setEnabled(TRUE);
			connectionDownType->setEnabled(TRUE);
			loopLedDt->setEnabled(TRUE);
			onewayLedDt->setEnabled(TRUE);

			if(ISSET(room->d->type,CONNECTION_DOOR))
			{
				connectionDownType->setSelected(0,TRUE);
				doorDownEdit->setText(DOORNAME(room,room->d));
				doorDownEdit->setEnabled(TRUE);
				doorNorthType->setCurrentItem((int)DOORTYPE(room,room->d));
				doorDownType->setEnabled(TRUE);
			}
			else
			{
				connectionDownType->setSelected(0,FALSE);
			}

			if(ISSET(room->d->type,CONNECTION_CLIMB))
				connectionDownType->setSelected(1,TRUE);
			else
				connectionDownType->setSelected(1,FALSE);

			if(ISSET(room->d->type,CONNECTION_LOOP))
				loopLedD->setChecked(true);
			else
				loopLedD->setChecked(false);

			if(ISSET(room->d->type,CONNECTION_ONEWAY))
				onewayLedD->setChecked(true);
			else
				onewayLedD->setChecked(false);
		}
		else
		{
			connectDownButton->setEnabled(TRUE);
			disconnectDownButton->setEnabled(FALSE);
			connectionDownType->setEnabled(FALSE);
			connectionDownType->clearSelection();
			loopLedDt->setEnabled(FALSE);
			onewayLedDt->setEnabled(FALSE);
		}
	
		emitChangedAllowed = true;
		
		emit roomChanged(room);
	}
}

void RoomEditDialogImpl::updateInfoObjects()
{
	Room *room = Data().getCurrentRoom();  
	InfoItem *item;
	QStringList list;

	long x1b=0,y1b=0,x2b=0,y2b=0;
	//int index=0;
	int selected=objects->currentItem();
	if (room) {
		x1b = ROOMTOVIEWPORT(room->mapx) - 4*MAPROOMSIZE;
		y1b = ROOMTOVIEWPORT(room->mapy) - 4*MAPROOMSIZE;
		x2b = ROOMTOVIEWPORT(room->mapx) + 5*MAPROOMSIZE;
		y2b = ROOMTOVIEWPORT(room->mapy) + 5*MAPROOMSIZE;
	}


	objects->clear();
	for ( item = Data().infoItems.first(); item; item = Data().infoItems.next() )
	{
		if (room){
			if ((int)item->x1>x1b && (int)item->x1<x2b && (int)item->y1>y1b && (int)item->y1<y2b)
				list.append(item->itemName);
		}else
			list.append(item->itemName);
	}
	list.sort();
	objects->insertStringList(list);
	objects->insertItem("Create New Object",0);
	objects->setCurrentItem(selected);

	objectX2->setEnabled(TRUE);
	objectY2->setEnabled(TRUE);

	if (objects->currentItem()!=0)
	{
		item = Data().getInfoItem(objects->currentText());

		objectType->setEnabled(FALSE);
		objectName->setEnabled(FALSE);
		objectText->setEnabled(FALSE);
		objectText->clear();
		switch(item->itemType)
		{
		case text:objectType->setCurrentItem(0);
			objectText->setEnabled(TRUE);
			objectText->setText(item->itemText);
			objectX2->setEnabled(FALSE);
			objectY2->setEnabled(FALSE);
			break;
		case line:objectType->setCurrentItem(1);break;
		case arrow:objectType->setCurrentItem(2);break;
		}
		objectName->setText(item->itemName);
		objectX1->setValue(item->x1);
		objectY1->setValue(item->y1);
		objectX2->setValue(item->x2);
		objectY2->setValue(item->y2);

		objectCreate->setEnabled(FALSE);
		objectModify->setEnabled(TRUE);
		objectDelete->setEnabled(TRUE);

		startLabel->setText("");
		endLabel->setText("");
	}
	else
	{
		switch (pointToSelect)
		{
		case 1:startLabel->setText("Select ...");endLabel->setText("");break;
		case 2:startLabel->setText("");endLabel->setText("Select ...");break;
		default:;
		}

		objectType->setEnabled(TRUE);
		objectName->setEnabled(TRUE);
		if (objectType->currentItem()==0){
			objectText->setEnabled(TRUE);
			objectX2->setEnabled(FALSE);
			objectY2->setEnabled(FALSE);
		}
		else{
			objectText->setEnabled(FALSE);
		}
		objectCreate->setEnabled(TRUE);
		objectModify->setEnabled(FALSE);
		objectDelete->setEnabled(FALSE);
	}
}


//Events
void RoomEditDialogImpl::hideEvent ( QHideEvent * )
{
	if (!isVisible())
		emit dialogClosed();
}

void RoomEditDialogImpl::showEvent ( QShowEvent * )
{
	if (isVisible())
		updateDialog();
}

void RoomEditDialogImpl::mousePressed( QMouseEvent*, double xs, double ys)
{
	if(objects->currentItem()== 0)
		if (objectType->currentItem()==0)
		{
			pointToSelect=1;objectX1->setValue((int)xs);objectY1->setValue((int)ys);
		}
		else
			switch (pointToSelect)
		{
			case 1:pointToSelect=2;objectX1->setValue((int)xs);objectY1->setValue((int)ys);break;
			case 2:pointToSelect=1;objectX2->setValue((int)xs);objectY2->setValue((int)ys);break;
			default:;
		};
}




void RoomEditDialogImpl::objectsActivated(int)
{
	updateInfoObjects();
}

void RoomEditDialogImpl::objectTypeActivated(int)
{
	updateInfoObjects();
}

void RoomEditDialogImpl::objectNameTextChanged(const QString&)// text)
{}

void RoomEditDialogImpl::objectTextChanged(const QString&)// text)
{/*
 InfoItem *item = Data().getInfoItem(objects->currentText());
 if (!item) return;
 item->itemText=text;
 updateInfoObjects();
 emit infoObjectChanged(item);*/
}

void RoomEditDialogImpl::objectX1ValueChanged(int)
{
	/*InfoItem *item = Data().getInfoItem(objects->currentText());
	if (!item) return;
	item->x1 = objectX1->value();
	item->y1 = objectY1->value();
	item->x2 = objectX2->value();
	item->y2 = objectY2->value();

	updateInfoObjects();
	emit infoObjectChanged(item);*/
}

void RoomEditDialogImpl::objectY1ValueChanged(int)
{
	/*InfoItem *item = Data().getInfoItem(objects->currentText());
	if (!item) return;
	item->x1 = objectX1->value();
	item->y1 = objectY1->value();
	item->x2 = objectX2->value();
	item->y2 = objectY2->value();

	updateInfoObjects();
	emit infoObjectChanged(item);*/
}

void RoomEditDialogImpl::objectX2ValueChanged(int)
{
	/* InfoItem *item = Data().getInfoItem(objects->currentText());
	if (!item) return;
	item->x1 = objectX1->value();
	item->y1 = objectY1->value();
	item->x2 = objectX2->value();
	item->y2 = objectY2->value();

	updateInfoObjects();
	emit infoObjectChanged(item);*/
}

void RoomEditDialogImpl::objectY2ValueChanged(int)
{
	/*InfoItem *item = Data().getInfoItem(objects->currentText());
	if (!item) return;
	item->x1 = objectX1->value();
	item->y1 = objectY1->value();
	item->x2 = objectX2->value();
	item->y2 = objectY2->value();

	updateInfoObjects();
	emit infoObjectChanged(item);*/
}

void RoomEditDialogImpl::itemObjectAlreadyExistsError()
{

	QMessageBox::critical( this, tr("MMapper error"),
		tr("Item with this name already exists!!!\n"));
}


void RoomEditDialogImpl::objectCreateClicked()
{
	InfoItem *item;
	for ( item = Data().infoItems.first(); item; item = Data().infoItems.next() )
		if (item->itemName == objectName->text())
		{
			itemObjectAlreadyExistsError();
			return;
		}

		item = Data().createNewInfoObject(objectName->text());

		item->itemName = objectName->text();
		item->itemText = objectText->text();
		switch(objectType->currentItem())
		{
		case 0:item->itemType=text;;break;
		case 1:item->itemType=line;;break;
		case 2:item->itemType=arrow;break;
		}
		item->x1 = objectX1->value();
		item->y1 = objectY1->value();
		item->x2 = objectX2->value();
		item->y2 = objectY2->value();

		updateInfoObjects();
		emit infoObjectChanged(item);
}

void RoomEditDialogImpl::objectModifyClicked()
{
	InfoItem *item = Data().getInfoItem(objects->currentText());

	item->itemName = objectName->text();
	item->itemText = objectText->text();
	switch(objectType->currentItem())
	{
	case 0:item->itemType=text;;break;
	case 1:item->itemType=line;;break;
	case 2:item->itemType=arrow;break;
	}
	item->x1 = objectX1->value();
	item->y1 = objectY1->value();
	item->x2 = objectX2->value();
	item->y2 = objectY2->value();

	updateInfoObjects();
	emit infoObjectChanged(item);
}

void RoomEditDialogImpl::objectDeleteClicked()
{

	InfoItem *item = Data().getInfoItem(objects->currentText());
	Data().removeInfoObject(item);
	updateInfoObjects();
	emit infoObjectChanged(NULL);
}

