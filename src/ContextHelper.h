#ifndef CONTEXT_HELPER_H_INCLUDED
#define CONTEXT_HELPER_H_INCLUDED

#ifndef _MT
#error Required Option: C++ Code Generation Runtime Library = Multi-thread (Debug) DLL
#endif
#ifndef _DLL
#error Required Option: C++ Code Generation Runtime Library = Multi-thread (Debug) DLL
#endif

#include <qstring.h>

//  ==========================================================
//   
/// \class CContextHelper
/// \brief helper function(s) for context help subsystem
/// The class is common for all UI programs offering context help
/// possibility. The granularity of this context help is on the level of
/// dialogs (nonmodal windows), help on controls is provided through tooltips.
/// Suitable to use for MS Windows compressed HTML help.
///
/// Help file(s) have to be in directory helpDir (defaults to help) under the .exe directory 
/// \date 6-2003
///

class CContextHelper {
public:
	CContextHelper();
	/// constructor setting the helpId
    CContextHelper(QString a_dialogId);
    
	virtual ~CContextHelper(void);

    /// dialogId setter
    /// \param dialogId Textual Dialog ID, for which the context help 
    /// is to be displayed. \n
    /// Conceptually would be better to set this attribute as class 
    /// const member and initialize it in constructor initialization chain.
    /// But this class will be used in QT Widget derived classes 
    /// and they restrict access to its own constructor, if created from the 
    /// QT designer. So the setting was moved to stand-alone setter
    /// and has to be done in init() function or just before using the 
    /// getHelpFileName() function
    virtual void setDialogId(QString a_dialogId);

    /// According to current locale function tries to find
    /// locale-specific help file else it falls back to default language version
    /// otherwise. Helpsystem will treat it as 
    /// simple HTML file. Filename has the format \a helpDir\a \\dialogId_\a locale().html
    /// \param existing if set, the file has to exist within filesystem
    /// \return assembled filename or QString::null if existing filename was demanded and 
    ///         none found
    QString getHelpFileName(bool existing, bool compressedHelp) const;

private:
    /// relative path assumed to be relative from .exe module
    static const QString helpDir;
	/// extension for plain html help files
	static const QString htmlExtension;

    /// dialog id, for which the help is to be shown
    QString dialogId;
};

#endif