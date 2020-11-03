/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "mmapper.h"
//#include <kstandarddirs.h>

#include "pref.h"
#include <qnamespace.h>
#include <qbuttongroup.h>
#include <qsplitter.h>
#include <qmessagebox.h>

#include <qdragobject.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

//#include <kglobal.h>
#include <qlocale.h>
//#include <kiconloader.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qaccel.h>
#include <qfiledialog.h>
#include <qconfig.h>
#include <qurl.h>

//#include <kedittoolbar.h>

//#include <kstdaccel.h>
#include <qaction.h>

#include <qlayout.h>
#include <qhbox.h>

#include "ContextHelper.h"
#include "HelpWindow.h"

int mode; // follow, edit, online

MMapper::MMapper() : QMainWindow( 0, "MMapper" ) {

    sync = FALSE;

    // set default parser regexps if first time run
    if (Config().m_firstRun) {
        MMapperPreferences dlg(this);
        dlg.updateDialog();
        dlg.resetPatternsClicked();
        dlg.slotApply();
    }

    // accept dnd
    setAcceptDrops(true);

    // edit Dialog
    m_edit_dialog = new RoomEditDialogImpl(this);

    QSplitter *container = new QSplitter(this);
    container->setOrientation(Qt::Vertical);

    // canvas view
    m_canvas_view = new MapCanvasView ( Canvas(), container );

    // log window
    m_log_view = new MMapperLogView(container);

    m_log_view->log(QString("[MMapper] Server bound to port: %1").arg(Config().m_localPort));

    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(container);

    //m_canvas_view->setFocus();
    m_log_view->setFocusPolicy(QWidget::StrongFocus);
    m_canvas_view->setFocusPolicy(QWidget::StrongFocus);


    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();

    // setAutoSaveSettings();

    m_canvas_view->setEnabled(TRUE);
    m_canvas_view->setFocus();

    // screen update handlers
    connect(&Data(), SIGNAL(logText(const QString& )), m_log_view, SLOT(log(const QString& )));
    connect(&Data(), SIGNAL(logDebug(const QString& )), m_log_view, SLOT(logDebug(const QString& )));
    connect(&Data(), SIGNAL(roomChanged(Room*)), &Canvas(), SLOT(updateRoom(Room*)));
    connect(&Data(), SIGNAL(infoObjectChanged(InfoItem*)), &Canvas(), SLOT(updateInfoObject(InfoItem*)));
    connect(m_edit_dialog, SIGNAL(roomChanged(Room*)), &Canvas(), SLOT(updateRoom(Room*)));
    connect(m_edit_dialog, SIGNAL(infoObjectChanged(InfoItem*)), &Canvas(), SLOT(updateInfoObject(InfoItem*)));

    // room select handlers
    connect(m_edit_dialog, SIGNAL(selectRoom(Room*)),m_canvas_view, SLOT(selectRoom(Room*)));
    connect(m_edit_dialog, SIGNAL(selectRoom2(Room*)),m_canvas_view, SLOT(selectRoom2(Room*)));
    connect(&Data(), SIGNAL(selectRoom(Room*)),m_canvas_view, SLOT(selectRoom(Room*)));
    connect(&Data(), SIGNAL(selectRoom2(Room*)),m_canvas_view, SLOT(selectRoom2(Room*)));

    connect(m_edit_dialog, SIGNAL(selectRoom(unsigned,unsigned)),m_canvas_view, SLOT(selectRoom(unsigned,unsigned)));

    // update dialog handlers
    connect(m_canvas_view, SIGNAL(roomSelected()),m_edit_dialog, SLOT(updateDialog()));


    // dialog closed handler
    connect(m_edit_dialog, SIGNAL(dialogClosed()), SLOT(roomEditDialogClosed()));

    // mouse events handlers
    connect(m_canvas_view, SIGNAL(mousePressed(QMouseEvent*,double,double)), m_edit_dialog, SLOT(mousePressed(QMouseEvent*,double,double)));

    connect(m_canvas_view, SIGNAL(mousePressed(QMouseEvent*,double,double)), SLOT(mousePressed(QMouseEvent*,double,double)));
    connect(m_canvas_view, SIGNAL(mouseMoved(QMouseEvent*,double,double)), SLOT(mouseMoved(QMouseEvent*,double,double)));
    connect(m_canvas_view, SIGNAL(mouseDoubleClicked(QMouseEvent*,double,double)), SLOT(mouseDoubleClicked(QMouseEvent*,double,double)));

    //connect parser
    connect ( &Parser(), SIGNAL(showPath(QPtrQueue<Command>)),
              &Canvas(), SLOT(showPath(QPtrQueue<Command>)) );
    connect ( &Parser(), SIGNAL(logText(const QString&)),
              m_log_view, SLOT(log(const QString&)) );
    connect ( &Parser(), SIGNAL(characterMoved(Command*,const QString& ,const QString& ,const QString&, Q_UINT32, Q_UINT32 )), SLOT(characterMoved(Command*,const QString&,const QString& , const QString&, Q_UINT32, Q_UINT32 )));
    connect ( &Parser(), SIGNAL(syncCommand(const QString&,const QString& , const QString&, Q_UINT32, Q_UINT32 )),
              SLOT(syncCommand(const QString&,const QString& , const QString&, Q_UINT32, Q_UINT32 )) );
    connect ( &Parser(), SIGNAL(setInfoCommand(const QString&, const QString&, Q_UINT32, Q_UINT32)),
              SLOT(setInfoCommand(const QString&, const QString&, Q_UINT32, Q_UINT32)) );
    connect ( &Parser(), SIGNAL(syncOffMessage()),
              SLOT(syncOffMessage()) );
    connect ( &Parser(), SIGNAL(resetCommand()),
              SLOT(resetCommand()) );
    connect ( &Parser(), SIGNAL(performDoorCommand(DirectionType, DoorActionType, QPtrQueue<Command>)),
              SLOT(performDoorCommand(DirectionType, DoorActionType, QPtrQueue<Command>)) );
    connect ( &Parser(), SIGNAL(genericDoorCommand(QString, DirectionType, QPtrQueue<Command>)),
              SLOT(genericDoorCommand(QString, DirectionType, QPtrQueue<Command>)) );
    connect ( &Parser(), SIGNAL(setDoorCommand(QString, DirectionType, QPtrQueue<Command>)),
              SLOT(setDoorCommand(QString, DirectionType, QPtrQueue<Command>)) );

    
    connect ( &Parser(), SIGNAL(setFollowMode()), SLOT(pressedMode1Button()) );
    connect ( &Parser(), SIGNAL(setEditMode()), SLOT(pressedMode2Button()) );
    connect ( &Parser(), SIGNAL(setPlayMode()), SLOT(pressedMode3Button()) );

    //proxy
    connect ( &Server(), SIGNAL(newConnect( ClientSocket * )),  SLOT(newConnect( ClientSocket * )));

	init();
}

void MMapper::init() {
	setIcon( QPixmap::fromMimeSource( "hi32-app-mmapper.png" ) );

	// actions

	/*
    if (toolBar("mainToolBar")->isVisible())
        showtoolbarmain->setChecked(TRUE);
    else
        showtoolbarmain->setChecked(FALSE);

    if (toolBar("modeToolBar")->isVisible())
        showtoolbarmode->setChecked(TRUE);
    else
        showtoolbarmode->setChecked(FALSE);

    if (toolBar("toolsToolBar")->isVisible())
        showtoolbartools->setChecked(TRUE);
    else
        showtoolbartools->setChecked(FALSE);

    if (toolBar("editToolBar")->isVisible())
        showtoolbaredit->setChecked(TRUE);
    else
        showtoolbaredit->setChecked(FALSE);
    if (statusBar()->isVisible())
        showstatusbar->setChecked(TRUE);
    else
        showstatusbar->setChecked(FALSE);

    if (m_log_view->isVisible())
        showlogview->setChecked(TRUE);
    else
        showlogview->setChecked(FALSE);

		*/

	if (Config().m_autoLoadWorld && !Config().m_autoLoadFileName.isEmpty() &&
            Data().loadData(Config().m_autoLoadFileName)) {
        Data().setFileName(Config().m_autoLoadFileName);
        m_canvas_view->selectRoom(Data().getSelectedRoom1x(),Data().getSelectedRoom1y());
        changeCaption(Data().getFileName());
    };
}

MMapper::~MMapper() {}


// *******************************************
// Action slots
// *******************************************

void MMapper::dragEnterEvent(QDragEnterEvent *) {}

void MMapper::dropEvent(QDropEvent *) {}

void MMapper::fileNew() {
    if(Data().dataChanged()) {
		const int answer = QMessageBox::question(this, tr("MMapper open confirm"),
                        tr("New data will overwrite all changes you made.\n"
                           "Do you really want to create new data?"),
                        QMessageBox::Yes, QMessageBox::No);
        switch( answer ) {
        case QMessageBox::Yes:
            break;
        case QMessageBox::No:
            return;
            break;
        }
    }

    Data().setFileName("");
    Data().deleteData();
    Canvas().update();
    m_canvas_view->selectRoom(1,1);
    changeCaption("noname");
}

