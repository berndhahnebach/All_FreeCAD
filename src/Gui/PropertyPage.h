/***************************************************************************
                          PropertyPage.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file PropertyPage.h
 *  \brief The property page base class
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
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



#ifndef __PROPERTY_PAGE__H__
#define __PROPERTY_PAGE__H__

#include "PrefWidgets.h"

namespace Gui {
namespace Dialog {

/** Base class for property pages.
 * The pages of @ref DlgCustomizeImp and @ref DlgPreferencesImp
 * should inherit CPropertyPage to use apply/cancel mechanism.
 * Inherited classes should reimplement @ref apply() and
 * @ref cancel().
 */
class GuiExport CPropertyPage
{
protected:
	CPropertyPage();

public:
	virtual ~CPropertyPage() {}
	/** Returns whether the page was modified or not */
	bool isModified();
	/** Sets the page to be modified */
	void setModified(bool b);
	/** Applies all changes calling @ref apply and reset the modified state */
	void onApply();
	/** Discards all changes calling @ref apply and reset the modified state */
	void onCancel();

protected:
	/** Applies all changes. Reimplement this in your subclasses. */
	virtual void apply();
	/** Discards all changes. Reimplement this in your subclasses. */
	virtual void cancel();

private:
	bool bChanged; /**< for internal use only */
};

/**
 * @see CPropertyPage
 */
class GuiExport PreferencePage : public CPropertyPage, public FCWidgetPrefsManager
{
public:
	PreferencePage();
	virtual ~PreferencePage();
};

} // namespace Dialog
} // namespace Gui

#endif // __PROPERTY_PAGE__H__
