## Makefile.am for mmapper

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = mmapper

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path. 
mmapper_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
mmapper_LDADD   = $(LIB_KFILE) $(LIB_KDEPRINT)

# which sources should be compiled for mmapper
mmapper_SOURCES = main.cpp mmapper.cpp pref.cpp proxyserver.h proxyserver.cpp clientsocket.h clientsocket.cpp mapcanvasview.h mapcanvasview.cpp mapcanvas.h mapcanvas.cpp mapdata.h mapdata.cpp commandeval.h commandeval.cpp roomeditdialogimpl.h roomeditdialog.ui roomeditdialogimpl.cpp mmapperlogview.cpp prefpageonebase.ui prefpagetwobase.ui

# these are the headers for your project
noinst_HEADERS = mmapper.h pref.h roomeditdialogimpl.h mmapperlogview.h ansi.h defs.h

# client stuff

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/mmapper.pot

KDE_ICON = mmapper

# this is where the kdelnk file will go 
kdelnkdir = $(kde_appsdir)/Utilities
kdelnk_DATA = mmapper.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/mmapper
rc_DATA = mmapperui.rc
AM_CFLAGS = 
AM_CXXFLAGS = 
AM_FFLAGS = 

