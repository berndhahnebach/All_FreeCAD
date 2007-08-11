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


#ifndef GUI_PYTHONCONSOLE_H
#define GUI_PYTHONCONSOLE_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include "PythonEditor.h"

#include <Base/PyObjectBase.h>

namespace Gui {

/**
 * This class implements an interactive Python interpreter.
 * @author Werner Mayer
 */
class GuiExport InteractiveInterpreter
{
public:
    InteractiveInterpreter();
    ~InteractiveInterpreter();

    bool push(const char*);
    int compileCommand(const char*) const;

private:
    bool runSource(const char*) const;
    PyObject* compile(const char*) const;
    void runCode(PyCodeObject*) const;
    void setPrompt();

private:
    struct InteractiveInterpreterP* d;
};

/**
 * This class implements the history for the Python console.
 * @author Werner Mayer
 */
class GuiExport ConsoleHistory
{
public:
    ConsoleHistory();
    ~ConsoleHistory();

    void first();
    bool more();
    bool next();
    bool prev();
    bool isEmpty() const;
    QString value() const;
    void append( const QString& );
    const QStringList& values() const;

private:
    QStringList _history;
    QStringList::ConstIterator it;
};

/**
 * Python text console with syntax highlighting.
 * @author Werner Mayer
 */
class PythonConsoleHighlighter;
class GuiExport PythonConsole : public TextEdit, public WindowParameter
{
    Q_OBJECT

public:
    PythonConsole(QWidget *parent = 0);
    ~PythonConsole();

    void OnChange( Base::Subject<const char*> &rCaller,const char* rcReason );
    void printStatement( const QString& cmd );

public Q_SLOTS:
    void onSaveHistoryAs();
    void onInsertFileName();
    void onCopyHistory();
    void onCopyCommand();

protected:
    void keyPressEvent  ( QKeyEvent         * e );
    void showEvent      ( QShowEvent        * e );
    void dropEvent      ( QDropEvent        * e );
    void dragEnterEvent ( QDragEnterEvent   * e );
    void dragMoveEvent  ( QDragMoveEvent    * e );

    void overrideCursor(const QString& txt);

    /** Pops up the context menu with some extensions */
    void contextMenuEvent ( QContextMenuEvent* e );
    bool canInsertFromMimeData ( const QMimeData * source ) const;
    QMimeData * createMimeDataFromSelection () const;
    void insertFromMimeData ( const QMimeData * source );

private:
    void runSource(const QString&);
    bool isComment(const QString&) const;
    void printPrompt(bool);
    void insertPythonOutput( const QString& );
    void insertPythonError ( const QString& );

private:
    struct PythonConsoleP* d;

    friend class PythonStdoutPy;
    friend class PythonStderrPy;

private:
    PythonConsoleHighlighter* pythonSyntax;
};

/**
 * Syntax highlighter for Python console.
 * @author Werner Mayer
 */
class GuiExport PythonConsoleHighlighter : public PythonSyntaxHighlighter
{
public:
    PythonConsoleHighlighter(QTextEdit* );
    ~PythonConsoleHighlighter();

    void highlightBlock ( const QString & text );

    void highlightOutput (bool b);
    void highlightError (bool b);

protected:
    void colorChanged( const QString& type, const QColor& col );

private:
    bool _output, _error;
    friend class PythonConsole;
};

} // namespace Gui

#endif // GUI_PYTHONCONSOLE_H
