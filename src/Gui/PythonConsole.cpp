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


#include "PreCompiled.h"

#include "PythonConsole.h"
#include "PythonConsolePy.h"
#include "CallTips.h"
#include "Application.h"
#include "Action.h"
#include "Command.h"
#include "DlgEditorImp.h"
#include "FileDialog.h"
#include "MainWindow.h"


#include <Base/Interpreter.h>
#include <Base/Exception.h>
#include <Base/PyCXX/Exception.hxx>

using namespace Gui;

namespace Gui {
struct PythonConsoleP
{
    enum CopyType {Normal, History, Command};
    CopyType type;
    PyObject *_stdoutPy, *_stderrPy, *_stdinPy;
    PyObject *_stdout, *_stderr, *_stdin;
    InteractiveInterpreter* interpreter;
    CallTipsList* callTipsList;
    ConsoleHistory history;
    QString output, error;
    QStringList statements;
    bool interactive;
    QMap<QString, QColor> colormap; // Color map
    PythonConsoleP()
    {
        type = Normal;
        interpreter = 0;
        colormap["Text"] = Qt::black;
        colormap["Bookmark"] = Qt::cyan;
        colormap["Breakpoint"] = Qt::red;
        colormap["Keyword"] = Qt::blue;
        colormap["Comment"] = QColor(0, 170, 0);
        colormap["Block comment"] = QColor(160, 160, 164);
        colormap["Number"] = Qt::blue;
        colormap["String"] = Qt::red;
        colormap["Character"] = Qt::red;
        colormap["Class name"] = QColor(255, 170, 0);
        colormap["Define name"] = QColor(255, 170, 0);
        colormap["Operator"] = QColor(160, 160, 164);
        colormap["Python output"] = QColor(170, 170, 127);
        colormap["Python error"] = Qt::red;
    }
};
struct InteractiveInterpreterP
{
    PyObject* interpreter;
    PyObject* sysmodule;
    QStringList buffer;
};
} // namespace Gui

InteractiveInterpreter::InteractiveInterpreter()
{
    // import code.py and create an instance of InteractiveInterpreter
    PyObject* module = PyImport_ImportModule("code");
    if (!module)
    throw Base::PyException();
    PyObject* func = PyObject_GetAttrString(module, "InteractiveInterpreter");
    PyObject* args = Py_BuildValue("()");
    d = new InteractiveInterpreterP;
    d->interpreter = PyEval_CallObject(func,args);
    Py_DECREF(args);
    Py_DECREF(func);
    Py_DECREF(module);

    setPrompt();
}

InteractiveInterpreter::~InteractiveInterpreter()
{
    Py_XDECREF(d->interpreter);
    Py_XDECREF(d->sysmodule);
    delete d;
}

/**
 * Set the ps1 and ps2 members of the sys module if not yet defined.
 */
void InteractiveInterpreter::setPrompt()
{
    // import code.py and create an instance of InteractiveInterpreter
    d->sysmodule = PyImport_ImportModule("sys");
    if (!PyObject_HasAttrString(d->sysmodule, "ps1"))
        PyObject_SetAttrString(d->sysmodule, "ps1", PyString_FromString(">>> "));
    if (!PyObject_HasAttrString(d->sysmodule, "ps2"))
        PyObject_SetAttrString(d->sysmodule, "ps2", PyString_FromString("... "));
}

/**
 * Compile a command and determine whether it is incomplete.
 * 
 * The source string may contain \n characters.
 * Return value / exceptions raised:
 * \item Return a code object if the command is complete and valid
 * \item Return None if the command is incomplete
 * \item Raise SyntaxError, ValueError or OverflowError if the command is a
 * syntax error (OverflowError and ValueError can be produced by
 * malformed literals).
 */
