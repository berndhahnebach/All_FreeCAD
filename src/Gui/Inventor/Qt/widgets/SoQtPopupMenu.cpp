/**************************************************************************\
 *
 *  This file is part of the Coin GUI toolkit libraries.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.  See the
 *  file LICENSE.LGPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you want to use this library with software that is incompatible
 *  licensewise with the LGPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/

// @configure_input@

/*!
  \class SoQtPopupMenu Inventor/Qt/widgets/SoQtPopupMenu.h
  \brief The SoQtPopupMenu class implements a common interface for popup
  menu management for all the Coin GUI toolkit libraries.

  The point of this class is to enable client code to be exactly the
  same for all GUI libraries, even without being template-based.
*/

// includes the FreeCAD configuration
#include "../../../Config.h"


#include <assert.h>

#include <Inventor/SoLists.h>
#include <Inventor/errors/SoDebugError.h>

#include <soQtdefs.h>
#include <Inventor/Qt/widgets/SoQtPopupMenu.h>
#include <Inventor/Qt/widgets/QtNativePopupMenu.h>

// *************************************************************************

// Documentation for abstract methods.

/*!
  \fn void SoQtPopupMenu::popUp(QWidget* inside, int x, int y)

  Invokes this popup menu instance.

  The coordinates for the popup will be used relative to the origo of
  the widget argument.
*/

// *************************************************************************

// The private data for the SoQtPopupMenu.

class SoQtPopupMenuP {
public:
  SbIntList * menuitems;
  SbIntList * radiogroups;
  SbPList * callbacks;
  SbPList * userdata;
};


#define PRIVATE(o) (o->pimpl)

// *************************************************************************

/*!
  Instantiation method.

  SoQtPopupMenu is implemented with this "factory" method instead of
  a public constructor to provide a completely generic interface on
  top of the toolkit's native popup menu implementation.
*/
SoQtPopupMenu *
SoQtPopupMenu::createInstance(void)
{
  return new QtNativePopupMenu();
}


/*!
  The class constructor is protected, as SoQtPopupMenu uses a factory
  instantiation method.
*/
SoQtPopupMenu::SoQtPopupMenu(void)
{
  PRIVATE(this) = new SoQtPopupMenuP;

  PRIVATE(this)->menuitems = new SbIntList;
  PRIVATE(this)->radiogroups = new SbIntList;
  PRIVATE(this)->callbacks = NULL;
  PRIVATE(this)->userdata = NULL;
} // SoQtPopupMenu()

/*!
  Destructor.
*/
SoQtPopupMenu::~SoQtPopupMenu()
{
  delete PRIVATE(this)->menuitems;
  delete PRIVATE(this)->radiogroups;
  delete PRIVATE(this)->callbacks;
  delete PRIVATE(this)->userdata;
} // ~SoQtPopupMenu()

// *************************************************************************

/*!
*/

void
SoQtPopupMenu::setMenuItemMarked(
  int itemid,
  SbBool marked)
{
  int groupid = this->getRadioGroup(itemid);
  if (groupid == -1) {
    this->_setMenuItemMarked(itemid, marked);
    return;
  }
  const int numGroupedItems = PRIVATE(this)->menuitems->getLength();
  int groupsize = 0;
  int i;
  for (i = 0; i < numGroupedItems; i++) {
    if (((*PRIVATE(this)->radiogroups)[i] == groupid) &&
         ((*PRIVATE(this)->menuitems)[i] != -1))
      groupsize++;
  }
  if (this->getMenuItemMarked(itemid) == marked) {
#if SOQT_DEBUG && 0
    SoDebugError::postWarning("SoQtPopupMenu::SetMenuItemEnabled",
      "Marked state already '%s'.", marked ? "on" : "off");
#endif // SOQT_DEBUG
  } else if (groupsize == 0) {
    assert(0 && "how weird...");
  } else if (groupsize == 1) {
    this->_setMenuItemMarked(itemid, marked);
  } else if ((groupsize == 2) ||
              ((groupsize > 2) && marked)) {
    for (i = 0; i < numGroupedItems; i++) {
      if (((*PRIVATE(this)->radiogroups)[i] == groupid) &&
           ((*PRIVATE(this)->menuitems)[i] != -1)) {
        this->_setMenuItemMarked((*PRIVATE(this)->menuitems)[i],
          ((*PRIVATE(this)->menuitems)[i] == itemid) ? marked :
          ((groupsize == 2) ? (! marked) : FALSE));
      }
    }
  } else {
#if SOQT_DEBUG
    SoDebugError::postWarning("SoQtPopupMenu::SetMenuItemEnabled",
      "Couldn't handle request.  "
      "Consider that radio grouping is handled automatically.");
#endif // SOQT_DEBUG
  }
} // setMenuItemMarked()

// *************************************************************************

/*!
*/

int
SoQtPopupMenu::newRadioGroup(
  int groupid)
{
  const int numGroupedItems = PRIVATE(this)->radiogroups->getLength();
  int id = groupid;
  if (id != -1) {
    SbBool hit = FALSE;
    int i;
    for (i = 0; i < numGroupedItems && hit == FALSE; i++) {
      if ((*PRIVATE(this)->radiogroups)[i] == id)
        hit = TRUE;
    }
    if (hit == TRUE) {
#if SOQT_DEBUG
      SoDebugError::postInfo("SoQtPopupMenu::NewRadioGroup",
        "requested group id already taken");
#endif // SOQT_DEBUG
      return -1; 
    }
  } else {
    id = 0;
    SbBool found;
    do {
      id++;
      found = FALSE;
      int i;
      for (i = 0; i < numGroupedItems && found == FALSE; i++) {
        if ((*PRIVATE(this)->radiogroups)[i] == id)
          found = TRUE;
      }
    } while (found == TRUE);
  }
  // id is OK here
  PRIVATE(this)->menuitems->append(-1); // fake menu item
  PRIVATE(this)->radiogroups->append(id); // register id in array
  return id;
} // newRadioGroup()

