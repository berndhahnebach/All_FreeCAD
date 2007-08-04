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

#include <Base/Parameter.h>
#include <App/Application.h>

#include "FileDialog.h"
#include "BitmapFactory.h"

using namespace Gui;


/* TRANSLATOR Gui::FileDialog */

/** 
 * This is a convenience static function that returns an existing file selected by the user. 
 * If the user pressed Cancel, it returns a null string.
 */
QString FileDialog::getOpenFileName( QWidget * parent, const QString & caption, const QString & dir, 
                                     const QString & filter, QString * selectedFilter, Options options )
{
    QString dirName = dir;
    QString selName = QString::null;
    if (!dir.isEmpty()) {
        QFileInfo info(dir);
        if (info.isFile() && info.exists()) {
            selName = info.fileName();
            dirName = info.absolutePath();
        }
    } else {
        dirName = getWorkingDirectory();
    }

    FileDialog fd(parent, caption, dirName, filter);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setFileMode( ExistingFile );
    if ( caption.isEmpty() )
        fd.setWindowTitle( FileDialog::tr( "Open" ) );
    if ( !selName.isEmpty() )
        fd.selectFile( selName );
    if ( selectedFilter )
        fd.setFilter( *selectedFilter );
  //if ( buttonText != QString::null )
  //  fd.setLabelText(QFileDialog::Accept, buttonText);

    if ( fd.exec() == QDialog::Accepted ) {
        if ( selectedFilter )
            *selectedFilter = fd.selectedFilter();
        QString file = fd.selectedFiles().front();
        setWorkingDirectory(file);
        return file;
    } else
        return QString::null;
}

/**
 * This is a convenience static function that will return a file name selected by the user. The file does not have to exist.
 */
QString FileDialog::getSaveFileName ( QWidget * parent, const QString & caption, const QString & dir, 
                                      const QString & filter, QString * selectedFilter, Options options )
{
    QString dirName = dir;
    QString selName = QString::null;
    if (!dir.isEmpty()) {
        QFileInfo info(dir);
        if (!info.isDir()) {
            selName = info.fileName();
            dirName = info.absolutePath();
        }
    } else {
        dirName = getWorkingDirectory();
    }

    FileDialog fd(parent, caption, dirName, filter);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setFileMode( AnyFile );
    if ( caption.isEmpty() )
        fd.setWindowTitle( FileDialog::tr( "Save as" ) );
    if ( !selName.isEmpty() )
        fd.selectFile( selName );
    if ( selectedFilter )
        fd.setFilter( *selectedFilter );
  //if ( buttonText != QString::null )
  //  fd.setLabelText(QFileDialog::Accept, buttonText);

    if ( fd.exec() == QDialog::Accepted ) {
        if ( selectedFilter )
            *selectedFilter = fd.selectedFilter();
        QString file = fd.selectedFiles().front();
        setWorkingDirectory(file);
        return file;
    } else
        return QString::null;
}

/**
 * This is a convenience static function that will return an existing directory selected by the user.
 */
QString FileDialog::getExistingDirectory( QWidget * parent, const QString & caption, const QString & dir, Options options )
{
    QString path = QFileDialog::getExistingDirectory(parent, caption, dir, options);
    // valid path was selected
    if ( !path.isEmpty() ) {
        QDir d(path);
        path = d.path(); // get path in Qt manner
    }

    return path;
}

/**
 * This is a convenience static function that will return one or more existing files selected by the user.
 */
QStringList FileDialog::getOpenFileNames ( QWidget * parent, const QString & caption, const QString & dir,
                                           const QString & filter, QString * selectedFilter, Options options )
{
    QString dirName = dir.isEmpty() ? getWorkingDirectory() : dir;
    FileDialog fd(parent, caption, dirName, filter);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setFileMode( ExistingFiles );
    fd.setWindowTitle(caption);
    if ( selectedFilter )
        fd.setFilter( *selectedFilter );
  //if ( buttonText != QString::null )
  //  fd.setLabelText(QFileDialog::Accept, buttonText);

    QStringList files;
    if ( fd.exec() == QDialog::Accepted ) {
        if ( selectedFilter )
            *selectedFilter = fd.selectedFilter();
        files = fd.selectedFiles();
        if (!files.isEmpty()) {
            setWorkingDirectory(files.front());
        }
    }

    return files;
}