PyObject* InteractiveInterpreter::compile(const char* source) const
{
    PyObject* func = PyObject_GetAttrString(d->interpreter, "compile");
    PyObject* args = Py_BuildValue("(s)", source);
    PyObject* eval = PyEval_CallObject(func,args);  // must decref later

    Py_DECREF(args);
    Py_DECREF(func);

    if (eval){
        return eval;
    } else {
        // do not throw Base::PyException as this clears the error indicator
        throw Base::Exception();
    }

    // can never happen
    return 0;
}

/**
 * Compile a command and determine whether it is incomplete.
 * 
 * The source string may contain \n characters.
 * Return value:
 * \item Return  1 if the command is incomplete
 * \item Return  0 if the command is complete and valid
 * \item Return -1 if the command is a syntax error 
 * (OverflowError and ValueError can be produced by
 * malformed literals).
 */
int InteractiveInterpreter::compileCommand(const char* source) const
{
    PyObject* func = PyObject_GetAttrString(d->interpreter, "compile");
    PyObject* args = Py_BuildValue("(s)", source);
    PyObject* eval = PyEval_CallObject(func,args);  // must decref later

    Py_DECREF(args);
    Py_DECREF(func);

    int ret = 0;
    if (eval){
        if (PyObject_TypeCheck(Py_None, eval->ob_type))
            ret = 1; // incomplete
        else
            ret = 0; // complete
        Py_DECREF(eval);
    } else {
        ret = -1;    // invalid
    }

    return ret;
}

/**
 * Compile and run some source in the interpreter.
 *
 * One several things can happen:
 *
 * \item The input is incorrect; compile() raised an exception (SyntaxError or OverflowError).  
 * A syntax traceback will be printed by calling Python's PyErr_Print() method to the redirected stderr.
 *
 * \item The input is incomplete, and more input is required; compile() returned 'None'. 
 * Nothing happens.
 *
 * \item The input is complete; compile() returned a code object.  The code is executed by calling 
 * runCode() (which also handles run-time exceptions, except for SystemExit).
 * 
 * The return value is True if the input is incomplete, False in the other cases (unless
 * an exception is raised). The return value can be used to decide whether to use sys.ps1 
 * or sys.ps2 to prompt the next line.
 */
bool InteractiveInterpreter::runSource(const char* source) const
{
    PyObject* code;
    try {
        code = compile(source);
    } catch (const Base::Exception&) {
        // A system, overflow or value error was raised.
        // We clear the traceback info as this might be a longly
        // message we don't need.
        PyObject *errobj, *errdata, *errtraceback;
        PyErr_Fetch(&errobj, &errdata, &errtraceback);
        PyErr_Restore(errobj, errdata, 0);
        // print error message
        if (PyErr_Occurred()) PyErr_Print();
            return false;
    }

    // the command is incomplete
    if (PyObject_TypeCheck(Py_None, code->ob_type)) {
        Py_DECREF(code);
        return true;
    }

    // run the code and return false
    runCode((PyCodeObject*)code);
    return false;
}

/* Execute a code object.
 *
 * When an exception occurs,  a traceback is displayed.
 * All exceptions are caught except SystemExit, which is reraised.
 */
void InteractiveInterpreter::runCode(PyCodeObject* code) const
{
    PyObject *module, *dict, *presult;           /* "exec code in d, d" */
    module = PyImport_AddModule("__main__");     /* get module, init python */
    if (module == NULL) 
        throw Base::PyException();                 /* not incref'd */
    dict = PyModule_GetDict(module);             /* get dict namespace */
    if (dict == NULL) 
        throw Base::PyException();                 /* not incref'd */

    // It seems that the return value is always 'None' or Null
    presult = PyEval_EvalCode(code, dict, dict); /* run compiled bytecode */
    Py_XDECREF(code);                            /* decref the code object */
    if (!presult) {
        if (PyErr_ExceptionMatches(PyExc_SystemExit)) {
            // throw SystemExit exception
            throw Base::SystemExitException();
        }
        if ( PyErr_Occurred() )                    /* get latest python exception information */
            PyErr_Print();                           /* and print the error to the error output */
    } else {
        Py_DECREF(presult);
    }
}

