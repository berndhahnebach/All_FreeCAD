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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qcheckbox.h>
# include <qlayout.h>
# include <qmessagebox.h>
# include <qpainter.h>
#endif

#include "FileDialog.h"

using namespace Gui;


QString FileDialog::getOpenFileName( const QString & startWith, const QString& filter,
                                     QWidget *parent, const char* name )
{
  return getOpenFileName( startWith, filter, parent, name, QString::null  );
}

QString FileDialog::getOpenFileName( const QString & startWith, const QString& filter, QWidget *parent,
                                     const char* name, const QString& caption )
{
  return QFileDialog::getOpenFileName( startWith, filter, parent, name, caption );
}

QString FileDialog::getSaveFileName( const QString & startWith, const QString& filter,
                                     QWidget *parent, const char* name )
{
  return getSaveFileName( startWith, filter, parent, name, QString::null  );
}

QString FileDialog::getSaveFileName( const QString & startWith, const QString& filter, QWidget *parent,
                                     const char* name, const QString& caption )
{
  return QFileDialog::getSaveFileName( startWith, filter, parent, name, caption );
}

QString FileDialog::getSaveFileName ( const QString & initially, const QString & filter, QWidget * parent,
                                      const QString & caption )
{
  FileDialog fd(AnyFile, initially, filter, parent, tr("Save Dialog"), true);
  fd.setCaption(caption);
  if ( fd.exec() )
    return fd.selectedFileName();
  else
    return QString("");
}

/**
 * Constructs a file dialog called \a name with the parent \a parent and the mode \a mode. 
 * If \a modal is TRUE then the file dialog is modal; otherwise it is modeless. 
 */
FileDialog::FileDialog (Mode mode, QWidget* parent, const char* name, bool modal)
    : QFileDialog(parent, name, modal)
{
}

/**
 * Constructs a file dialog called \a name with the parent \a parent and the mode \a mode.
 * If \a modal is TRUE then the file dialog is modal; otherwise it is modeless. 
 *
 * If \a dirName is specified then it will be used as the dialog's working directory, i.e. 
 * it will be the directory that is shown when the dialog appears. If \a filter is specified
 * it will be used as the dialog's file filter.
 */
FileDialog::FileDialog (Mode mode, const QString& dirName, const QString& filter,
                        QWidget* parent, const char* name, bool modal)
    : QFileDialog(dirName, filter, parent, name, modal)
{
  setMode(mode);
}

FileDialog::~FileDialog()
{
}

/**
 * Hides the modal dialog and sets the result code to Accepted.
 */
void FileDialog::accept()
{
  QString fn = selectedFileName();

  if ( QFile(fn).exists() && mode() == AnyFile )
  {
    QString msg = tr("'%1' already exists.\nReplace existing file?").arg(fn);
    if (QMessageBox::question(this, "Existing file", msg, "Yes", "No", QString::null, 1) == 0)
      QFileDialog::accept();
  }
  else
    QFileDialog::accept();
}

/**
 * Returns the name of the selected file with file extension.
 */
QString FileDialog::selectedFileName()
{
  QString fn = selectedFile();

  // if empty do nothing
  if (fn.isEmpty())
    return fn;

  // search for extension
  int pos = fn.findRev('.');
  if (pos == -1)
  {
    // try to figure out the selected filter
    QString filt = selectedFilter();
    int dot = filt.find('*');
    int blank = filt.find(' ', dot);
    int brack = filt.find(')', dot);
    if (dot != -1 && blank != -1)
    {
      QString sub = filt.mid(dot+1, blank-dot-1);
      fn = fn + sub;
    }
    else if (dot != -1 && brack != -1)
    {
      QString sub = filt.mid(dot+1, brack-dot-1);
      fn = fn + sub;
    }
  }

  return fn;
}

// ======================================================================

/**
 * Constructs a image preview widget called \a name with the parent \a parent.
 */
ImagePreview::ImagePreview( QWidget *parent, const char* name ) : QScrollView( parent, name ) 
{
  viewport()->setBackgroundMode( PaletteBase );
}

/**
 * Sets the pixmap \a pix to be displayed by this preview. 
 */
void ImagePreview::setPixmap( const QPixmap &pix )
{
  _pixmap = pix;
  resizeContents( _pixmap.size().width(), _pixmap.size().height() );
  viewport()->repaint( FALSE );
}

/**
 * The function draws the pixmap set with setPixmap() inside the rectangle (\a clipx, \a clipy, 
 * \a clipw, \a cliph) of the contents using painter \a p.
 */
void ImagePreview::drawContents( QPainter *p, int clipx, int clipy, int clipw, int cliph )
{
  bool mv = false;
  int w = _pixmap.width();
  int h = _pixmap.height();

  p->fillRect( clipx, clipy, clipw, cliph, colorGroup().brush( QColorGroup::Base ) );

  // move the pixmap to the center if it is small enough
  if ( w <= clipw && h <= cliph)
  {
    mv = true;
    p->save();
    p->translate( (clipw-w)/2 , (cliph-h)/2 );
  }

  p->drawPixmap( 0, 0, _pixmap );

  if ( mv )
    p->restore();
}

// ======================================================================

/**
 * Constructs a preview label called \a name with the parent \a parent.
 */
PreviewLabel::PreviewLabel( QWidget *parent, const char* name ) : QWidget( parent, name ) 
{
  QGridLayout* layout = new QGridLayout( this, 1, 1, 0, -1, "PreviewLabel"); 
  _preview = new ImagePreview( this );
  _cbview = new QCheckBox( this );
  _cbview->setText( tr("Preview") );
  _cbview->setChecked( true );

  layout->addWidget( _preview, 0, 0 );
  layout->addWidget( _cbview, 1, 0 );
}

/**
 * Previews the pixmap if \a u is an image file.
 */
void PreviewLabel::previewUrl( const QUrl &u )
{
  QString path = u.path();
  QPixmap pix( path );

  if ( _cbview->isChecked() )
    _preview->setPixmap( pix );
  else
    _preview->setPixmap( QPixmap() );
}

// ======================================================================

/**
 * Constructs an empty file icon provider called \a name, with the parent \a parent.
 */
PixmapFileProvider::PixmapFileProvider( QObject * parent, const char * name )
  : QFileIconProvider( parent, name )
{
}

/**
 * Returns a pointer to a pixmap that should be used to signify the file with the information \a info.
 * If pixmap() returns 0, QFileDialog draws the default pixmap.
 */
const QPixmap * PixmapFileProvider::pixmap ( const QFileInfo & info )
{
  QString fn = info.filePath();
  bool b=info.exists();
  b=info.isFile();
  if ( info.exists() && info.isFile() )
  {
    const char* ext = QPixmap::imageFormat( fn );
    
    // seems to be valid image file
    if ( ext )
    {
      QPixmap* px = new QPixmap;
      px->load( fn, ext );
      return px;
    }
  }

  return 0;
}

#include "moc_FileDialog.cpp"
