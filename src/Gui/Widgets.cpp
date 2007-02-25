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

#include "Widgets.h"
#include "Application.h"
#include "Action.h"
#include "PrefWidgets.h"

using namespace Gui;

/* TRANSLATOR Gui::CheckMessageBox */

/**
 * Constructs a message box with no text and a button with the label "OK".
 * If \a parent is 0, the message box becomes an application-global modal dialog box. 
 * If \a parent is a widget, the message box becomes modal relative to \a parent.
 * The \a parent and \a name arguments are passed to the QMessageBox constructor.
 */
CheckMessageBox::CheckMessageBox(QWidget * parent, const char * name)
  : QMessageBox(parent, name), checkBox(0L), layout(0L)
{
}

/**
 * Constructs a message box with a \a caption, a \a text, an \a icon, and up to three buttons.
 */
CheckMessageBox::CheckMessageBox(const QString & caption, const QString & text, Icon icon, int button0, int button1,
                           int button2, QWidget * parent, const char * name, bool modal, Qt::WFlags f)
    : QMessageBox(caption, text, icon, button0, button1, button2, parent, name, modal, f),
    checkBox(0L), layout(0L)
{
  QString msg = tr("Never show this message again.");
  QString entry = text;
  QString cn = parent ? parent->className() : "Unknown";
  QString path  = QString("General/NeverShowDialog/%1").arg( cn );


  checkBox = new Gui::PrefCheckBox(this);
  checkBox->setText( msg );
  checkBox->setEntryName( entry.latin1() );
  checkBox->setParamGrpPath( path.latin1() );
  checkBox->onRestore();

  // if checked throw an exception to avoid to show
  // (exception is caught in the calling static function)
  if (checkBox->isChecked())
  {
    delete checkBox;
    checkBox = 0L;

    throw Base::Exception("Do not show dialog.");
  }

  layout = new Q3GridLayout(this, 1, 1, 50);
  layout->addWidget(checkBox, 0, 0);
}

/**
 * Destroys the message box.
 */
CheckMessageBox::~CheckMessageBox()
{
  if (checkBox)
    checkBox->onSave();
  delete checkBox;
  delete layout;
}

/**
 * Opens an information message box with the caption \a caption and the text \a text. 
 * The dialog may have up to three buttons. 
 */
int CheckMessageBox::information(QWidget * parent, const QString & caption, const QString & text,
                            int button0, int button1, int button2)
{
  CheckMessageBox dlg(caption, text, Information, button0, button1, button2, parent,  "CheckMessageBox");
  return dlg.exec();
}

/**
 * Displays an information message box with caption \a caption, text \a text and one, two or 
 * three buttons. Returns the index of the button that was clicked (0, 1 or 2).
 */
int CheckMessageBox::information( QWidget * parent, const QString & caption, const QString & text,
                             const QString & button0Text, const QString & button1Text,
                             const QString & button2Text, int defaultButtonNumber, int escapeButtonNumber )
{
  return _msg( Information, parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber,
               escapeButtonNumber );
}

/**
 * Opens a question message box with the caption \a caption and the text \a text. 
 * The dialog may have up to three buttons. 
 */
int CheckMessageBox::question(QWidget * parent, const QString & caption, const QString & text,
                            int button0, int button1, int button2)
{
  CheckMessageBox dlg(caption, text, Question, button0, button1, button2, parent,  "CheckMessageBox");
  return dlg.exec();
}

/**
 * Displays a question message box with caption \a caption, text \a text and one, two or 
 * three buttons. Returns the index of the button that was clicked (0, 1 or 2).
 */
int CheckMessageBox::question( QWidget * parent, const QString & caption, const QString & text,
                             const QString & button0Text, const QString & button1Text,
                             const QString & button2Text, int defaultButtonNumber, int escapeButtonNumber )
{
  return _msg( Question, parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber,
               escapeButtonNumber );
}

/**
 * Opens a warning message box with the caption \a caption and the text \a text. 
 * The dialog may have up to three buttons. 
 */
int CheckMessageBox::warning ( QWidget * parent, const QString & caption, const QString & text,
                          int button0, int button1, int button2 )
{
  CheckMessageBox dlg(caption, text, Warning, button0, button1, button2, parent,  "CheckMessageBox");
  return dlg.exec();
}

/**
 * Displays a warning message box with caption \a caption, text \a text and one, two or 
 * three buttons. Returns the index of the button that was clicked (0, 1 or 2).
 */
