/***************************************************************************
                          HtmlView.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief The Html view module
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/
 



#ifndef __HTML_VIEW_H__
#define __HTML_VIEW_H__
 
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <assert.h>
#endif

#include "../Base/Export.h"
#include <qtextbrowser.h>

class FCTextBrowser : public QTextBrowser
{
  Q_OBJECT

  public:
    FCTextBrowser(QWidget * parent=0, const char * name=0);

  protected slots:
    void popupMenuAboutToShow();
    void setBackwardAvailable(bool);
    void setForwardAvailable (bool);
    void refreshPage();

  protected:
    void viewportMousePressEvent ( QMouseEvent * e );

    bool bBackward, bForward;
    QPopupMenu* pclPopup;
};

class GuiExport FCHtmlView : public QWidget
{ 
    Q_OBJECT

public:
    FCHtmlView( const QString& home_,  const QString& path, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCHtmlView();

private slots:
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void textChanged();
    void openFile();

    void pathSelected( const QString & );
    void histChosen( int );
    void bookmChosen( int );
    void addBookmark();

protected:
    virtual void mouseDoubleClickEvent ( QMouseEvent * e );

private:
    void readHistory();
    void readBookmarks();

    QString selectedURL;
    QStringList history, bookmarks;
    QMap<int, QString> mHistory, mBookmarks;

    QButtonGroup* pclButtonGrp;
    QToolButton*  pclButtonBack;
    QToolButton*  pclButtonForward;
    QToolButton*  pclButtonHome;
    QComboBox*    pclPathCombo;
    QTextBrowser* pclBrowser;

protected:
    QGridLayout*  pclFormLayout;
    QHBoxLayout*  pclButtonGrpLayout;
};

#endif // __HTML_VIEW_H__
