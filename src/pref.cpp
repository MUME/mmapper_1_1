/***************************************************************************
*   Copyright (C) 2003 by Marek Krejza                                    *
*   mkrejza@cbox.cz                                                       *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "pref.h"

#include <qlocale.h>

#include <qlayout.h>
#include <qlabel.h>

#include <qapplication.h>       // for 'kapp'
//#include <kconfig.h>            // for KConfig
#include <qmessagebox.h>
//#include <ktextrequester.h>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qsettings.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <memory>

Configuration::Configuration() {
	read(); // read the settings or set them to the default values
};

void Configuration::read() {

	//    KConfig *conf=kapp->config();

	std::auto_ptr<QSettings> conf(new QSettings);

	conf->setPath("Caligor", "MMapper", QSettings::User);

	conf->beginGroup("General");
	m_firstRun = conf->readBoolEntry("Run first time", TRUE);
	conf->resetGroup();

	// read general options
	conf->beginGroup("Connection");

	m_remoteServerName = conf->readEntry("Server name", "fire.pvv.org");
	//qDebug("[R] Canvas height: %s", m_remoteServerName.ascii() );
	m_remotePort = conf->readNumEntry("Remote port number", 4242);
	//qDebug("[R] Remote port: %i", m_remotePort);

	m_localPort = conf->readNumEntry("Local port number", 4242);
	//qDebug("[R] Local port: %i",m_localPort);
	conf->resetGroup();

	conf->beginGroup("Canvas");
	m_canvasWidth = conf->readNumEntry("Canvas width", 60 /*0*/);
	//qDebug("[R] Canvas width: %i", m_canvasWidth );
	m_canvasHeight = conf->readNumEntry("Canvas height", 30/*0*/);
	//qDebug("[R] Canvas height: %i", m_canvasHeight );
	m_showUpdated = conf->readBoolEntry("Show updated rooms", TRUE);
	m_drawNotMappedExits = conf->readBoolEntry("Draw not mapped exits", TRUE);
	conf->resetGroup();

	conf->beginGroup("Debug");
	m_autoLog = conf->readBoolEntry("Enable log", FALSE);
	m_logFileName = conf->readEntry("Log file name", "~/mmapper.log");
	conf->resetGroup();

	conf->beginGroup("Auto load world");
	m_autoLoadWorld = conf->readBoolEntry("Auto load", FALSE );
	m_autoLoadFileName = conf->readEntry("File name", "world.dat");
	conf->resetGroup();

	conf->beginGroup("Parser");
	m_roomNameColor = conf->readEntry("Room name ansi color", "[32m");    
	m_autoRoomUpdate = conf->readBoolEntry("Auto room update", TRUE);
	m_autoSync = conf->readBoolEntry("Map auto sync", TRUE);
	m_autoRoomCreate = conf->readBoolEntry("Map auto room create", TRUE);
	m_syncSearchDepth = conf->readNumEntry("Map sync search depth", 5);
	m_moveForcePatterns = conf->readListEntry("Move force patterns");
	m_moveCancelPatterns = conf->readListEntry("Move cancel patterns");
	m_endDescPatterns = conf->readListEntry("Non-standard room description patterns");
	conf->resetGroup();

	conf->beginGroup("Mume native");
	m_brief = conf->readBoolEntry("Brief mode", FALSE);
	conf->resetGroup();
};

