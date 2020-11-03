#include "HelpWindow.h"

#include <qfile.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include "ContextHelper.h"

CHelpWindow * CHelpWindow::pHelpWindow = NULL;

CHelpWindow::CHelpWindow(QWidget *parent, const char *name) : QDialog(parent, name, false)
{
    pBrowser = NULL;
}

CHelpWindow::~CHelpWindow(void)
{
}

// static 
bool CHelpWindow::showHtmlHelp(const QString& helpName, bool shouldBeModal) 
{
	// init the window
	if (pHelpWindow == NULL) 
    {
        pHelpWindow = new CHelpWindow(NULL);
    }

	if (pHelpWindow) 
    {
		pHelpWindow->setModal(shouldBeModal);
        return pHelpWindow->showFile(helpName);
    }
	else 
        return false;
}

bool CHelpWindow::showHelp(const CContextHelper& contextHelper, QWidget *helpCaller)
{
	const QString helpName = contextHelper.getHelpFileName(true, false);
	bool retVal = (! helpName.isEmpty());

	if (retVal) 
	{
		const bool callerIsModal = (helpCaller && helpCaller->isModal());

		retVal = CHelpWindow::showHtmlHelp(helpName, callerIsModal);
	}
	else 
		CHelpWindow::complainAboutMissingFile(helpCaller, contextHelper);			

	return retVal;
}

bool CHelpWindow::showFile(const QString& helpName) 
{
    if (helpName.isEmpty()) return false;

    if (pBrowser == NULL) 
    {
        QLayout *pLayout = new QVBoxLayout(this);

        pBrowser = new QTextBrowser( this );
        pBrowser->setFrameStyle( QFrame::Panel | QFrame::Sunken );

		connect(pBrowser, SIGNAL(sourceChanged(const QString&)), SLOT(browserSourceChanged(const QString&)));

        pLayout->add(pBrowser);

		pCloseButton = new QPushButton(tr("Close this help window"), this, "close button");
		pLayout->add(pCloseButton);

		connect(pCloseButton, SIGNAL(clicked()), SLOT(closeButonClicked()));

        resize( 500,700 );
    }

	if (isModal()) 
	{
		pCloseButton->show();
	}
	else 
	{
		pCloseButton->hide();
	}
    pBrowser->setSource("file:/" + helpName);

	setHelpCaption();

    hide();	show();

    return true;
}

void CHelpWindow::setHelpCaption()
{
	// remove the protocol id from the source
	const QString src = pBrowser->source().remove(QRegExp("^\\w+:/"));
	const QString title = pBrowser->documentTitle();
	QString helpCaption(title);

	if (title.isEmpty())
	{
		helpCaption = src;
	}
	else
	{
		helpCaption.append(" - ");
		helpCaption.append(src);
	}

	setCaption(helpCaption);
}

void CHelpWindow::keyPressEvent(QKeyEvent *e)
{
	switch (e->key()) {
	case Key_F5:
		pBrowser->reload();
		return;
	default: /* no action */;
	}
	QWidget::keyPressEvent(e);
}

void CHelpWindow::browserSourceChanged(const QString&)
{
	setHelpCaption();
}

void CHelpWindow::closeButonClicked()
{
	accept();
}

void CHelpWindow::complainAboutMissingFile(QWidget * parent, const QString& missingFile)
{
	QMessageBox::information(parent, tr("Warning"), 
		tr("Help file %1 not found.").arg(missingFile));
}

void CHelpWindow::complainAboutMissingFile(QWidget * parent, const CContextHelper& helper)
{
	const QString missingFile(helper.getHelpFileName(false, false));

	complainAboutMissingFile(parent, missingFile);
}