int CheckMessageBox::warning ( QWidget * parent, const QString & caption, const QString & text,
                          const QString & button0Text, const QString & button1Text,
                          const QString & button2Text, int defaultButtonNumber, int escapeButtonNumber)
{
  return _msg( Warning, parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber,
               escapeButtonNumber );
}

/**
 * Opens a critical message box with the caption \a caption and the text \a text. 
 * The dialog may have up to three buttons. 
 */
int CheckMessageBox::critical ( QWidget * parent, const QString & caption, const QString & text,
                           int button0, int button1, int button2 )
{
  CheckMessageBox dlg(caption, text, Critical, button0, button1, button2, parent,  "CheckMessageBox");
  return dlg.exec();
}

/**
 * Displays a critical message box with caption \a caption, text \a text and one, two or 
 * three buttons. Returns the index of the button that was clicked (0, 1 or 2).
 */
int CheckMessageBox::critical ( QWidget * parent, const QString & caption, const QString & text,
                           const QString & button0Text, const QString & button1Text,
                           const QString & button2Text, int defaultButtonNumber, int escapeButtonNumber)
{
  return _msg( Critical, parent, caption, text, button0Text, button1Text, button2Text, defaultButtonNumber,
               escapeButtonNumber );
}

int CheckMessageBox::_msg( Icon icon, QWidget * parent, const QString & caption, const QString & text,
                           const QString & button0Text, const QString & button1Text, const QString & button2Text,
                           int defaultButtonNumber, int escapeButtonNumber )
{
  int b[3];
  b[0] = 1;
  b[1] = !button1Text.isEmpty() ? 2 : 0;
  b[2] = !button2Text.isEmpty() ? 3 : 0;

  int i;
  for( i=0; i<3; i++ )
  {
    if ( b[i] > 0  && defaultButtonNumber == i )
      b[i] += QMessageBox::Default;
    if ( b[i] > 0  && escapeButtonNumber == i )
      b[i] += QMessageBox::Escape;
  }

  CheckMessageBox* dlg=0L;
  try{
    dlg=new CheckMessageBox( caption, text, icon, b[0], b[1], b[2], parent, "CheckMessageBox");
    if ( !button0Text.isEmpty() )
      dlg->setButtonText(0, button0Text);
    if ( !button1Text.isEmpty() )
      dlg->setButtonText(1, button1Text);
    if ( !button2Text.isEmpty() )
      dlg->setButtonText(2, button2Text);
    int res = dlg->exec();
    delete dlg;
    return (res-1);
  } catch (...)
  {
    return -1;
  }
}

// ------------------------------------------------------------------------------

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
  mimeData->setData("text/x-action-items", itemData);

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

  if ( key == Qt::Key_Control )
    return;
  else if ( key == Qt::Key_Shift )
    return;
  else if ( key == Qt::Key_Alt )
    return;
  else if ( state == Qt::NoModifier && key == Qt::Key_Backspace )
    return; // clears the edit field

  switch( state )
  {
  case Qt::ControlModifier:
    {
      QKeySequence key(Qt::CTRL+key);
      txt += (QString)(key);
      setText(txt);
    } break;
  case Qt::AltModifier:
    {
      QKeySequence key(Qt::ALT+key);
      txt += (QString)(key);
      setText(txt);
    } break;
  case Qt::ShiftModifier:
    {
      QKeySequence key(Qt::SHIFT+key);
      txt += (QString)(key);
      setText(txt);
    } break;
  case Qt::ControlModifier+Qt::AltModifier:
    {
      QKeySequence key(Qt::CTRL+Qt::ALT+key);
      txt += (QString)(key);
      setText(txt);
    } break;
  case Qt::ControlModifier+Qt::ShiftModifier:
    {
      QKeySequence key(Qt::CTRL+Qt::SHIFT+key);
      txt += (QString)(key);
      setText(txt);
    } break;
  case Qt::ShiftModifier+Qt::AltModifier:
    {
      QKeySequence key(Qt::SHIFT+Qt::ALT+key);
      txt += (QString)(key);
      setText(txt);
    } break;
  case Qt::ControlModifier+Qt::AltModifier+Qt::ShiftModifier:
    {
      QKeySequence key(Qt::CTRL+Qt::ALT+Qt::SHIFT+key);
      txt += (QString)(key);
      setText(txt);
    } break;
  default:
    {
      QKeySequence key(key);
      txt += (QString)(key);
      setText(txt);
    } break;
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
CheckListDialog::CheckListDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  if ( !name )
    setName( "CheckListDialog" );
  resize( 402, 270 );
  setProperty( "sizeGripEnabled", QVariant( (int) 0 ) );
  CheckListDialogLayout = new Q3GridLayout( this );
  CheckListDialogLayout->setSpacing( 6 );
  CheckListDialogLayout->setMargin( 11 );

  Layout2 = new Q3HBoxLayout;
  Layout2->setSpacing( 6 );
  Layout2->setMargin( 0 );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setProperty( "text", tr( "&OK" ) );
  buttonOk->setProperty( "autoDefault", QVariant((int) 0 ) );
  buttonOk->setProperty( "default", QVariant( (int) 0 ) );
  Layout2->addWidget( buttonOk );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout2->addItem( spacer );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setProperty( "text", tr( "&Cancel" ) );
  buttonCancel->setProperty( "autoDefault", QVariant( (int) 0 ) );
  Layout2->addWidget( buttonCancel );

  CheckListDialogLayout->addLayout( Layout2, 1, 0 );

  GroupBox1 = new Q3GroupBox( this, "GroupBox1" );
  GroupBox1->setColumnLayout(0, Qt::Vertical );
  GroupBox1->layout()->setSpacing( 0 );
  GroupBox1->layout()->setMargin( 0 );
  GroupBox1Layout = new Q3GridLayout( GroupBox1->layout() );
  GroupBox1Layout->setAlignment( Qt::AlignTop );
  GroupBox1Layout->setSpacing( 6 );
  GroupBox1Layout->setMargin( 11 );

  ListView = new Q3ListView( GroupBox1, "ListView" );
  ListView->addColumn( "Items" );
  ListView->setRootIsDecorated( TRUE );

  GroupBox1Layout->addWidget( ListView, 0, 0 );

  CheckListDialogLayout->addWidget( GroupBox1, 0, 0 );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
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
  for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
  {
    Q3CheckListItem* item = new Q3CheckListItem( ListView, *it, Q3CheckListItem::CheckBox );
    item->setEnabled( false );
  }
}