void Configuration::write() const {
	//    KConfig *conf=kapp->config();
	std::auto_ptr<QSettings> conf(new QSettings);

	conf->setPath("Caligor", "MMapper", QSettings::User);

	conf->beginGroup("General");
	conf->writeEntry("Run first time", FALSE);
	conf->resetGroup();

	// write general options
	conf->beginGroup("Connection");
	conf->writeEntry("Server name", m_remoteServerName);
	//qDebug("[W] server name %s", m_remoteServerName.ascii());

	//qDebug("[W] Remote port: %i",m_remotePort);
	conf->writeEntry("Remote port number", (int) m_remotePort);
	//qDebug("[W] Local port: %i",m_localPort);
	conf->writeEntry("Local port number", (int) m_localPort);
	conf->resetGroup();

	// write style options
	conf->beginGroup("Canvas");
	//qDebug("[W] Canvas width: %i",m_canvasWidth);
	conf->writeEntry("Canvas width", (int) m_canvasWidth);
	//qDebug("[W] Canvas height: %i",m_canvasHeight);
	conf->writeEntry("Canvas height", (int) m_canvasHeight);
	conf->writeEntry("Show updated rooms", m_showUpdated);
	conf->writeEntry("Draw not mapped exits", m_drawNotMappedExits);
	conf->resetGroup();

	conf->beginGroup("Debug");
	conf->writeEntry("Enable log", m_autoLog);
	conf->writeEntry("Log file name", m_logFileName);
	conf->resetGroup();

	conf->beginGroup("Auto load world");
	conf->writeEntry("Auto load", m_autoLoadWorld);
	conf->writeEntry("File name", m_autoLoadFileName);
	conf->resetGroup();

	conf->beginGroup("Parser");
	conf->writeEntry("Room name ansi color", m_roomNameColor);
	conf->writeEntry("Auto room update", m_autoRoomUpdate);
	conf->writeEntry("Map auto sync", m_autoSync);
	conf->writeEntry("Map auto room create", m_autoRoomCreate);
	conf->writeEntry("Map sync search depth", m_syncSearchDepth);
	conf->writeEntry("Move force patterns", m_moveForcePatterns);
	conf->writeEntry("Move cancel patterns", m_moveCancelPatterns);
	conf->writeEntry("Non-standard room description patterns", m_endDescPatterns);
	conf->resetGroup();

	conf->beginGroup("Mume native");
	conf->writeEntry("Brief mode", m_brief);
	conf->resetGroup();
};

Configuration& Config() {
	static Configuration conf;
	return conf;
};


