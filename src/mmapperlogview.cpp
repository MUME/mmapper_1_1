/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "mmapperlogview.h"

#include <qpainter.h>
#include <qlayout.h>

#include <qurl.h>

//#include <ktrader.h>
//#include <klibloader.h>
#include <qmessagebox.h>
//#include <krun.h>
#include <qlocale.h>
#include "defs.h"

MMapperLogView::MMapperLogView(QWidget *parent)
        : QWidget(parent) {

    QHBoxLayout *top_layout = new QHBoxLayout(this);

    ts = NULL;
    if (Config().m_autoLog) {
	f = new QFile(Config().m_logFileName);
	if ( f && f->open( IO_WriteOnly ) ){
	   ts = new QTextStream ( f );   
	}
    }
    
    infoText = new QTextView ( this );
    top_layout->addWidget( infoText );
    if (ts) *ts << "\n[MMapper] *** MUME Mapper proxy version "+version+" by Caligor ***\n\n";
    infoText->append( tr("[MMapper] *** MUME Mapper proxy version "+version+" by Caligor ***") );
    infoText->append( tr("\n") );
    infoText->show();
}

MMapperLogView::~MMapperLogView() {}


void MMapperLogView::logDebug(const QString& text){
    if (ts) {
	*ts << "***DEBUG***\n" << text << "\n";
    }
}


void MMapperLogView::log(const QString& text){
    infoText->append(text); 
    if (ts) *ts << text << "\n";
}


//#include "mmapperlogview.moc"
