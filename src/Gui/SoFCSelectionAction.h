/***************************************************************************
 *   Copyright (c) 2005 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef _SoFCSelectionAction_h
#define _SoFCSelectionAction_h

//#include <Inventor/SoAction.h> 
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/events/SoSubEvent.h>

class SoSFString;
class SoSFColor;

namespace Gui {

class SelectionChanges;

/**
 * The SoFCSelectionAction class is used to inform an SoFCSelection node
 * whether an object gets selected. 
 * @author Jürgen Riegel
 */
class GuiExport SoFCSelectionAction : public SoAction
{
    SO_ACTION_HEADER(SoFCSelectionAction);

public:
    SoFCSelectionAction (const SelectionChanges &SelCh);
    ~SoFCSelectionAction();

    static void initClass();
    static void finish(void);

    const SelectionChanges &SelChange;

protected:
    virtual void beginTraversal(SoNode *node);

private:
    static void callDoAction(SoAction *action,SoNode *node);
};

/**
 * The SoFCEnableSelectionAction class is used to inform an SoFCSelection node
 * whether selection is enabled or disabled. 
 * @author Werner Mayer
 */
class GuiExport SoFCEnableSelectionAction : public SoAction
{
    SO_ACTION_HEADER(SoFCEnableSelectionAction);

public:
    SoFCEnableSelectionAction (const SbBool& sel);
    ~SoFCEnableSelectionAction();

    const SbBool& selection;

    static void initClass();
    static void finish(void);

protected:
    virtual void beginTraversal(SoNode *node);

private:
    static void callDoAction(SoAction *action,SoNode *node);
};

/**
 * The SoFCEnableHighlightAction class is used to inform an SoFCSelection node
 * whether preselection is enabled or disabled. 
 * @author Werner Mayer
 */
class GuiExport SoFCEnableHighlightAction : public SoAction
{
    SO_ACTION_HEADER(SoFCEnableHighlightAction);

public:
    SoFCEnableHighlightAction (const SbBool& sel);
    ~SoFCEnableHighlightAction();

    const SbBool& highlight;

    static void initClass();
    static void finish(void);

protected:
    virtual void beginTraversal(SoNode *node);

private:
    static void callDoAction(SoAction *action,SoNode *node);
};

/**
 * The SoFCSelectionColorAction class is used to inform an SoFCSelection node
 * which selection color is used. 
 * @author Werner Mayer
 */
class GuiExport SoFCSelectionColorAction : public SoAction
{
    SO_ACTION_HEADER(SoFCSelectionColorAction);

public:
    SoFCSelectionColorAction (const SoSFColor& col);
    ~SoFCSelectionColorAction();

    const SoSFColor& selectionColor;

    static void initClass();
    static void finish(void);

protected:
    virtual void beginTraversal(SoNode *node);

private:
    static void callDoAction(SoAction *action,SoNode *node);
};

/**
 * The SoFCHighlightColorAction class is used to inform an SoFCSelection node
 * which preselection color is used. 
 * @author Werner Mayer
 */
class GuiExport SoFCHighlightColorAction : public SoAction
{
    SO_ACTION_HEADER(SoFCHighlightColorAction);

public:
    SoFCHighlightColorAction (const SoSFColor& col);
    ~SoFCHighlightColorAction();

    const SoSFColor& highlightColor;

    static void initClass();
    static void finish(void);

protected:
    virtual void beginTraversal(SoNode *node);

private:
    static void callDoAction(SoAction *action,SoNode *node);
};

/**
 * The SoFCDocumentAction class is used to inform an SoFCSelection node
 * when a document has been renamed. 
 * @author Werner Mayer
 */
class GuiExport SoFCDocumentAction : public SoAction
{
    SO_ACTION_HEADER(SoFCDocumentAction);

public:
    SoFCDocumentAction (const SoSFString& docName);
    ~SoFCDocumentAction();

    const SoSFString& documentName;

    static void initClass();
    static void finish(void);

protected:
    virtual void beginTraversal(SoNode *node);

private:
    static void callDoAction(SoAction *action,SoNode *node);
};

/**
 * The SoFCDocumentObjectAction class is used to get the name of the document,
 * object and component at a certain position of an SoFCSelection node.
 * @author Werner Mayer
 */
class GuiExport SoFCDocumentObjectAction : public SoAction
{
    SO_ACTION_HEADER(SoFCDocumentObjectAction);

public:
    SoFCDocumentObjectAction ();
    ~SoFCDocumentObjectAction();

    void setHandled();
    SbBool isHandled() const;

    static void initClass();
    static void finish(void);

protected:
    virtual void beginTraversal(SoNode *node);

private:
    static void callDoAction(SoAction *action,SoNode *node);

public:
    SbString documentName;
    SbString objectName;
    SbString componentName;

private:
    SbBool _handled;
};

} // namespace Gui


#endif // _SoFCSelectionAction_h 
