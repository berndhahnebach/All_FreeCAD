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
# include <qdir.h>
# include <qfileinfo.h>
# include <qlayout.h>
# include <qlineedit.h>
# include <qmessagebox.h>
# include <qpainter.h>
# include <qpushbutton.h>
#endif

#include "FileDialog.h"
#include "BitmapFactory.h"

using namespace Gui;


/* TRANSLATOR Gui::FileDialog */

QString FileDialog::getOpenFileName ( const QString & startWith, const QString & filter, QWidget * parent,
                                      const char* name, const QString & caption, const QString& buttonText,
                                      bool * ok )
{
  FileDialog fd(ExistingFile, startWith, filter, parent, name, true);
  fd.setCaption(caption);
  if ( buttonText != QString::null )
  {
    // search for the OK button to change its text
    QObject* btn = fd.child( "OK", "QPushButton", true );
    if ( btn )
      static_cast<QPushButton*>(btn)->setText( buttonText );
  }

  bool ok_ = false;
  ok_ = ( fd.exec() == QDialog::Accepted );
  if ( ok )
  	*ok = ok_;
  if ( ok_ )
    return fd.selectedFileName();
  else
    return QString::null;
}

QString FileDialog::getSaveFileName ( const QString & startWith, const QString & filter, QWidget * parent,
                                      const char* name, const QString & caption, bool * ok  )
{
  FileDialog fd(AnyFile, startWith, filter, parent, name, true);
  fd.setCaption(caption);

  bool ok_ = false;
  ok_ = ( fd.exec() == QDialog::Accepted );
  if ( ok )
  	*ok = ok_;
  if ( ok_ )
    return fd.selectedFileName();
  else
    return QString::null;
}

QString FileDialog::getExistingDirectory( const QString & dir, QWidget *parent, const char* name,
                                          const QString& caption, bool dirOnly, bool resolveSymlinks,
                                          bool * ok )
{
  QString path = QFileDialog::getExistingDirectory( dir, parent, name, caption, dirOnly, resolveSymlinks );
  // valid path was selected
  if ( !path.isEmpty() )
  {
    QDir d(path);
    path = d.path(); // get path in Qt manner
  }

  if ( ok )
  	*ok = !path.isEmpty();

  return path;
}

/**
 * Constructs a file dialog called \a name with the parent \a parent and the mode \a mode. 
 * If \a modal is TRUE then the file dialog is modal; otherwise it is modeless. 
 */
FileDialog::FileDialog (Mode mode, QWidget* parent, const char* name, bool modal)
    : QFileDialog(parent, name, modal)
{
  setMode(mode);
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

/**
 * Constructs a file dialog called \a name with the parent \a parent.
 * If \a modal is TRUE then the file dialog is modal; otherwise it is modeless. 
 *
 * If \a dirName is specified then it will be used as the dialog's working directory, i.e. 
 * it will be the directory that is shown when the dialog appears. If \a filter is specified
 * it will be used as the dialog's file filter.
 */
FileDialog::FileDialog (const QString& dirName, const QString& filter, QWidget* parent, const char* name, bool modal)
  : QFileDialog(dirName, filter, parent, name, modal)
{
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
    if ( QMessageBox::question(this, tr("Existing file"), msg, QMessageBox::Yes, 
         QMessageBox::No|QMessageBox::Default|QMessageBox::Escape) == QMessageBox::Yes )
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
    return QString::null;

  // check if fn is a directory, because Qt 3.2.1 returns a non-emtpy
  // string even if cancel was pressed
  QFileInfo fi( fn );
  if ( fi.isDir() )
    return QString::null;

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

/* TRANSLATOR Gui::PreviewLabel */

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
FileIconProvider::FileIconProvider( QObject * parent, const char * name )
  : QFileIconProvider( parent, name )
{
  QFileDialog::setIconProvider( this );
}

FileIconProvider::~FileIconProvider()
{
  QFileDialog::setIconProvider( 0L );
}

/**
 * Returns a pointer to a pixmap that should be used to signify the file with the information \a info.
 * If pixmap() returns 0, QFileDialog draws the default pixmap.
 */
const QPixmap * FileIconProvider::pixmap ( const QFileInfo & info )
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
      return BitmapFactory().fileFormat( "image_xpm" );
    }
    else // other file formats 
    {
      QString ext = info.extension().upper();
      QPixmap* px = BitmapFactory().fileFormat( ext.latin1() );
      if ( px )
        return px;
    }
  }

  return QFileIconProvider::pixmap( info );
}

