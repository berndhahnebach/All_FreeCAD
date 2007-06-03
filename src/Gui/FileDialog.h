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


#ifndef GUI_FILEDIALOG_H
#define GUI_FILEDIALOG_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

class QCheckBox;
class QLineEdit;
class QPushButton;

namespace Gui {

/**
 * The FileDialog class provides dialogs that allow users to select files or directories.
 * \author Werner Mayer
 */
class GuiExport FileDialog : public QFileDialog
{
    Q_OBJECT

public:
    static QString getOpenFileName( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), 
                                    const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0 );
    static QString getSaveFileName( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), 
                                    const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0 );
    static QString getExistingDirectory( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), 
                                         Options options = ShowDirsOnly );
    static QStringList getOpenFileNames( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(),
                                         const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0 );
    static QString getWorkingDirectory();
    static void setWorkingDirectory( const QString& );

public:
  FileDialog ( QWidget * parent, Qt::WFlags flags );
  FileDialog ( QWidget * parent = 0, const QString & caption = QString(), const QString & directory = QString(), 
               const QString & filter = QString() );
  virtual ~FileDialog();

protected:
  QString selectedFileName();

protected Q_SLOTS:
  virtual void accept();
};

// ======================================================================

/**
 * The FileOptionsDialog class provides an extensible file dialog with an additonal widget either at the right 
 * or at the bottom, that can be shown or hidden with the 'Extended' button.
 * @author Werner Mayer
 */
class GuiExport FileOptionsDialog : public QFileDialog
{
  Q_OBJECT

public:
  enum ExtensionPosition {
      ExtensionRight    = 0,
      ExtensionBottom   = 1
  };

  FileOptionsDialog ( QWidget* parent, Qt::WFlags );
  virtual ~FileOptionsDialog();

  void accept();

  void setOptionsWidget( ExtensionPosition pos , QWidget*, bool show = false );
  QWidget* getOptionsWidget() const;

protected Q_SLOTS:
  void toggleExtension();

Q_SIGNALS:
  void filterSelected(const QString&);

private:
  QPushButton* extensionButton;
};

// ======================================================================
#if 0
/**
 * The ImagePreview class draws a pixmap on an area.
 * \author Werner Mayer
 */
class ImagePreview : public Q3ScrollView
{
public:
  ImagePreview( QWidget *parent=0, const char* name=0 );

  void setPixmap( const QPixmap &pix );
  void drawContents( QPainter *p, int clipx, int clipy, int clipw, int cliph );

private:
  QPixmap _pixmap;
};

// ======================================================================

/**
 * The PreviewLabel class is a preview widget that can be used with FileDialogs.
 * \author Werner Mayer
 */
class PreviewLabel : public QWidget, public Q3FilePreview
{
  Q_OBJECT

public:
  PreviewLabel( QWidget *parent=0, const char* name =0 );

  void previewUrl( const Q3Url &u );

private:
  ImagePreview* _preview;
  QCheckBox* _cbview;
};
#endif
// ======================================================================

/**
 * The FileIconProvider class provides icons for FileDialog to use.
 * \author Werner Mayer
 */
class FileIconProvider : public QFileIconProvider
{
public:
    FileIconProvider();
    ~FileIconProvider();

    QIcon icon ( IconType type ) const;
    QIcon icon ( const QFileInfo & info ) const;
    QString type ( const QFileInfo & info ) const;
};

// ======================================================================

/**
 * The FileChooser class provides a lineedit with a button on the right side
 * to specify a file or directory.
 * \author Werner Mayer
 */
class GuiExport FileChooser : public QWidget
{
  Q_OBJECT

  Q_ENUMS( Mode )
  Q_PROPERTY( Mode mode READ mode WRITE setMode )
  Q_PROPERTY( QString  fileName  READ fileName      WRITE setFileName      )
  Q_PROPERTY( QString  filter    READ filter        WRITE setFilter        )
  Q_PROPERTY( QString  buttonText  READ buttonText  WRITE setButtonText    )

public:
  enum Mode { File, Directory };

  FileChooser ( QWidget * parent = 0 );
  virtual ~FileChooser();

  /** 
   * Returns the set filter.
   */
  QString filter() const;

  /** 
   * Returns the filename.
   */
  QString fileName() const;

  /**
   * Returns true if this widgets is set to choose a file, if it is
   * set to choose false is returned.
   */
  Mode mode() const;

  /**
   * Returns the button's text.
   */
  QString buttonText() const;

public Q_SLOTS:
  virtual void setFileName( const QString &fn );
  virtual void setMode( Mode m );
  virtual void setFilter ( const QString & );
  virtual void setButtonText ( const QString & );

Q_SIGNALS:
  void fileNameChanged( const QString & );
  void fileNameSelected( const QString & );

private Q_SLOTS:
  void chooseFile();

private:
  QLineEdit *lineEdit;
  QPushButton *button;
  Mode md;
  QString _filter;
};

} // namespace Gui

#endif // GUI_FILEDIALOG_H