void MMapper::fileOpen() {
	if (Data().dataChanged()) {
		const int answer = QMessageBox::question(this, tr("MMapper open confirm"),
			tr("Loading new data will overwrite all changes you made.\n"
			"Do you really want to load new data?"),
			QMessageBox::Yes, QMessageBox::No);

        if ( answer != QMessageBox::Yes ) return;
	}
	QString url = QFileDialog::getOpenFileName();
	if (! url.isEmpty()) {
		Data().loadData(url);
		m_canvas_view->selectRoom(Data().getSelectedRoom1x(),Data().getSelectedRoom1y());
		changeCaption(url);
	}
}

void MMapper::fileSave() {
	if (Data().dataChanged() && !Data().getFileName().isEmpty()) {
		const int answer = QMessageBox::question( this, tr("MMapper save confirm"),
			tr("Saving the file will overwrite the original file on the disk.\n"
			"Do you really want to save?"),
			QMessageBox::Yes, QMessageBox::No);

		if ( answer != QMessageBox::Yes ) return;
	}

    if (!Data().getFileName().isEmpty()) {
        Data().saveData(Data().getFileName());
    } else
        fileSaveAs();
}

void MMapper::fileSaveAs() {
    const QString url = QFileDialog::getSaveFileName();

    if (!url.isEmpty()) {
    //if (!url.isEmpty() && !url.isMalformed()) {
        Data().saveData(url);
        changeCaption(url);
    }
}

void MMapper::filePrint() {}

void MMapper::optionsShowLogView(bool on) {
	m_log_view->setShown(on);
}

void MMapper::optionsShowToolbar1() {
    //if (toolBar("mainToolBar")->isVisible())
    //    toolBar("mainToolBar")->hide();
    //else
    //    toolBar("mainToolBar")->show();
    //saveNewConfig();
}

void MMapper::optionsShowToolbar2() {
    //if (toolBar("toolsToolBar")->isVisible())
    //    toolBar("toolsToolBar")->hide();
    //else
    //    toolBar("toolsToolBar")->show();
    //saveNewConfig();
}

void MMapper::optionsShowToolbar3() {
    //if (toolBar("modeToolBar")->isVisible())
    //    toolBar("modeToolBar")->hide();
    //else
    //    toolBar("modeToolBar")->show();
    //saveNewConfig();
}

void MMapper::optionsShowToolbar4() {
    //if (toolBar("editToolBar")->isVisible())
    //    toolBar("editToolBar")->hide();
    //else
    //    toolBar("editToolBar")->show();
    //saveNewConfig();
}

void MMapper::optionsShowStatusbar() {
    //if (statusBar()->isVisible())
    //    statusBar()->hide();
    //else
    //    statusBar()->show();
    //saveNewConfig();
}

void MMapper::optionsPreferences() {
    MMapperPreferences dlg(this);

	dlg.exec();
}


void MMapper::optionsConfigureKeys() {
    //KKeyDialog::configure(actionCollection(), xmlFile());
    ////KKeyDialog::configureKeys(actionCollection(), xmlFile());
    //
    //saveNewConfig();
}

void MMapper::optionsConfigureToolbars() {
    //saveNewConfig();

    //// use the standard toolbar editor
    //KEditToolbar dlg(actionCollection());
    //connect(&dlg, SIGNAL(newToolbarConfig()),
    //        this, SLOT(applyNewConfig()));
    //if (dlg.exec()) {
    //    createGUI();
    //}
}

void MMapper::saveNewConfig() {
//#if defined(KDE_MAKE_VERSION)
//# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
//    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
//# else
//
//    saveMainWindowSettings(KGlobal::config() );
//# endif
//#else
//
//    saveMainWindowSettings(KGlobal::config() );
//#endif
}

void MMapper::applyNewConfig() {
//#if defined(KDE_MAKE_VERSION)
//# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
//    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
//# else
//
//    applyMainWindowSettings(KGlobal::config());
//# endif
//#else
//
//    applyMainWindowSettings(KGlobal::config());
//#endif
}





// *******************************************
// Setup actions
// *******************************************

