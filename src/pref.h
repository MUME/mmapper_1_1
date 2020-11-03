/***************************************************************************
 *   Copyright (C) 2003 by Marek Krejza                                    *
 *   mkrejza@cbox.cz                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _MMAPPERPREF_H_
#define _MMAPPERPREF_H_

#include <qtabdialog.h>
#include <qframe.h>


#include <qstring.h>
#include <qfont.h>
#include <qcolor.h>

#include "prefpageonebase.h"
#include "prefpagetwobase.h"
#include "ansi.h"

/**
 * @short  First page of preferences dialog.
 *
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class PrefPageOne : public PrefPageOneBase {
    Q_OBJECT
public:
    PrefPageOne(QWidget *parent = 0);
};

/**
 * @short  Second page of preferences dialog.
 *
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class PrefPageTwo : public PrefPageTwoBase {
    Q_OBJECT
public:
    PrefPageTwo(QWidget *parent = 0);
};



/**
 * @short  This is the one and only configuration object.
 *
 * The member functions read() and write() can be used to load and save
 * the properties to the application configuration file.
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class Configuration {
  public:
    /// Reads the configuration data from the application config file.
    /// If a property does not already exist in the config file it will be
    /// set to a default value.
    void read();
    void write() const;
    
    bool m_firstRun;

    QString m_remoteServerName;         /// Remote host and port settings
    Q_UINT32   m_remotePort;
    Q_UINT32   m_localPort;         /// Port to bind to on local machine
    
    Q_UINT32 m_canvasWidth;
    Q_UINT32 m_canvasHeight;
    
    
    bool m_autoLog;         // enables log to file
    QString m_logFileName;  // file name to log
    bool m_autoLoadWorld;
    QString m_autoLoadFileName;
    
    
    QString m_roomNameColor; // ANSI room name color
    bool m_autoRoomUpdate;  // automatic room name and description update
    bool m_autoSync;        // automatic sync on/off
    int  m_syncSearchDepth; // Depth of search from current room, before whole rooms array search
    bool m_brief;
    bool m_showUpdated;
    bool m_autoRoomCreate;
    bool m_drawNotMappedExits;
    
    QStringList m_moveCancelPatterns; // string wildcart patterns, that cancel last move command
    QStringList m_moveForcePatterns;  // string wildcart patterns, that force new move command
    QStringList m_endDescPatterns;
    bool m_patternChanged;

  private:
    Configuration();
    Configuration(const Configuration&);

    friend Configuration& Config();
};

/// Returns a reference to the application configuration object.
Configuration& Config();


/**
 * @short  Application preferences dialog class.
 *
 * Application settings stored in Configuration object can be changed by user in this dialog.
 * @see Configuration
 * @author Marek Krejza <mkrejza@cbox.cz>
 */
class MMapperPreferences : public QTabDialog {
    Q_OBJECT
public:
    MMapperPreferences(QWidget *parent);

    /// Transfers the settings from the configuration object to the dialog.
    void updateDialog();
    /// Transfers the settings from the dialog to the configuration object.
    void updateConfiguration();

public slots:
    /// Will be called when the "Default" button has been clicked.
    void slotDefault();
    /// Will be called when the "Apply" button has been clicked.
    void slotApply();
    /// Will be called whenever a setting was changed.
    void enableApply();
    
    

    void removeForcePatternClicked();
    void removeCancelPatternClicked();
    void removeEndDescPatternClicked();
    void addForcePatternClicked();
    void addCancelPatternClicked();
    void addEndDescPatternClicked();
    void testPatternClicked();
    void validPatternClicked();
    void forcePatternsListActivated(const QString&);
    void cancelPatternsListActivated(const QString&);
    void endDescPatternsListActivated(const QString&);
    
    void resetPatternsClicked();

signals:
    /// Will be emitted when the new settings should be applied.
    void settingsChanged();
    
    
    
private:
    PrefPageOne *m_pageOne;
    PrefPageTwo *m_pageTwo;
    
    bool m_patternChanged;
};



#endif // _MMAPPERPREF_H_
