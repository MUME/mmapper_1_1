/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef _MMAPPER_H_
#define _MMAPPER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qapplication.h>
#include <qmainwindow.h>

#include <qtimer.h>
#include <qcanvas.h>

#include "mmapperlogview.h"
#include "mapcanvasview.h"
#include "mapcanvas.h"
#include "mapdata.h"
#include "roomeditdialogimpl.h"
#include "commandeval.h"

#include <qaction.h>
#include "proxyserver.h"
#include "clientsocket.h"

#define PLAY_MODE 3
#define FOLLOW_MODE 1
#define EDIT_MODE 2

/**
 * This class serves as the main window for MMapper.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author $AUTHOR <$EMAIL>
 * @version $APP_VERSION
 */

#define KEY_UP    4115
#define KEY_DOWN  4117
#define KEY_LEFT  4114
#define KEY_RIGHT 4116
#define KEY_PGUP  4118
#define KEY_PGDN  4119



/**
 * @short This class serves as the main window for MMapper.
 * 
 * It handles the menus, toolbars, and status bars. 
 *
 * It also includes main alghoritm for synchronization character on map
 * with it's real position on MUD (MUME). Main function called, 
 * whenever new description is recognized by parser is characterMoved().
 * 
 * When user send some MMapper command (command preceded by "_"), this class is notified
 * about it with event from CommandEval class. So all MMapper commands are handled here. 
 *
 * @see CommandEval
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MMapper : public QMainWindow {
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    MMapper();

    /**
     * Default Destructor
     */
    virtual ~MMapper();

    
signals:
    void sendToClient(const QString&);
    void sendToServer(const QString&);

    
protected:
    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

    virtual void keyPressEvent( QKeyEvent* e );
	virtual void closeEvent(QCloseEvent * e);
       
    Room* checkNeighbourRooms(int,const QString&, const QString&, const QString&, Room*, Room*, Q_UINT32, Q_UINT32, bool* );
    Room* mapSync(const QString&, const QString&, const QString&, Room*, Q_UINT32, Q_UINT32);
    void mapSyncDirection(const QString&,const QString&, const QString&, Room*, Connection*, Q_UINT32,			  Q_UINT32, DirectionType);
    
    void follow(Room *r, Connection *c);
    void logRoomInfo(Room *);

public slots:
    
    void mousePressed( QMouseEvent*, double, double );
    void mouseMoved( QMouseEvent*, double, double );
    void mouseDoubleClicked( QMouseEvent*, double, double );
    
    void characterMoved(Command*,const QString&, const QString&,const QString&, Q_UINT32, Q_UINT32);
    
    //commands
    void syncCommand(const QString&, const QString&, const QString&, Q_UINT32, Q_UINT32);
    void setInfoCommand(const QString&, const QString&, Q_UINT32, Q_UINT32);
    void resetCommand();
    void performDoorCommand(DirectionType dir, DoorActionType action, QPtrQueue<Command> queue);
    void genericDoorCommand(QString command, DirectionType dir, QPtrQueue<Command> queue);
    void setDoorCommand(QString doorname, DirectionType dir, QPtrQueue<Command> queue);

    //messages
    void syncOffMessage();
    

private slots:
    
    void newConnect( ClientSocket * );
    void connectionClosed ();

    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    
    void optionsShowLogView(bool);
    void optionsShowToolbar1();        
    void optionsShowToolbar2();        
    void optionsShowToolbar3();        
    void optionsShowToolbar4();        

    void optionsShowStatusbar();
    void optionsConfigureToolbars();
    void optionsConfigureKeys();
    void optionsPreferences();
    
    void moveNorth();
    void moveSouth();
    void moveEast();
    void moveWest();
    void roomDelete();
    
    void applyNewConfig();
    void saveNewConfig();

    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);

    void pressedMode1Button();
    void pressedMode2Button();
    void pressedMode3Button();
    void reparseButtonPressed();
    void redrawWorldButtonPressed();
    
    void roomEditDialogClosed();
    
    void disconnectRoomNorth();
    void disconnectRoomSouth();
    void disconnectRoomEast();
    void disconnectRoomWest();
    
    void newRoomNorth();
    void newRoomSouth();
    void newRoomEast();
    void newRoomWest();
    
    void selectLeft();
    void selectRight();
    void selectUp();
    void selectDown();
    
    void zoomIn();
    void zoomOut();
	void zoomAll();
    
	void helpIndex();
	void aboutMapper();

    void init();

private:

    void setupAccel();
    void setupActions();

    int lastMode;
    bool sync; // synchronized map with mud
    
    MMapperLogView *m_log_view;

    QToolBar *m_tool_bar;

    MapCanvasView *m_canvas_view;
    RoomEditDialogImpl *m_edit_dialog;
    
    QAction *m1;
    QAction *m2;
    QAction *m3;
    
    QAction *showtoolbarmain;
    QAction *showtoolbarmode;
    QAction *showtoolbartools;
    QAction *showtoolbaredit;
    QAction *showstatusbar;
    QAction *showlogview;

	QAction *fileOpenAction, *fileExitAction, *fileSaveAction, *fileSaveAsAction, *fileNewAction,
        *showLogViewAction, *preferencesAction, *redrawAction;

    friend class RoomEditDialogImpl;
};

#endif // _MMAPPER_H_