MMapperPreferences::MMapperPreferences(QWidget *parent)
//        : KDialogBase(TreeList, tr("MMapper Preferences"),
//              /*Help|*/Default|Apply|Cancel, Apply) {
: QTabDialog(parent) {
	setDefaultButton();
	setOkButton();
	setCancelButton();
	setApplyButton(tr("Apply"));

	connect(this, SIGNAL(applyButtonPressed()), SLOT(slotApply()));
	connect(this, SIGNAL(defaultButtonPressed()), SLOT(slotDefault()));

	// this is the base class for your preferences dialog.  it is now
	// a Treelist dialog.. but there are a number of other
	// possibilities (including Tab, Swallow, and just Plain)
	m_patternChanged=FALSE;

	/*
	QBoxLayout *l;
	QFrame *frame;
	frame = addPage(tr("Global"), tr("Global settings page"));
	l = new QVBoxLayout (frame);
	m_pageOne = new PrefPageOne(frame);
	l->addWidget(m_pageOne);

	frame = addPage(tr("Parser"), tr("Parser options page"));
	l = new QVBoxLayout (frame);
	m_pageTwo = new PrefPageTwo(frame);
	l->addWidget(m_pageTwo);
	*/

	m_pageOne = new PrefPageOne(this); addTab(m_pageOne, tr("Global"));
	m_pageTwo = new PrefPageTwo(this); addTab(m_pageTwo, tr("Parser"));

	// connect interactive widgets and selfmade signals to the enableApply slotDefault
	connect( m_pageOne->m_remoteName, SIGNAL( textChanged(const QString&) ), this, SLOT( enableApply() )  );
	connect( m_pageOne->autoLoadFileName, SIGNAL( textChanged(const QString&) ), this, SLOT( enableApply() )  );
	connect( m_pageOne->autoLoadCheck, SIGNAL(stateChanged(int)),SLOT(enableApply()));
	connect( m_pageOne->logFileName, SIGNAL( textChanged(const QString&) ), this, SLOT( enableApply() )  );
	connect( m_pageOne->logCheck, SIGNAL(stateChanged(int)),SLOT(enableApply()));

	connect( m_pageOne->m_remotePort, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() )  );
	connect( m_pageOne->m_localPort, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() )  );
	connect( m_pageOne->m_canvasWidth, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() )  );
	connect( m_pageOne->m_canvasHeight, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );

	connect ( m_pageOne->brief, SIGNAL(stateChanged(int)),SLOT(enableApply()));
	connect ( m_pageOne->autoRoomCreate, SIGNAL(stateChanged(int)),SLOT(enableApply()));
	connect ( m_pageOne->updated, SIGNAL(stateChanged(int)),SLOT(enableApply()));
	connect ( m_pageOne->drawNotMappedExits, SIGNAL(stateChanged(int)),SLOT(enableApply()));

	connect( m_pageTwo->removeCancelPattern, SIGNAL(clicked()),SLOT(removeCancelPatternClicked()));
	connect( m_pageTwo->removeForcePattern, SIGNAL(clicked()),SLOT(removeForcePatternClicked()));
	connect( m_pageTwo->removeEndDescPattern, SIGNAL(clicked()),SLOT(removeEndDescPatternClicked()));

	connect( m_pageTwo->addCancelPattern, SIGNAL(clicked()),SLOT(addCancelPatternClicked()));
	connect( m_pageTwo->addForcePattern, SIGNAL(clicked()),SLOT(addForcePatternClicked()));
	connect( m_pageTwo->addEndDescPattern, SIGNAL(clicked()),SLOT(addEndDescPatternClicked()));

	connect( m_pageTwo->resetPatterns, SIGNAL(clicked()),SLOT(resetPatternsClicked()));

	connect( m_pageTwo->testPattern, SIGNAL(clicked()),SLOT(testPatternClicked()));
	connect( m_pageTwo->validPattern, SIGNAL(clicked()),SLOT(validPatternClicked()));

	connect( m_pageTwo->forcePatternsList,SIGNAL(activated(const QString&)),
		SLOT(forcePatternsListActivated(const QString&)));
	connect( m_pageTwo->cancelPatternsList,SIGNAL(activated(const QString&)),
		SLOT(cancelPatternsListActivated(const QString&)));
	connect( m_pageTwo->endDescPatternsList,SIGNAL(activated(const QString&)),
		SLOT(endDescPatternsListActivated(const QString&)));


	connect ( m_pageTwo->ansiColor, SIGNAL(textChanged(const QString&)),SLOT(enableApply()));
	connect ( m_pageOne->autoUpdate, SIGNAL(stateChanged(int)),SLOT(enableApply()));
	connect ( m_pageOne->autoSync, SIGNAL(stateChanged(int)),SLOT(enableApply()));
	connect ( m_pageOne->syncDepth, SIGNAL(valueChanged(int)),SLOT(enableApply()));

	Config().read();
	updateDialog();
}

PrefPageOne::PrefPageOne(QWidget *parent)
: PrefPageOneBase(parent) {
}

PrefPageTwo::PrefPageTwo(QWidget *parent)
: PrefPageTwoBase(parent) {
}




void MMapperPreferences::updateDialog() {

	m_pageOne->m_remoteName->setText( Config().m_remoteServerName );
	m_pageOne->m_remotePort->setValue( Config().m_remotePort );
	m_pageOne->m_localPort->setValue( Config().m_localPort );
	m_pageOne->m_canvasWidth->setValue( Config().m_canvasWidth );
	m_pageOne->m_canvasHeight->setValue( Config().m_canvasHeight );

	m_pageOne->logCheck->setChecked( Config().m_autoLog ); 
	m_pageOne->logFileName->setText( Config().m_logFileName );
	m_pageOne->autoLoadCheck->setChecked( Config().m_autoLoadWorld ); 
	m_pageOne->autoLoadFileName->setText( Config().m_autoLoadFileName );

	m_pageOne->brief->setChecked( Config().m_brief );
	m_pageOne->updated->setChecked( Config().m_showUpdated );
	m_pageOne->autoRoomCreate->setChecked( Config().m_autoRoomCreate );
	m_pageOne->drawNotMappedExits->setChecked( Config().m_drawNotMappedExits );

	m_pageTwo->ansiColor->setText( Config().m_roomNameColor );
	m_pageOne->autoUpdate->setChecked( Config().m_autoRoomUpdate );
	m_pageOne->autoSync->setChecked( Config().m_autoSync );
	m_pageOne->syncDepth->setValue( Config().m_syncSearchDepth );
	m_pageTwo->forcePatternsList->clear();
	m_pageTwo->forcePatternsList->insertStringList( Config().m_moveForcePatterns );
	m_pageTwo->cancelPatternsList->clear();
	m_pageTwo->cancelPatternsList->insertStringList( Config().m_moveCancelPatterns );
	m_pageTwo->endDescPatternsList->clear();
	m_pageTwo->endDescPatternsList->insertStringList( Config().m_endDescPatterns );


	//enableButtonApply(false);   // disable apply button
};


