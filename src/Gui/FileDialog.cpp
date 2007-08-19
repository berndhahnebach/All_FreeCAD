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
 * This is a convenience static function that will return a file name selected by the user. The file does not have to exist.
 */
QString FileDialog::getSaveFileName (QWidget * parent, const QString & caption, const QString & dir, 
                                     const QString & filter, QString * selectedFilter, Options options)
{
    QString dirName = dir;
    if (dirName.isEmpty()) {
        dirName = getWorkingDirectory();
    } else {
        QFileInfo fi(dir);
        if (fi.isRelative()) {
            dirName = getWorkingDirectory();
            dirName += "/";
            dirName += fi.fileName();
        }
    
        // get the suffix for the filter
        QRegExp rx;
        rx.setPattern("\\s(\\(\\*\\.\\w{1,})\\W");
        int index = rx.indexIn(filter);
        if (index != -1) {
            // get the suffix with the leading dot
            QString suffix = filter.mid(index+3, rx.matchedLength()-4);
            if (fi.suffix().isEmpty())
                dirName += suffix;
        }
    }

    QString windowTitle = caption;
    if (windowTitle.isEmpty())
        windowTitle = FileDialog::tr("Save as");
    
    // NOTE: We must not change the specified file name afterwards as we may return the name of an already
    // existing file. Hence we must extract the first matching suffix from the filter list and append it 
    // before showing the file dialog.
    QString file = QFileDialog::getSaveFileName(parent, windowTitle, dirName, filter, selectedFilter, options);
    if (!file.isEmpty()) {
        setWorkingDirectory(file);
        return file;
    } else {
        return QString::null;
    }
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
 * This is a convenience static function that returns an existing file selected by the user. 
 * If the user pressed Cancel, it returns a null string.
 */
QString FileDialog::getOpenFileName(QWidget * parent, const QString & caption, const QString & dir, 
                                    const QString & filter, QString * selectedFilter, Options options)
{
    QString dirName = dir;
    if (dirName.isEmpty()) {
        dirName = getWorkingDirectory();
    }

    QString windowTitle = caption;
    if (windowTitle.isEmpty())
        windowTitle = FileDialog::tr("Open");
    QString file = QFileDialog::getOpenFileName(parent, windowTitle, dirName, filter, selectedFilter, options);
    if (!file.isEmpty()) {
        setWorkingDirectory(file);
        return file;
    } else {
        return QString::null;
    }
}

/**
 * This is a convenience static function that will return one or more existing files selected by the user.
 */
QStringList FileDialog::getOpenFileNames (QWidget * parent, const QString & caption, const QString & dir,
                                          const QString & filter, QString * selectedFilter, Options options)
{
    QString dirName = dir;
    if (dirName.isEmpty()) {
        dirName = getWorkingDirectory();
    }

    QString windowTitle = caption;
    if (windowTitle.isEmpty())
        windowTitle = FileDialog::tr("Open");
    QStringList files = QFileDialog::getOpenFileNames(parent, windowTitle, dirName, filter, selectedFilter, options);
    if (!files.isEmpty()) {
        setWorkingDirectory(files.front());
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
    path = hPath->GetASCII("FileOpenSavePath", path.c_str());
    return QString::fromUtf8(path.c_str());
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
        dirName = info.absolutePath();
    }

    FCHandle<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")
                               ->GetGroup("Preferences")->GetGroup("General");
    hPath->SetASCII("FileOpenSavePath", dirName.toUtf8());
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
    if (pos == ExtensionRight) {
        setExtension(w);
        setOrientation( Qt::Horizontal );
    } else if (pos == ExtensionBottom) {
        setExtension(w);
        setOrientation(Qt::Vertical);
    }

    w->hide();
    if (show)
        toggleExtension();
}

QWidget* FileOptionsDialog::getOptionsWidget() const
{
    return this->extension();
}

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

    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(fileNameChanged(const QString &)));

    button = new QPushButton("...", this);
    button->setFixedWidth(2*button->fontMetrics().width( " ... " ));
    layout->addWidget(button);

    connect( button, SIGNAL(clicked()), this, SLOT(chooseFile()));

    setFocusProxy(lineEdit);
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

    if ( !fn.isEmpty() ) {
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

