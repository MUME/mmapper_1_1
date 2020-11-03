/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <qsocket.h>

/**
 * @short ClientSocket class represents network connection socket.
 *
 * Instance of this class is automaticaly created by ProxyServer, whenever some client
 * connects to it. It connects to MUD (MUME) server and passes by all communication 
 * between client and MUD server. 
 * Data, which client sends to MUD server and vice versa are parsed vith CommandEval class.
 * @see ProxyServer
 * @see CommandEval
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class ClientSocket : public QSocket {
    Q_OBJECT
public:
    ClientSocket( int sock, QObject *parent = 0, const char *name = 0);

    ~ClientSocket();

	void connectToMUME();
signals:
    void logText( const QString& );
//    void command( const QString& );
//    void answer( const QString& );

public slots:
    void sendToClient(const QString&);
    void sendToServer(const QString&);
private slots:
    void readClient();

    void serverConnected();
    void serverConnectionClosed();
    void serverError( int );
    void serverReadyRead();

private:

    QSocket *server;
    
    QString line;
    bool finished;
    QCString buff;
    
    QString prompt;
    int count;
    int index;
    
    QString linebuf;
    QString buffer;
    
    QString processCommands(const QString& buf);

    
};



#endif