// --------------------------------------------------------------------

/* TRANSLATOR Gui::FileChooser */

/**
 * Constructs a file chooser called \a name with the parent \a parent.
 */
FileChooser::FileChooser ( QWidget * parent, const char * name )
: QWidget(parent, name), md( File ), _filter( QString::null )
{
  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setMargin( 0 );
  layout->setSpacing( 6 );

  lineEdit = new QLineEdit( this, "filechooser_lineedit" );
  layout->addWidget( lineEdit );

  connect( lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SIGNAL( fileNameChanged( const QString & ) ) );

  button = new QPushButton( "...", this, "filechooser_button" );
  button->setFixedWidth( 2*button->fontMetrics().width( " ... " ) );
  layout->addWidget( button );

  connect( button, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );

  setFocusProxy( lineEdit );
}

FileChooser::~FileChooser()
{
}

/**
 * \property FileChooser::fileName
 *
 * This property holds the file name.
 * Set this property's value with setFileName() and get this property's value with fileName().
 *
 * \sa fileName(), setFileName().
 */
QString FileChooser::fileName() const
{
  return lineEdit->text();
}

/** 
 * Sets the file name \a s.
 */
void FileChooser::setFileName( const QString& s )
{
  lineEdit->setText( s );
}

/**
 * Opens a FileDialog to choose either a file or a directory in dependency of the
 * value of the Mode property.
 */
void FileChooser::chooseFile()
{
  QString fn;
  if ( mode() == File )
  {
    fn = Gui::FileDialog::getOpenFileName( lineEdit->text(), _filter, this, 0,
      tr( "Select file" ), tr( "Select" ));
  }
  else
  {
    fn = Gui::FileDialog::getExistingDirectory( lineEdit->text(), this );
  }

  if ( !fn.isEmpty() ) 
  {
  	lineEdit->setText( fn );
	  emit fileNameChanged( fn );
  }
}

/**
 * \property FileChooser::mode
 *
 * This property holds whether the widgets selects either a file or a directory.
 * The default value of chooseFile is set to File.
 *
 * \sa chooseFile(), mode(), setMode().
 */
FileChooser::Mode FileChooser::mode() const
{
  return md;
}

/**
 * If \a m is File the widget is set to choose a file, otherwise it is set to
 * choose a directory.
 */
void FileChooser::setMode( FileChooser::Mode m )
{
  md = m;
}

/**
 * \property FileChooser::filter
 *
 * This property holds the set filter to choose a file. This property is used only if 
 * \property FileChooser::Mode is set to File.
 *
 * \sa chooseFile(), filter(), setFilter().
 */
QString FileChooser::filter() const
{
  return _filter;
}

/**
 * Sets the filter for choosing a file.  
 */
void FileChooser::setFilter ( const QString& filter )
{
  _filter = filter;
}

/**
 * Sets the browse button's text to \a txt.
 */
void FileChooser::setButtonText( const QString& txt )
{
  button->setText( txt );
  int w1 = 2*button->fontMetrics().width( txt );
  int w2 = 2*button->fontMetrics().width( " ... " );
  button->setFixedWidth( (w1 > w2 ? w1 : w2) );
}

/**
 * Returns the browse button's text.
 */
QString FileChooser::buttonText() const
{
  return button->text();
}

#include "moc_FileDialog.cpp"
