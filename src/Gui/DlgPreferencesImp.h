/** \file DlgPreferencesImp.h
 *  \brief  
 *  \author Juergen Riegel, Werner Mayer
 *  \version 1.7
 *  \date    2003/06/20 19:36:36
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

// forward declaration
class QTabWidget;

/**
 * This class is the container class which handles all the preference pages
 * @see class FCDlgPreferencesImp
 */
class DlgPreferencesImp : public DlgPreferences, public FCWindowParameter
{ 
    Q_OBJECT

  public:
    DlgPreferencesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgPreferencesImp();
	/// Add a preferences page group
    void addPreferenceGroup(const char* name, const char* Pixmap = "PrefTree_GroupClosed");
	/// Add a page
    void addPreferencePage(QWidget* page, const char* name, const char* group = 0L, const char* Pixmap = "PrefTree_PrefPage");

  protected:
    QTabWidget* getPreferenceGroup(const char* name);
    QTabWidget* getOrAddPreferenceGroup(const char* name);
    std::string getGroupName() const;
    std::string getPageName() const;
    void connectWidget(QWidget* page) const;

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

/**
 * This class is a 2nd implementation of the container class.
 * It uses a listbox for the groups instead of a listview.
 * 
 * The single preference pages can be made by the Qt Designer. 
 * For a new page you must take the "QWidget" entry.
 * For automation of saving/loading of the elements of such a page
 * (e.g. combo boxes, line edits, check boxes, ...) you can use the
 * @ref FCWidgetPrefs classes like:
 * FCEditSpinBox, FCLineEdit, FCComboBox, FCListBox, FCCheckBox, FCRadioButton and FCSlider
 *
 * Your implementation class must inherit the Qt-generated class and @ref FCWidgetPrefsManager.
 * In the constructor of your class you must call for each object in your page
 * append(<objectname>->getHandler());
 * to use the full automation
 */
class FCDlgPreferencesImp : public QDialog, public FCWindowParameter
{ 
    Q_OBJECT

  public:
    /// construction
    FCDlgPreferencesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    /// destruction
    ~FCDlgPreferencesImp();

    /** Adds a new preference group */
    void addPreferenceGroup(const char* name, const char* Pixmap, const char* Pixmap2);
    
    /**
     * Adds a new preference page to the current group
     * Before the first call of this method @ref addPreferenceGroup must be called
     * otherwise the preference pages will be lost.
     */
    void addPreferencePage(QWidget* page, const char* name);

  protected:
    /// for internal use only
    QTabWidget* getPreferenceGroup(int id);
    /// for internal use only
    QTabWidget* getOrAddPreferenceGroup(const char* name, const char* Pixmap, const char* Pixmap2);
    /// for internal use only
    void connectWidget(QWidget* page) const;

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
    std::map<std::string, int> m_mGroupIDs;
    QTabWidget               * m_pCurTab;

  private slots:
    /// for internal use only
    void prefPageClicked(int item );
};

#endif // DLGPREFERENCESIMP_H
