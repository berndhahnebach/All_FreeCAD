/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QApplication>
# include <QButtonGroup>
# include <QComboBox>
# include <QGridLayout>
# include <QGroupBox>
# include <QLineEdit>
# include <QPushButton>
# include <QRadioButton>
#endif

#include <Base/Parameter.h>
#include <App/Application.h>

#include "FileDialog.h"
#include "MainWindow.h"
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
            dirName += QLatin1String("/");
            dirName += fi.fileName();
        }
    
        // get the suffix for the filter
        QRegExp rx;
        rx.setPattern(QLatin1String("\\s(\\(\\*\\.\\w{1,})\\W"));
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

#if defined(FC_OS_MACOSX)
    options |= QFileDialog::DontUseNativeDialog;
#endif
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
#if defined(FC_OS_MACOSX)
    options |= QFileDialog::DontUseNativeDialog;
#endif
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
#if defined(FC_OS_MACOSX)
    options |= QFileDialog::DontUseNativeDialog;
#endif
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
#if defined(FC_OS_MACOSX)
    options |= QFileDialog::DontUseNativeDialog;
#endif
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
    Base::Reference<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")
                               ->GetGroup("Preferences")->GetGroup("General");
    std::string dir = hPath->GetASCII("FileOpenSavePath", path.c_str());
    QFileInfo fi(QString::fromUtf8(dir.c_str()));
    if (!fi.exists())
        dir = path;
    return QString::fromUtf8(dir.c_str());
}

/**
 * Sets the working directory to \a dir for the file dialog.
 * If \a dir is a file then the path only is taken.
 * getWorkingDirectory() returns the working directory.
 */
void FileDialog::setWorkingDirectory(const QString& dir)
{
    QString dirName = dir;
    if (!dir.isEmpty()) {
        QFileInfo info(dir);
        dirName = info.absolutePath();
    }

    Base::Reference<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")
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
#if QT_VERSION >= 0x040500
    grid->addWidget(extensionButton, 4, 2, Qt::AlignLeft);
#else
    grid->addWidget(extensionButton, 4, 5, Qt::AlignLeft);
#endif

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
    QString fn = filename->text();
    if (fn.startsWith(QLatin1String("*"))) {
        QFileInfo fi(fn);
        QString ext = fi.suffix();
        ext.prepend(QLatin1String("*."));
        QStringList filters = this->filters();
        bool ok=false;
        // Compare the given suffix with the suffixes of all filters
        QString filter;
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
    else if (!fn.isEmpty()) {
        QFileInfo fi(fn);
        QString ext = fi.completeSuffix();
        QRegExp rx(QLatin1String("\\(\\*.(\\w+)"));
        QString suf = selectedFilter();
        if (rx.indexIn(suf) >= 0)
            suf = rx.cap(1);
        if (ext.isEmpty())
            setDefaultSuffix(suf);
        else if (ext.toLower() != suf.toLower()) {
            fn = QString::fromAscii("%1.%2").arg(fn).arg(suf);
            selectFile(fn);
        }
    }

    QFileDialog::accept();
}

void FileOptionsDialog::toggleExtension()
{
    QWidget* w = extension();
    if (w)
        showExtension(!w->isVisible());
}

void FileOptionsDialog::setOptionsWidget(FileOptionsDialog::ExtensionPosition pos, QWidget* w, bool show)
{
    if (pos == ExtensionRight) {
        setExtension(w);
        setOrientation(Qt::Horizontal);
    }
    else if (pos == ExtensionBottom) {
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

    button = new QPushButton(QLatin1String("..."), this);
    button->setFixedWidth(2*button->fontMetrics().width(QLatin1String(" ... ")));
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
 * FileChooser::Mode is set to File.
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
    int w1 = 2*button->fontMetrics().width(txt);
    int w2 = 2*button->fontMetrics().width(QLatin1String(" ... "));
    button->setFixedWidth( (w1 > w2 ? w1 : w2) );
}

/**
 * Returns the browse button's text.
 */
QString FileChooser::buttonText() const
{
    return button->text();
}

// ----------------------------------------------------------------------

/* TRANSLATOR Gui::SelectModule */

SelectModule::SelectModule (const QString& type, const SelectModule::Dict& types, QWidget * parent)
  : QDialog(parent, Qt::WindowTitleHint)
{
    setWindowTitle(tr("Select module"));
    groupBox = new QGroupBox(this);
    groupBox->setTitle(tr("Open %1 as").arg(type));

    group = new QButtonGroup(this);
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);

    gridLayout1 = new QGridLayout(groupBox);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(9);

    int index = 0;
    for (SelectModule::Dict::const_iterator it = types.begin(); it != types.end(); ++it) {
        QRadioButton* button = new QRadioButton(groupBox);

        QRegExp rx;
        QString filter = it.key();
        QString module = it.value();

        // ignore file types in (...)
        rx.setPattern(QLatin1String("\\s+\\([\\w\\*\\s\\.]+\\)$"));
        int pos = rx.indexIn(filter);
        if (pos != -1) {
            filter = filter.left(pos);
        }

        // ignore Gui suffix in module name
        rx.setPattern(QLatin1String("Gui$"));
        pos = rx.indexIn(module);
        if (pos != -1) {
            module = module.left(pos);
        }

        button->setText(QString::fromAscii("%1 (%2)").arg(filter).arg(module));
        button->setObjectName(it.value());
        gridLayout1->addWidget(button, index, 0, 1, 1);
        group->addButton(button, index);
        index++;
    }

    gridLayout->addWidget(groupBox, 0, 0, 1, 1);
    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(spacerItem, 1, 0, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    spacerItem1 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem1);

    okButton = new QPushButton(this);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    okButton->setText(tr("Select"));
    okButton->setEnabled(false);

    hboxLayout->addWidget(okButton);
    gridLayout->addLayout(hboxLayout, 2, 0, 1, 1);

    // connections
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(group, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClicked()));
}