/**
 * Sets the items to the dialog's list view. If the boolean type of a CheckListItem
 * is set to false the item is not checkable any more.
 */
void CheckListDialog::setCheckableItems( const Q3ValueList<CheckListItem>& items )
{
  for ( Q3ValueList<CheckListItem>::ConstIterator it = items.begin(); it!=items.end(); ++it )
  {
    Q3CheckListItem* item = new Q3CheckListItem( ListView, (*it).first, Q3CheckListItem::CheckBox );
    item->setEnabled( (*it).second );
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
  Q3ListViewItemIterator it = ListView->firstChild();

  for ( ; it.current(); it++)
  {
    if ( ((Q3CheckListItem*)it.current())->isOn() )
    {
      checked.push_back(((Q3CheckListItem*)it.current())->text().latin1());
    }
  }

  QDialog::accept();
}

// ------------------------------------------------------------------------------

/**
 * Constructs a colored button called \a name with parent \a parent.
 */
ColorButton::ColorButton( QWidget* parent )
    : QPushButton( parent )
{
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

/**
 * Draws the button label.
 */
void ColorButton::paintEvent ( QPaintEvent * e )
{
  QPushButton::paintEvent( e );
  
  QPalette::ColorGroup group = isEnabled() ? hasFocus() ? QPalette::Active : QPalette::Inactive : QPalette::Disabled;
  QColor pen = palette().color(group,QPalette::ButtonText);

  QPainter paint(this);
  paint.setPen( pen );
  paint.setBrush( QBrush( _col ) );
  paint.drawRect( width()/4, height()/4, width()/2, height()/2 );
}

/**
 * Opens a QColorDialog to set a new color.
 */
void ColorButton::onChooseColor()
{
  QColor c = QColorDialog::getColor( _col, this );
  if ( c.isValid() )
  {
    setColor( c );
    changed();
  }
}

// ------------------------------------------------------------------------------

UrlLabel::UrlLabel ( QWidget * parent, Qt::WFlags f )
  : QLabel("TextLabel", parent, f)
{
  _url = "http://localhost";
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

void UrlLabel::mouseReleaseEvent ( QMouseEvent * )
{
  // The webbrowser Python module allows to start the system browser in an OS-independent way
  PyObject* module = PyImport_ImportModule("webbrowser");
  if ( module ) {
    // get the methods dictionary and search for the 'open' method
    PyObject* dict = PyModule_GetDict(module);
    PyObject* func = PyDict_GetItemString(dict, "open");
    if ( func ) {
      PyObject* args = Py_BuildValue("(s)", this->_url.ascii());
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

#include "moc_Widgets.cpp"