/**
 * Store the line into the internal buffer and compile the total buffer.
 * In case it is a complete Python command the buffer is emptied.
 */
bool InteractiveInterpreter::push(const char* line)
{
    d->buffer.append(line);
    QString source = d->buffer.join("\n");
    try {
        // Source is already UTF-8, so we can use toAscii()
        bool more = runSource(source.toAscii());
        if (!more)
            d->buffer.clear();
        return more;
    } catch (const Base::SystemExitException&) {
        d->buffer.clear();
        throw;
    } catch (...) {
        // indication of unhandled exception
        d->buffer.clear();
        if (PyErr_Occurred())
            PyErr_Print();
        throw;
    }

    return false;
}

QStringList InteractiveInterpreter::getBuffer() const
{
    return d->buffer;
}

void InteractiveInterpreter::setBuffer(const QStringList& buf)
{
    d->buffer = buf;
}

void InteractiveInterpreter::clearBuffer()
{
    d->buffer.clear();
}

/* TRANSLATOR Gui::PythonConsole */

/**
 *  Constructs a PythonConsole which is a child of 'parent'. 
 */
PythonConsole::PythonConsole(QWidget *parent)
  : TextEdit(parent), WindowParameter( "Editor" )
{
    d = new PythonConsoleP();
    d->interactive = false;

    // create an instance of InteractiveInterpreter
    try { 
        d->interpreter = new InteractiveInterpreter();
    } catch (const Base::Exception& e) {
        setPlainText(e.what());
        setEnabled(false);
    }

    // use the console highlighter
    pythonSyntax = new PythonConsoleHighlighter(this);

    // create the window for call tips
    d->callTipsList = new CallTipsList(this);
    d->callTipsList->setFrameStyle(QFrame::Box|QFrame::Raised);
    d->callTipsList->setLineWidth(2);
    installEventFilter(d->callTipsList);
    viewport()->installEventFilter(d->callTipsList);
    d->callTipsList->setSelectionMode( QAbstractItemView::SingleSelection );
    d->callTipsList->hide();

#ifdef FC_OS_LINUX
    QFont serifFont( "Courier", 12, QFont::Normal );
#else
    QFont serifFont( "Courier", 10, QFont::Normal );
#endif
    setFont(serifFont);
    
    // set colors and font from settings
    ParameterGrp::handle hPrefGrp = getWindowParameter();
    hPrefGrp->Attach( this );
    hPrefGrp->NotifyAll();

    // disable undo/redo stuff
    setUndoRedoEnabled( false );
    setAcceptDrops( true );

    // try to override Python's stdout/err
    d->_stdoutPy = new PythonStdoutPy(this);
    d->_stderrPy = new PythonStderrPy(this);
    d->_stdinPy  = new PythonStdinPy (this);
    d->_stdout = PySys_GetObject("stdout");
    d->_stderr = PySys_GetObject("stderr");
    d->_stdin  = PySys_GetObject("stdin");
    PySys_SetObject("stdin", d->_stdinPy);

    const char* version  = PyString_AsString(PySys_GetObject("version"));
    const char* platform = PyString_AsString(PySys_GetObject("platform"));
    d->output = QString("Python %1 on %2\n"
    "Type 'help', 'copyright', 'credits' or 'license' for more information.").arg(version).arg(platform);
    printPrompt(false);
}

/** Destroys the object and frees any allocated resources */
PythonConsole::~PythonConsole()
{
    getWindowParameter()->Detach( this );
    delete pythonSyntax;
    Py_XDECREF(d->_stdoutPy);
    Py_XDECREF(d->_stderrPy);
    Py_XDECREF(d->_stdinPy);
    delete d;
}