void MMapper::setupActions() {
	// toolbars
	QToolBar *mainToolBar = new QToolBar( tr("Main toolbar"), this, DockTop ); 

	// main menu
	// menubar
	QMenuBar *menubar = menuBar();

	fileNewAction = new QAction( QIconSet( QPixmap::fromMimeSource( "filenew.png" )), 
		tr("&New..."), QKeySequence(QString("Ctrl+N")),
		this, "fileNewAction" );
	connect(fileNewAction, SIGNAL(activated()), SLOT(fileNew()));

	fileOpenAction = new QAction( QIconSet( QPixmap::fromMimeSource( "fileopen.png" )), 
		tr("&Open..."), QKeySequence(QString("Ctrl+O")),
		this, "fileOpenAction" );
	connect(fileOpenAction, SIGNAL(activated()), SLOT(fileOpen()));

	fileSaveAction = new QAction( QIconSet( QPixmap::fromMimeSource( "filesave.png" )), 
		tr("&Save"), QKeySequence(QString("Ctrl+S")),
		this, "fileSaveAction" );
	connect(fileSaveAction, SIGNAL(activated()), SLOT(fileSave()));

	fileSaveAsAction = new QAction( tr("&Save as..."), QKeySequence(),
		this, "fileSaveAsAction" );
	connect(fileSaveAsAction, SIGNAL(activated()), SLOT(fileSaveAs()));

	fileExitAction = new QAction( QIconSet( QPixmap::fromMimeSource( "exit.png" ) ),
		tr("&Quit"), QKeySequence(),
		this, "fileExitAction" );
	connect(fileExitAction, SIGNAL(activated()), qApp, SLOT(quit()));

	showLogViewAction = new QAction( tr("Show &Log View"), 0,/*Qt::CTRL+Qt::Key_L,*/ 
		this, "showlogview");
	showLogViewAction->setToggleAction(true);
	showLogViewAction->setOn(true);
	connect(showLogViewAction, SIGNAL(toggled(bool)), SLOT(optionsShowLogView(bool)));

	preferencesAction = new QAction(QIconSet( QPixmap::fromMimeSource( "preferences.png" )),
		tr("&Preferences"), Qt::CTRL+Qt::Key_P, 
		this, "preferences");
	connect(preferencesAction, SIGNAL(activated()), SLOT(optionsPreferences()));

	////    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
	////    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
	//    //KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());
	//    showstatusbar = new KToggleAction( tr("Show &Status bar"), 0, this, SLOT(optionsShowStatusbar()), actionCollection(), "showstatusbar");
	//    showtoolbarmain = new KToggleAction( tr("Show &Main Toolbar"), 0,/*Qt::CTRL+Qt::Key_M,*/ this, SLOT(optionsShowToolbar1()), actionCollection(), "showtoolbar1");
	//    showtoolbarmode = new KToggleAction( tr("Show M&ode Toolbar"), 0,/*Qt::CTRL+Qt::Key_O,*/ this, SLOT(optionsShowToolbar2()), actionCollection(), "showtoolbar2");
	//    showtoolbartools = new KToggleAction( tr("Show &Utils Toolbar"), 0,/*Qt::CTRL+Qt::Key_U,*/ this, SLOT(optionsShowToolbar3()), actionCollection(), "showtoolbar3");
	//    showtoolbaredit = new KToggleAction( tr("Show &Edit Toolbar"), 0,/*Qt::CTRL+Qt::Key_U,*/ this, SLOT(optionsShowToolbar4()), actionCollection(), "showtoolbar4");
	////    (void) new KAction( tr("&Reparse room descriptions"), QIconSet(BarIcon("pencil")), Qt::CTRL+Qt::Key_R, this, SLOT(reparseButtonPressed()), actionCollection(), "reparse_descriptions");
	//

	//helpContentsAction = new QAction( this, "helpContentsAction" );
	//helpAboutAction = new QAction( this, "helpAboutAction" );

	fileNewAction->addTo( mainToolBar );
	fileOpenAction->addTo( mainToolBar );
	fileSaveAction->addTo(mainToolBar);
	preferencesAction->addTo( mainToolBar );

	QPopupMenu *fileMenu = new QPopupMenu( this );

	fileNewAction->addTo(fileMenu);
	fileOpenAction->addTo( fileMenu );
	fileMenu->insertSeparator();
	fileSaveAction->addTo( fileMenu );
	fileSaveAsAction->addTo( fileMenu );
	fileMenu->insertSeparator();
	preferencesAction->addTo(fileMenu);
	fileMenu->insertSeparator();
	fileExitAction->addTo( fileMenu );
	menubar->insertItem( tr("&File"), fileMenu, 1 );

	// Edit menu
	{
		QToolBar *editToolBar = new QToolBar( tr("Edit map toolbar"), this, DockTop ); 
		QAction *deleteAction = new QAction( QPixmap::fromMimeSource("button_cancel.png"), 
			tr("Delete room(s)"), 0, this, "delete action");
		connect(deleteAction, SIGNAL(activated()), this, SLOT(roomDelete()));
		deleteAction->addTo(editToolBar);

		// room move
		QAction *moveNorthAction = new QAction( QPixmap::fromMimeSource("movenorth.png"), 
			tr("Move room(s) north"), 0, this, "move north action");
		connect(moveNorthAction, SIGNAL(activated()), this, SLOT(moveNorth()));
		moveNorthAction->addTo(editToolBar);

		QAction *moveSouthAction = new QAction( QPixmap::fromMimeSource("movesouth.png"), 
			tr("Move room(s) south"), 0, this, "move south action");
		connect(moveSouthAction, SIGNAL(activated()), this, SLOT(moveSouth()));
		moveSouthAction->addTo(editToolBar);

		QAction *moveEastAction = new QAction( QPixmap::fromMimeSource("moveeast.png"), 
			tr("Move room(s) east"), 0, this, "move east action");
		connect(moveEastAction, SIGNAL(activated()), this, SLOT(moveEast()));
		moveEastAction->addTo(editToolBar);

		QAction *moveWestAction = new QAction( QPixmap::fromMimeSource("movewest.png"), 
			tr("Move room(s) west"), 0, this, "move west action");
		connect(moveWestAction, SIGNAL(activated()), this, SLOT(moveWest()));
		moveWestAction->addTo(editToolBar);

		// room creation
		QAction *newNorthAction = new QAction( QPixmap::fromMimeSource("newnorth.png"), 
			tr("New room north"), 0, this, "new room north action");
		connect(newNorthAction, SIGNAL(activated()), this, SLOT(newRoomNorth()));
		newNorthAction->addTo(editToolBar);

		QAction *newSouthAction = new QAction( QPixmap::fromMimeSource("newsouth.png"), 
			tr("New room south"), 0, this, "new room south action");
		connect(newSouthAction, SIGNAL(activated()), this, SLOT(newRoomSouth()));
		newSouthAction->addTo(editToolBar);

		QAction *newEastAction = new QAction( QPixmap::fromMimeSource("neweast.png"), 
			tr("New room east"), 0, this, "new room east action");
		connect(newEastAction, SIGNAL(activated()), this, SLOT(newRoomEast()));
		newEastAction->addTo(editToolBar);

		QAction *newWestAction = new QAction( QPixmap::fromMimeSource("newwest.png"), 
			tr("New room west"), 0, this, "new room west action");
		connect(newWestAction, SIGNAL(activated()), this, SLOT(newRoomWest()));
		newWestAction->addTo(editToolBar);

		// room disconnect
		QAction *disconnectNorthAction =new QAction( QPixmap::fromMimeSource("disconnectnorth.png"), 
			tr("Disconnect room north"), 0, this, "disconnect room north action");
		connect(disconnectNorthAction, SIGNAL(activated()), this, SLOT(disconnectRoomNorth()));
		disconnectNorthAction->addTo(editToolBar);

		QAction *disconnectSouthAction =new QAction( QPixmap::fromMimeSource("disconnectsouth.png"), 
			tr("Disconnect room south"), 0, this, "disconnect room south action");
		connect(disconnectSouthAction, SIGNAL(activated()), this, SLOT(disconnectRoomSouth()));
		disconnectSouthAction->addTo(editToolBar);

		QAction *disconnectEastAction =new QAction( QPixmap::fromMimeSource("disconnecteast.png"), 
			tr("Disconnect room east"), 0, this, "disconnect room east action");
		connect(disconnectEastAction, SIGNAL(activated()), this, SLOT(disconnectRoomEast()));
		disconnectEastAction->addTo(editToolBar);

		QAction *disconnectWestAction =new QAction( QPixmap::fromMimeSource("disconnectwest.png"), 
			tr("Disconnect room west"), 0, this, "disconnect room west action");
		connect(disconnectWestAction, SIGNAL(activated()), this, SLOT(disconnectRoomWest()));
		disconnectWestAction->addTo(editToolBar);

		QPopupMenu *editMenu = new QPopupMenu( this );

		moveNorthAction->addTo(editMenu);
		moveSouthAction->addTo(editMenu);
		moveEastAction->addTo(editMenu);
		moveWestAction->addTo(editMenu);

		editMenu->insertSeparator();

		newNorthAction->addTo(editMenu);
		newSouthAction->addTo(editMenu);
		newEastAction->addTo(editMenu);
		newWestAction->addTo(editMenu);

		editMenu->insertSeparator();

		disconnectNorthAction->addTo(editMenu);
		disconnectSouthAction->addTo(editMenu);
		disconnectEastAction->addTo(editMenu);
		disconnectWestAction->addTo(editMenu);

		editMenu->insertSeparator();

		deleteAction->addTo(editMenu);

		menubar->insertItem(tr("&Edit"), editMenu, 4);
	}
	
	// setup Settings menu

//    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
////    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
////    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
//    //KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());
//
//    showlogview = new KToggleAction( tr("Show &Log View"), 0,/*Qt::CTRL+Qt::Key_L,*/ this, SLOT(optionsShowLogView()), actionCollection(), "showlogview");
//
//    showstatusbar = new KToggleAction( tr("Show &Status bar"), 0, this, SLOT(optionsShowStatusbar()), actionCollection(), "showstatusbar");
//
//    showtoolbarmain = new KToggleAction( tr("Show &Main Toolbar"), 0,/*Qt::CTRL+Qt::Key_M,*/ this, SLOT(optionsShowToolbar1()), actionCollection(), "showtoolbar1");
//
//    showtoolbarmode = new KToggleAction( tr("Show M&ode Toolbar"), 0,/*Qt::CTRL+Qt::Key_O,*/ this, SLOT(optionsShowToolbar2()), actionCollection(), "showtoolbar2");
//
//    showtoolbartools = new KToggleAction( tr("Show &Utils Toolbar"), 0,/*Qt::CTRL+Qt::Key_U,*/ this, SLOT(optionsShowToolbar3()), actionCollection(), "showtoolbar3");
//
//    showtoolbaredit = new KToggleAction( tr("Show &Edit Toolbar"), 0,/*Qt::CTRL+Qt::Key_U,*/ this, SLOT(optionsShowToolbar4()), actionCollection(), "showtoolbar4");
//
//    (void) new KAction( tr("&Preferences"), QIconSet(BarIcon("configure")), Qt::CTRL+Qt::Key_P, this, SLOT(optionsPreferences()), actionCollection(), "preferences");
//
////    (void) new KAction( tr("&Reparse room descriptions"), QIconSet(BarIcon("pencil")), Qt::CTRL+Qt::Key_R, this, SLOT(reparseButtonPressed()), actionCollection(), "reparse_descriptions");
//
//    (void) new KAction( tr("Re&draw world"), QIconSet(BarIcon("paintbrush")), Qt::CTRL+Qt::Key_D, this, SLOT(redrawWorldButtonPressed()), actionCollection(), "redraw_world");
//
	// Mode menu
	{
		QToolBar *modeToolBar = new QToolBar( tr("Mode toolbar"), this, DockRight ); 

		QActionGroup *pModeGroup = new QActionGroup(this, "mode group", true);
		m1 = new QAction( QPixmap::fromMimeSource("follow.png"), tr("Offline &follow mode"), 
			Qt::Key_F1, pModeGroup, "follow_mode");
		m1->setToggleAction(true);
		connect(m1, SIGNAL(activated()), this, SLOT(pressedMode1Button()));

		m2 = new QAction( QPixmap::fromMimeSource("edit.png"), tr("Ed&it mode (online/offline)"), 
			Qt::Key_F2, pModeGroup, "edit_mode");
		m2->setToggleAction(true);
		connect(m2, SIGNAL(activated()), this, SLOT(pressedMode2Button()));

		m3 = new QAction(QPixmap::fromMimeSource("online.png"),  tr("Online &play mode"), 
			Qt::Key_F3, pModeGroup, "play_mode");
		m3->setToggleAction(true);
		connect(m3, SIGNAL(activated()), this, SLOT(pressedMode3Button()));

		pModeGroup->addTo(modeToolBar);

		QPopupMenu *modeMenu = new QPopupMenu( this );

		m1->addTo( modeMenu );
		m2->addTo( modeMenu );
		m3->addTo( modeMenu );
		menubar->insertItem( tr("&Mode"), modeMenu, 3 );
	}

	{
		QPopupMenu *viewMenu = new QPopupMenu( this );

		redrawAction = new QAction( tr("Re&draw world"), Qt::CTRL+Qt::Key_D, 
			this, "redraw_world");
		connect(redrawAction, SIGNAL(activated()), SLOT(redrawWorldButtonPressed()));

		redrawAction->addTo(viewMenu);

		viewMenu->insertSeparator();

		showLogViewAction->addTo(viewMenu);

		viewMenu->insertSeparator();

		QAction *zoomInAction = new QAction(QIconSet(QPixmap::fromMimeSource("viewmag+.png")), 
			tr("Zoom &in"), Qt::CTRL + Qt::Key_Plus, this, "zoom in action");
		connect(zoomInAction, SIGNAL(activated()), SLOT(zoomIn()));

		zoomInAction->addTo(viewMenu);

		QAction *zoomOutAction = new QAction(QIconSet(QPixmap::fromMimeSource("viewmag-.png")), 
			tr("Zoom &out"), Qt::CTRL + Qt::Key_Minus, this, "zoom out action");
		connect(zoomOutAction, SIGNAL(activated()), SLOT(zoomOut()));

		zoomOutAction->addTo(viewMenu);
		// toolbars visibility

		menubar->insertItem(tr("&View"), viewMenu, 5);
	}

	{
		QPopupMenu *helpMenu = new QPopupMenu( this );

		QAction *helpAction = new QAction( QPixmap::fromMimeSource("help.png"), 
			tr("Help index"), 0, this, "help index action");
		connect(helpAction, SIGNAL(activated()), this, SLOT(helpIndex()));

		helpAction->addTo(helpMenu);
		
		helpMenu->insertSeparator();

		QAction *aboutAction = new QAction( tr("About Mapper"), 0, this, "about app action");
		connect(aboutAction, SIGNAL(activated()), this, SLOT(aboutMapper()));

		aboutAction->addTo(helpMenu);

		QAction *aboutQtAction = new QAction( tr("About QT"), 0, this, "about Qt action");
		connect(aboutQtAction, SIGNAL(activated()), qApp, SLOT(aboutQt()));

		aboutQtAction->addTo(helpMenu);

		menubar->insertItem(tr("&Help"), helpMenu, 6);
	}


    mode = PLAY_MODE;
    lastMode = PLAY_MODE;
    changeStatusbar("Welcome to MMapper ...");
}


