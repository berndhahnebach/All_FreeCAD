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
 

/**
 * TODO: 
 *  + Bookmarks dürfen nur genau einmal in Liste stehen
 *  + Bookmarks müssen sofort verfügbar sein
 */

#ifndef __HTML_VIEW_H__
#define __HTML_VIEW_H__
 
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <assert.h>
#endif

#include "../Base/Export.h"
#include <qtextbrowser.h>
#include "window.h"

class FCTextBrowser : public QTextBrowser
{
  Q_OBJECT

  public:
    FCTextBrowser(QWidget * parent=0, const char * name=0);

    virtual void setText (const QString & contents, const QString & context=QString::null);
    virtual void setSource (const QString & name);

  signals:
    void showPopupMenu();

  protected:
    virtual void viewportMousePressEvent (QMouseEvent * e);
};

class GuiExport FCHtmlView : public FCDockWindow
{ 
    Q_OBJECT

  public:
    FCHtmlView( const QString& home_, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCHtmlView();

    void setEnableHistory  (bool b=true);
    void setEnableBookmarks(bool b=true);

  private slots:
    void setBackwardAvailable( bool );
    void setForwardAvailable ( bool );

    void textChanged();
    void openFile();

    void pathSelected( const QString & );
    void histChosen( int );
    void bookmChosen( int );
    void addBookmark();
    void checkBookmarks();

  protected:
    virtual QString getRelativeURL (const QString& path) const;
    virtual QString getAbsoluteURL (const QString& path) const;
    virtual QString getHelpDirectory() const;

  protected slots:
    void refreshPage();
    void popupMenuAboutToShow();
    void showPopupMenu();

  private:
    QStringList readHistory();
    QStringList readBookmarks();

    QString selectedURL;
    QMap<int, QString> mHistory, mBookmarks;

    QButtonGroup*  pclButtonGrp;
    QToolButton*   pclButtonBack;
    QToolButton*   pclButtonForward;
    QToolButton*   pclButtonHome;
    QComboBox*     pclPathCombo;
    FCTextBrowser* pclBrowser;

  protected:
    bool bBackward, bForward;
    bool bHistory, bBookm;

    QString       m_strDocDir;
    QString       m_strCaption;
    QGridLayout*  pclFormLayout;
    QHBoxLayout*  pclButtonGrpLayout;
    QPopupMenu*   pclPopup;
    QPopupMenu*   pclHistory;
    QPopupMenu*   pclBookm;
};

#endif // __HTML_VIEW_H__
