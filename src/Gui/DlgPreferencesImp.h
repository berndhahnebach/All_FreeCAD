/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef DLGPREFERENCESIMP_H
#define DLGPREFERENCESIMP_H

#include "DlgPreferences.h"

// forward declaration
class QTabWidget;
class QListBox;

namespace Gui {
namespace Dialog {

/**
 * This class implements a dialog containing several preference pages.
 * 
 * Each preference page can be created by the Qt Designer selecting the "Widget" item
 * in the project dialog.
 * For automation of saving or loading the settings of the elements of such a page
 * (e.g. combo boxes, line edits, check boxes, ...) you can make use of the classes
 * inherited by @ref PrefWidget class like:
 * PrefSpinBox, PrefLineEdit, PrefComboBox, PrefListBox, PrefCheckBox, PrefRadioButton and
 * PrefSlider
 *
 * The implementation class must inherit from the Qt-generated class and @ref PreferencePage.
 * For each element inside your page you want to save or load its settings automatically, you 
 * have to call append(<objectname>->getHandler()) in the constructor of your class. Furthermore
 * you have to make sure to have specified the "prefEntry" and "prefPath" properties for each element
 * in the Qt designer.
 *
 * See the example below for more details:
 *
 * \code
 *  // This class was created by Qt's uic tool 
 *  class MyPrefPage : public QWidget
 *  {
 *  public:
 *    MyPrefPage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 )
 *    {
 *      myLineEdit->setProperty( "prefEntry", "lineedit" );
 *      myLineEdit->setProperty( "prefPath", "GroupName" );
 *      myCheckBox->setProperty( "prefEntry", "checkbox" );
 *      myCheckBox->setProperty( "prefPath", "GroupName" );
 *      ...
 *    }
 *
 *    PrefLineEdit* myLineEdit;
 *    PrefCheckBox* myCheckBox;
 * };
 * \endcode
 * In the derived class you just have to append these elements
 * \code
 *  class MyPrefPageImp : public MyPrefPage, public Gui::Dialog::PreferencePage
 *  {
 *  public:
 *    MyPrefPageImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 )
 *    {
 *      append( myLineEdit->getHandler() );
 *      append( myCheckBox->getHandler() );
 *    }
 * };
 * \endcode
 *
 * \author Werner Mayer, Jürgen Riegel
 */
class GuiExport DlgPreferencesImp : public DlgPreferences
{ 
Q_OBJECT

public:
  static void addPage( const QString& className, const QString& group );

  DlgPreferencesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgPreferencesImp();

private:
  /** @name for internal use only */
  //@{
  void onPrefPageClicked(int item );

  void addPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2);
  void addPreferencePage( QWidget* page );
  QTabWidget* getPreferenceGroup(int id);
  QTabWidget* getOrAddPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2);
  void connectWidget(QWidget* page) const;
  //@}

private:
  QMap<QString, int> _mGroupIDs; /**< Name of preference page with its ID */
  QTabWidget*        _pCurTab;   /**< Tab widget */
  static QStringList _pages; /**< Name of all registered preference pages */
};

} // namespace Dialog
} // namespace Gui

#endif // DLGPREFERENCESIMP_H