void MMapper::changeStatusbar(const QString& text) {
    statusBar()->message(text);
}

void MMapper::changeCaption(const QString& text) {
    setCaption(text);
}


//////////////////////////////////////////
// keyboard handling

void MMapper::keyPressEvent( QKeyEvent* e ) {
    //qDebug("[MMapper] KEY: %i  %i", e->key(), e->ascii());
    //qDebug("Up: %i",Qt::Key_Up);
    Room* r = Data().getCurrentRoom();
    Room* rn = NULL;

    //all modes
    switch(e->key()) {
    case Qt::Key_Plus:
        if (e->state() & ControlButton) {
            m_canvas_view->zoomIn();
        }
        break;
    case Qt::Key_Minus:
        if (e->state() & ControlButton) {
            m_canvas_view->zoomOut();
        }
        break;
    case 4144:
        pressedMode1Button();
        m1->setOn(TRUE);
        break;
    case 4145:
        pressedMode2Button();
        m2->setOn(TRUE);
        break;
    case 4146:
        pressedMode3Button();
        m3->setOn(TRUE);
        break;

    default:
        ;
    }


    if ( mode == PLAY_MODE ) { // Online mode
        switch (e->key()) {

        case Qt::Key_B: // reset descriptions
            resetCommand();
            break;

        default:
            e->ignore();
        }
        return;
    }

    if ( mode == EDIT_MODE )
        switch (e->key()) { // Edit mode

        case Qt::Key_B: // reset descriptions
            resetCommand();
            break;
        case Qt::Key_F:
            if (r) {
                r->roomTerrain=TFIELD;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_L:
            if (r) {
                r->roomTerrain=TFOREST;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_D:
            if (r) {
                r->roomTerrain=TRANDOM;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_V:
            if (r) {
                r->roomTerrain=TWATER;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_T:
            if (r) {
                r->roomTerrain=TCITY;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_C:
            if (r) {
                r->roomTerrain=TINDOORS;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_R:
            if (r) {
                r->roomTerrain=TROAD;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_X:
            if (r) {
                r->roomTerrain=TDEATHTRAP;
                Canvas().updateRoomAndNeighbours(r);
            }
            break;
        case Qt::Key_A:
            if (r) {
                r->roomMob=MAGGRESIVE;
                Canvas().updateRoom(r);
            }
            break;
        case Qt::Key_P:
            if (r) {


                r->roomMob=MPEACEFULL;
                Canvas().updateRoom(r);
            }
            break;
        case Qt::Key_M:
            if (r) {
                r->roomMob=MNOMOB;
                Canvas().updateRoom(r);
            }
            break;

        case Qt::Key_N:
            moveNorth();
            break;
        case Qt::Key_S:
            moveSouth();
            break;
        case Qt::Key_E:
            moveEast();
            break;
        case Qt::Key_W:
            moveWest();
            break;

        case KEY_UP:
        case Qt::Key_8:
            if (e->state() & ShiftButton)
                moveNorth();
            else if (e->state() & ControlButton)
                newRoomNorth();
            else if (e->state() & AltButton)
                disconnectRoomNorth();
            else
                selectUp();
            break;
        case KEY_DOWN:
        case Qt::Key_2:
            if (e->state() & ShiftButton)
                moveSouth();
            else if (e->state() & ControlButton)
                newRoomSouth();
            else if (e->state() & AltButton)
                disconnectRoomSouth();
            else
                selectDown();
            break;
        case KEY_RIGHT:
        case Qt::Key_6:
            if (e->state() & ShiftButton)
                moveEast();
            else if (e->state() & ControlButton)
                newRoomEast();
            else if (e->state() & AltButton)
                disconnectRoomEast();
            else
                selectRight();
            break;
        case KEY_LEFT:
        case Qt::Key_4:
            if (e->state() & ShiftButton)
                moveWest();
            else if (e->state() & ControlButton)
                newRoomWest();
            else if (e->state() & AltButton)
                disconnectRoomWest();
            else
                selectLeft();
            break;

        case Qt::Key_Delete:
            roomDelete();
            break;
        default:
            e->ignore();
        }

    if ( mode == FOLLOW_MODE  && (r=Data().getCurrentRoom())) // Follow mode
    {
        rn = NULL;
        switch (e->key()) {
        case KEY_UP:
        case Qt::Key_8:
            follow(r, r->n);
            break;
        case KEY_DOWN:
        case Qt::Key_2:
            follow(r, r->s);
            break;
        case KEY_RIGHT:
        case Qt::Key_6:
            follow(r, r->e);
            break;
        case KEY_LEFT:
        case Qt::Key_4:
            follow(r, r->w);
            break;
        case Qt::Key_Minus:
        case Qt::Key_9:
        case KEY_PGUP:
            follow(r, r->u);
            break;
        case Qt::Key_Plus:
        case Qt::Key_3:
        case KEY_PGDN:
            follow(r, r->d);
            break;
        default:
            e->ignore();
        }
    }

}

//////////////////////////////////////////
// toolbars handling

void MMapper::redrawWorldButtonPressed() {
    m_log_view->log("[MMapper] redraw world ...");
    m_log_view->update();
    Canvas().updateWorld();
    m_log_view->log("[MMapper] redraw world finished ...");
}


void MMapper::reparseButtonPressed() {

    //m_log_view->log("[MMapper] Reparse called ...");
    //m_log_view->update();
    //if(Data().reparseRoomDesc())
    //    m_log_view->log("[MMapper] Reparse changed some room parsed descriptions!");
    m_log_view->log("[MMapper] Reparse not needed anymore!");

    //m_log_view->log("[MMapper] Reparse finished ...");
}


void MMapper::pressedMode1Button() {
    m_edit_dialog->hide();
    mode = FOLLOW_MODE ;
    m1->setOn(TRUE);
    changeStatusbar("Follow mode ...");
}

void MMapper::pressedMode2Button() {
    m_edit_dialog->show();

    lastMode=mode;
    if(lastMode == EDIT_MODE )
        lastMode = PLAY_MODE;
    mode = EDIT_MODE;
    m2->setOn(TRUE);
    changeStatusbar("Edit mode ...");
    m_canvas_view->setFocus();

}

void MMapper::pressedMode3Button() {
    m_edit_dialog->hide();
    mode = PLAY_MODE;
    m3->setOn(TRUE);
    changeStatusbar("Online mode ...");
}

//////////////////////////
// Edit actions

void MMapper::moveNorth() {
    Data().moveNorth();
}

void MMapper::moveSouth() {
    Data().moveSouth();
}

void MMapper::moveEast() {
    Data().moveEast();
}

void MMapper::moveWest() {
    Data().moveWest();
}

void MMapper::roomDelete() {
    m_edit_dialog->deleteClicked();
}

void MMapper::selectLeft() {
    m_canvas_view->markMoveLeft();
}

void MMapper::selectRight() {
    m_canvas_view->markMoveRight();
}

void MMapper::selectUp() {
    m_canvas_view->markMoveUp();
}

void MMapper::selectDown() {
    m_canvas_view->markMoveDown();
}

void MMapper::newRoomWest() {
    m_edit_dialog->newRoomWestClicked();
}

void MMapper::newRoomEast() {
    m_edit_dialog->newRoomEastClicked();
}

void MMapper::newRoomNorth() {
    m_edit_dialog->newRoomNorthClicked();
}

void MMapper::newRoomSouth() {
    m_edit_dialog->newRoomSouthClicked();
}

void MMapper::disconnectRoomNorth() {
    m_edit_dialog->disconnectNorthClicked();
}

void MMapper::disconnectRoomSouth() {
    m_edit_dialog->disconnectSouthClicked();
}

void MMapper::disconnectRoomEast() {
    m_edit_dialog->disconnectEastClicked();
}

void MMapper::disconnectRoomWest() {
    m_edit_dialog->disconnectWestClicked();
}

void MMapper::zoomIn() {
    m_canvas_view->zoomIn();
}

void MMapper::zoomOut() {
    m_canvas_view->zoomOut();
}

void MMapper::zoomAll() {
	m_canvas_view->zoomAll();
}

void MMapper::helpIndex()
{
	const CContextHelper helper("index");
	CHelpWindow::showHelp(helper, this);
}

void MMapper::aboutMapper()
{
	QMessageBox::information(this, tr("History"), 
		tr("There was once mighty code wizard named Caligor.\n"
		"His bright ideas gave birth to this great program.\n\n"
		"Program copyright 2003 Marek Krejza <mkrejza@cbox.cz>\n\n" 
		"Contributors: \n"
		"\tp(Krush) - English proof-reader and FAQ addons\n"
		"\tp(Korir) - ideas and bugreports\n"
		"\tp(Kovish) - port on Win32 platform\n"
		"\tKonqui the KDE Dragon <konqui@kde.org>\n"
		"\tTux the Linux Penguin <tux@linux.org>\n\n"
		"\tDocumentation copyright 2003 Marek Krejza <mkrejza@cbox.cz>\n"
		));
}
//////////////////////////////////////////
// Room edit dialog functions (slots)

void MMapper::roomEditDialogClosed() {
    if (lastMode!=mode)
        switch (lastMode) {
        case FOLLOW_MODE:
            m1->setOn(TRUE);
            break;
        case EDIT_MODE:
            m2->setOn(TRUE);
            break;
        case PLAY_MODE:
            m3->setOn(TRUE);
            break;
        }
    mode=lastMode;
}


//////////////////////////////////////////
// mouse events handling

void MMapper::mousePressed( QMouseEvent* e, double xs, double ys) {
    unsigned int xp,yp;

    xp = VIEWPORTTOROOM(xs);
    yp = VIEWPORTTOROOM(ys);

    switch (e->button()) {
    case LeftButton:
        if ( ( mode != EDIT_MODE ) && Data().findRoomAt(xp,yp)) {
            m_canvas_view->selectRoom(xp,yp);
	    logRoomInfo(Data().getCurrentRoom());
            sync=TRUE;
            Parser().emptyQueue();
        }
        if ( mode == EDIT_MODE ) {
            m_canvas_view->selectRoom(xp,yp);
	    logRoomInfo(Data().getCurrentRoom());
            sync=TRUE;
            Parser().emptyQueue();
        }
        break;
    case RightButton:
        if ( ( mode == EDIT_MODE )) {
            m_canvas_view->selectRoom2(xp,yp);
        }
        break;
    case MidButton:
    case NoButton:
    default:
        e->ignore();
    }
}

void MMapper::mouseMoved( QMouseEvent* e, double xs, double ys) {
    unsigned int xp,yp;

    xp = VIEWPORTTOROOM(xs);
    yp = VIEWPORTTOROOM(ys);

    switch (e->state()) {
    case LeftButton:
        if ( ( mode != EDIT_MODE ) && Data().findRoomAt(xp,yp)) {
            m_canvas_view->selectRoom(xp,yp);
	    logRoomInfo(Data().getCurrentRoom());
            sync=TRUE;
            Parser().emptyQueue();
        }
        if ( mode == EDIT_MODE ) {
            m_canvas_view->selectRoom(xp,yp);
	    logRoomInfo(Data().getCurrentRoom());
            sync=TRUE;
            Parser().emptyQueue();
        }
        break;
    case RightButton:
        if ( ( mode == EDIT_MODE ) && Data().findRoomAt(xp,yp)) {
            m_canvas_view->selectRoom2(xp,yp);
        }
        break;
    case MidButton:
    case NoButton:
    default:
        e->ignore();
    }
}

void MMapper::mouseDoubleClicked( QMouseEvent*, double xs, double ys) {
    unsigned int xp,yp;

    xp = VIEWPORTTOROOM(xs);
    yp = VIEWPORTTOROOM(ys);
    if ( mode == EDIT_MODE ) {
        Data().createAndConnectNewRoom(xp,yp);
        //emit roomSelected(Data().findRoomAt(xp,yp));
        m_canvas_view->selectRoom(xp,yp);
    }
}



//////////////////////////
// Connection handling

void MMapper::newConnect( ClientSocket *s ) {
    m_log_view->log( tr("[MMapper] New connection\n") );

    connect( this , SIGNAL(sendToClient(const QString&)), s, SLOT(sendToClient(const QString&)));
    connect( this , SIGNAL(sendToServer(const QString&)), s, SLOT(sendToServer(const QString&)));
    connect ( s, SIGNAL(logText(const QString&)),
              m_log_view, SLOT(log(const QString&)) );
    connect ( s, SIGNAL(connectionClosed()),
              SLOT(connectionClosed()) );

    connect( &Parser(), SIGNAL(sendToClient(const QString&)), s, SLOT(sendToClient(const QString&)));
}

void MMapper::connectionClosed () {
    m_log_view->log( tr("[MMapper] Connection closed\n") );
}



//////////////////////////////////////////
// follow mode handling

void MMapper::logRoomInfo(Room *rn) {
    QStringList list;
    QString out;
    QString tmp;
    
    if (!rn) return;
    
    m_log_view->log("\n");
    m_log_view->log("*** "+rn->roomName+" ***");

    list=list.split("\n",rn->roomDesc);
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
        m_log_view->log(*it);

    if (ISSET(rn->roomFlags,FLAGS_VALID)) {
        bool first=TRUE;
        out=QString("Exits: ");

        if (ISSET(rn->roomFlags, EXIT_N) || ISSET(rn->roomFlags, DOOR_N)) {
            tmp = "north";
            if (ISSET(rn->roomFlags, DOOR_N)) {
                tmp.prepend('(');
                tmp.append(')');
            }
            if (ISSET(rn->roomFlags, ROAD_N)) {
                tmp.prepend('=');
                tmp.append('=');
            }

            if (first)
                first = FALSE;
            else
                tmp.prepend(", ");
            out+=tmp;
        }

        if (ISSET(rn->roomFlags, EXIT_E) || ISSET(rn->roomFlags, DOOR_E)) {
            tmp = "east";
            if (ISSET(rn->roomFlags, DOOR_E)) {
                tmp.prepend('(');
                tmp.append(')');
            }

            if (ISSET(rn->roomFlags, ROAD_E)) {
                tmp.prepend('=');
                tmp.append('=');
            }

            if (first)
                first = FALSE;
            else
                tmp.prepend(", ");
            out+=tmp;
        }

        if (ISSET(rn->roomFlags, EXIT_S) || ISSET(rn->roomFlags, DOOR_S)) {
            tmp = "south";
            if (ISSET(rn->roomFlags, DOOR_S)) {
                tmp.prepend('(');
                tmp.append(')');
            }
            if (ISSET(rn->roomFlags, ROAD_S)) {
                tmp.prepend('=');
                tmp.append('=');
            }

            if (first)
                first = FALSE;
            else
                tmp.prepend(", ");
            out+=tmp;
        }

        if (ISSET(rn->roomFlags, EXIT_W) || ISSET(rn->roomFlags, DOOR_W)) {
            tmp = "west";
            if (ISSET(rn->roomFlags, DOOR_W)) {
                tmp.prepend('(');
                tmp.append(')');
            }
            if (ISSET(rn->roomFlags, ROAD_W)) {
                tmp.prepend('=');
                tmp.append('=');
            }

            if (first)
                first = FALSE;
            else
                tmp.prepend(", ");
            out+=tmp;
        }

        if (ISSET(rn->roomFlags, EXIT_U) || ISSET(rn->roomFlags, DOOR_U)) {
            tmp = "up";
            if (ISSET(rn->roomFlags, DOOR_U)) {
                tmp.prepend('(');
                tmp.append(')');
            }
            if (ISSET(rn->roomFlags, ROAD_U)) {
                tmp.prepend('=');
                tmp.append('=');
            }

            if (first)
                first = FALSE;
            else
                tmp.prepend(", ");
            out+=tmp;
        }

        if (ISSET(rn->roomFlags, EXIT_D) || ISSET(rn->roomFlags, DOOR_D)) {
            tmp = "down";
            if (ISSET(rn->roomFlags, DOOR_D)) {
                tmp.prepend('(');
                tmp.append(')');
            }
            if (ISSET(rn->roomFlags, ROAD_D)) {
                tmp.prepend('=');
                tmp.append('=');
            }

            if (first)
                first = FALSE;
            else
                tmp.prepend(", ");
            out+=tmp;
        }
    }
    m_log_view->log(out+".");
    
    //send room info (doors) to client
    out = "";
    if ((rn->n) && ISSET(rn->n->type,CONNECTION_DOOR)) {
        out = out + " N:" + DOORNAME(rn,rn->n);
	if ((DOORNAME(rn,rn->n)) == "") out += "exit";
    }
    if ((rn->s) && ISSET(rn->s->type,CONNECTION_DOOR)) {
        out = out + " S:" + DOORNAME(rn,rn->s);
	if ((DOORNAME(rn,rn->s)) == "") out += "exit";
    }
    if ((rn->e) && ISSET(rn->e->type,CONNECTION_DOOR)) {
        out = out + " E:" + DOORNAME(rn,rn->e);
	if ((DOORNAME(rn,rn->e)) == "") out += "exit";
    }
    if ((rn->w) && ISSET(rn->w->type,CONNECTION_DOOR)) {
        out = out + " W:" + DOORNAME(rn,rn->w);
	if ((DOORNAME(rn,rn->w)) == "") out += "exit";
    }
    if ((rn->u) && ISSET(rn->u->type,CONNECTION_DOOR)) {
        out = out + " U:" + DOORNAME(rn,rn->u);
	if ((DOORNAME(rn,rn->u)) == "") out += "exit";
    }
    if ((rn->d) && ISSET(rn->d->type,CONNECTION_DOOR)) {
        out = out + " D:" + DOORNAME(rn,rn->d);
	if ((DOORNAME(rn,rn->d)) == "") out += "exit";
    }

    if (!out.isEmpty())
        m_log_view->log("Doors:"+out+"\r\n");

    m_log_view->log(" ");
    m_log_view->log(">");

    //qDebug("\n********** Room description **********\n");
    //qDebug("%s",rn->roomDesc.ascii());

    
}

void MMapper::follow(Room *r, Connection *c) {
    Room *rn;

    if ( !c || !(rn=ISCONNECTEDTO(r,c)) ) {
        m_log_view->log("You can't go there !!!");
        return;
    }
    
    m_canvas_view->selectRoom(rn);
    logRoomInfo(rn);
}




//////////////////////////////////////////
// online char moving


Room* MMapper::checkNeighbourRooms(int depth, const QString& name, const QString& desc, const QString& parseddesc, Room* r, Room* comingFrom, Q_UINT32 roomFlags, Q_UINT32 promptFlags, bool *multiple) {
    Room *tmp;
    if (depth<1 || !r)
        return NULL;

    bool found = FALSE;
    bool multiplefound = FALSE;
    Room *foundRoom = NULL;

    if ((r->n) && ((tmp=ISCONNECTEDTO(r,r->n))!=comingFrom) && Data().compareRooms(tmp, name, parseddesc, roomFlags, promptFlags)) {
        found = TRUE;
        foundRoom=tmp;
    }
    if ((r->s) && ((tmp=ISCONNECTEDTO(r,r->s))!=comingFrom) && Data().compareRooms(tmp, name, parseddesc, roomFlags, promptFlags)) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->e) && ((tmp=ISCONNECTEDTO(r,r->e))!=comingFrom) && Data().compareRooms(tmp, name, parseddesc, roomFlags, promptFlags)) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->w) && ((tmp=ISCONNECTEDTO(r,r->w))!=comingFrom) && Data().compareRooms(tmp, name, parseddesc, roomFlags, promptFlags)) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }
    if ((r->u) && ((tmp=ISCONNECTEDTO(r,r->u))!=comingFrom) && Data().compareRooms(tmp, name, parseddesc, roomFlags, promptFlags)) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->d) && ((tmp=ISCONNECTEDTO(r,r->d))!=comingFrom) && Data().compareRooms(tmp, name, parseddesc, roomFlags, promptFlags)) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if (found) {
        if (!multiplefound)
            return foundRoom;
        else {
            *multiple = TRUE;
            return foundRoom;
        }
    }

    if ((r->n) && ((tmp=ISCONNECTEDTO(r,r->n))!=comingFrom) && (tmp=checkNeighbourRooms(depth-1, name, desc, parseddesc, tmp, r, roomFlags, promptFlags, multiple))) {
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->s) && ((tmp=ISCONNECTEDTO(r,r->s))!=comingFrom) && (tmp=checkNeighbourRooms(depth-1, name, desc, parseddesc, tmp, r, roomFlags, promptFlags, multiple))) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->e) && ((tmp=ISCONNECTEDTO(r,r->e))!=comingFrom) && (tmp=checkNeighbourRooms(depth-1, name, desc, parseddesc, tmp, r, roomFlags, promptFlags, multiple))) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->w) && ((tmp=ISCONNECTEDTO(r,r->w))!=comingFrom) && (tmp=checkNeighbourRooms(depth-1, name, desc, parseddesc, tmp, r, roomFlags, promptFlags, multiple))) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->u) && ((tmp=ISCONNECTEDTO(r,r->u))!=comingFrom) && (tmp=checkNeighbourRooms(depth-1, name, desc, parseddesc, tmp, r, roomFlags, promptFlags, multiple))) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if ((r->d) && ((tmp=ISCONNECTEDTO(r,r->d))!=comingFrom) && (tmp=checkNeighbourRooms(depth-1, name, desc, parseddesc, tmp, r, roomFlags, promptFlags, multiple))) {
        if (found)
            multiplefound=TRUE; // multiple rooms found
        found = TRUE;
        foundRoom=tmp;
    }

    if (found) {
        if (!multiplefound)
            return foundRoom;
        else {
            *multiple = TRUE;
            return foundRoom;
        }
    }

    return NULL;
}

