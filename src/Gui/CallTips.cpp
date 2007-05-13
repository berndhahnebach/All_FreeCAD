/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#ifndef _PreComp_
#endif

#include "CallTips.h"
#include <Base/PyCXX/Objects.hxx>

using namespace Gui;

CallTipsList::CallTipsList(QTextEdit* parent)
  :  QListWidget(parent), textEdit(parent), cursorPos(0)
{
    // make the user assume that the widget is active
    QPalette pal = parent->palette();
    pal.setInactive( pal.active() );
    parent->setPalette( pal );

    connect(this, SIGNAL(itemActivated(QListWidgetItem *)), 
            this, SLOT(callTipItemActivated(QListWidgetItem *)));

    hideKeys.append(Qt::Key_Space);
    hideKeys.append(Qt::Key_Exclam);
    hideKeys.append(Qt::Key_QuoteDbl);
    hideKeys.append(Qt::Key_NumberSign);
    hideKeys.append(Qt::Key_Dollar);
    hideKeys.append(Qt::Key_Percent);
    hideKeys.append(Qt::Key_Ampersand);
    hideKeys.append(Qt::Key_Apostrophe);
    hideKeys.append(Qt::Key_Asterisk);
    hideKeys.append(Qt::Key_Plus);
    hideKeys.append(Qt::Key_Comma);
    hideKeys.append(Qt::Key_Minus);
    hideKeys.append(Qt::Key_Period);
    hideKeys.append(Qt::Key_Slash);
    hideKeys.append(Qt::Key_Colon);
    hideKeys.append(Qt::Key_Semicolon);
    hideKeys.append(Qt::Key_Less);
    hideKeys.append(Qt::Key_Equal);
    hideKeys.append(Qt::Key_Greater);
    hideKeys.append(Qt::Key_Question);
    hideKeys.append(Qt::Key_At);
    hideKeys.append(Qt::Key_Backslash);

    compKeys.append(Qt::Key_ParenLeft);
    compKeys.append(Qt::Key_ParenRight);
    compKeys.append(Qt::Key_BracketLeft);
    compKeys.append(Qt::Key_BracketRight);
    compKeys.append(Qt::Key_BraceLeft);
    compKeys.append(Qt::Key_BraceRight);
}

CallTipsList::~CallTipsList()
{
}

void CallTipsList::findCurrentWord(const QString& wordPrefix)
{ 
    for (int i=0; i<count(); ++i) {
        QString text = item(i)->text();
        if (text.startsWith(wordPrefix)) {
            setCurrentRow(i);
            return;
        }
    }

    setItemSelected(currentItem(), false);
}

void CallTipsList::validateCursor()
{
    QTextCursor cursor = textEdit->textCursor();
    int currentPos = cursor.position();
    if (currentPos < this->cursorPos) {
        hide();
    } else {
        cursor.setPosition(this->cursorPos);
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        QString word = cursor.selectedText();
        if (currentPos > this->cursorPos+word.length()) {
            hide();
        } else {
            keyboardSearch(word);
        }
    }
}

QMap<QString, CallTip> CallTipsList::extractTips(const QString& context) const
{
    QMap<QString, CallTip> tips;

    try {
        // search for the given object
        Py::Module module("__main__");
        Py::Dict dict = module.getDict();
        if (!dict.hasKey(std::string(context.toAscii())))
            return tips;
        Py::Object obj = dict.getItem(std::string(context.toAscii()));
        Py::List list(PyObject_Dir(obj.ptr()), true);
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            Py::String funcname(*it);
            Py::Object attr = obj.getAttr(funcname.as_string());

            CallTip tip;
            QString str = funcname.as_string().c_str();
            tip.name = str;

            if (attr.isCallable())
                tip.type = CallTip::Method;
            //else if (attr.hasAttr("__class__"))
            //    tip.type = CallTip::Class;
            else
                tip.type = CallTip::Atribute;

            if (str == "__doc__" && attr.isString()) {
                Py::Object help = attr;
                if (help.isString()) {
                    Py::String doc(help);
                    QString longdoc = doc.as_string().c_str();
                    int pos = longdoc.indexOf(QChar('\n'));
                    pos = qMin(pos, 70);
                    if (pos < 0) 
                        pos = qMin(longdoc.length()-1, 70);
                    tip.description = longdoc;
                    tip.parameter = longdoc.left(pos);
                }
            } else if (attr.hasAttr("__doc__")) {
                Py::Object help = attr.getAttr("__doc__");
                if (help.isString()) {
                    Py::String doc(help);
                    QString longdoc = doc.as_string().c_str();
                    int pos = longdoc.indexOf(QChar('\n'));
                    pos = qMin(pos, 70);
                    if (pos < 0) 
                        pos = qMin(longdoc.length()-1, 70);
                    tip.description = longdoc;
                    tip.parameter = longdoc.left(pos);
                }
            }
            tips[str] = tip;
        }
    } catch (const Py::Exception& ) {
        // Just do nothing
    }
