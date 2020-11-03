/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MMAPPERLOGVIEW_H
#define MMAPPERLOGVIEW_H

#include <qwidget.h>
#include <qtextview.h>
#include <qtextstream.h>
#include <qfile.h>

#include "pref.h"


class QPainter;

/**
 * @short MMapperLogView class accepts and displays application log messages.
 *
 * It can display two types of messages. First type is normal application message, second type is 
 * debug message. Normal messages are displayed in view that is provided with this class, debug
 * messages are sent also to external log file too. 
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MMapperLogView : public QWidget {
    Q_OBJECT
public:
    MMapperLogView(QWidget *parent);
    virtual ~MMapperLogView();


public slots:

    void log(const QString&);
    void logDebug(const QString& text);


private:
    QTextView *infoText;
    
    QFile *f;
    QTextStream *ts;
};


#endif