Room* MMapper::mapSync(const QString& name,const QString& desc, const QString& parseddesc, Room* r, Q_UINT32 roomFlags, Q_UINT32 promptFlags) {
    Room *rn=r;
    bool multiple = FALSE;

    if (name.isEmpty() || desc.isEmpty())
        return NULL;

    if ((r) &&
            ( Data().compareRooms(r, name, parseddesc, roomFlags, promptFlags) ||
              (rn=checkNeighbourRooms(Config().m_syncSearchDepth, name, desc, parseddesc, r, NULL, roomFlags, promptFlags, &multiple)) )) {

        if (multiple) {
            m_log_view->log( tr("[MMapper] speed search: multiple rooms found!!! Can't decide your position.") );
            return NULL;
        }

        m_log_view->log( tr("[MMapper] speed search: found room.") );
        return rn;
    }

    multiple = FALSE;
    if((rn=Data().findRoom(name, parseddesc, roomFlags, promptFlags, &multiple))) {

        if (multiple) {
            m_log_view->log( tr("[MMapper] full search: multiple rooms found!!! Can't decide your position.") );
            return NULL;
        }

        m_log_view->log( tr("[MMapper] full search: found room.") );
        return rn;
    }

    m_log_view->log( tr("[MMapper] full search: no room found!!!") );
    return NULL;
}