void MMapperPreferences::updateConfiguration() {

	Config().m_remoteServerName = m_pageOne->m_remoteName->text();
	Config().m_remotePort = m_pageOne->m_remotePort->value();
	Config().m_localPort = m_pageOne->m_localPort->value();
	Config().m_canvasWidth = m_pageOne->m_canvasWidth->value();
	Config().m_canvasHeight = m_pageOne->m_canvasHeight->value();

	Config().m_autoLog = m_pageOne->logCheck->isChecked(); 
	Config().m_logFileName = m_pageOne->logFileName->text();
	Config().m_autoLoadWorld = m_pageOne->autoLoadCheck->isChecked(); 
	Config().m_autoLoadFileName = m_pageOne->autoLoadFileName->text();

	Config().m_brief = m_pageOne->brief->isChecked();
	Config().m_showUpdated = m_pageOne->updated->isChecked();
	Config().m_autoRoomCreate = m_pageOne->autoRoomCreate->isChecked();
	Config().m_drawNotMappedExits = m_pageOne->drawNotMappedExits->isChecked();

	Config().m_roomNameColor = m_pageTwo->ansiColor->text();
	Config().m_autoRoomUpdate = m_pageOne->autoUpdate->isChecked();
	Config().m_autoSync = m_pageOne->autoSync->isChecked();
	Config().m_syncSearchDepth = m_pageOne->syncDepth->value();

	Config().m_moveForcePatterns.clear();
	for ( int i=0; i<m_pageTwo->forcePatternsList->count(); i++)
		Config().m_moveForcePatterns.append( m_pageTwo->forcePatternsList->text(i) );

	Config().m_moveCancelPatterns.clear();
	for ( int i=0; i<m_pageTwo->cancelPatternsList->count(); i++)
		Config().m_moveCancelPatterns.append( m_pageTwo->cancelPatternsList->text(i) );

	Config().m_endDescPatterns.clear();
	for ( int i=0; i<m_pageTwo->endDescPatternsList->count(); i++)
		Config().m_endDescPatterns.append( m_pageTwo->endDescPatternsList->text(i) );

	//    enableButtonApply(false);   // disable apply button
};


void MMapperPreferences::slotDefault() {
	if (QMessageBox::question(this, 
		tr("Confirmation"),
		tr("This will set the default options in ALL pages\n"
		"of the preferences dialog!\nContinue?"), 
		tr("Continue"), tr("Cancel"), QString::null, 0, 1) == 0)
	{
		m_pageOne->m_remoteName->setText( "fire.pvv.org" );
		m_pageOne->m_remotePort->setValue( 4242 );
		m_pageOne->m_localPort->setValue( 4242 );
		m_pageOne->m_canvasWidth->setValue( 600 );
		m_pageOne->m_canvasHeight->setValue( 300 );
		m_pageOne->logCheck->setChecked(FALSE);
		m_pageOne->autoLoadCheck->setChecked(FALSE);

		m_pageOne->brief->setChecked(FALSE);
		m_pageOne->updated->setChecked(FALSE);
		m_pageOne->autoRoomCreate->setChecked(TRUE);	
		m_pageOne->drawNotMappedExits->setChecked(TRUE);

		m_pageTwo->ansiColor->setText( "[32m" );
		m_pageOne->autoUpdate->setChecked( TRUE );
		m_pageOne->autoSync->setChecked( TRUE );
		m_pageOne->syncDepth->setValue( 5 );

		resetPatternsClicked();
		enableApply();   // enable apply button
	};
};