/*
    // get the global dictionary
    PyObject *module, *dict;
    module = PyImport_AddModule("__main__");
    dict = PyModule_GetDict(module);

    // search for the given object
    module = PyDict_GetItemString(dict, context.toAscii());

        // it's a class object, no module
        PyObject* list = PyObject_Dir(module);
        if (list) {
            Py_ssize_t count = PyList_Size(list);
            for (Py_ssize_t i=0; i<count; ++i) {
                PyObject* key = PyList_GetItem(list, i);
                PyObject* attr = PyObject_GetAttr(module, key);
                if (attr) {
                    QString funcName = PyString_AsString(key);
                    if (!funcName.startsWith("__")) {
                        CallTip tip;
                        tip.name = funcName;
                        PyObject* doc = PyObject_GetAttrString(attr, "__doc__");
                        if (doc && PyString_Check(doc)) {
                            tip.longdoc = PyString_AsString(doc);
                            int pos = tip.longdoc.indexOf(QChar('\n'));
                            pos = qMin(pos, 70);
                            if (pos < 0) pos = qMin(tip.longdoc.length()-1, 70);
                            tip.tooltip = tip.longdoc.left(pos);
                            Py_DECREF(doc);
                        }
                    
                        tips[funcName] = tip;
                    }

                    Py_DECREF(attr);
                }
            }
            
            Py_DECREF(list);
        }

*/
#if 0
    // get the global dictionary
    PyObject *module, *dict;
    module = PyImport_AddModule("__main__");
    dict = PyModule_GetDict(module);

    // search for the given object
    module = PyDict_GetItemString(dict, context.toAscii());
    if (!module) return tips;
    if (PyModule_Check(module)){
        // get the dictionary of the module
        PyObject* dict = PyModule_GetDict(module);

        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(dict, &pos, &key, &value)) {
            // get all methods of the module
            QString funcName = PyString_AsString(key);
            if (!funcName.startsWith("__")) {
                CallTip tip;
                tip.name = funcName;
                // get the name of the method and its doc string, if available
                PyObject* doc = PyObject_GetAttrString(value, "__doc__");
                if (doc && PyString_Check(doc)) {
                    tip.longdoc = PyString_AsString(doc);
                    Py_DECREF(doc);
                }

                tips[funcName] = tip;
            }
        }
    } else {
        // it's a class object, no module
        PyObject* list = PyObject_Dir(module);
        if (list) {
            Py_ssize_t count = PyList_Size(list);
            for (Py_ssize_t i=0; i<count; ++i) {
                PyObject* key = PyList_GetItem(list, i);
                PyObject* attr = PyObject_GetAttr(module, key);
                if (attr) {
                    QString funcName = PyString_AsString(key);
                    if (!funcName.startsWith("__")) {
                        CallTip tip;
                        tip.name = funcName;
                        PyObject* doc = PyObject_GetAttrString(attr, "__doc__");
                        if (doc && PyString_Check(doc)) {
                            tip.longdoc = PyString_AsString(doc);
                            Py_DECREF(doc);
                        }
                    
                        tips[funcName] = tip;
                    }

                    Py_DECREF(attr);
                }
            }
            
            Py_DECREF(list);
        }
    }
#endif

    return tips;
}