/** Set new font and colors according to the paramerts. */  
void PythonConsole::OnChange( Base::Subject<const char*> &rCaller,const char* sReason )
{
    ParameterGrp::handle hPrefGrp = getWindowParameter();

    if (strcmp(sReason, "FontSize") == 0 || strcmp(sReason, "Font") == 0) {
#ifdef FC_OS_LINUX
        int fontSize = hPrefGrp->GetInt("FontSize", 15);
#else
        int fontSize = hPrefGrp->GetInt("FontSize", 10);
#endif
        QString fontFamily = hPrefGrp->GetASCII( "Font", "Courier" ).c_str();
        
        QFont font(fontFamily, fontSize);
        setFont(font);
        QFontMetrics metric(font);
        int width = metric.width("0000");
        setTabStopWidth(width);
    } else {
        QMap<QString, QColor>::ConstIterator it = d->colormap.find(sReason);
        if (it != d->colormap.end()) {
            QColor color = it.value();
            unsigned long col = (color.red() << 24) | (color.green() << 16) | (color.blue() << 8);
            col = hPrefGrp->GetUnsigned( sReason, col);
            color.setRgb((col>>24)&0xff, (col>>16)&0xff, (col>>8)&0xff);
            pythonSyntax->setColor( sReason, color );
        }
    }
}

/**
 * Checks the input of the console to make the correct indentations.
 * After a command is prompted completely the Python interpreter is started.
 */
void PythonConsole::keyPressEvent(QKeyEvent * e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        switch( e->key() ) 
        {
        case Qt::Key_Up:
            {
                // no modification, just history facility
                if (!d->history.isEmpty()) {
                    if (d->history.prev()) {
                        QString cmd = d->history.value();
                        overrideCursor(cmd);
                    }   return;
                }
            }   break;
        case Qt::Key_Down:
            {
                // no modification, just history facility
                if (!d->history.isEmpty()) {
                    if (d->history.next()) {
                        QString cmd = d->history.value();
                        overrideCursor(cmd);
                    }   return;
                }
            }   break;
        default:
            break;
        }
    }

    switch (e->key())
    {
    // running Python interpreter?
    case Qt::Key_Return:
    case Qt::Key_Enter:
        {
            // make sure to be at the end
            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
 
            // get the last paragraph's text
            QTextBlock block = cursor.block();
            QString line = block.text();

            // and skip the first 4 characters consisting of either ">>> " or "... "
            line = line.mid(4);

            // put statement to the history
            d->history.append(line);

            // evaluate and run the command
            runSource(line);
        }   break;
    case Qt::Key_Period:
        {
            QTextCursor cursor = textCursor();
            QTextBlock block = cursor.block();
            QString text = block.text();
            int length = cursor.position() - block.position();
            TextEdit::keyPressEvent(e);
            d->callTipsList->showTips(text.left(length));
        }   break;
    case Qt::Key_Home:
        {
            if (e->modifiers() & Qt::ControlModifier) {
                TextEdit::keyPressEvent(e);
            } else {
                QTextCursor::MoveMode mode = e->modifiers() & Qt::ShiftModifier
                     ? QTextCursor::KeepAnchor
                     : QTextCursor::MoveAnchor;
                QTextCursor cursor = textCursor();
                QTextBlock block = cursor.block();
                QString text = block.text();
                int cursorPos = block.position();
                if (text.startsWith(">>> ") || text.startsWith("... "))
                    cursorPos += 4;
                cursor.setPosition(cursorPos, mode);
                setTextCursor(cursor);
                ensureCursorVisible();
            }
        }   break;
    default: 
        {
            TextEdit::keyPressEvent(e);

            // This can't be done in CallTipsList::eventFilter() because we must first perform
            // the event and afterwards update the list widget
            if (d->callTipsList->isVisible()) {
                d->callTipsList->validateCursor();
            }
        }   break;
    }  
}

/**
 * Insert an output message to the console. This message comes from
 * the Python interpreter and is redirected from sys.stdout.
 */
void PythonConsole::insertPythonOutput( const QString& msg )
{
    d->output += msg;
}

/**
 * Insert an error message to the console. This message comes from
 * the Python interpreter and is redirected from sys.stderr.
 */
void PythonConsole::insertPythonError ( const QString& err )
{
    d->error += err;
}

