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

#ifndef _PreComp_
# include <qaccel.h>
# include <qcolordialog.h>
# include <qcursor.h>
# include <qgroupbox.h>
# include <qlabel.h>
# include <qlayout.h>
# include <qpainter.h>
# include <qstyle.h>
#endif

#include "Widgets.h"
#include "Application.h"
#include "Action.h"
#include "CustomWidgets.h"

#include "Language/Translator.h"

#include "../Base/Exception.h"

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
                           int button2, QWidget * parent, const char * name, bool modal, WFlags f)
    : QMessageBox(caption, text, icon, button0, button1, button2, parent, name, modal, f),
    checkBox(0L), layout(0L)
{
  QString txt = Translator::getFindSourceText( text );

  QString msg = tr("Never show this message again.");
  QString entry = txt;
  QString cn = parent ? parent->className() : "Unknown";
  QString path  = QString("General/NeverShowDialog/%1").arg( cn );


  checkBox = new Gui::PrefCheckBox(this, "PrefCheckBox");
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

  layout = new QGridLayout(this, 1, 1, 50);
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

namespace Gui{
class CommandViewItemPrivate
{
public:
  QString sAction;
  QString description;
};
} // namespace Gui

/**
 * Constructs a command view item and inserts it into the icon view \a parent using 
 * the action's text as the text and the actoin's icon as the icon.
 */
CommandViewItem::CommandViewItem ( QIconView * parent, const QString& action, QAction* pAct )
  : QIconViewItem(parent, pAct->menuText(), pAct->iconSet().pixmap())
{
  d = new CommandViewItemPrivate;
  d->sAction = action;
  d->description = pAct->toolTip();
}

/**
 * Destroys the icon view item and tells the parent icon view that the item has been destroyed.
 */
CommandViewItem::~CommandViewItem ()
{
  delete d;
}

/**
 * Returns the description text of the command view item.
 */
QString CommandViewItem::text() const
{
  return d->description;
}

/**
 * Returns the command name of the command view item.
 */
QString CommandViewItem::commandName()
{
  return d->sAction;
}

// ------------------------------------------------------------------------------

/**
 * Constructs an empty command view called \a name, with parent \a parent and using the widget flags \a f.
 */
CommandIconView::CommandIconView ( QWidget * parent, const char * name, WFlags f )
    : QIconView(parent, name, f)
{
  // settings for the view showing the icons
  setResizeMode(Adjust);
  setItemsMovable(false);
  setWordWrapIconText(false);
  setGridX(50);
  setGridY(50);

  setSelectionMode(Extended);

  // clicking on a icon a signal with its description will be emitted
  connect(this, SIGNAL ( currentChanged ( QIconViewItem * ) ), this, SLOT ( onSelectionChanged(QIconViewItem * ) ) );
}

/**
 * Destroys the icon view and deletes all items.
 */
CommandIconView::~CommandIconView ()
{
}

/**
 * This slot is called when a new item becomes current. \a item is the new current item 
 * (or 0 if no item is now current). This slot emits the emitSelectionChanged()
 * signal for its part.
 */
void CommandIconView::onSelectionChanged(QIconViewItem * item)
{
  emit emitSelectionChanged(item->text());
}

/**
 * Returns the QDragObject that should be used for drag-and-drop. This function is called 
 * by the icon view when starting a drag to get the dragobject that should be used for the drag. 
 */
QDragObject * CommandIconView::dragObject ()
{
  ActionDrag::actions.clear();
  if ( !currentItem() )
    return 0;

  bool bFirst = true;
  ActionDrag *ad=NULL;
  //  QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );
  for ( QIconViewItem *item = firstItem(); item; item = item->nextItem() )
  {
    if ( item->isSelected() )
    {
      if (typeid(*item) == typeid(CommandViewItem))
      {
        ad = new ActionDrag( ((CommandViewItem*)item)->commandName(), this );
        if (bFirst)
        {
          bFirst = false;
          ad->setPixmap( *currentItem()->pixmap(), QPoint( currentItem()->pixmapRect().width() / 2,
                         currentItem()->pixmapRect().height() / 2 ) );
        }
      }
    }
  }

  return ad;
}

// ------------------------------------------------------------------------------

/* TRANSLATOR Gui::AccelLineEdit */

/**
 * Constructs a line edit with no text.
 * The \a parent and \a name arguments are sent to the QLineEdit constructor.
 */
AccelLineEdit::AccelLineEdit ( QWidget * parent, const char * name )
  : QLineEdit(parent, name)
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

  if (e->ascii() == 0)
    return; // only meta key pressed

  int key = e->key();

  // I hope this works for every keyboard
  if ( key == Key_mu )
    key = Key_M;
  else if ( key == Key_At )
    key = Key_Q;

  // ignore these keys
  if ( key < Key_0 || key > Key_Z )
     return;

  switch(e->state())
  {
  case ControlButton:
    txt += QAccel::keyToString(CTRL+key);
    setText(txt);
    break;
  case ControlButton+AltButton:
    txt += QAccel::keyToString(CTRL+ALT+key);
    setText(txt);
    break;
  case ControlButton+ShiftButton:
    txt += QAccel::keyToString(CTRL+SHIFT+key);
    setText(txt);
    break;
  case ControlButton+AltButton+ShiftButton:
    txt += QAccel::keyToString(CTRL+ALT+SHIFT+key);
    setText(txt);
    break;
  case AltButton:
  case ShiftButton:
    break;
  default:// CTRL
    if ( key != Key_Backspace && key != Key_Delete)
    {
      txt += QAccel::keyToString(CTRL+key);
      setText(txt);
    }
    break;
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
CheckListDialog::CheckListDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
  if ( !name )
    setName( "CheckListDialog" );
  resize( 402, 270 );
  setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
  CheckListDialogLayout = new QGridLayout( this );
  CheckListDialogLayout->setSpacing( 6 );
  CheckListDialogLayout->setMargin( 11 );

  Layout2 = new QHBoxLayout;
  Layout2->setSpacing( 6 );
  Layout2->setMargin( 0 );

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setProperty( "text", tr( "&OK" ) );
  buttonOk->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  buttonOk->setProperty( "default", QVariant( TRUE, 0 ) );
  Layout2->addWidget( buttonOk );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout2->addItem( spacer );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setProperty( "text", tr( "&Cancel" ) );
  buttonCancel->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
  Layout2->addWidget( buttonCancel );

  CheckListDialogLayout->addLayout( Layout2, 1, 0 );

  GroupBox1 = new QGroupBox( this, "GroupBox1" );
  GroupBox1->setColumnLayout(0, Qt::Vertical );
  GroupBox1->layout()->setSpacing( 0 );
  GroupBox1->layout()->setMargin( 0 );
  GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
  GroupBox1Layout->setAlignment( Qt::AlignTop );
  GroupBox1Layout->setSpacing( 6 );
  GroupBox1Layout->setMargin( 11 );

  ListView = new QListView( GroupBox1, "ListView" );
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
    QCheckListItem* item = new QCheckListItem( ListView, *it, QCheckListItem::CheckBox );
    item->setEnabled( false );
  }
}