void MMapperPreferences::slotApply() {

	if (m_pageTwo->forcePatternsList->hasFocus()) return; 
	if (m_pageTwo->cancelPatternsList->hasFocus()) return;
	if (m_pageTwo->endDescPatternsList->hasFocus()) return;   


	updateConfiguration();      // transfer settings to configuration object
	Config().write();
	emit settingsChanged();     // apply the preferences
	//	//enableButtonApply(false);   // disable apply button again
};


void MMapperPreferences::enableApply() {
	//enableButtonApply(true);   // enable apply button
};



void MMapperPreferences::removeForcePatternClicked(){
	m_pageTwo->forcePatternsList->removeItem(m_pageTwo->forcePatternsList->currentItem());
	//enableButtonApply(true);
}

void MMapperPreferences::removeCancelPatternClicked(){
	m_pageTwo->cancelPatternsList->removeItem(m_pageTwo->cancelPatternsList->currentItem());
	//enableButtonApply(true);   
}

void MMapperPreferences::removeEndDescPatternClicked(){
	m_pageTwo->endDescPatternsList->removeItem(m_pageTwo->endDescPatternsList->currentItem());
	//enableButtonApply(true);    
}

void MMapperPreferences::testPatternClicked(){
	QString pattern=m_pageTwo->newPattern->text();
	QString str=m_pageTwo->testString->text();
	bool matches = FALSE;
	QRegExp rx; 

	if ((pattern)[0] != '#') {
	}
	else{
		switch ((int)((QChar)(pattern)[1])){
		case 33:  // !
			rx.setPattern((pattern).remove(0,2));
			if(rx.exactMatch(str)) matches = TRUE;
			break;
		case 60:  // <
			if(str.startsWith((pattern).remove(0,2))) matches = TRUE;
			break;
		case 61:  // =
			if( str==((pattern).remove(0,2)) ) matches = TRUE;
			break;
		case 62:  // >
			if(str.endsWith((pattern).remove(0,2))) matches = TRUE;
			break;
		case 63:  // ?
			if(str.contains((pattern).remove(0,2))) matches = TRUE;
			break;
		default:;
		}
	}

	str= (matches) ? tr("Pattern matches!!!") : tr("Pattern doesn't match!!!");	

	QMessageBox::information( this, tr("Pattern match test"), str);
}

void MMapperPreferences::validPatternClicked(){

	QString pattern=m_pageTwo->newPattern->text();    
	QString str = "Pattern '"+pattern+"' is valid!!!";

	if (((pattern)[0] != '#') || (((pattern)[1] != '!') && ((pattern)[1] != '?') && ((pattern)[1] != '<') && ((pattern)[1] != '>') && ((pattern)[1] != '=')) ) {
		str = "Pattern must begin with '#t', where t means type of pattern (!?<>=)";	    
	}else
		if ((pattern)[1] == '!'){
			QRegExp rx(pattern.remove(0,2));  
			if (!rx.isValid())
				str="Pattern '"+pattern+"' is not valid!!!";	
		}

		QMessageBox::information( this, "Pattern match test",
			str ,
			"&Discard",
			0,      // Enter == button 0
			0 );    
}

void MMapperPreferences::forcePatternsListActivated(const QString& str){   
	m_pageTwo->newPattern->setText(str);
}

void MMapperPreferences::cancelPatternsListActivated(const QString& str){
	m_pageTwo->newPattern->setText(str);
}

void MMapperPreferences::endDescPatternsListActivated(const QString& str){
	m_pageTwo->newPattern->setText(str);
}

void MMapperPreferences::addForcePatternClicked(){
	QString str;
	if((str=m_pageTwo->newPattern->text())!="") {
		m_pageTwo->forcePatternsList->insertItem(str);
		m_pageTwo->forcePatternsList->setCurrentItem(m_pageTwo->forcePatternsList->count()-1);
		//enableButtonApply(true);
	}
}

