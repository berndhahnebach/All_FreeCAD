/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <QAbstractTextDocumentLayout>
# include <QApplication>
# include <QClipboard>
# include <QDateTime>
# include <QHBoxLayout>
# include <QMessageBox>
# include <QPainter>
# include <QPrinter>
# include <QPrintDialog>
# include <QScrollBar>
# include <QtWebKit/QWebView>
# include <QTextBlock>
# include <QTextCodec>
# include <QTextStream>
# include <QTimer>
#endif

#include "BrowserView.h"
#include "Application.h"
#include "FileDialog.h"
#include "Macro.h"

#include <Base/Parameter.h>

using namespace Gui;


/* TRANSLATOR Gui::BrowserView */

/**
 *  Constructs a BrowserView which is a child of 'parent', with the
 *  name 'name'.
 */
BrowserView::BrowserView(QWidget* parent)
    : MDIView(0,parent,0), WindowParameter( "Browser" )
{
#if QT_VERSION >= 0x040400
    WebView = new QWebView(parent);
#endif
}

/** Destroys the object and frees any allocated resources */
BrowserView::~BrowserView()
{
#if QT_VERSION >= 0x040400
    delete WebView;
#endif
}


void BrowserView::Load(const char* URL)
{
#if QT_VERSION >= 0x040400
    WebView->load(QUrl(QString::fromUtf8(URL)));
#endif
}

void BrowserView::OnChange(Base::Subject<const char*> &rCaller,const char* rcReason)
{
    //ParameterGrp::handle hPrefGrp = getWindowParameter();
    //if (strcmp(rcReason, "EnableLineNumber") == 0) {
    //    bool show = hPrefGrp->GetBool( "EnableLineNumber", true );
    //    if ( show )
    //        d->lineMarker->show();
    //    else
    //        d->lineMarker->hide();
    //}
}


/**
 * Runs the action specified by \a pMsg.
 */
bool BrowserView::onMsg(const char* pMsg,const char** ppReturn)
{
    //if (strcmp(pMsg,"Save")==0){
    //    saveFile();
    //    return true;
    //} else if (strcmp(pMsg,"Run")==0){
    //    run();
    //    return true;
    //} else if (strcmp(pMsg,"SaveAs")==0){
    //    saveAs();
    //    return true;
    //} else if (strcmp(pMsg,"Cut")==0){
    //    cut();
    //    return true;
    //} else if (strcmp(pMsg,"Copy")==0){
    //    copy();
    //    return true;
    //} else if (strcmp(pMsg,"Paste")==0){
    //    paste();
    //    return true;
    //} else if (strcmp(pMsg,"Undo")==0){
    //    undo();
    //    return true;
    //} else if (strcmp(pMsg,"Redo")==0){
    //    redo();
    //    return true;
    //} else if (strcmp(pMsg,"ViewFit")==0){
    //    // just ignore this
    //    return true;
    //}

    return false;
}

/**
 * Checks if the action \a pMsg is available. This is for enabling/disabling
 * the corresponding buttons or menu items for this action.
 */
bool BrowserView::onHasMsg(const char* pMsg) const
{
    //if (strcmp(pMsg,"Run")==0)  return true;
    //if (strcmp(pMsg,"SaveAs")==0)  return true;
    //if (strcmp(pMsg,"Print")==0) return true;
    //if (strcmp(pMsg,"PrintPdf")==0) return true;
    //if (strcmp(pMsg,"Save")==0) { 
    //    return d->textEdit->document()->isModified();
    //} else if (strcmp(pMsg,"Cut")==0) {
    //    bool canWrite = !d->textEdit->isReadOnly();
    //    return (canWrite && (d->textEdit->textCursor().hasSelection()));
    //} else if (strcmp(pMsg,"Copy")==0) {
    //    return ( d->textEdit->textCursor().hasSelection() );
    //} else if (strcmp(pMsg,"Paste")==0) {
    //    QClipboard *cb = QApplication::clipboard();
    //    QString text;

    //    // Copy text from the clipboard (paste)
    //    text = cb->text();

    //    bool canWrite = !d->textEdit->isReadOnly();
    //    return ( !text.isEmpty() && canWrite );
    //} else if (strcmp(pMsg,"Undo")==0) {
    //    return d->textEdit->document()->isUndoAvailable ();
    //} else if (strcmp(pMsg,"Redo")==0) {
    //    return d->textEdit->document()->isRedoAvailable ();
    //}

    return false;
}

/** Checking on close state. */
bool BrowserView::canClose(void)
{
    //if ( !d->textEdit->document()->isModified() )
    //    return true;
    //this->setFocus(); // raises the view to front
    //switch( QMessageBox::question(this, tr("Unsaved document"), 
    //                                tr("The document has been modified.\n"
    //                                   "Do you want to save your changes?"),
    //                                 QMessageBox::Yes|QMessageBox::Default, QMessageBox::No, 
    //                                 QMessageBox::Cancel|QMessageBox::Escape))
    //{
    //    case QMessageBox::Yes:
    //        return saveFile();
    //    case QMessageBox::No:
    //        return true;
    //    case QMessageBox::Cancel:
    //        return false;
    //    default:
    //        return false;
    //}
    return true;
}



#include "moc_BrowserView.cpp"