/*!
*/

int
SoQtPopupMenu::getRadioGroup(
  int itemid)
{
  assert(itemid != -1);
  const int numGroupedItems = PRIVATE(this)->radiogroups->getLength();
  int i;
  for (i = 0; i < numGroupedItems; i++) {
    if ((*PRIVATE(this)->menuitems)[i] == itemid)
      return (*PRIVATE(this)->radiogroups)[i];
  }
  return -1;
} // getRadioGroup()

/*!
*/

int
SoQtPopupMenu::getRadioGroupSize(
  int groupid)
{
  int count = 0;
  const int numItems = PRIVATE(this)->radiogroups->getLength();
  int i;
  for (i = 0; i < numItems; i++) {
    if ((*PRIVATE(this)->radiogroups)[i] == groupid &&
         (*PRIVATE(this)->menuitems)[i] != -1)
      count++;
  }
  return count;
} // getRadioGroupSize()

/*!
*/

void
SoQtPopupMenu::addRadioGroupItem(
  int groupid,
  int itemid)
{
  PRIVATE(this)->menuitems->append(itemid);
  PRIVATE(this)->radiogroups->append(groupid);
} // addRadioGroupItem()

/*!
*/

void
SoQtPopupMenu::removeRadioGroupItem(
  int itemid)
{
  assert(itemid != -1);
  const int numGroupedItems = PRIVATE(this)->menuitems->getLength();
  int i;
  for (i = 0; i < numGroupedItems; i++) {
    if ((*PRIVATE(this)->menuitems)[i] == itemid) {
      PRIVATE(this)->menuitems->remove(i);
      PRIVATE(this)->radiogroups->remove(i);
      return;
    }
  }
} // removeRadioGroupItem()

void
SoQtPopupMenu::setRadioGroupMarkedItem(
  int itemid)
{
  const int numItems = PRIVATE(this)->menuitems->getLength();
  int i;
  int groupid = -1;
  for (i = 0; i < numItems && groupid == -1; i++) {
    if ((*PRIVATE(this)->menuitems)[i] == itemid)
      groupid = (*PRIVATE(this)->radiogroups)[i];
  }
  if (groupid == -1)
    return;
  for (i = 0; i < numItems; i++) {
    if ((*PRIVATE(this)->radiogroups)[i] == groupid) {
      int item = (*PRIVATE(this)->menuitems)[i];
      if (item != -1 && item != itemid)
        this->setMenuItemMarked(item, FALSE);
    }
  }
} // setRadioGroupItemEnabled()

int
SoQtPopupMenu::getRadioGroupMarkedItem(
  int groupid)
{
  const int numItems = PRIVATE(this)->menuitems->getLength();
  int i;
  for (i = 0; i < numItems; i++) {
    if ((*PRIVATE(this)->radiogroups)[i] == groupid) {
      int item = (*PRIVATE(this)->menuitems)[i];
      if (item != -1 && this->getMenuItemMarked(item))
        return item;
    }
  }
  return -1;
} // setRadioGroupItemEnabled()

// *************************************************************************

/*!
*/

void
SoQtPopupMenu::addMenuSelectionCallback(
  SoQtMenuSelectionCallback * callback,
  void * user)
{
  if (PRIVATE(this)->callbacks == NULL) {
    PRIVATE(this)->callbacks = new SbPList;
    PRIVATE(this)->userdata = new SbPList;
  }
  PRIVATE(this)->callbacks->append((void *) callback);
  PRIVATE(this)->userdata->append(user);
} // addMenuSelectionCallback()

/*!
*/

void
SoQtPopupMenu::removeMenuSelectionCallback(
  SoQtMenuSelectionCallback * callback,
  void * user)
{
  if (PRIVATE(this)->callbacks == NULL) {
#if SOQT_DEBUG
    SoDebugError::postInfo("SoQtPopupMenu::RemoveMenuSelectionCallback",
      "no callbacks at all");
#endif // SOQT_DEBUG
    return;
  }
  int numCallbacks = PRIVATE(this)->callbacks->getLength();
  int i;
  for (i = 0; i < numCallbacks; i++) {
    if ((*PRIVATE(this)->callbacks)[i] == (void *) callback &&
         (*PRIVATE(this)->userdata)[i] == user) {
      PRIVATE(this)->callbacks->remove(i);
      PRIVATE(this)->userdata->remove(i);
      i--;
      numCallbacks--;
    }
  }
} // removeMenuSelectionCallback()

/*!
*/

void
SoQtPopupMenu::invokeMenuSelection(
  int itemid)
{
  if (PRIVATE(this)->callbacks == NULL)
    return;
  const int numCallbacks = PRIVATE(this)->callbacks->getLength();
  int i;
  for (i = 0; i < numCallbacks; i++) {
    SoQtMenuSelectionCallback * cb = (SoQtMenuSelectionCallback *)
      (*PRIVATE(this)->callbacks)[i];
    (*cb)(itemid, (*PRIVATE(this)->userdata)[i]);
  }
} // invokeMenuSelection()

// *************************************************************************

