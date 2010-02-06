/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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


#ifndef GUI_EDITORVIEW_H
#define GUI_EDITORVIEW_H

#include "MDIView.h"
#include "Window.h"

class QTextEdit;

namespace Gui {

class LineMarkerP;
class LineMarker: public QWidget
{
    Q_OBJECT

public:
    LineMarker(QWidget* parent=0);
    virtual ~LineMarker();

    void setTextEdit(QTextEdit *edit);

protected:
    void paintEvent ( QPaintEvent * );

private:
    int m_debugLine;
    QRect debugRect;
    QPixmap debugMarker;
    QTextEdit *edit;
};

/**
 * A special view class which sends the messages from the application to
 * the editor and embeds it in a window.
 * @author Werner Mayer
 */
class EditorViewP;
class GuiExport EditorView : public MDIView, public WindowParameter
{
    Q_OBJECT

public:
    EditorView(QTextEdit* editor, QWidget* parent);
    ~EditorView();

    QTextEdit* getEditor() const;
    void OnChange(Base::Subject<const char*> &rCaller,const char* rcReason);

    const char *getName(void) const {return "EditorView";}
    void onUpdate(void){};

    bool onMsg(const char* pMsg,const char** ppReturn);
    bool onHasMsg(const char* pMsg) const;

    bool canClose(void);

    /** @name Standard actions of the editor */
    //@{
    bool open   (const QString &f);
    bool saveAs ();
    void cut    ();
    void copy   ();
    void paste  ();
    void undo   ();
    void redo   ();
    void run    ();
    void print  ();
    void printPdf();
    //@}

    QStringList undoActions() const;
    QStringList redoActions() const;
    QString fileName() const;

protected:
    void focusInEvent(QFocusEvent* e);

private Q_SLOTS:
    void checkTimestamp();
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void undoAvailable(bool);
    void redoAvailable(bool);

private:
    void setCurrentFileName(const QString &fileName);
    bool saveFile();

private:
    EditorViewP* d;
};

class PythonDebugger;
class GuiExport PythonEditorView : public EditorView
{
    Q_OBJECT

public:
    PythonEditorView(QTextEdit* editor, QWidget* parent);
    ~PythonEditorView();

public Q_SLOTS:
    void executeScript();
    void startDebug();
    void stopDebug();
    void nextStep();

private:
    // Actions
    QAction* execute;
    QAction* dbgStart;
    QAction* dbgStop;
    QAction* dbgNext;
    PythonDebugger* _dbg;
};

} // namespace Gui

#endif // GUI_EDITORVIEW_H