/**
 * Sets the items to the dialog's list view. If the boolean type of a CheckListItem
 * is set to false the item is not checkable any more.
 */
void CheckListDialog::setCheckableItems( const QValueList<CheckListItem>& items )
{
  for ( QValueList<CheckListItem>::ConstIterator it = items.begin(); it!=items.end(); ++it )
  {
    QCheckListItem* item = new QCheckListItem( ListView, (*it).first, QCheckListItem::CheckBox );
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
  QListViewItemIterator it = ListView->firstChild();

  for ( ; it.current(); it++)
  {
    if ( ((QCheckListItem*)it.current())->isOn() )
    {
      checked.push_back(((QCheckListItem*)it.current())->text().latin1());
    }
  }

  QDialog::accept();
}

// ------------------------------------------------------------------------------

/**
 * Constructs a colored button called \a name with parent \a parent.
 */
ColorButton::ColorButton(QWidget* parent, const char* name)
    : QPushButton( parent, name )
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
void ColorButton::drawButtonLabel( QPainter *paint )
{
  QColor pen = isEnabled() ? hasFocus() ? palette().active().buttonText() : palette().inactive().buttonText()
                   : palette().disabled().buttonText();
  paint->setPen( pen );
  paint->setBrush( QBrush( _col ) );

  paint->drawRect( width()/4, height()/4, width()/2, height()/2 );
}

/**
 * Opens a QColorDialog to set a new color.
 */
void ColorButton::onChooseColor()
{
  QColor c = QColorDialog::getColor( palette().active().background(), this );
  if ( c.isValid() )
  {
    setColor( c );
    emit changed();
  }
}

#include "moc_Widgets.cpp"
