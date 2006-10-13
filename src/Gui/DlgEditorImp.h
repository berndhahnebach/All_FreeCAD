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

#include <qmap.h>

#include "DlgEditor.h"

namespace Gui {
class PythonSyntaxHighlighter;

namespace Dialog {

/** This class implements a preferences page for the editor settings.
 *  Here you can change different color settings and font for editors.
 *  @author Werner Mayer
 */
class DlgSettingsEditorImp : public DlgEditorSettingsBase
{
Q_OBJECT

public:
  DlgSettingsEditorImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgSettingsEditorImp();

public slots:
  void onDisplayColor(const QString& name);
  void onChosenColor();

protected:
  void saveSettings();
  void loadSettings();

  void languageChange();

private:
  void trToOrig();
  QMap<QString, unsigned long> _mColors; /**< Color map containing color settings. */
  QMap<QString, QString> _trMap; /**< Holds the original text to each translated text. */
  Gui::PythonSyntaxHighlighter* pythonSyntax;

  DlgSettingsEditorImp( const DlgSettingsEditorImp & );
  DlgSettingsEditorImp& operator=( const DlgSettingsEditorImp & );
};


/** This class implements a color map of string->color.
 * The string represents the type of word (text, keywords, operators, ...).
 * To each type a special color is assigned to. These types and their colors
 * are used within the Python syntax highlighing. The colors are stored as unsigned long,
 * not as QColor objects. 
 * To get back the (r,g,b) values from a long head the statement as follows:
 * unsigned long col = ...
 * QColor color((col>>24) & 0xff, (col >> 16) & 0xff, (col >> 8) & 0xff);
 * @author Werner Mayer
 */
class DefColorMap
{
protected:
  DefColorMap(void);
  ~DefColorMap(void);

  static DefColorMap *_pcSingleton;
  QMap<QString, unsigned long> m_clDefColors;

public:
  QStringList types() const;
  unsigned long color(const QString& name);
  static void destruct(void);
  static DefColorMap &instance(void);
};

/** Returns the @ref DefColorMap instance */
inline DefColorMap &GetDefCol(void)
{
  return DefColorMap::instance();
}

} // namespace Dialog
} // namespace Gui

#endif // DLG_EDITOR_IMP_H
