#ifndef  HELP_WINDOW_INCLUDED
#define HELP_WINDOW_INCLUDED

#include <qdialog.h>
#include <qtextbrowser.h>

//  ==========================================================
//   
/// \class CHelpWindow
/// \brief HTML browser used for browsing over help texts
/// Implementation shares one and only instance of the help window
/// to prevent flooding of desktop with help windows.
/// If the showHelp() is called from the modal dialogs, the modal parameter
/// should also be true otherwise all the events (help scrolling) will be stolen by the 
/// modal parent. Modal call has to be closed by "Close" button, non-modal can freely float around.
/// \par Compressed HTML help (CHM) support
/// By setting member variable chmHelp to true compressed CHM files start to be used
/// \author Jan 'Kovis' Struhar jstruhar@amberg.cz
/// \date 7-2003
///

class CContextHelper;

class CHelpWindow : public QDialog
{
	Q_OBJECT
public:
	/// convenience function, which accepts often used CContextHelper class
	/// and according to parent pointer widget modality it chooses the help browser modality
	/// displays the complaint if the file would not found - so to say complete service
	/// instead of duplicating this functionality in every help enabled dialog
	///\return false if the window could not be open
	static bool showHelp(const CContextHelper& helper, QWidget *helpCaller);

	/// convenience overload of the above function
	static void complainAboutMissingFile(QWidget * parent, const CContextHelper& helper);

protected slots:
	/// update the caption when the browsed file was changed
	void browserSourceChanged(const QString&);
	void closeButonClicked();

protected:
	/// static instance of the one and only help browser
	static CHelpWindow *pHelpWindow;

	/// plainly show the help browser in desired modality with given filename open
	///\return false if the window could not be open
	static bool showHtmlHelp(const QString& helpname, bool shouldBeModal);

	/// inform the user, that required help file is missing - currently a message box
	static void complainAboutMissingFile(QWidget * parent, const QString& missingFile);

	QTextBrowser *pBrowser;
	QPushButton *pCloseButton;

    CHelpWindow(QWidget *parent, const char *name = NULL);
    ~CHelpWindow(void);

    bool showFile(const QString& helpname);

	/// to be called after the browser source was set, updates the window caption properly
	void setHelpCaption();

	/// overridden to refresh on F5
	void keyPressEvent(QKeyEvent *);
};

#endif