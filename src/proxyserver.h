/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include <qserversocket.h>
#include "clientsocket.h"

/**
 * @short This class works as proxy server.
 * 
 * When application starts, this object binds to predefined port (@ref Configuration) on local system, 
 * and waits for connections from clients. When connection arrives, new ClientSocket
 * object is created and it takes all responsibility for future communication.
 * @see ClientSocket
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class ProxyServer : public QServerSocket {
    Q_OBJECT
public:

    void newConnection( int socket );

	~ProxyServer();


signals:
    void newConnect( ClientSocket * );
    
private:
    ProxyServer( int port ,QObject *parent = 0);

    friend ProxyServer& Server();
};

/// Returns a reference to the application configuration object.
ProxyServer& Server();

#endif
