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
#include "Window.h"

// forward declaration
class QTabWidget;

namespace Gui {
namespace Dialog {

/**
 * This class implements a dialog containing several preference pages.
 * 
 * Each preference page can be created by the Qt Designer selecting the "Widget" project.
 * For automation of saving/loading of the elements of such a page
 * (e.g. combo boxes, line edits, check boxes, ...) you can use the
 * @ref FCWidgetPrefs classes like:
 * FCEditSpinBox, FCLineEdit, FCComboBox, FCListBox, FCCheckBox, FCRadioButton and FCSlider
 *
 * Your implementation class must inherit the Qt-generated class and @ref FCPreferencePage.
 * In the constructor of your class you must call for each object in your page
 * append(<objectname>->getHandler());
 * to use the full automation
 * \author Werner Mayer, Jürgen Riegel
 */
class GuiExport DlgPreferencesImp : public QDialog, public FCWindowParameter
{ 
Q_OBJECT

public:
  static void addPage(const QString& name);
  static void addGroup(const QString& name);

  DlgPreferencesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgPreferencesImp();

private slots:
  /// click the OK button
  void onOK();
  /// click the Apply button
  void onApply();
  /// click the Cancel button
  void onCancel();
  /// for internal use only
  void onPrefPageClicked(int item );

private:
  void addPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2);

  void addPreferencePage(QWidget* page, const QString& name);
  /** @name for internal use only */
  //@{
  QTabWidget* getPreferenceGroup(int id);
  QTabWidget* getOrAddPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2);
  void connectWidget(QWidget* page) const;
  //@}

  /** @name buttons and stack, ... */
  //@{	
  QPushButton* PushButton14;
  QPushButton* PushButton15;
  QPushButton* PushButton13;
  QListBox* ListBox;
  QWidgetStack* tabWidgetStack;
  QGridLayout* DlgPreferencesLayout;
  QGridLayout* Layout6;
  //@}

private:
  std::map<QString, int> m_mGroupIDs; /**< Name of preference page with its ID */
  QTabWidget               * m_pCurTab; /**< Tab widget */
  static std::vector<QString> aclPages; /**< Name of all registered preference pages */
};

} // namespace Dialog
} // namespace Gui

#endif // DLGPREFERENCESIMP_H
