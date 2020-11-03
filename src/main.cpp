/*
* Copyright (C) 2003 Marek Krejza <krejza@cbox.cz>
*/

#include "mmapper.h"
#include <qapplication.h>
//#include <kaboutdata.h>
#include <qlocale.h>
//#include <kcmdlineargs.h>
//#include <kstandarddirs.h>

#if defined(KDE_MAKE_VERSION)
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
#include <qsplashscreen.h>
#endif
#endif

#ifdef WIN32
#include <qsplashscreen.h>
#endif
//static const char *description =
//    I18N_NOOP("MMapper\n - specially designed for MUME (fire.pvv.org:4242)");


int main(int argc, char **argv) {

	//KAboutData about("mmapper", I18N_NOOP("MMapper"), version, description,
	//                 KAboutData::License_GPL , "Marek Krejza", 0, 0, "mkrejza@cbox.cz");
	//about.addAuthor( "Marek Krejza (Caligor)", 0, "mkrejza@cbox.cz", "mmapper.czechian.net" );
	//
	//KCmdLineArgs::init(argc, argv, &about);

	QApplication app(argc, argv);

	QStringList imagepath("pixmaps");
	imagepath.append("icons");
	QMimeSourceFactory *pFac = QMimeSourceFactory::defaultFactory ();
	pFac->setFilePath(imagepath);

#if defined(KDE_MAKE_VERSION)
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
	QPixmap pixmap( locate( "appdata", "pixmaps/splash.png" ) );
	QSplashScreen *splash = new QSplashScreen( pixmap );
	splash->show();
#endif
#endif
#ifdef WIN32
	QPixmap pixmap( QPixmap::fromMimeSource("splash.png") );
	QSplashScreen *splash = new QSplashScreen( pixmap );
	splash->show();
#endif

	MMapper *widget = new MMapper;
	widget->show();

	app.setMainWidget(widget);

#if defined(KDE_MAKE_VERSION)
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)      
	splash->finish( widget );
	delete splash;    
#endif
#endif      

#ifdef WIN32
	splash->finish( widget );
	delete splash;    
#endif

	return app.exec();
}
