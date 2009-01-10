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

#include <Base/Exception.h>
#include <Base/Interpreter.h>

#include "Widgets.h"
#include "Application.h"
#include "Action.h"
#include "PrefWidgets.h"

using namespace Gui;

/**
 * Constructs an empty command view with parent \a parent.
 */
CommandIconView::CommandIconView ( QWidget * parent )
  : QListWidget(parent)
{
    connect(this, SIGNAL (currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
            this, SLOT (onSelectionChanged(QListWidgetItem *, QListWidgetItem *)) );
}

/**
 * Destroys the icon view and deletes all items.
 */
CommandIconView::~CommandIconView ()
{
}

/**
 * Stores the name of the selected commands for drag and drop. 
 */
void CommandIconView::startDrag ( Qt::DropActions supportedActions )
{
    QList<QListWidgetItem*> items = selectedItems();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    QPixmap pixmap;
    dataStream << items.count();
    for (QList<QListWidgetItem*>::ConstIterator it = items.begin(); it != items.end(); ++it) {
        if (it == items.begin())
            pixmap = qVariantValue<QPixmap>((*it)->data(Qt::UserRole));
        dataStream << (*it)->text();
    }

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(QString::fromAscii("text/x-action-items"), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
    drag->setPixmap(pixmap);
    drag->start(Qt::MoveAction);
}

/**
 * This slot is called when a new item becomes current. \a item is the new current item 
 * (or 0 if no item is now current). This slot emits the emitSelectionChanged()
 * signal for its part.
 */
void CommandIconView::onSelectionChanged(QListWidgetItem * item, QListWidgetItem *)
{
    if (item)
        emitSelectionChanged(item->toolTip());
}

// ------------------------------------------------------------------------------

/* TRANSLATOR Gui::AccelLineEdit */

/**
 * Constructs a line edit with no text.
 * The \a parent and \a name arguments are sent to the QLineEdit constructor.
 */
AccelLineEdit::AccelLineEdit ( QWidget * parent )
  : QLineEdit(parent)
{
    setText(tr("none"));
}

/**
 * Checks which keys are pressed and show it as text.
 */
void AccelLineEdit::keyPressEvent ( QKeyEvent * e)
{
    QString txt;
    setText(tr("none"));

    int key = e->key();
    Qt::KeyboardModifiers state = e->modifiers();

    if (key == Qt::Key_Control)
        return;
    else if (key == Qt::Key_Shift)
        return;
    else if (key == Qt::Key_Alt)
        return;
    else if (state == Qt::NoModifier && key == Qt::Key_Backspace)
        return; // clears the edit field

    switch(state)
    {
    case Qt::ControlModifier:
        {
            QKeySequence ks(Qt::CTRL+key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    case Qt::AltModifier:
        {
            QKeySequence ks(Qt::ALT+key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    case Qt::ShiftModifier:
        {
            QKeySequence ks(Qt::SHIFT+key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    case Qt::ControlModifier+Qt::AltModifier:
        {
            QKeySequence ks(Qt::CTRL+Qt::ALT+key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    case Qt::ControlModifier+Qt::ShiftModifier:
        {
            QKeySequence ks(Qt::CTRL+Qt::SHIFT+key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    case Qt::ShiftModifier+Qt::AltModifier:
        {
            QKeySequence ks(Qt::SHIFT+Qt::ALT+key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    case Qt::ControlModifier+Qt::AltModifier+Qt::ShiftModifier:
        {
            QKeySequence ks(Qt::CTRL+Qt::ALT+Qt::SHIFT+key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    default:
        {
            QKeySequence ks(key);
            txt += (QString)(ks);
            setText(txt);
        }   break;
    }
}

// ------------------------------------------------------------------------------

/* TRANSLATOR Gui::CheckListDialog */

/**
 *  Constructs a CheckListDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
CheckListDialog::CheckListDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
    ui.setupUi(this);
}

/**
 *  Destroys the object and frees any allocated resources
 */
CheckListDialog::~CheckListDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/**
 * Sets the items to the dialog's list view. By default all items are checkable..
 */
void CheckListDialog::setCheckableItems( const QStringList& items )
{
    for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it ) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
        item->setText(0, *it);
        item->setCheckState(0, Qt::Unchecked);
    }
}

/**
 * Sets the items to the dialog's list view. If the boolean type of a CheckListItem
 * is set to false the item is not checkable any more.
 */
void CheckListDialog::setCheckableItems( const QList<CheckListItem>& items )
{
    for ( QList<CheckListItem>::ConstIterator it = items.begin(); it != items.end(); ++it ) {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
        item->setText(0, (*it).first);
        item->setCheckState(0, ( (*it).second ? Qt::Checked : Qt::Unchecked));
    }
}

/**
 * Returns a list of the check items.
 */
QStringList CheckListDialog::getCheckedItems() const
{
    return checked;
}

/**
 * Collects all checked items to be able to return them by call \ref getCheckedItems().
 */
void CheckListDialog::accept ()
{
    QTreeWidgetItemIterator it(ui.treeWidget, QTreeWidgetItemIterator::Checked);
    while (*it) {
        checked.push_back((*it)->text(0));
        ++it;
    }

    QDialog::accept();
}

// ------------------------------------------------------------------------------

/**
 * Constructs a colored button called \a name with parent \a parent.
 */
ColorButton::ColorButton( QWidget* parent )
    : QPushButton( parent ), _allowChange(true), _drawFrame(true)
{
    _col = palette().color(QPalette::Active,QPalette::Midlight);
    connect( this, SIGNAL( clicked() ), SLOT( onChooseColor() ));
}

/**
 * Destroys the button.
 */
ColorButton::~ColorButton()
{
}

/** 
 * Sets the color \a c to the button. 
 */
void ColorButton::setColor( const QColor& c )
{
    _col = c;
    update();
}

/** 
 * Returns the current color of the button.
 */
QColor ColorButton::color() const
{
    return _col;
}

void ColorButton::setAllowChangeColor(bool ok)
{
    _allowChange = ok;
}

bool ColorButton::allowChangeColor() const
{
    return _allowChange;
}

void ColorButton::setDrawFrame(bool ok)
{
    _drawFrame = ok;
}

bool ColorButton::drawFrame() const
{
    return _drawFrame;
}

/**
 * Draws the button label.
 */
void ColorButton::paintEvent (QPaintEvent * e)
{
    // first paint the complete button
    QPushButton::paintEvent(e);

    // repaint the rectangle area
    QPalette::ColorGroup group = isEnabled() ? hasFocus() ? QPalette::Active : QPalette::Inactive : QPalette::Disabled;
    QColor pen = palette().color(group,QPalette::ButtonText);
    {
        QPainter paint(this);
        paint.setPen( pen );

        if (_drawFrame) {
            paint.setBrush(QBrush(_col));
            paint.drawRect(5, 5, width()-10, height()-10);
        } else {
            paint.fillRect(5, 5, width()-10, height()-10, QBrush(_col));
        }
    }

    // overpaint the rectangle to paint icon and text 
    QStyleOptionButton opt;
    opt.init(this);
    opt.text = text();
    opt.icon = icon();
    opt.iconSize = iconSize();

    QStylePainter p(this);
    p.drawControl(QStyle::CE_PushButtonLabel, opt);
}

/**
 * Opens a QColorDialog to set a new color.
 */
void ColorButton::onChooseColor()
{
    if (!_allowChange)
        return;
    QColor c = QColorDialog::getColor( _col, this );
    if (c.isValid()) {
        setColor( c );
        changed();
    }
}

// ------------------------------------------------------------------------------

UrlLabel::UrlLabel(QWidget * parent, Qt::WFlags f)
  : QLabel(parent, f)
{
    _url = QString::fromAscii("http://localhost");
    setToolTip(this->_url);
}

UrlLabel::~UrlLabel()
{
}

void UrlLabel::enterEvent ( QEvent * )
{
    setCursor(Qt::PointingHandCursor);
}

void UrlLabel::leaveEvent ( QEvent * )
{
    setCursor(Qt::ArrowCursor);
}

void UrlLabel::mouseReleaseEvent (QMouseEvent *)
{
    // The webbrowser Python module allows to start the system browser in an OS-independent way
    Base::PyGILStateLocker lock;
    PyObject* module = PyImport_ImportModule("webbrowser");
    if (module) {
        // get the methods dictionary and search for the 'open' method
        PyObject* dict = PyModule_GetDict(module);
        PyObject* func = PyDict_GetItemString(dict, "open");
        if (func) {
            PyObject* args = Py_BuildValue("(s)", (const char*)this->_url.toAscii());
            PyObject* result = PyEval_CallObject(func,args);
            // decrement the args and module reference
            Py_XDECREF(result);
            Py_DECREF(args);
            Py_DECREF(module);
        }
    }
}

QString UrlLabel::url() const
{
    return this->_url;
}

void UrlLabel::setUrl(const QString& u)
{
    this->_url = u;
    setToolTip(this->_url);
}

// --------------------------------------------------------------------

/* TRANSLATOR Gui::LabelButton */

/**
 * Constructs a file chooser called \a name with the parent \a parent.
 */
LabelButton::LabelButton (QWidget * parent)
  : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(1);

    label = new QLabel(this);
    label->setAutoFillBackground(true);
    layout->addWidget(label);

    button = new QPushButton(QLatin1String("..."), this);
    button->setFixedWidth(button->fontMetrics().width(QLatin1String(" ... ")));
    layout->addWidget(button);

    connect(button, SIGNAL(clicked()), this, SLOT(browse()));
}

LabelButton::~LabelButton()
{
}

QLabel *LabelButton::getLabel() const
{
    return label;
}

QPushButton *LabelButton::getButton() const
{
    return button;
}

QVariant LabelButton::value() const
{
    return _val;
}

void LabelButton::setValue(const QVariant& val)
{
    _val = val;
    showValue(_val);
    valueChanged(_val);
}

#include "moc_Widgets.cpp"
