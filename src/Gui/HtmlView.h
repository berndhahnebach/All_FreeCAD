/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef __HTML_VIEW_H__
#define __HTML_VIEW_H__

#ifndef _PreComp_
# include <qcombobox.h>
# include <qcursor.h>
# include <qmime.h>
# include <qtextbrowser.h>
# include <qvalidator.h>
#endif

#include "Window.h"
#include "Process.h"

class QHBoxLayout;
class QButtonGroup;
class QToolButton;
class QGridLayout;
class QPopupMenu;

/**
 * An special implementation of the mime source factory
 */
class FCBrowserSourceFactory : public QMimeSourceFactory
{
public:
  FCBrowserSourceFactory();
  virtual ~FCBrowserSourceFactory();

  virtual const QMimeSource* data(const QString& abs_name) const;
  virtual QString makeAbsolute(const QString& abs_or_rel_name, const QString& context) const;
  virtual void setText( const QString& abs_name, const QString& text );
  virtual void setImage( const QString& abs_name, const QImage& im );
  virtual void setPixmap( const QString& abs_name, const QPixmap& pm );
  virtual void setData( const QString& abs_name, QMimeSource* data );
  virtual void setFilePath( const QStringList& );
  virtual QStringList filePath() const;
  virtual void setExtensionType( const QString& ext, const char* mimetype );

  static bool canConvertToHTML (const QString& url);
};

/**
 * An extension of the 'QTextBrowser' class
 */
class FCTextBrowserPrivate;
class FCTextBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  FCTextBrowser(QWidget * parent=0, const char * name=0);
  virtual ~FCTextBrowser();

  virtual void setText (const QString & contents, const QString & context=QString::null);
  virtual void setSource (const QString & name);

  virtual void backward();
  virtual void forward();

  void setMinWidthToReach (int);

signals:
  /// show the popup menu
  void showPopupMenu();
  /// start an external browser to display complex web sites
  void startExtBrowser(QString);
  void minWidthReached (bool);

protected:
  virtual void viewportResizeEvent     (QResizeEvent* e);
  virtual void viewportMousePressEvent (QMouseEvent * e);
  virtual void contentsDropEvent       (QDropEvent  * e);
  virtual void contentsDragEnterEvent  (QDragEnterEvent * e);
  virtual void contentsDragMoveEvent   (QDragMoveEvent  * e );

private:
  FCTextBrowserPrivate* d;
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
class FCHtmlViewPrivate;
class GuiExport FCHtmlView : public FCDockWindow, public FCParameterGrp::ObserverType, public FCProcess::ObserverType
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
  /// observers methods
  void OnChange (FCSubject<FCParameterGrp::MessageType> &rCaller,FCParameterGrp::MessageType rcReason);
  void OnChange (FCSubject<FCProcess::MessageType> &rCaller,FCProcess::MessageType rcReason);
  /// returns the actual browser
  FCTextBrowser* getBrowser() const { return pclBrowser; }

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
  void onMinWidthReached (bool);

protected:
  void init ();
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

  QButtonGroup*  pclButtonGrp;
  QToolButton*   pclButtonBack;
  QToolButton*   pclButtonForward;
  QToolButton*   pclButtonHome;
  QToolButton*   pclButtonOpen;
  FCHtmlComboBox*pclPathCombo;
  FCTextBrowser* pclBrowser;
  QGridLayout*   pclFormLayout;
  QHBoxLayout*   pclButtonGrpLayout;
  QPopupMenu*    pclPopup;
  QPopupMenu*    pclHistory;
  QPopupMenu*    pclBookm;

private:
  FCHtmlViewPrivate* d;
};


class FCWhatsThis : public Qt
{
public:
  FCWhatsThis( QWidget *);
  FCWhatsThis( QWidget *, QString url);
  virtual ~FCWhatsThis();

  virtual QString text( const QPoint & );

  static void add( QWidget *, const QString &);
  static void remove( QWidget * );
  static QString textFor( QWidget *, const QPoint & pos = QPoint() );

  static void enterWhatsThisMode();
  static bool inWhatsThisMode();
  static void leaveWhatsThisMode( const QString& = QString::null, const QPoint& pos = QCursor::pos() );

private:
  QString m_sURL;
};

#endif // __HTML_VIEW_H__
