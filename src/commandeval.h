/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMMANDEVAL_H
#define COMMANDEVAL_H

#include <qobject.h>
#include <qptrqueue.h>
#include <qregexp.h>
#include "defs.h"

// 0 - null, 1 n, 2 s, 3 e, 4 w, 5, u, 6 d, 7 look, 8 exa
#define id_north 1
#define id_south 2
#define id_east 3
#define id_west 4
#define id_up 5
#define id_down 6
#define id_flee 10

#define id_scout 11

#define id_null 0
#define id_look 20
#define id_examine 21
#define id_sync 22
#define id_reset 23
#define id_unknown 100


#define AUTHOR MAREK
/**
 * @short Command class represents move command sended by client to server.
 *
 * @see CommandEval
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class Command
{
public:
    Command(int c){id=c;};  
    
public:
  int id;   
};

/**
 * @short CommandEval class works as client/server communication parser.
 *
 * This class parses communication data (text lines) betwen server and client (and vice versa)
 * and does corresponding reactions. 
 * For example if room name arrived from MUD server, this class recognizes
 * it, and starts to parse room descriptions from next incoming lines of text. Or if text data
 * starts with "Exits:", parser recognizes end of description and emits signal that character
 * changed it's position on MUME.
 * @see ProxyServer
 * @see ClientSocket
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class CommandEval : public QObject
{
Q_OBJECT
public:
    
    QString parseClientCommand(QString&);
    QString parseServerInput(QString&,bool);
    void promptArrived(QString&);
    
    QString parseRoomName(QString& str);
    bool parseEndDesc(const QString& str);
    bool nonStandardDesc(const QString& str);
    void parseAnswers(QString& str);
    
    //QString parseDesc(const QString& str);
    
    QString removeAnsiMarks(QString&);    
    
	~CommandEval();

public slots:
    void emptyQueue();

    

signals:
    void logText( const QString& );
    void characterMoved(Command*,const QString&,const QString&,const QString&, Q_UINT32, Q_UINT32);
    void sendToClient(const QString&);
    void showPath(QPtrQueue<Command> queue);
    
    void syncCommand(const QString&, const QString&, const QString&, Q_UINT32, Q_UINT32);
    void setInfoCommand(const QString&, const QString&, Q_UINT32, Q_UINT32);
    void performDoorCommand(DirectionType dir, DoorActionType action, QPtrQueue<Command> queue);
    void genericDoorCommand(QString command, DirectionType dir, QPtrQueue<Command> queue);
    void setDoorCommand(QString doorname, DirectionType dir, QPtrQueue<Command> queue);
    void resetCommand();
    
    void setEditMode();
    void setPlayMode();
    void setFollowMode();
    
    void syncOffMessage();
    
protected:
   bool readingRoomDesc;
   bool descriptionready;
   
   QString roomName;
   QString roomDesc;
   QString parsedRoomDesc;
   
   Q_UINT32 roomFlags;
   Q_UINT32 promptFlags;
   
   bool following;
   //QString followName;
   DirectionType followDir;
   
   QPtrQueue<Command> queue;
   
   // spped temporary variables
   QRegExp rx;
   QStringList list;
   QStringList patternlist;
   QString tmpstr;
   int index;
   
private:    
    CommandEval(QObject *parent = 0, const char *name = 0);

   friend CommandEval& Parser();
};

/// Returns a reference to the application parser object.
CommandEval& Parser();



#endif
