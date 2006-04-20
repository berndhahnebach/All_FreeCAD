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


#ifndef __HELP_VIEW_H__
#define __HELP_VIEW_H__

#include <qmime.h>
#include <qtextbrowser.h>

#include "DockWindow.h"
#include "Window.h"

class QHBoxLayout;
class QButtonGroup;
class QToolButton;
class QGridLayout;
class QPopupMenu;

namespace Gui {
namespace DockWnd {

class HelpViewPrivate;

/**
 * The HelpSourceFactory class is an extension of QMimeSourceFactory showing
 * an error page if given data cannot be decoded.
 * \author Werner Mayer
 */
class HelpSourceFactory : public QMimeSourceFactory
{
public:
  HelpSourceFactory();
  virtual ~HelpSourceFactory();

  virtual const QMimeSource* data(const QString& abs_name) const;
  virtual QString makeAbsolute(const QString& abs_or_rel_name, const QString& context) const;
  virtual void setText( const QString& abs_name, const QString& text );
  virtual void setImage( const QString& abs_name, const QImage& im );
  virtual void setPixmap( const QString& abs_name, const QPixmap& pm );
  virtual void setData( const QString& abs_name, QMimeSource* data );
  virtual void setFilePath( const QStringList& );
  virtual QStringList filePath() const;
  virtual void setExtensionType( const QString& ext, const char* mimetype );
};

/**
 * The TextBrowser class is an extension of Qt's QTextBrowser providing
 * a context menu and the possibility to render files via drag and drop.
 * Since the backward/forward stuff of QTextBrowser doesn't work properly 
 * it is reimplemented.
 * \author Werner Mayer
 */
class TextBrowserPrivate;
class TextBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  TextBrowser(QWidget * parent=0, const char * name=0);
  virtual ~TextBrowser();

  virtual void setText (const QString & contents, const QString & context=QString::null);
  virtual void setSource (const QString & name);

  virtual void backward();
  virtual void forward();

signals:
  /// start an external browser to display complex web sites
  void startExternalBrowser( const QString& );

protected:
  virtual void contentsDropEvent       (QDropEvent  * e);
  virtual void contentsDragEnterEvent  (QDragEnterEvent * e);
  virtual void contentsDragMoveEvent   (QDragMoveEvent  * e );
  virtual void viewportContextMenuEvent( QContextMenuEvent * );

private slots:
  void setBackwardAvailable( bool b);
  void setForwardAvailable( bool b);
  void done( bool );

private:
  TextBrowserPrivate* d;
};

/**
 * The help viewer class embeds a textbrowser to render help files.
 * \author Wenrer Mayer
 */
class HelpViewViewPrivate;
class GuiExport HelpView : public Gui::DockWindow
{
  Q_OBJECT

public:
  HelpView( const QString& home_, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~HelpView();

  void setFileSource( const QString& );

signals:
  void setSource( const QString& );

protected slots:
  void openHelpFile();
  void startExternalBrowser( const QString& );
};

} // namespace DockWnd
} // namespace Gui

#endif // __HELP_VIEW_H__