/**
 * Returns the working directory for the file dialog. This path can be used in
 * combination with getSaveFileName(), getOpenFileName(), getOpenFileNames() or
 * getExistingDirectory() to open the dialog in this path.
 */
QString FileDialog::getWorkingDirectory()
{
    std::string path = App::GetApplication().Config()["UserHomePath"];
    FCHandle<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")
                               ->GetGroup("Preferences")->GetGroup("General");
    return hPath->GetASCII("FileOpenSavePath", path.c_str()).c_str();
}

/**
 * Sets the working directory to \a dir for the file dialog.
 * If \a dir is a file then the path only is taken.
 * getWorkingDirectory() returns the working directory.
 */
void FileDialog::setWorkingDirectory( const QString& dir )
{
    QString dirName = dir;
    if (!dir.isEmpty()) {
        QFileInfo info(dir);
        if (info.isFile()) {
            dirName = info.absolutePath();
        }
    }

    FCHandle<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")
                               ->GetGroup("Preferences")->GetGroup("General");
    hPath->SetASCII("FileOpenSavePath", dirName.toAscii());
}

/**
 * Constructs a file dialog called \a name and with the parent \a parent. 
 * If \a modal is TRUE then the file dialog is modal; otherwise it is modeless. 
 */
FileDialog::FileDialog ( QWidget* parent, Qt::WFlags f)
    : QFileDialog(parent, f )
{
}

/**
 * Constructs a file dialog called \a name and with the parent \a parent.
 * If \a modal is TRUE then the file dialog is modal; otherwise it is modeless. 
 *
 * If \a dirName is specified then it will be used as the dialog's working directory, i.e. 
 * it will be the directory that is shown when the dialog appears. If \a filter is specified
 * it will be used as the dialog's file filter.
 */
FileDialog::FileDialog ( QWidget* parent, const QString& caption, const QString& dirName, const QString& filter )
    : QFileDialog(parent, caption, dirName, filter)
{
}

FileDialog::~FileDialog()
{
}

/**
 * Returns the name of the selected file with file extension.
 */
