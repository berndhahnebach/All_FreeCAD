/***************************************************************************
                          DlgEditorImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Do the editor settings
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#ifndef DLG_EDITOR_IMP_H
#define DLG_EDITOR_IMP_H

#include "DlgEditor.h"
#include "PropertyPage.h"

/**
 * This class implements the settings for the editor
 * @see class FCScintillaEdit
 */
class FCDlgEditorSettings : public FCDlgEditorSettingsBase, public FCPreferencePage, public FCWidgetPrefs
{
  Q_OBJECT

  public:
    FCDlgEditorSettings( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgEditorSettings();

    /** Does not do anything */
    void OnChange(FCSubject<const char*> &rCaller, const char * sReason);

  public slots:
    /** Assign the color to the FCColorButton object.
     * To get the color it searches for the color
     * in the @ref FCDefColorMap before.
     */
    void onAssignColor(const QString& name);
    /** Updates the color map if a color was changed */
    void onChosenColor();
    /** Sets the font, but it is disabled in this version */
    void onChosenFont(const QString & item);

  protected:
    /** Restores the color map */
    void restorePreferences();
    /** Saves the color map */
    void savePreferences();
    std::map<QString, long> m_clColors;
};

/** 
 * This class implements a color map of QString->QColor.
 * A string does not represent the name of the color itself
 * but it represents the "setting" the color is assigned to
 * (e.g. text color, keyword color, ...)
 */
class FCDefColorMap
{
	private:
		// Singleton
		FCDefColorMap(void);
		~FCDefColorMap(void);

		static FCDefColorMap *_pcSingleton;

    std::map<QString, long> m_clDefColors;

	public:
    std::vector<QString> GetKeys() const;
		long GetColor(const QString& name);
		static void Destruct(void);
		static FCDefColorMap &Instance(void);

		friend FCDefColorMap &GetDefCol(void); 
};

inline FCDefColorMap &GetDefCol(void)
{
  return FCDefColorMap::Instance();
}

#endif // DLG_EDITOR_IMP_H