void MMapper::mapSyncDirection(const QString& name, const QString& desc, const QString& parseddesc, Room *sr, Connection *c, Q_UINT32 roomFlags, Q_UINT32 promptFlags, DirectionType dir) {

    if (!sr) {
        sync = FALSE;
        return;
    }

    if (!c && mode == EDIT_MODE && Config().m_autoRoomCreate) {
        //room not probably on map (at least is not connected to source room)
        Room* dr = NULL;
        Room* tr = NULL;

        if ( (dir == dir_east) && (tr = Data().findRoomAt(sr->mapx+1, sr->mapy)) && Data().compareRooms(tr, name,  parseddesc, roomFlags, promptFlags) ) {
            dr = tr;
        } else
            if ( (dir == dir_west) && (tr = Data().findRoomAt(sr->mapx-1, sr->mapy)) && Data().compareRooms(tr, name, parseddesc, roomFlags, promptFlags) ) {
                dr = tr;
            } else
                if ( (dir == dir_south) && (tr = Data().findRoomAt(sr->mapx, sr->mapy+1)) && Data().compareRooms(tr, name, parseddesc, roomFlags, promptFlags) ) {
                    dr = tr;
                } else
                    if ( (dir == dir_north) && (tr = Data().findRoomAt(sr->mapx, sr->mapy-1)) && Data().compareRooms(tr, name, parseddesc, roomFlags, promptFlags) ) {
                        dr = tr;
                    }


        if ((!dr) && (dr = mapSync( name, desc, parseddesc, sr, roomFlags, promptFlags))) {
            m_log_view->log( "[MMapper] Similar room already on map !!!" );
            emit sendToClient("[MMapper] Similar room already on map !!!\r\n");
	    
	    // if this line is commented, then always new room is created
	    dr = NULL;
        }
	
        if ((!dr) && (dr = Data().createNewRoom( sr, dir ))) {
            m_log_view->log( "[MMapper] *New room created." );
        }
        //

        c = Data().connectRooms( sr, dr, dir, roomFlags );
        if (c)
            m_log_view->log( "[MMapper] *New room connected" );
        //if (!Config().m_autoRoomUpdate) {
            Canvas().updateRoom(sr);
            Canvas().updateRoom(dr);
        //}
    }


    if (c) { //room is on map
        Room* dr=ISCONNECTEDTO(sr,c);

        if (!name.isEmpty() && (Config().m_brief || !desc.isEmpty()) ) { // room info arrived

            if ( dr->roomUpdated ) { // room updated
                /*
                if ( dr->parsedRoomDesc.isEmpty() ) // need to reparse descriptions?
                                  dr->parsedRoomDesc = Parser().parseDesc(dr->roomDesc);
                */
                if (/*Config().m_strictly_control_rooms*/1) { //strictly control rooms

                    if ( Data().compareRooms(dr, name, parseddesc, roomFlags, promptFlags, TRUE) ) { // room match
                        m_canvas_view->selectRoom(dr);

                        if (/*update terrain &&*/ ISSET(promptFlags,PROMPT_INFO_VALID)) {
                            int tt=promptFlags & 15;
                            if (tt<14 && dr->roomTerrain != (RoomTerrainType)tt) {
                                dr->roomTerrain = (RoomTerrainType)tt;
                                Canvas().updateRoom(dr);
                            }
                            if (ISSET(promptFlags,SUN_ROOM))
                                dr->roomLight = LIT;
                            if (ISSET( roomFlags, FLAGS_VALID))
                                dr->roomFlags |= roomFlags;
                        }
                    } else { // room doesn't match

                        sync = FALSE;
                        if (Config().m_autoSync && (dr = mapSync(name, desc, parseddesc, sr, roomFlags, promptFlags)) ) { // try to autosync
                            m_canvas_view->selectRoom(dr);
                            sync = TRUE;
                        }

                    }
                } else { //dont strictly control rooms
                    m_canvas_view->selectRoom(dr);
                }
            } else { // room not updated

                if (Config().m_autoRoomUpdate) {
                    Data().updateRoom( dr, name, desc, roomFlags, promptFlags);
                    Canvas().updateRoom(sr);
                    Canvas().updateRoom(dr);
                    m_log_view->log( "[MMapper] *Room description added." );
                }
                m_canvas_view->selectRoom(dr);
            }

        } else { // no room info arrived
            m_log_view->log( "[MMapper] No room descriptions (fog or dark?)" );
            m_log_view->log( "[MMapper] No room check performed ..." );
            m_canvas_view->selectRoom(dr);
        }

    } else { //room is not on map
        sync = FALSE;
        m_log_view->log( "[MMapper] No room in that direction on the map!!!" );
        m_log_view->log( "[MMapper] Lost synchronization with map !!!" );
        emit sendToClient("[MMapper] Lost synchronization with map !!!\r\n");
    }

}


