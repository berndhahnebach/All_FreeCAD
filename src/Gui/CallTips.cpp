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

#include <Base/PyCXX/Objects.hxx>
#include <Base/PyObjectBase.h>
#include <App/PropertyContainerPy.h>
#include "CallTips.h"

using namespace Gui;

CallTipsList::CallTipsList(QTextEdit* parent)
  :  QListWidget(parent), textEdit(parent), cursorPos(0)
{
    // make the user assume that the widget is active
    QPalette pal = parent->palette();
    pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight));
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText));
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

void CallTipsList::keyboardSearch(const QString& wordPrefix)
{ 
    // first search for the item that matches perfectly
    for (int i=0; i<count(); ++i) {
        QString text = item(i)->text();
        if (text.startsWith(wordPrefix)) {
            setCurrentRow(i);
            return;
        }
    }

    // now do a case insensitive comparison
    for (int i=0; i<count(); ++i) {
        QString text = item(i)->text();
        if (text.startsWith(wordPrefix, Qt::CaseInsensitive)) {
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
        } else if (!word.isEmpty()){
            // If the word is empty we should not allow to do a search,
            // otherwise we may select the next item which is not okay in this
            // context. This might happen if e.g. Shift is pressed.
            keyboardSearch(word);
        }
    }
}

QString CallTipsList::extractContext(const QString& line) const
{
    int len = line.size();
    int index = len-1;
    for (int i=0; i<len; i++) {
        int pos = len-1-i;
        const char ch = line.at(pos).toAscii();
        if ((ch >= 48 && ch <= 57)  ||  // Numbers
            (ch >= 65 && ch <= 90)  ||  // Uppercase letters
            (ch >= 97 && ch <= 122) ||  // Lowercase letters
            (ch == '.') || (ch == '_'))
            index = pos;
        else 
            break;
    }

    return line.mid(index);
}

QMap<QString, CallTip> CallTipsList::extractTips(const QString& context) const
{
    QMap<QString, CallTip> tips;
    if (context.isEmpty())
        return tips;

    try {
        QStringList items = context.split('.');
        Py::Module module("__main__");
        Py::Dict dict = module.getDict();
        QString modname = items.front();
        items.pop_front();
        if (!dict.hasKey(std::string(modname.toAscii())))
            return tips; // unknown object

        // get the Python object we need
        Py::Object obj = dict.getItem(std::string(modname.toAscii()));
        while (!items.isEmpty()) {
            QByteArray name = items.front().toAscii();
            std::string attr = name.constData();
            items.pop_front();
            if (obj.hasAttr(attr))
                obj = obj.getAttr(attr);
            else
                return tips;
        }
        
        // Checks whether the type is a subclass of PyObjectBase because to get the doc string
        // of a member we must get it by its type instead of its instance otherwise we get the
        // wrong string, namely that of the type of the member. 
        // Note: 3rd party libraries may use their own type object classes so that we cannot 
        // reliably use Py::Type. To be on the safe side we should use Py::Object to assigh
        // the used type object to.
        //Py::Object type = obj.type();
        Py::Object type(PyObject_Type(obj.ptr()), true);
        Py::Object inst = obj; // the object instance 
        union PyType_Object typeobj = {&Base::PyObjectBase::Type};
        bool subclass = PyObject_IsSubclass(type.ptr(), typeobj.o);
        if (subclass)
            obj = type;
        Py::List list(PyObject_Dir(obj.ptr()), true);

        // If we derive from PropertyContainerPy we can search for the properties in the
        // C++ twin class.
        union PyType_Object proptypeobj = {&App::PropertyContainerPy::Type};
        if (PyObject_IsSubclass(type.ptr(), proptypeobj.o)) {
            App::PropertyContainerPy* cont = (App::PropertyContainerPy*)(inst.ptr());
            Py::List prop = cont->getPropertiesList();
            // These are the attributes of the instance itself which are NOT accessible by
            // its type object
            extractTipsFromObject(inst, prop, tips);
        }

        // These are the attributes from the type object
        extractTipsFromObject(obj, list, tips);
    } catch (Py::Exception& e) {
        // Just clear the Python exception
        e.clear();
    }

    return tips;
}

void CallTipsList::extractTipsFromObject(Py::Object& obj, Py::List& list, QMap<QString, CallTip>& tips) const
{
    try {
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            Py::String attrname(*it);
            Py::Object attr = obj.getAttr(attrname.as_string());

            CallTip tip;
            QString str = attrname.as_string().c_str();
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
    } catch (Py::Exception& e) {
        // Just clear the Python exception
        e.clear();
    }
}

void CallTipsList::showTips(const QString& line)
{
    QString context = extractContext(line);
    QMap<QString, CallTip> tips = extractTips(context);
    clear();
    for (QMap<QString, CallTip>::Iterator it = tips.begin(); it != tips.end(); ++it) {
        addItem(it.key());
        QListWidgetItem *item = this->item(this->count()-1);
        item->setData(Qt::ToolTipRole, QVariant(it.value().description));
        item->setData(Qt::UserRole, QVariant(it.value().parameter));
        switch (it.value().type)
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

void CallTipsList::showEvent(QShowEvent* e)
{
    QListWidget::showEvent(e);
    // install this object to filter timer events for the tooltip label
    qApp->installEventFilter(this);
}

void CallTipsList::hideEvent(QHideEvent* e)
{
    QListWidget::hideEvent(e);
    qApp->removeEventFilter(this);
}

/** 
 * Get all incoming events of the text edit and redirect some of them, like key up and
 * down, mouse press events, ... to the widget itself.
 */
bool CallTipsList::eventFilter(QObject * watched, QEvent * event)
{
    // This is a trick to avoid to hide the tooltip window after the defined time span
    // of 10 seconds. We just filter out all timer events to keep the label visible.
    if (watched->inherits("QLabel")) {
        QLabel* label = qobject_cast<QLabel*>(watched);
        // Ignore the timer events to prevent from being closed
        if (label->windowFlags() & Qt::ToolTip && event->type() == QEvent::Timer)
            return true;
    }
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
            } else if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Tab) {
                itemActivated(currentItem());
                return true;
            } else if (this->compKeys.indexOf(ke->key()) > -1) {
                itemActivated(currentItem());
                return false;
            } else if (ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_Control || 
                       ke->key() == Qt::Key_Meta || ke->key() == Qt::Key_Alt || 
                       ke->key() == Qt::Key_AltGr) {
                // filter these meta keys to avoid to call keyboardSearch()
                return true;
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
                    } else {
                        QToolTip::showText(p, QString());
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
