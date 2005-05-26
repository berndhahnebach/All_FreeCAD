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


#ifndef ___FILE_DIALOG_H__
#define ___FILE_DIALOG_H__

#ifndef _PreComp_
# include <qfiledialog.h>
# include <qpixmap.h>
# include <qscrollview.h>
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
  static QString getOpenFileName ( const QString & startWith = QString::null, const QString & filter = QString::null, 
                                   QWidget* parent = 0, const char* name = 0, const QString & caption = QString::null,
                                   const QString& buttonText = QString::null, bool * ok = 0 );
  static QString getSaveFileName ( const QString & startWith = QString::null, const QString & filter = QString::null, 
                                   QWidget* parent = 0, const char* name = 0, 
                                   const QString & caption = QString::null, bool * ok = 0 );
  static QString getExistingDirectory( const QString & dir = QString::null, QWidget *parent = 0, 
                                       const char* name = 0, const QString& caption = QString::null, 
                                       bool dirOnly = true, bool resolveSymlinks = true, bool * ok = 0 );

public:
  FileDialog (Mode mode, QWidget* parent = 0, const char* name = 0, bool modal = false);
  FileDialog (Mode mode, const QString& dirName, const QString& filter = QString::null, QWidget* parent = 0, const char* name = 0, bool modal = false);
  FileDialog (const QString& dirName, const QString& filter = QString::null, QWidget* parent = 0, const char* name = 0, bool modal = false);
  virtual ~FileDialog();

  QString selectedFileName();

protected slots:
  virtual void accept();
};

// ======================================================================

/**
 * The ImagePreview class draws a pixmap on an area.
 * \author Werner Mayer
 */
class ImagePreview : public QScrollView
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
class PreviewLabel : public QWidget, public QFilePreview
{
  Q_OBJECT

public:
  PreviewLabel( QWidget *parent=0, const char* name =0 );

  void previewUrl( const QUrl &u );

private:
  ImagePreview* _preview;
  QCheckBox* _cbview;
};

// ======================================================================

/**
 * The FileIconProvider class provides icons for FileDialog to use.
 * \author Werner Mayer
 */
class FileIconProvider : public QFileIconProvider
{
public:
  FileIconProvider( QObject * parent = 0, const char * name = 0 );
  ~FileIconProvider();

  const QPixmap * pixmap ( const QFileInfo & info );
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

public:
  enum Mode { File, Directory };

  FileChooser ( QWidget * parent = 0, const char * name = 0 );
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

public slots:
  virtual void setFileName( const QString &fn );
  virtual void setMode( Mode m );
  virtual void setFilter ( const QString & );

signals:
  void fileNameChanged( const QString & );

private slots:
  void chooseFile();

private:
  QLineEdit *lineEdit;
  QPushButton *button;
  Mode md;
  QString _filter;
};

} // namespace Gui

#endif // ___FILE_DIALOG_H__