void CallTipsList::showTips(const QString& context)
{
    QMap<QString, CallTip> tips = extractTips(context);
    clear();
    for (QMap<QString, CallTip>::Iterator it = tips.begin(); it != tips.end(); ++it) {
        addItem(it.key());
        QListWidgetItem *item = this->item(this->count()-1);
        item->setData(Qt::ToolTipRole, QVariant(it.data().description));
        item->setData(Qt::UserRole, QVariant(it.data().parameter));
        switch (it.data().type)
        {
        case CallTip::Class:
            {
                const char * class_xpm[] = {
                "12 12 2 1",
                ".	c None",
                "#	c #fa96fa",
                "............",
                "............",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "............",
                "............"};
                QPixmap px(class_xpm);
                item->setIcon(px);
            }   break;
        case CallTip::Method:
            {
                const char * meth_xpm[] = {
                "12 12 2 1",
                ".	c None",
                "#	c #fa96fa",
                "............",
                "............",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "............",
                "............"};
                QPixmap px(meth_xpm);
                item->setIcon(px);
            }   break;
        case CallTip::Atribute:
            {
                const char * attr_xpm[] = {
                "12 12 2 1",
                ".	c None",
                "#	c #96c8ff",
                "............",
                "............",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "..########..",
                "............",
                "............"};
                QPixmap px(attr_xpm);
                item->setIcon(px);
            }   break;
        default:
            break;
        }
    }

    if (count()==0)
        return; // nothing found

    // get the minimum width and height of the box
    int h = 0;
    int w = 0;
    for (int i = 0; i < count(); ++i) {
        QRect r = visualItemRect(item(i));
        w = qMax(w, r.width());
        h += r.height();
    }

    // Add an offset
    w += 2*frameWidth();
    h += 2*frameWidth();

    // get the start position of the word prefix
    QTextCursor cursor = textEdit->textCursor();
    this->cursorPos = cursor.position();
    QRect rect = textEdit->cursorRect(cursor);
    int posX = rect.x();
    int posY = rect.y();
    int boxH = h;
    int boxW = w;

    // Decide whether to show downstairs or upstairs
    if (posY > textEdit->viewport()->height()/2) {
        h = qMin(qMin(h,posY), 250);
        if (h < boxH)
            w += textEdit->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        setGeometry(posX,posY-h, w, h);
    } else {
        h = qMin(qMin(h,textEdit->viewport()->height()-fontMetrics().height()-posY), 250);
        if (h < boxH)
            w += textEdit->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        setGeometry(posX, posY+fontMetrics().height(), w, h);
    }
    
    setCurrentRow(0);
    show();
}

/** 
 * Get all incoming events of the text edit and redirect some of them, like key up and
 * down, mouse press events, ... to the widget itself.
 */
bool CallTipsList::eventFilter(QObject * watched, QEvent * event)
{
    if (isVisible() && watched == textEdit->viewport()) {
        if (event->type() == QEvent::MouseButtonPress)
            hide();
    } else if (isVisible() && watched == textEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* ke = (QKeyEvent*)event;
            if (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down) {
                keyPressEvent(ke);
                return true;
            } else if (ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown) {
                keyPressEvent(ke);
                return true;
            } else if (ke->key() == Qt::Key_Escape) {
                hide();
                return true;
            } else if (this->hideKeys.indexOf(ke->key()) > -1) {
                hide();
                return false;
            } else if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
                itemActivated(currentItem());
                return true;
            } else if (this->compKeys.indexOf(ke->key()) > -1) {
                itemActivated(currentItem());
                return false;
            }
        } else if (event->type() == QEvent::KeyRelease) {
            QKeyEvent* ke = (QKeyEvent*)event;
            if (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ||
                ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown) {
                QList<QListWidgetItem *> items = selectedItems();
                if (!items.isEmpty()) {
                    QPoint p(width(), 0);
                    QString text = items.front()->toolTip();
                    if (!text.isEmpty()){
                        QToolTip::showText(mapToGlobal(p), text);
                    }
                }
                return true;
            }
        } else if (event->type() == QEvent::FocusOut) {
            if (!hasFocus())
                hide();
        }
    }

    return QListWidget::eventFilter(watched, event);
}

void CallTipsList::callTipItemActivated(QListWidgetItem *item)
{
    hide();
    QString text = item->text();
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(this->cursorPos);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    cursor.insertText( text );
    textEdit->ensureCursorVisible();

    QRect rect = textEdit->cursorRect(cursor);
    int posX = rect.x();
    int posY = rect.y();

    QPoint p(posX, posY);
    p = textEdit->mapToGlobal(p);
    QToolTip::showText(p, item->data(Qt::UserRole).toString());
}

#include "moc_CallTips.cpp" 
