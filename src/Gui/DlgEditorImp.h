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


#ifndef DLG_EDITOR_IMP_H
#define DLG_EDITOR_IMP_H

#include "DlgEditor.h"
#include "PropertyPage.h"

namespace Gui {
namespace Dialog {

/** This class implements a preferences page for the editor settings.
 *  Here you can change differnt color settings and font for the editors.
 *  @see class PreferencePage
 *  @see class FCWidgetPrefs
 *  \author Werner Mayer
 */
class DlgSettingsEditorImp : public DlgEditorSettingsBase, public Gui::Dialog::PreferencePage, public FCWidgetPrefs
{
Q_OBJECT

public:
  DlgSettingsEditorImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgSettingsEditorImp();

  void OnChange(FCSubject<const char*> &rCaller, const char * sReason);

public slots:
  void onDisplayColor(const QString& name);
  void onChosenColor();
  void onChosenFont(const QString & item);

private:
  void restorePreferences();
  void savePreferences();

private:
  std::map<QString, long> _mColors; /**< Color map containing color settings */

  DlgSettingsEditorImp( const DlgSettingsEditorImp & );
  DlgSettingsEditorImp& operator=( const DlgSettingsEditorImp & );
};


/** This class implements a color map of QString->QColor.
 * A string does not represent the name of the color itself
 * but it represents the "setting" the color is assigned to
 * (e.g. text color, keyword color, ...)
 * \author Werner Mayer
 */
class DefColorMap
{
private:
  DefColorMap(void);
  ~DefColorMap(void);

  static DefColorMap *_pcSingleton;
  std::map<QString, long> m_clDefColors;

public:
  std::vector<QString> GetKeys() const;
  long GetColor(const QString& name);
  static void Destruct(void);
  static DefColorMap &Instance(void);
};

/** Returns the @ref DefColorMap instance */
inline DefColorMap &GetDefCol(void)
{
  return DefColorMap::Instance();
}

} // namespace Dialog
} // namespace Gui

#endif // DLG_EDITOR_IMP_H