/** Prints the ps1 prompt (>>> ) for complete and ps2 prompt (... ) for
 * incomplete commands to the console window. 
 */ 
void PythonConsole::printPrompt(bool incomplete)
{
    // write normal messages
    if (!d->output.isEmpty()) {
        pythonSyntax->highlightOutput(true);
        append(d->output);
        d->output = QString::null;
        pythonSyntax->highlightOutput(false);
    }

    // write error messages
    if (!d->error.isEmpty()) {
        pythonSyntax->highlightError(true);
        append(d->error);
        d->error = QString::null;
        pythonSyntax->highlightError(false);
    }

    // Append the prompt string 
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::End);
    QTextBlock block = cursor.block();

    // Python's print command appends a trailing '\n' to the system output.
    // In this case, however, we should not add a new text block. We force
    // the current block to be normal text (user state = 0) to be highlighted 
    // correctly and append the '>>> ' or '... ' to this block.
    if (block.length() > 1)
        cursor.insertBlock(cursor.blockFormat(), cursor.charFormat());
    else
        block.setUserState(0);

    incomplete ? cursor.insertText("... ") : cursor.insertText(">>> ");
    cursor.endEditBlock();

    // move cursor to the end
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

/**
 * Builds up the Python command and pass it to the interpreter.
 */
void PythonConsole::runSource(const QString& line)
{
    bool incomplete = false;
    PySys_SetObject("stdout", d->_stdoutPy);
    PySys_SetObject("stderr", d->_stderrPy);
    d->interactive = true;
    
    try {
        // launch the command now
        incomplete = d->interpreter->push(line.toUtf8());
        setFocus(); // if focus was lost
    } catch (const Base::SystemExitException&) {
        int ret = QMessageBox::question(this, tr("System exit"), tr("The application is still running.\nDo you want to exit without saving your data?"),
        QMessageBox::Yes, QMessageBox::No|QMessageBox::Escape|QMessageBox::Default);
        if (ret == QMessageBox::Yes) {
            Base::Interpreter().systemExit();
        } else {
            PyErr_Clear();
        }
    } catch (const Py::Exception&) {
        QMessageBox::critical(this, tr("Python console"), tr("Unhandled PyCXX exception."));
    } catch (const Base::Exception&) {
        QMessageBox::critical(this, tr("Python console"), tr("Unhandled FreeCAD exception."));
    } catch (...) {
        QMessageBox::critical(this, tr("Python console"), tr("Unhandled unknown C++ exception."));
    }

    PySys_SetObject("stdout", d->_stdout);
    PySys_SetObject("stderr", d->_stderr);
    printPrompt(incomplete);
    d->interactive = false;
    for (QStringList::Iterator it = d->statements.begin(); it != d->statements.end(); ++it)
        printStatement(*it);
    d->statements.clear();
}

bool PythonConsole::isComment(const QString& source) const
{
    if (source.isEmpty())
        return false;
    int i=0;
    while (i < source.length()) {
        QChar ch = source.at(i++);
        if (ch.isSpace())
            continue;
        if (ch == '#')
            return true;
    }

    return false;
}

/**
 * Prints the Python statement cmd to the console.
 * @note The statement gets only printed and added to the history but not invoked.
 */
void PythonConsole::printStatement( const QString& cmd )
{
    // If we are in interactive mode we have to wait until the command is finished,
    // afterwards we can print the statements.
    if (d->interactive) {
        d->statements << cmd;
        return;
    }

    QTextCursor cursor = textCursor();
    QStringList statements = cmd.split("\n");
    for (QStringList::Iterator it = statements.begin(); it != statements.end(); ++it) {
        // go to the end before inserting new text 
        cursor.movePosition(QTextCursor::End);
        cursor.insertText( *it );
        d->history.append( *it );
        printPrompt(false);
    }
}

/**
 * Shows the Python window and sets the focus to set text cursor.
 */