SelectModule::~SelectModule()
{
}

void SelectModule::accept()
{
    if (group->checkedButton())
        QDialog::accept();
}

void SelectModule::reject()
{
    if (group->checkedButton())
        QDialog::reject();
}

void SelectModule::onButtonClicked()
{
    if (group->checkedButton())
        okButton->setEnabled(true);
    else
        okButton->setEnabled(false);
}

QString SelectModule::getModule() const
{
    QAbstractButton* button = group->checkedButton();
    return (button ? button->objectName() : QString());
}

SelectModule::Dict SelectModule::exportHandler(const QString& fileName, const QString& filter)
{
    return exportHandler(QStringList() << fileName, filter);
}

SelectModule::Dict SelectModule::exportHandler(const QStringList& fileNames, const QString& filter)
{
    // first check if there is a certain filter selected
    SelectModule::Dict dict;
    if (!filter.isEmpty()) {
        // If an export filter is specified search directly for the module
        std::map<std::string, std::string> filterList = App::GetApplication().getExportFilters();
        std::map<std::string, std::string>::const_iterator it;
        it = filterList.find((const char*)filter.toUtf8());
        if (it != filterList.end()) {
            QString module = QString::fromAscii(it->second.c_str());
            for (QStringList::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
                dict[*it] = module;
            }
            return dict;
        }
    }

    // the global filter (or no filter) was selected. We now try to sort filetypes that are
    // handled by more than one module and ask to the user to select one.
    QMap<QString, SelectModule::Dict> filetypeHandler;
    QMap<QString, QStringList > fileExtension;
    for (QStringList::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
        QFileInfo fi(*it);
        QString ext = fi.completeSuffix().toLower();
        std::map<std::string, std::string> filters = App::GetApplication().getExportFilters(ext.toAscii());
        
        if (filters.empty()) {
            ext = fi.suffix().toLower();
            filters = App::GetApplication().getExportFilters(ext.toAscii());
        }

        fileExtension[ext].push_back(*it);
        for (std::map<std::string, std::string>::iterator jt = filters.begin(); jt != filters.end(); ++jt)
            filetypeHandler[ext][QString::fromUtf8(jt->first.c_str())] = QString::fromAscii(jt->second.c_str());
        // set the default module handler
        if (!filters.empty())
            dict[*it] = QString::fromAscii(filters.begin()->second.c_str());
    }

    for (QMap<QString, SelectModule::Dict>::const_iterator it = filetypeHandler.begin(); 
        it != filetypeHandler.end(); ++it) {
        if (it.value().size() > 1) {
            SelectModule dlg(it.key(),it.value(), getMainWindow());
            QApplication::beep();
            if (dlg.exec()) {
                QString mod = dlg.getModule();
                const QStringList& files = fileExtension[it.key()];
                for (QStringList::const_iterator jt = files.begin(); jt != files.end(); ++jt)
                    dict[*jt] = mod;
            }
        }
    }

    return dict;
}

SelectModule::Dict SelectModule::importHandler(const QString& fileName, const QString& filter)
{
    return importHandler(QStringList() << fileName, filter);
}

SelectModule::Dict SelectModule::importHandler(const QStringList& fileNames, const QString& filter)
{
    // first check if there is a certain filter selected
    SelectModule::Dict dict;
    if (!filter.isEmpty()) {
        // If an import filter is specified search directly for the module
        std::map<std::string, std::string> filterList = App::GetApplication().getImportFilters();
        std::map<std::string, std::string>::const_iterator it;
        it = filterList.find((const char*)filter.toUtf8());
        if (it != filterList.end()) {
            QString module = QString::fromAscii(it->second.c_str());
            for (QStringList::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
                dict[*it] = module;
            }
            return dict;
        }
    }

    // the global filter (or no filter) was selected. We now try to sort filetypes that are
    // handled by more than one module and ask to the user to select one.
    QMap<QString, SelectModule::Dict> filetypeHandler;
    QMap<QString, QStringList > fileExtension;
    for (QStringList::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
        QFileInfo fi(*it);
        QString ext = fi.completeSuffix().toLower();
        std::map<std::string, std::string> filters = App::GetApplication().getImportFilters(ext.toAscii());
        
        if (filters.empty()) {
            ext = fi.suffix().toLower();
            filters = App::GetApplication().getImportFilters(ext.toAscii());
        }

        fileExtension[ext].push_back(*it);
        for (std::map<std::string, std::string>::iterator jt = filters.begin(); jt != filters.end(); ++jt)
            filetypeHandler[ext][QString::fromUtf8(jt->first.c_str())] = QString::fromAscii(jt->second.c_str());
        // set the default module handler
        if (!filters.empty())
            dict[*it] = QString::fromAscii(filters.begin()->second.c_str());
    }

    for (QMap<QString, SelectModule::Dict>::const_iterator it = filetypeHandler.begin(); 
        it != filetypeHandler.end(); ++it) {
        if (it.value().size() > 1) {
            SelectModule dlg(it.key(),it.value(), getMainWindow());
            QApplication::beep();
            if (dlg.exec()) {
                QString mod = dlg.getModule();
                const QStringList& files = fileExtension[it.key()];
                for (QStringList::const_iterator jt = files.begin(); jt != files.end(); ++jt)
                    dict[*jt] = mod;
            }
        }
    }

    return dict;
}


#include "moc_FileDialog.cpp"