QString FileDialog::selectedFileName()
{
  QString fn = selectedFiles().front();

  // if empty do nothing
  if (fn.isEmpty())
    return QString::null;

  // check if fn is a directory, because Qt 3.2.1 returns a non-emtpy
  // string even if cancel was pressed
  QFileInfo fi( fn );
  if ( fi.isDir() )
    return QString::null;

  // search for extension
  int pos = fi.fileName().lastIndexOf('.');
  if (pos == -1)
  {
    // try to figure out the selected filter
    QString filt = selectedFilter();
    int dot = filt.indexOf('*');
    int blank = filt.indexOf(' ', dot);
    int brack = filt.indexOf(')', dot);
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

/* TRANSLATOR Gui::FileOptionsDialog */

FileOptionsDialog::FileOptionsDialog( QWidget* parent, Qt::WFlags fl )
  : QFileDialog( parent, fl )
{
  extensionButton = new QPushButton( this );
  extensionButton->setText( tr( "Extended" ) );

  //search for the grid layout and add the new button
  QGridLayout* grid = this->findChild<QGridLayout*>();
  grid->addWidget(extensionButton, 4, 5, Qt::AlignLeft);

  connect(extensionButton, SIGNAL(clicked()), this, SLOT(toggleExtension()));

  // get the filter combobox to connect its activated() signal with our filterSelected() signal
  QComboBox* box = this->findChildren<QComboBox*>().last();
  connect(box, SIGNAL(activated(const QString&)), this, SIGNAL(filterSelected(const QString&)));
}

FileOptionsDialog::~FileOptionsDialog()
{
}

void FileOptionsDialog::accept()
{
  // Fixes a bug of the default implementation when entering an asterik
  QLineEdit* filename = this->findChild<QLineEdit*>();
  QString filter = filename->text();
  if (filter.contains('*')) {
    QFileInfo fi(filter);
    QString ext = fi.suffix();
    ext.prepend("*.");
    QStringList filters = this->filters();
    bool ok=false;
    // Compare the given suffix with the suffixes of all filters
    for (QStringList::ConstIterator it = filters.begin(); it != filters.end(); ++it) {
      if ((*it).contains(ext)) {
        filter = *it;
        ok = true;
        break;
      }
    }

    // if no appropriate filter was found the add the 'All files' filter
    if (!ok) {
      filter = tr("All files (*.*)");
      filters << filter;
      setFilters(filters);
    }

    // empty the line edit
    filename->blockSignals(true);
    filename->clear();
    filename->blockSignals(false);
    selectFilter(filter);

    return;
  }

  QFileDialog::accept();
}

void FileOptionsDialog::toggleExtension()
{
  QWidget* w = extension();
  if (w)
    showExtension(!w->isVisible());
}

void FileOptionsDialog::setOptionsWidget( FileOptionsDialog::ExtensionPosition pos, QWidget* w, bool show )
{
  if ( pos == ExtensionRight )
  {
    setExtension( w );
    setOrientation( Qt::Horizontal );
  }
  else if ( pos == ExtensionBottom )
  {
    setExtension( w );
    setOrientation( Qt::Vertical );
  }

  w->hide();
  if ( show )
    toggleExtension();
}

QWidget* FileOptionsDialog::getOptionsWidget() const
{
  return this->extension();
}

// ======================================================================
#if 0
/**
 * Constructs a image preview widget called \a name with the parent \a parent.
 */
ImagePreview::ImagePreview( QWidget *parent, const char* name ) : Q3ScrollView( parent, name ) 
{
  viewport()->setBackgroundMode( Qt::PaletteBase );
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
  Q3GridLayout* layout = new Q3GridLayout( this, 1, 1, 0, -1, "PreviewLabel"); 
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
void PreviewLabel::previewUrl( const Q3Url &u )
{
  QString path = u.path();
  QPixmap pix( path );

  if ( _cbview->isChecked() )
    _preview->setPixmap( pix );
  else
    _preview->setPixmap( QPixmap() );
}
#endif
// ======================================================================

/**
 * Constructs an empty file icon provider called \a name, with the parent \a parent.
 */
FileIconProvider::FileIconProvider()
{
}

FileIconProvider::~FileIconProvider()
{
}

QIcon FileIconProvider::icon ( IconType type ) const
{
    return QFileIconProvider::icon(type);
}

QIcon FileIconProvider::icon ( const QFileInfo & info ) const
{
    return QFileIconProvider::icon(info);
}

QString FileIconProvider::type ( const QFileInfo & info ) const
{
    return QFileIconProvider::type(info);
}

// --------------------------------------------------------------------

/* TRANSLATOR Gui::FileChooser */

/**
 * Constructs a file chooser called \a name with the parent \a parent.
 */
FileChooser::FileChooser ( QWidget * parent )
  : QWidget(parent), md( File ), _filter( QString::null )
{
  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setMargin( 0 );
  layout->setSpacing( 6 );

  lineEdit = new QLineEdit( this );
  layout->addWidget( lineEdit );

  connect( lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SIGNAL( fileNameChanged( const QString & ) ) );

  button = new QPushButton( "...", this );
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
  	fn = QFileDialog::getOpenFileName( this, tr( "Select a file" ), lineEdit->text(), _filter );
  else
  	fn = QFileDialog::getExistingDirectory( this, tr( "Select a directory" ), lineEdit->text() );

  if ( !fn.isEmpty() ) 
  {
  	lineEdit->setText( fn );
	  fileNameSelected( fn );
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
