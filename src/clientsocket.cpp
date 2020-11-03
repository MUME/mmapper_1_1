/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "clientsocket.h"
#include <stdlib.h>
#include "pref.h"
#include "commandeval.h"

// #include <arpa/telnet.h>
#include <qstring.h>

ClientSocket::ClientSocket(int sock, QObject *parent, const char *name)
        : QSocket(parent, name) {
    setSocket ( sock );

    server = new QSocket( this );
    buffer = "";
    linebuf = "";

    //connect( &eval, SIGNAL(sendToClient(const QString&)), SLOT(sendToClient(const QString&)));
    connect( server, SIGNAL(connected()), SLOT(serverConnected()));
    connect( server, SIGNAL(connectionClosed()), SLOT(serverConnectionClosed()));
    connect( server, SIGNAL(error(int)), SLOT(serverError(int)));
    connect( server, SIGNAL(readyRead()), SLOT(serverReadyRead()));
}

void ClientSocket::connectToMUME()
{
	server->connectToHost(Config().m_remoteServerName,Config().m_remotePort);
}

void ClientSocket::sendToClient(const QString& str) {
    writeBlock(str, strlen(str) );
}

void ClientSocket::sendToServer(const QString& str) {
    server->writeBlock(str, strlen(str) );
}


ClientSocket::~ClientSocket() {}

void ClientSocket::serverConnected() {
    emit logText ( tr("[MMapper] Server connected ...\n") );
    connect( this, SIGNAL(readyRead()), SLOT(readClient()) );
    connect( this, SIGNAL(connectionClosed()), SLOT(deleteLater()) );
}

void ClientSocket::serverConnectionClosed() {
    emit logText ( tr("[MMapper] Server connection closed ...\n") );
    close();
    delete this;
}

void ClientSocket::serverError(int e) {
    //
    emit logText ( tr("[MMapper] Server not available error!!!\n") );
    emit logText ( tr("[MMapper] (Error number: '%1')\n").arg(e) );
    close();
    this->deleteLater();
}


void ClientSocket::serverReadyRead() {

    int i;
    bool descriptionline = FALSE;

    count = server->bytesAvailable();
    if (count > 0) {
        buff.resize(count);

        //read data from server
        server->readBlock(buff.data(), count);
        writeBlock(buff.data(), count);

        // skip /0 chars
        for (i = 0; i < count; i++)
            if ( buff[i] == '\0' )
                buff[i] = ' ';

        line = processCommands((QString)buff);

        if (line.endsWith("\n") || line.endsWith("\r"))
            finished = TRUE;
        else
            finished = FALSE;

        QStringList list = QStringList::split("\n",line,TRUE);
        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {

            line = *it;
	 
	    /*
	    if (line[0]=='\r'){
		line.remove(0,1);	    
	    }
	    */
	    if (line.at(line.length()-1)=='\r'){
		//line.remove(line.length()-1,1);
		descriptionline = FALSE;
	    }
	    else
		descriptionline = TRUE;
	    
            if (!linebuf.isEmpty()) {
                line = linebuf + line;
                linebuf = "";
            }

            if ((*it == list.last()) && !finished) {

                if (line.endsWith(">")) {
                    prompt = line.left(line.length()-1);
                    Parser().promptArrived(prompt);
                    continue;
                }
                linebuf = *it;
                continue;
            }

            if ( (index = line.find('>')) > -1) {
                prompt = line.left(index);
                if ( line.find('<') < 0 ) {
                    Parser().promptArrived(prompt);

                    line = line.mid(index+1);
                    if (!line.isEmpty())
                        Parser().parseServerInput(line,descriptionline);
                }
            } else {
                //emit logText ( "[line] "+ line+"\n" );
                Parser().parseServerInput(line,descriptionline);
            }
        }
    }
}




void ClientSocket::readClient() {

    while(canReadLine()) {
        line = readLine();
        if (!(line = Parser().parseClientCommand(line)).isEmpty())
            server->writeBlock(line, strlen(line) );
    }

    if ( (count = bytesAvailable()) > 0) {
        buff.resize(count);

        //read data from client
        readBlock(buff.data(), count);

        //pass them to the server
        server->writeBlock(buff.data(), count);
    }
}


// filter(s telnet commands and removes them from the buffer
// in debugmode inserts human readable commands/options to buffer
QString ClientSocket::processCommands(const QString& buf) {
    uint i = 0;
    uint di = 0;
    
    
    //Description of room:
    // \e[32mXXXXXX\e[0m\r\n
    // TTTTTTTT\n\r
    // TTTTTTTTTT\n\r
    // TT\n\r
    // A large YYYY standing here.\r\n
    
    
    //QString test;

    QString result = buffer+buf;
    buffer = "";

    // search for command sequences
    i = 0;
    while ( i < result.length() ) {
	
	//test = result.mid(i-5,10);

        //c = result.at(i);
        if ( (result.at(i) < 32) && (result.at(i) != '\n') && (result.at(i) != '\r') ) {

            if ( (i+1) == result.length() )
                result=result.remove(i,1);
            else
                if ( (result.at(i) != (char)27) || (result.at(i+1) != '[') )
                    result=result.remove(i,1);
                else
                    i++;
        } else
            if ( result.at(i) == 255 ) { // IAC
                di = 0;

                //check range
                if ( (i + 1) >= result.length() ) {
                    buffer = result.mid(i);
                    result = result.left(i-1);
                    return result;
                }

                if ( result.at(i+1) == 255 ) { // again IAC?
                    di = 1;
                    i++;
                } else
                    if ( (result.at(i+1) >= 240) &&  (result.at(i+1) <= 250) ) {
                        di = 2;
                    } else
                        if ( (result.at(i+1) >= 251) &&  (result.at(i+1) <= 254) ) {
                            di = 3;

                            //check range
                            if ( (i + 2) >= result.length() ) {
                                buffer = result.mid(i);
                                result = result.left(i-1);
                                return result;
                            }
                        }

                if ( di > 0 )
                    result = result.remove(i,di);
            } else
                i++; // no IAC -> skip byte
    }

    return result;

}