void PythonConsole::showEvent ( QShowEvent * e )
{
    TextEdit::showEvent( e );
    // set also the text cursor to the edit field
    setFocus();
}

/**
 * Drops the event \a e and writes the right Python command.
 */
void PythonConsole::dropEvent ( QDropEvent * e )
{
    const QMimeData* mimeData = e->mimeData();
    if (mimeData->hasFormat("text/x-action-items")) {
        QByteArray itemData = mimeData->data("text/x-action-items");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        int ctActions; dataStream >> ctActions;
        for (int i=0; i<ctActions; i++) {
            QString action;
            dataStream >> action;
            printStatement( QString("Gui.runCommand(\"%1\")").arg(action));
        }

        e->setDropAction(Qt::CopyAction);
        e->accept();
    } else {
        e->ignore();
    }
}

/** Dragging of action objects is allowed. */ 
void PythonConsole::dragMoveEvent( QDragMoveEvent *e )
{
    const QMimeData* mimeData = e->mimeData();
    if (mimeData->hasFormat("text/x-action-items"))
        e->accept();
    else
        e->ignore();
}

/** Dragging of action objects is allowed. */ 
void PythonConsole::dragEnterEvent ( QDragEnterEvent * e )
{
    const QMimeData* mimeData = e->mimeData();
    if (mimeData->hasFormat("text/x-action-items"))
        e->accept();
    else
        e->ignore();
}

bool PythonConsole::canInsertFromMimeData ( const QMimeData * source ) const
{
    return source->hasText();
}

QMimeData * PythonConsole::createMimeDataFromSelection () const
{
    QMimeData* mime = new QMimeData();
    
    switch (d->type) {
        case PythonConsoleP::Normal:
            {
                const QTextDocumentFragment fragment(textCursor());
                mime->setText(fragment.toPlainText());
            }   break;
        case PythonConsoleP::Command:
            {
                QTextCursor cursor = textCursor();
                int s = cursor.selectionStart();
                int e = cursor.selectionEnd();
                QTextBlock b;
                QStringList lines;
                for (b = document()->begin(); b.isValid(); b = b.next()) {
                    int pos = b.position();
                    if ( pos >= s && pos <= e ) {
                        if (b.userState() > -1 && b.userState() < pythonSyntax->maximumUserState()) {
                            QString line = b.text();
                            // and skip the first 4 characters consisting of either ">>> " or "... "
                            line = line.mid(4);
                            lines << line;
                        }
                    }
                }

                QString text = lines.join("\n");
                mime->setText(text);
            }   break;
        case PythonConsoleP::History:
            {
                const QStringList& hist = d->history.values();
                QString text = hist.join("\n");
                mime->setText(text);
            }   break;
    }

    return mime;
}

/**
 * Decodes the clipboard plain text and insert line by line to this text edit and
 * simulate a "return" event to let decide keyPressEvent() how to continue. This
 * is to run the Python interpreter if needed.
 */
