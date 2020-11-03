/***************************************************************************
*   Copyright (C) 2003 by Marek Krejza                                    *
*   mkrejza@cbox.cz                                                       *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef ROOMEDITDIALOGIMPL_H
#define ROOMEDITDIALOGIMPL_H

#include <qobject.h>
#include <qwidget.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qmessagebox.h>

#include "roomeditdialog.h"
#include "mapdata.h"


/**
* @short This is implementaion of RoomEditDialog class.
* 
* This is base edit dialog, with a lot of functionality. It allows user to edit 
* MUD world map. This dialog is automaticaly opened, when user switches to edit mode.
* @see MapCanvas, MapCanvasView, MapData, Room, Connection, InfoItem, MMapper
* @author Marek Krejza <mkrejza@cbox.cz>
*/
class RoomEditDialogImpl : public RoomEditDialog
{
	Q_OBJECT
public:
	RoomEditDialogImpl(QWidget *parent = 0, const char *name = 0, WFlags f = 0);

	~RoomEditDialogImpl();

	virtual void keyPressEvent( QKeyEvent* e );


signals:
	void roomChanged(Room*);
	void infoObjectChanged(InfoItem*);
	void selectRoom(Room*);
	void selectRoom2(Room*);
	void selectRoom(unsigned, unsigned);
	void dialogClosed();

public slots:
	void mousePressed( QMouseEvent*, double, double );

	void objectsActivated(int);
	void objectTypeActivated(int);
	void objectNameTextChanged(const QString&);
	void objectTextChanged(const QString&);
	void objectX1ValueChanged(int);
	void objectY1ValueChanged(int);
	void objectX2ValueChanged(int);
	void objectY2ValueChanged(int);
	void objectCreateClicked();
	void objectModifyClicked();
	void objectDeleteClicked();

	void updateDialog();
	void updateInfoObjects();

	void moveNorthClicked();
	void moveSouthClicked();
	void moveEastClicked();
	void moveWestClicked();

	void newRoomNorthClicked();
	void newRoomSouthClicked();
	void newRoomEastClicked();
	void newRoomWestClicked();

	void connectDownClicked();
	void connectUpClicked();
	void connectNorthClicked();
	void connectSouthClicked();
	void connectEastClicked();
	void connectWestClicked();

	void disconnectDownClicked();
	void disconnectUpClicked();
	void disconnectNorthClicked();
	void disconnectSouthClicked();
	void disconnectEastClicked();
	void disconnectWestClicked();

	void connectToClicked();
	void connectToDownClicked();
	void connectToUpClicked();
	void connectToNorthClicked();
	void connectToSouthClicked();
	void connectToEastClicked();
	void connectToWestClicked();

	void deleteClicked();
	void createClicked();

	void DarkChanged(int);
	void IndoorChanged(int);
	void PortableChanged(int);
	void CheckExitsChanged(int);

	void alignTypeActivated(int);
	void terrainTypeActivated(int);
	void mobTypeActivated(int);
	void loadTypeActivated(int);

	void roomNameEditChanged(const QString&);
	void roomDescEditChanged();

	void doorDownTypeChanged(int);
	void doorUpTypeChanged(int);
	void doorWestTypeChanged(int);
	void doorEastTypeChanged(int);
	void doorSouthTypeChanged(int);
	void doorNorthTypeChanged(int);

	void doorDownEditChanged(const QString&);
	void doorUpEditChanged(const QString&);
	void doorWestEditChanged(const QString&);
	void doorEastEditChanged(const QString&);
	void doorSouthEditChanged(const QString&);
	void doorNorthEditChanged(const QString&);

	void connectionDownTypeChanged();
	void connectionUpTypeChanged();
	void connectionWestTypeChanged();
	void connectionEastTypeChanged();
	void connectionSouthTypeChanged();
	void connectionNorthTypeChanged();

protected:

	int pointToSelect;

	virtual void hideEvent ( QHideEvent * );
	virtual void showEvent ( QShowEvent * );

	int lastConnectClicked;

	void enableTab(int num);
	void disableTab(int num);

	void noRoom2Error();
	void itemObjectAlreadyExistsError();

	void enableNewRoomButtons();
	void disableNewRoomButtons();
	void enableConnectToButtons();
	void disableConnectToButtons();
	void enableDisconnectButtons();
	void disableDisconnectButtons();
	void enableConnectButtons();
	void disableConnectButtons();
	void disableDoorTypes();

private:
	/// if true, prevents emitting of Changed triggers - they would update the same room
	/// many times while dialog is filled with data
	bool emitChangedAllowed;

	static void enableWidgets(QWidget **widgets, const uint size, const bool enable);

	void setConnectionButtonsEnabled(const bool enable);
	void setConnectionToButtonsEnabled(const bool enable);
};

#endif