void MMapperPreferences::addCancelPatternClicked(){
	QString str;
	if((str=m_pageTwo->newPattern->text())!=""){
		m_pageTwo->cancelPatternsList->insertItem(str);
		m_pageTwo->cancelPatternsList->setCurrentItem(m_pageTwo->cancelPatternsList->count()-1);
		//enableButtonApply(true);
	}
}

void MMapperPreferences::addEndDescPatternClicked(){
	QString str;
	if((str=m_pageTwo->newPattern->text())!=""){
		m_pageTwo->endDescPatternsList->insertItem(str);
		m_pageTwo->endDescPatternsList->setCurrentItem(m_pageTwo->endDescPatternsList->count()-1);
		//enableButtonApply(true);
	}  
}


void MMapperPreferences::resetPatternsClicked(){

	// ************************************** MOVE CANCEL PATTERNS **************************************

	m_pageTwo->cancelPatternsList->clear();
	m_pageTwo->cancelPatternsList->insertItem("#=Your boat cannot enter this place.");
	m_pageTwo->cancelPatternsList->insertItem("#>guard steps in front of you.");
	m_pageTwo->cancelPatternsList->insertItem("#>bars your way.");
	m_pageTwo->cancelPatternsList->insertItem("#=Maybe you should get on your feet first?");
	m_pageTwo->cancelPatternsList->insertItem("#<Alas, you cannot go that way...");
	m_pageTwo->cancelPatternsList->insertItem("#<You need to swim");
	m_pageTwo->cancelPatternsList->insertItem("#=You failed swimming there.");
	m_pageTwo->cancelPatternsList->insertItem("#<You failed to climb there");
	m_pageTwo->cancelPatternsList->insertItem("#=No way! You are fighting for your life!");
	m_pageTwo->cancelPatternsList->insertItem("#<Nah...");
	m_pageTwo->cancelPatternsList->insertItem("#=You are too exhausted.");
	m_pageTwo->cancelPatternsList->insertItem("#=PANIC! You couldn't escape!");
	m_pageTwo->cancelPatternsList->insertItem("#=PANIC! You can't quit the fight!");
	m_pageTwo->cancelPatternsList->insertItem("#<ZBLAM");
	m_pageTwo->cancelPatternsList->insertItem("#<Oops!");
	m_pageTwo->cancelPatternsList->insertItem("#>seems to be closed.");
	m_pageTwo->cancelPatternsList->insertItem("#>you need to climb to go there.");
	m_pageTwo->cancelPatternsList->insertItem("#=In your dreams, or what?");
	m_pageTwo->cancelPatternsList->insertItem("#<You'd better be swimming");


	// ************************************** MOVE FORCE PATTERNS **************************************

	m_pageTwo->forcePatternsList->clear();
	m_pageTwo->forcePatternsList->insertItem("#=You flee head over heels.");
	m_pageTwo->forcePatternsList->insertItem("#=You are borne along by a strong current.");
	m_pageTwo->forcePatternsList->insertItem("#?leads you out");
	m_pageTwo->forcePatternsList->insertItem("#=You are dead! Sorry...");
	m_pageTwo->forcePatternsList->insertItem("#<Your feet slip, making you fall to the");
	m_pageTwo->forcePatternsList->insertItem("#<Suddenly an explosion of ancient rhymes");


	// *************************************** END DESC PATTERNS ****************************************

	m_pageTwo->endDescPatternsList->clear();
	m_pageTwo->endDescPatternsList->insertItem("#=It is pitch black...");
	m_pageTwo->endDescPatternsList->insertItem("#=You just see a dense fog around you...");

	// **************************************************************************************************

	m_pageTwo->cancelPatternsList->setCurrentItem(1);
	m_pageTwo->forcePatternsList->setCurrentItem(1);
	m_pageTwo->endDescPatternsList->setCurrentItem(1);

	//enableButtonApply(true);

}