void PythonConsole::insertFromMimeData ( const QMimeData * source )
{
    if (!source)
        return;

    // When inserting a big text block we must break it down into several command
    // blocks instead of processing the text block as a whole or each single line.
    // If we processed the complete block as a whole only the first valid Python
    // command would be executed and the rest would be ignored. However, if we 
    // processed each line separately the interpreter might be confused that a block 
    // is complete but it might be not. This is for instance, if a class or method 
    // definition contains several empty lines which leads to error messages (almost
    // indentation errors) later on.
    QString text = source->text();
    if (!text.isNull()) {
#if defined (Q_OS_LINUX) 
        // Need to convert CRLF to LF
        text.replace( "\r\n", "\n" );
#elif defined(Q_OS_WIN32)
        // Need to convert CRLF to LF
        text.replace( "\r\n", "\n" );
#elif defined(Q_OS_MAC)
        //need to convert CR to LF
        text.replace( '\r', '\n' );
#endif
        QStringList lines = text.split('\n');
        QTextCursor cursor = textCursor();
        QStringList buffer = d->interpreter->getBuffer();
        d->interpreter->clearBuffer();
        int countNewlines = lines.count() - 1, i = 0;
        for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it, ++i) {
            cursor.insertText( *it );
            if (i < countNewlines) {
                // put statement to the history
                d->history.append(*it);

                buffer.append(*it);
                int ret = d->interpreter->compileCommand(buffer.join("\n").toUtf8());
                if (ret == 1) { // incomplete
                    printPrompt(true);
                } else if (ret == 0) { // complete
                    // check if the following lines belong to the previous block
                    int k=i+1;
                    QString nextline = lines[k];
                    while ((nextline.isEmpty() || isComment(nextline)) && k < countNewlines) {
                        k++;
                        nextline = lines[k];
                    }
                    
                    int ret = d->interpreter->compileCommand(nextline.toUtf8());

                    // If the line is valid, i.e. complete or incomplete the previous block
                    // is finished
                    if (ret == -1) {
                        // the command is not finished yet
                        printPrompt(true);
                    } else {
                        runSource(buffer.join("\n"));
                        buffer.clear();
                    }
                } else { // invalid
                    runSource(buffer.join("\n"));
                    buffer.clear();
                    break;
                }
            } else {
                buffer.append(*it);
            }
        }

        d->interpreter->setBuffer(buffer);
    }

    ensureCursorVisible();
}

/**
 * Overwrites the text of the cursor.
 */
void PythonConsole::overrideCursor(const QString& txt)
{
    // Go to the last line and the fourth position, right after the prompt
    QTextCursor cursor = textCursor();
    QTextBlock block = cursor.block();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 4);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, block.text().length());
    cursor.removeSelectedText();
    cursor.insertText(txt);
    // move cursor to the end
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void PythonConsole::contextMenuEvent ( QContextMenuEvent * e )
{
    QMenu* menu = new QMenu(this);
    QAction *a;

    a = menu->addAction(QTextEdit::tr("&Copy"), this, SLOT(copy()), Qt::CTRL+Qt::Key_C);
    a->setEnabled(textCursor().hasSelection());

    a = menu->addAction(tr("&Copy command"), this, SLOT(onCopyCommand()));
    a->setEnabled(textCursor().hasSelection());

    a = menu->addAction(tr("&Copy history"), this, SLOT(onCopyHistory()));
    a->setEnabled(!d->history.isEmpty());

    a = menu->addAction( tr("Save history as..."), this, SLOT(onSaveHistoryAs()));
    a->setEnabled(!d->history.isEmpty());

    menu->addSeparator();
    a = menu->addAction(QTextEdit::tr("&Paste"), this, SLOT(paste()), Qt::CTRL+Qt::Key_V);
    const QMimeData *md = QApplication::clipboard()->mimeData();
    a->setEnabled(md && canInsertFromMimeData(md));

    a = menu->addAction(QTextEdit::tr("Select All"), this, SLOT(selectAll()), Qt::CTRL+Qt::Key_A);
    a->setEnabled(!document()->isEmpty());

    menu->addSeparator();
    menu->addAction( tr("Insert file name..."), this, SLOT(onInsertFileName()));

    menu->exec(e->globalPos());
    delete menu;
}

void PythonConsole::onSaveHistoryAs()
{
    QString cMacroPath = getDefaultParameter()->GetGroup( "Macro" )->GetASCII("MacroPath",App::GetApplication().GetHomePath()).c_str();
    QString fn = FileDialog::getSaveFileName(this, tr("Save History"), cMacroPath,"Macro Files (*.FCMacro *.py)");
    if (!fn.isEmpty()) {
        int dot = fn.indexOf('.');
        if (dot != -1) {
            QFile f(fn);
            if (f.open(QIODevice::WriteOnly)) {
                QTextStream t (&f);
                const QStringList& hist = d->history.values();
                for (QStringList::ConstIterator it = hist.begin(); it != hist.end(); ++it)
                    t << *it << "\n";
                f.close();
            }
        }
    }
}