void MMapper::characterMoved(Command* c,const QString& name, const QString& desc, const QString& parseddesc, Q_UINT32 roomFlags, Q_UINT32 promptFlags) {

    if (mode == FOLLOW_MODE) {
        sync = FALSE;
        return;
    }

    Room *r = Data().getCurrentRoom();
    if (sync) { // synced
        if (c && r) {

            switch (c->id) {
            case id_north: // known dir
                m_log_view->log( "[MMapper] moving north.");
                mapSyncDirection(name, desc, parseddesc, r, r->n, roomFlags, promptFlags, dir_north);
                break;
            case id_south: // known dir
                m_log_view->log( "[MMapper] moving south.");
                mapSyncDirection(name, desc, parseddesc, r, r->s, roomFlags, promptFlags, dir_south);
                break;
            case id_east: // known dir
                m_log_view->log( "[MMapper] moving east.");
                mapSyncDirection(name, desc, parseddesc, r, r->e, roomFlags, promptFlags, dir_east);
                break;
            case id_west: // known dir
                m_log_view->log( "[MMapper] moving west.");
                mapSyncDirection(name, desc, parseddesc, r, r->w, roomFlags, promptFlags, dir_west);
                break;
            case id_up: // known dir
                m_log_view->log( "[MMapper] moving up.");
                mapSyncDirection(name, desc, parseddesc, r, r->u, roomFlags, promptFlags, dir_up);
                break;
            case id_down: // known dir
                m_log_view->log( "[MMapper] moving down.");
                mapSyncDirection(name, desc, parseddesc, r, r->d, roomFlags, promptFlags, dir_down);
                break;
            default: // unknown dir
                m_log_view->log( "[MMapper] moving unknown direction (look, examine, port, flee ...)");

                // switch from edito mode to play mode
                if ( mode == EDIT_MODE ) {
                    pressedMode3Button();
                    m3->setOn(TRUE);
                }

                Room *tr;
                // try to autosync
                if (Config().m_autoSync && (tr = mapSync(name, desc, parseddesc, Data().getCurrentRoom(), roomFlags, promptFlags)) ) {
                    m_canvas_view->selectRoom(tr);
                    m_log_view->log( "[MMapper] Synchronization succesfull." );
                    emit sendToClient("[MMapper] Synchronization succesfull.\r\n");
                } else {
                    m_log_view->log( "[MMapper] Lost synchronization with map !!!" );
                    emit sendToClient("[MMapper] Lost synchronization with map !!!\r\n");
                    sync = FALSE;
                }
            }
        } else {  // no source room or command
            m_log_view->log( "[MMapper] moving unknown direction (no move command in stack ...)");

            // switch from edito mode to play mode
            if ( mode == EDIT_MODE ) {
                pressedMode3Button();
                m3->setOn(TRUE);
            }

            Room *tr;
            // try to autosync
            if (Config().m_autoSync && (tr = mapSync(name, desc, parseddesc, r, roomFlags, promptFlags)) ) {
                m_canvas_view->selectRoom(tr);
                m_log_view->log( "[MMapper] Synchronization succesfull." );
                emit sendToClient("[MMapper] Synchronization succesfull.\r\n");
            } else {
                sync = FALSE;
                m_log_view->log( "[MMapper] Lost synchronization with map !!!" );
                emit sendToClient("[MMapper] Lost synchronization with map !!!\r\n");
            }
        }
    } else { // not synced
        m_log_view->log( "[MMapper] Not synchronized with map!!!" );
        Room *tr;
        // try to autosync
        if (Config().m_autoSync && (tr = mapSync(name, desc, parseddesc, r, roomFlags, promptFlags)) ) {
            m_canvas_view->selectRoom(tr);
            sync = TRUE;
            m_log_view->log( "[MMapper] Synchronization succesfull." );
            emit sendToClient("[MMapper] Synchronization succesfull.\r\n");

        }
    }


    //send room info (doors) to client
    if (sync && (r=Data().getCurrentRoom())) {
        QString out;
        out = "";
        if ((r->n) && ISSET(r->n->type,CONNECTION_DOOR)) {
            out = out + " N:" + DOORNAME(r,r->n);
	    if ((DOORNAME(r,r->n)) == "") out += "exit";
        }
        if ((r->s) && ISSET(r->s->type,CONNECTION_DOOR)) {
            out = out + " S:" + DOORNAME(r,r->s);
	    if ((DOORNAME(r,r->s)) == "") out += "exit";
        }
        if ((r->e) && ISSET(r->e->type,CONNECTION_DOOR)) {
            out = out + " E:" + DOORNAME(r,r->e);
	    if ((DOORNAME(r,r->e)) == "") out += "exit";
        }
        if ((r->w) && ISSET(r->w->type,CONNECTION_DOOR)) {
            out = out + " W:" + DOORNAME(r,r->w);
	    if ((DOORNAME(r,r->w)) == "") out += "exit";
        }
        if ((r->u) && ISSET(r->u->type,CONNECTION_DOOR)) {
            out = out + " U:" + DOORNAME(r,r->u);
	    if ((DOORNAME(r,r->u)) == "") out += "exit";
        }
        if ((r->d) && ISSET(r->d->type,CONNECTION_DOOR)) {
            out = out + " D:" + DOORNAME(r,r->d);
	    if ((DOORNAME(r,r->d)) == "") out += "exit";
        }

        if (!out.isEmpty())
            emit sendToClient("Doors:"+out+"\r\n");
    }
}

//////////////////////////////////////////
// Commands and messages from parser
void MMapper::syncOffMessage() {
    sync=FALSE;
    Parser().emptyQueue();
    m_log_view->log( "[MMapper] Synchronization switched off (trigger)" );
}

void MMapper::syncCommand(const QString& name, const QString& desc, const QString& parseddesc, Q_UINT32 roomFlags, Q_UINT32 promptFlags) {
    Room *r;
    m_log_view->log( "[MMapper] Map sync command called for room: " + name );
    emit sendToClient("[MMapper] Map sync command called for room: " + name +"\r\n");

    if ( (r = mapSync( name, desc, parseddesc, Data().getCurrentRoom(), roomFlags, promptFlags)) )
        m_canvas_view->selectRoom(r);
    Parser().emptyQueue();
}

void MMapper::setInfoCommand(const QString& name, const QString& desc, Q_UINT32 roomFlags, Q_UINT32 promptFlags) {
    Room *r = Data().getCurrentRoom();
    if (r) {
        m_log_view->log( "[MMapper] Set info command called for room: " + r->roomName );
        emit sendToClient("[MMapper] Set info command called for room: " + r->roomName+"\r\n");
        Data().updateRoom(r, name, desc, roomFlags, promptFlags);
        Canvas().updateRoom(r);
    }
}

void MMapper::resetCommand() {
    Room *r = Data().getCurrentRoom();
    if (r) {
        m_log_view->log( "[MMapper] Reset command called for room: " + r->roomName );
        emit sendToClient("[MMapper] Reset command called for room: " + r->roomName+"\r\n");
        Data().resetRoom(r);
        Canvas().updateRoom(r);
    }
}

