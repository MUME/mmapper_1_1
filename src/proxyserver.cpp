/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "proxyserver.h"
#include <stdlib.h>
#include "pref.h"

ProxyServer& Server() {
    static ProxyServer server(Config().m_localPort);
    return server;
};

ProxyServer::ProxyServer( int port ,QObject *parent)
        : QServerSocket(port, 1, parent) {
    if(!ok()) {
        qWarning("Failed to bind to local port!!!");
        exit(1);
    }
}


ProxyServer::~ProxyServer() {}


void ProxyServer::newConnection( int socket ) {
    ClientSocket *s = new ClientSocket( socket, this );
    emit newConnect ( s );

	s->connectToMUME();
}
