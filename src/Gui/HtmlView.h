/***************************************************************************
                          HtmlView.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file HtmlView.h
 *  \brief The Html view module
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
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
 
#include <qtextbrowser.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include "Window.h"

/**
 * An extension of the 'QTextBrowser' class
 */
class FCTextBrowser : public QTextBrowser
{
  Q_OBJECT

  public:
    FCTextBrowser(QWidget * parent=0, const char * name=0);

    virtual void setText (const QString & contents, const QString & context=QString::null);
    virtual void setSource (const QString & name);

  signals:
    /// show the popup menu
    void showPopupMenu();
    /// start an external browser to display complex web sites
    void startExtBrowser(QString);

  protected:
    virtual void viewportMousePressEvent (QMouseEvent * e);
};

/**
 * Check the input of the combo box of the html view
 * @see FCHtmlComboBox
 * @see FCHtmlView
 */
class FCHtmlViewValidator : public QValidator
{
  public:
    FCHtmlViewValidator ( QWidget * parent, const char * name = 0 );
    virtual ~FCHtmlViewValidator ();

    virtual State validate ( QString &, int & ) const;
    virtual void fixup ( QString & ) const;
};

/**
 * Check the input of the combo box
 */
class FCHtmlComboBox : public QComboBox
{
  Q_OBJECT

  public:
    FCHtmlComboBox ( QWidget * parent=0, const char * name=0 );
    FCHtmlComboBox ( bool rw, QWidget * parent=0, const char * name=0 );

  signals:
    void returnPressed(QString text);

  protected slots:
    void slotKeyPressReturn();
};

/**
 * The HTML viewer class
 */
class GuiExport FCHtmlView : public FCDockWindow
{ 
    Q_OBJECT

  public:
    FCHtmlView( const QString& home_, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCHtmlView();

    /// enable the history
    void SetEnableHistory  (bool b=true);
    /// enable the bookmarks
    void SetEnableBookmarks(bool b=true);
    /// set the limit for the history
    bool SetMaxHistory (long lCnt);
    /// set the limit for the bookmarks
    bool SetMaxBookmarks (long lCnt);

  protected slots:
    void SetBackwardAvailable( bool );
    void SetForwardAvailable ( bool );
    void TextChanged();
    void OpenFile();
    void PathSelected( const QString & );
    void HistChosen( int );
    void BookmChosen( int );
    void AddBookmark();
    void CheckBookmarks();
    void RefreshPage();
    void PopupMenuAboutToShow();
    void ShowPopupMenu();
    void StartScriptOrBrowser(QString text);
    void StartExtBrowser(QString);

  protected:
    virtual QString GetRelativeURL (const QString& path) const;
    virtual QString GetAbsoluteURL (const QString& path) const;
    virtual QString GetDocDirectory();
    virtual QString GetScriptDirectory();
    virtual QString GetBrowserDirectory();
    virtual void ReadHistory();
    virtual void ReadBookmarks();
    virtual void SaveHistory();
    virtual void SaveBookmarks();
    virtual void CreateHistoryPopup();
    virtual void CreateBookmarkPopup();
    virtual void AddToPath (const QString& path);
    virtual void StartScript (QString path, QString protocol);
    virtual void StartBrowser(QString path, QString protocol);

#	pragma warning( disable : 4251 )
    QString        m_FCdoc, m_FCext, m_FCscript;
    FCmap<int, QString> mHistory, mBookmarks;
    bool bBackward, bForward;
    bool bHistory, bBookm;
    QButtonGroup*  pclButtonGrp;
    QToolButton*   pclButtonBack;
    QToolButton*   pclButtonForward;
    QToolButton*   pclButtonHome;
    QToolButton*   pclButtonOpen;
    FCHtmlComboBox*pclPathCombo;
    FCTextBrowser* pclBrowser;
    QString        selectedURL;
    QString        m_strDocDir;
    QString        m_strCaption;
    QGridLayout*   pclFormLayout;
    QHBoxLayout*   pclButtonGrpLayout;
    QPopupMenu*    pclPopup;
    QPopupMenu*    pclHistory;
    QPopupMenu*    pclBookm;
};

#endif // __HTML_VIEW_H__