void PythonConsole::onInsertFileName()
{
    QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), tr("Insert file name"), QString::null, tr("All Files (*.*)") );
    if ( fn.isEmpty() )
        return;
    insertPlainText(fn);
}

/**
 * Copy the history of the console into the clipboard.
 */
void PythonConsole::onCopyHistory()
{
    if (d->history.isEmpty())
        return;
    d->type = PythonConsoleP::History;
    QMimeData *data = createMimeDataFromSelection();
    QApplication::clipboard()->setMimeData(data);
    d->type = PythonConsoleP::Normal;
}

/**
 * Copy the selected commands into the clipboard. This is a subset of the history.
 */
void PythonConsole::onCopyCommand()
{
    d->type = PythonConsoleP::Command;
    copy();
    d->type = PythonConsoleP::Normal;
}

// ---------------------------------------------------------------------

PythonConsoleHighlighter::PythonConsoleHighlighter(QTextEdit* edit)
  : PythonSyntaxHighlighter(edit),_output(false), _error(false)
{
}

PythonConsoleHighlighter::~PythonConsoleHighlighter()
{
}

void PythonConsoleHighlighter::highlightBlock(const QString& text)
{
    const int ErrorOutput   = 20;
    const int MessageOutput = 21;

    // Restore user state to re-highlight the blocks in the appropriate format
    int stateOfPara = currentBlockState();
    if (previousBlockState() < 0)
        stateOfPara = MessageOutput; // the very first line is an output

    // for new lines check whether that's an output
    if (stateOfPara == -1) {
        if ( _output )
            stateOfPara = MessageOutput;
        else if ( _error )
            stateOfPara = ErrorOutput;
    }
    setCurrentBlockState(stateOfPara);

    switch ( stateOfPara )
    {
    case ErrorOutput:
        {
            // Error output
            QTextCharFormat errorFormat;
            errorFormat.setForeground(color("Python error"));
            errorFormat.setFontItalic(true);
            setFormat( 0, text.length(), errorFormat);
        }   break;
    case MessageOutput:
        {
            // Normal output
            QTextCharFormat outputFormat;
            outputFormat.setForeground(color("Python output"));
            setFormat( 0, text.length(), outputFormat);
        }   break;
    default:
        {
            PythonSyntaxHighlighter::highlightBlock(text);
        }   break;
    }
}

void PythonConsoleHighlighter::colorChanged( const QString& type, const QColor& col )
{
}

/**
 * If \a b is set to true the following input to the editor is highlighted as normal output.
 */
void PythonConsoleHighlighter::highlightOutput (bool b)
{
    _output = b;
}

/**
 * If \a b is set to true the following input to the editor is highlighted as error.
 */
void PythonConsoleHighlighter::highlightError (bool b)
{
    _error = b;
}

// ---------------------------------------------------------------------

ConsoleHistory::ConsoleHistory()
{
    it = _history.end();
}

ConsoleHistory::~ConsoleHistory()
{
}

void ConsoleHistory::first()
{
    it = _history.begin();
}

bool ConsoleHistory::more()
{
    return (it != _history.end());
}

bool ConsoleHistory::next() 
{
    if (it != _history.end()) {
        for (++it; it != _history.end(); ++it) {
            if (!it->isEmpty())
                break;
        }
        return true;
    }

    return false;
}

bool ConsoleHistory::prev() 
{
    if (it != _history.begin()) {
        for (--it; it != _history.begin(); --it) {
            if (!it->isEmpty())
                break;
        }
        return true;
    }

    return false;
}

bool ConsoleHistory::isEmpty() const
{
    return _history.isEmpty();
}

QString ConsoleHistory::value() const
{
    if ( it != _history.end() )
        return *it;
    else
        return QString::null;
}

void ConsoleHistory::append( const QString& item )
{
    _history.append( item );
    it = _history.end();
}

const QStringList& ConsoleHistory::values() const
{
    return this->_history;
}

#include "moc_PythonConsole.cpp"
