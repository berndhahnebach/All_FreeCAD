/** \file DlgPreferencesImp.h
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/

#ifndef DLGPREFERENCESIMP_H
#define DLGPREFERENCESIMP_H
#include "DlgPreferences.h"
#include "Window.h"

class QTabWidget;
class QTabWidget;

class DlgPreferencesImp : public DlgPreferences, public FCWindow
{ 
    Q_OBJECT

  public:
    DlgPreferencesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgPreferencesImp();
    void addPreferenceGroup(const char* name);
    void addPreferenceGroup(const char* name, const QPixmap& p);
    void addPreferencePage(QWidget* page, const char* name);
    void addPreferencePage(QWidget* page, const char* name, const QPixmap& p);
    void addPreferencePage(QWidget* page, const char* name, const char* group);

  protected:
    QTabWidget* getPreferenceGroup(const char* name);
    QTabWidget* getOrAddPreferenceGroup(const char* name);
    std::string getGroupName() const;
    std::string getPageName() const;

  private:
    std::map<std::string, int> m_mGroupsID;
    std::map<std::string, QListViewItem*> m_mGroupsItem;
    QTabWidget                          * m_pCurTab;
    QWidget                             * m_pCurPage;

    std::map <std::string, std::pair<QListViewItem*, QWidget*> >  m_mPages;

  private slots:
    void prefPageClicked(QListViewItem * item );
    void prefPageClicked(QWidget * item );

};

#endif // DLGPREFERENCESIMP_H
