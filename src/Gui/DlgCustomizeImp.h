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


#ifndef DLGCUSTOMIZE_H
#define DLGCUSTOMIZE_H

#ifndef _PreComp_
# include <qdialog.h>
# include <vector>
#endif

class QTabWidget;
class QGridLayout;
class QPushButton;
class QTabWidget;
class QHBoxLayout;

namespace Gui {
namespace Dialog {

/**
 * Dialog which contains all available commands and separate
 * them in several categories (file, edit, view,...).
 *
 * You can extend the existing toolbars or buttongroups with 
 * several commands just by drag and drop.
 * Furthermore it is possible to create new commands from macros.
 * @see DlgCustomCommandsImp
 * @see DlgCustomToolbarsImp
 * @see DlgCustomCmdbarsImp
 * @see DlgCustomActionsImp
 * \author Werner Mayer
 */
class DlgCustomizeImp : public QDialog
{ 
    Q_OBJECT

public:
  DlgCustomizeImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgCustomizeImp();

  static void addPage( const QString& className );
  void addPage ( QWidget* w );

private slots:
  void onApply();

private:
  /** @name for internal use only */
  //@{
  QPushButton* buttonApply; /**< the apply button */
  QPushButton* buttonClose; /**< the cancel button */
  QTabWidget* tabWidget; /**< tab wigdets containing all pages */ 
  QGridLayout* customLayout; /**< layout */
  QHBoxLayout* layout; /** horizontal layout */
  static QStringList _pages; /**< Name of all registered preference pages */
  //@}
};

} // namespace Dialog
} // namespace Gui

#endif // DLGCUSTOMIZE_H