void MMapper::performDoorCommand(DirectionType dir, DoorActionType action, QPtrQueue<Command> queue) {
    Room *r = Data().getCurrentRoom();
    Command *c = NULL;
    QString dn,cn;

    switch (action) {
    case door_open:
        cn = "open ";
        break;
    case door_close:
        cn = "close ";
        break;
    case door_lock:
        cn = "lock ";
        break;
    case door_unlock:
        cn = "unlock ";
        break;
    case door_pick:
        cn = "pick ";
        break;
    case door_rock:
        cn = "throw rock ";
        break;
    case door_bash:
        cn = "bash ";
        break;
    case door_break:
        cn = "cast 'break door' ";
        break;
    case door_block:
        cn = "cast 'block door' ";
        break;

    }

    while (!queue.isEmpty()) {
        c = queue.dequeue();
        switch (c->id) {
        case id_north:
            if (r->n)
                r = ISCONNECTEDTO (r, r->n);
            break;
        case id_south:
            if (r->s)
                r = ISCONNECTEDTO (r, r->s);
            break;
        case id_east:
            if (r->e)
                r = ISCONNECTEDTO (r, r->e);
            break;
        case id_west:
            if (r->w)
                r = ISCONNECTEDTO (r, r->w);
            break;
        case id_up:
            if (r->u)
                r = ISCONNECTEDTO (r, r->u);
            break;
        case id_down:
            if (r->d)
                r = ISCONNECTEDTO (r, r->d);
            break;
        case id_examine:
        case id_look:
        case id_scout:
        case id_unknown:
            break;
        }
    }

    switch (dir) {
    case dir_north:
        if (r->n && ISSET(r->n->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->n)) == "")
                dn = "exit";
	    dn = dn + " n";
            emit sendToServer(cn + dn + "\r\n");
            emit sendToClient("--->" + cn + dn + "\r\n");
        } else {
            emit sendToServer(cn + "exit n" + "\r\n");
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_south:
        if (r->s && ISSET(r->s->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->s)) == "")
                dn = "exit";
	    dn = dn + " s";
            emit sendToServer(cn + dn + "\r\n");
            emit sendToClient("--->" + cn + dn + "\r\n");
        } else {
            emit sendToServer(cn + "exit s" + "\r\n");
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_east:
        if (r->e && ISSET(r->e->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->e)) == "")
                dn = "exit";
	    dn = dn + " e";
            emit sendToServer(cn + dn + "\r\n");
            emit sendToClient("--->" + cn + dn + "\r\n");
        } else {
            emit sendToServer(cn + "exit e" + "\r\n");
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_west:
        if (r->w && ISSET(r->w->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->w)) == "")
                dn = "exit";
	    dn = dn + " w";
            emit sendToServer(cn + dn + "\r\n");
            emit sendToClient("--->" + cn + dn + "\r\n");
        } else {
            emit sendToServer(cn + "exit w" + "\r\n");
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_up:
        if (r->u && ISSET(r->u->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->u)) == "")
                dn = "exit";
	    dn = dn + " u";
            emit sendToServer(cn + dn + "\r\n");
            emit sendToClient("--->" + cn + dn + "\r\n");
        } else {
            emit sendToServer(cn + "exit u" + "\r\n");
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_down:
        if (r->d && ISSET(r->d->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->d)) == "")
                dn = "exit";
	    dn = dn + " d";
            emit sendToServer(cn + dn + "\r\n");
            emit sendToClient("--->" + cn + dn + "\r\n");
        } else {
            emit sendToServer(cn + "exit d" + "\r\n");
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_unknown:
        dn = "exit";
        emit sendToServer(cn + dn + "\r\n");
        emit sendToClient("--->" + cn + dn + "\r\n");
        break;
    }
}

void MMapper::genericDoorCommand(QString command, DirectionType dir, QPtrQueue<Command> queue) {
    Room *r = Data().getCurrentRoom();
    Command *c = NULL;
    QString dn;

    while (!queue.isEmpty()) {
        c = queue.dequeue();
        switch (c->id) {
        case id_north:
            if (r->n)
                r = ISCONNECTEDTO (r, r->n);
            break;
        case id_south:
            if (r->s)
                r = ISCONNECTEDTO (r, r->s);
            break;
        case id_east:
            if (r->e)
                r = ISCONNECTEDTO (r, r->e);
            break;
        case id_west:
            if (r->w)
                r = ISCONNECTEDTO (r, r->w);
            break;
        case id_up:
            if (r->u)
                r = ISCONNECTEDTO (r, r->u);
            break;
        case id_down:
            if (r->d)
                r = ISCONNECTEDTO (r, r->d);
            break;
        case id_examine:
        case id_look:
        case id_scout:
        case id_unknown:
            break;
        }
    }

    switch (dir) {
    case dir_north:
        if (r->n && ISSET(r->n->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->n)) == "")
                dn = "exit";
	    dn = dn + " n";
	    command = command.replace("$$DOOR_N$$",dn);
            emit sendToServer(command);
            emit sendToClient("--->" + command);
        } else {
	    command = command.replace("$$DOOR_N$$","exit n");
            emit sendToServer(command);
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_south:
        if (r->s && ISSET(r->s->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->s)) == "")
                dn = "exit";
	    dn = dn + " s";
	    command = command.replace("$$DOOR_S$$",dn);
            emit sendToServer(command);
            emit sendToClient("--->" + command);
        } else {
	    command = command.replace("$$DOOR_S$$","exit s");
            emit sendToServer(command);
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_east:
        if (r->e && ISSET(r->e->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->e)) == "")
                dn = "exit";
	    dn = dn + " e";
	    command = command.replace("$$DOOR_E$$",dn);
            emit sendToServer(command);
            emit sendToClient("--->" + command);
        } else {
	    command = command.replace("$$DOOR_E$$","exit e");
            emit sendToServer(command);
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_west:
        if (r->w && ISSET(r->w->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->w)) == "")
                dn = "exit";
	    dn = dn + " w";
	    command = command.replace("$$DOOR_W$$",dn);
            emit sendToServer(command);
            emit sendToClient("--->" + command);
        } else {
	    command = command.replace("$$DOOR_W$$","exit w");
            emit sendToServer(command);
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_up:
        if (r->u && ISSET(r->u->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->u)) == "")
                dn = "exit";
	    dn = dn + " u";
	    command = command.replace("$$DOOR_U$$",dn);
            emit sendToServer(command);
            emit sendToClient("--->" + command);
        } else {
	    command = command.replace("$$DOOR_U$$","exit u");
            emit sendToServer(command);
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_down:
        if (r->d && ISSET(r->d->type, CONNECTION_DOOR)) {
            if ((dn=DOORNAME(r,r->d)) == "")
                dn = "exit";
	    dn = dn + " d";
	    command = command.replace("$$DOOR_D$$",dn);
            emit sendToServer(command);
            emit sendToClient("--->" + command);
        } else {
	    command = command.replace("$$DOOR_D$$","exit d");
            emit sendToServer(command);
            emit sendToClient("No doors found in database, using exit instead!!!\r\n");
        }
        break;
    case dir_unknown:
        command = command.replace("$$DOOR$$","exit");
        emit sendToServer(command);
        emit sendToClient("--->" + command);
        break;
    }
}

void MMapper::setDoorCommand(QString doorname, DirectionType dir, QPtrQueue<Command> queue){

    Room *r = Data().getCurrentRoom();
    Command *c = NULL;
    QString dn;

    while (!queue.isEmpty()) {
        c = queue.dequeue();
        switch (c->id) {
        case id_north:
            if (r->n)
                r = ISCONNECTEDTO (r, r->n);
            break;
        case id_south:
            if (r->s)
                r = ISCONNECTEDTO (r, r->s);
            break;
        case id_east:
            if (r->e)
                r = ISCONNECTEDTO (r, r->e);
            break;
        case id_west:
            if (r->w)
                r = ISCONNECTEDTO (r, r->w);
            break;
        case id_up:
            if (r->u)
                r = ISCONNECTEDTO (r, r->u);
            break;
        case id_down:
            if (r->d)
                r = ISCONNECTEDTO (r, r->d);
            break;
        case id_examine:
        case id_look:
        case id_scout:
        case id_unknown:
            break;
        }
    }

    switch (dir) {
    case dir_north:
        if (r->n && ISSET(r->n->type, CONNECTION_DOOR)) {
            SETDOORNAME(r,r->n,doorname);
            emit sendToClient("[MMapper] north door name set: " + doorname + "\r\n");
        } else {
            emit sendToClient("[MMapper] No doors found in this direction!!!\r\n");
        }
        break;
    case dir_south:
        if (r->s && ISSET(r->s->type, CONNECTION_DOOR)) {
            SETDOORNAME(r,r->s,doorname);
            emit sendToClient("[MMapper] south door name set: " + doorname + "\r\n");
        } else {
            emit sendToClient("[MMapper] No doors found in this direction!!!\r\n");
        }
        break;
    case dir_east:
        if (r->e && ISSET(r->e->type, CONNECTION_DOOR)) {
            SETDOORNAME(r,r->e,doorname);
            emit sendToClient("[MMapper] east door name set: " + doorname + "\r\n");
        } else {
            emit sendToClient("[MMapper] No doors found in this direction!!!\r\n");
        }
        break;
    case dir_west:
        if (r->w && ISSET(r->w->type, CONNECTION_DOOR)) {
            SETDOORNAME(r,r->w,doorname);
            emit sendToClient("[MMapper] west door name set: " + doorname + "\r\n");
        } else {
            emit sendToClient("[MMapper] No doors found in this direction!!!\r\n");
        }
        break;
    case dir_up:
        if (r->u && ISSET(r->u->type, CONNECTION_DOOR)) {
            SETDOORNAME(r,r->u,doorname);
            emit sendToClient("[MMapper] up door name set: " + doorname + "\r\n");
        } else {
            emit sendToClient("[MMapper] No doors found in this direction!!!\r\n");
        }
        break;
    case dir_down:
        if (r->d && ISSET(r->d->type, CONNECTION_DOOR)) {
            SETDOORNAME(r,r->d,doorname);
            emit sendToClient("[MMapper] down door name set: " + doorname + "\r\n");
        } else {
            emit sendToClient("[MMapper] No doors found in this direction!!!\r\n");
        }
        break;
    case dir_unknown:
        emit sendToClient("[MMapper] No direction known!!!\r\n");
        break;
     }
     
     m_edit_dialog->updateDialog();
}

void MMapper::closeEvent(QCloseEvent * e)
{
	if (Data().dataChanged())
	{
		const int answer = QMessageBox::question(this, tr("Save the work?"), 
			tr("There are unsaved changes.\nShould the database be saved?"), 
			QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

		switch(answer) {
			case QMessageBox::Yes:
				fileSave(); // fall through
			case QMessageBox::No:
				e->accept();
				break;
			default:
				e->ignore();
				break;
		}
	}
	else
	{
		e->accept();
	}
}

