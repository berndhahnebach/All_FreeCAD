/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qapplication.h>
# include <qcombobox.h>
# include <qlabel.h>
# include <qlistview.h>
# include <qmessagebox.h>
# include <qprogressbar.h>
# include <qpushbutton.h>
#endif

#include <Base/PyExportImp.h>
#include <Base/Interpreter.h>
#include "UnitTestImp.h"
#include "UnitTestPy.h"


using namespace TestGui;


/* TRANSLATOR TestGui::UnitTest */

UnitTestDialog* UnitTestDialog::_instance=0;

/**
 * Creates and returns the one and only instance of this dialog.
 */
UnitTestDialog* UnitTestDialog::instance()
{
  // not initialized?
  if(!_instance)
  {
    _instance = new UnitTestDialog( qApp->mainWidget() );
  }

  return _instance;
}

/**
 * Destructs the instance of this dialog.
 */
void UnitTestDialog::destruct ()
{
  if ( _instance != 0 )
  {
    UnitTestDialog *pTmp = _instance;
    _instance = 0;
    delete pTmp;
  }
}

/**
 * Returns \a true if an instance of this dialog exists, \a false otherwise.
 */
bool UnitTestDialog::hasInstance()
{
  return _instance != 0;
}

/**
 *  Constructs a TestGui::UnitTestDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
UnitTestDialog::UnitTestDialog( QWidget* parent, const char* name, bool modal, WFlags f )
  : UnitTest( parent, name, modal, f )
{
  // As it doesn't seem to be able to change the "Highlight" color for the active colorgroup
  // we force e.g. the "Motif" style only for the progressbar to change the color to green.
  this->progressBar1->setStyle("Motif");
  setProgressColor(QColor(40,210,43)); // a darker green

  // red items
  listViewFailure->setPaletteForegroundColor(Qt::red);
}

/**
 *  Destroys the object and frees any allocated resources
 */
UnitTestDialog::~UnitTestDialog()
{
}

/**
 * Sets the color to the progressbar to \a col.
 */
void UnitTestDialog::setProgressColor( const QColor& col)
{
  QPalette pl = this->progressBar1->palette();
  QColorGroup cg = pl.active();
  cg.setColor(QColorGroup::Highlight, col);
  pl.setActive(cg);
  cg = pl.inactive();
  cg.setColor(QColorGroup::Highlight, col); 
  pl.setInactive(cg);
  this->progressBar1->setPalette(pl);
}

/**
 * Opens a dialog to display a detailed description about the error.
 */
void UnitTestDialog::showSelectedError(QListViewItem* item)
{
  QMessageBox::information(this, item->text(0), item->text(1));
}

/**
 * Shows the help dialog.
 */
void UnitTestDialog::showHelpDialog()
{
  QMessageBox::information(this, UnitTest::tr("Help"), UnitTest::tr(
    "Enter the name of a callable object which, when called, will return a TestCase."
    "Click 'start', and the test thus produced will be run.\n\n"
    "Double click on an error in the listbox to see more information about it,"
    "including the stack trace."));
}

/**
 * Shows the about dialog.
 */
void UnitTestDialog::showAboutDialog()
{
  QMessageBox::information(this, UnitTest::tr("About FreeCAD UnitTest"), UnitTest::tr( 
    "Copyright (c) Werner Mayer\n\n"
    "FreeCAD UnitTest is part of FreeCAD and supports writing Unit Tests for "
    "own modules."));
}

/**
 * Runs the unit tests.
 */
void UnitTestDialog::startTest()
{
  reset();
  setProgressColor(QColor(40,210,43)); // a darker green
  this->buttonStart->setDisabled(true);
  try {
    Base::Interpreter().runString("import qtunittest");
    Base::Interpreter().runString("g=qtunittest.QtTestRunner(0,\"\")");
    Base::Interpreter().runString("g.runClicked()");
  } catch ( const Base::PyException& e ) {
    showErrorDialog("Exception", e.getStackTrace().c_str());
  } catch ( const Base::Exception& e ) {
    showErrorDialog("Exception", e.what());
  }
  this->buttonStart->setEnabled(true);
}

/**
 * Shows an error dialog with \a title and \a message.
 */
void UnitTestDialog::showErrorDialog(const char* title, const char* message)
{
  QMessageBox::critical(this, title, message);
}

/**
 * Closes and resets this dialog.
 */
void UnitTestDialog::reject()
{
  reset();
  UnitTest::reject();
}

/**
 * Resets this dialog.
 */
void UnitTestDialog::reset()
{
  this->progressBar1->reset();
  this->listViewFailure->clear();
  this->textLabelRunCt->setText ("<font color=\"#0000ff\">0</font>");
  this->textLabelFailCt->setText("<font color=\"#0000ff\">0</font>");
  this->textLabelErrCt->setText ("<font color=\"#0000ff\">0</font>");
  this->textLabelRemCt->setText ("<font color=\"#0000ff\">0</font>");
}

/**
 * Adds a unit test. If a test with name \a unit already is added then nothing happens.
 */
void UnitTestDialog::addUnitTest( const QString& unit )
{
  int ct = this->comboTests->count();
  for ( int i=0; i<ct; i++ ) {
    if ( this->comboTests->text(i) == unit )
    {
      return;
    }
  }

  this->comboTests->insertItem( unit );
}

/**
 * Sets the unit test. If the item is not there yet it gets added.
 */
void UnitTestDialog::setUnitTest( const QString& unit )
{
  addUnitTest(unit);
  this->comboTests->setCurrentText( unit );
}

/**
 * Returns the unit test.
 */
QString UnitTestDialog::getUnitTest() const
{
  return this->comboTests->currentText();
}

/**
 * Sets the text in the status bar.
 */
void UnitTestDialog::setStatusText( const QString& text )
{
  this->textLabelStatus->setText(text);
}

/**
 * Sets the progress of the progress bar whereas fraction is in between 0.0 and 1.0.
 * It also sets the color of the progress bar to red if a failure or error in the unit 
 * test occurred.
 */
void UnitTestDialog::setProgressFraction( float fraction, const QString& color )
{
  if (fraction==0.0f) {
    this->progressBar1->setTotalSteps(100);
  } else {
    if ( color == "red" ){
      setProgressColor(Qt::red);
    }

    this->progressBar1->setProgress((int)(100*fraction));
  }
}

/**
 * Emtpies the error listview.
 */
void UnitTestDialog::clearErrorList()
{
  this->listViewFailure->clear();
}

/**
 * Inserts an item with text \a failure to the listview. \a details will be shown
 * when double-clicking on the matching listitem.
 */
void UnitTestDialog::insertError( const QString& failure, const QString& details )
{
  QListViewItem* item = new QListViewItem(this->listViewFailure);
  item->setText(0,failure);
  item->setText(1,details);
  this->listViewFailure->insertItem(item);
}

/**
 * Sets the number of performed tests to \a ct.
 */
void UnitTestDialog::setRunCount(int ct)
{
  this->textLabelRunCt->setText(QString("<font color=\"#0000ff\">%1</font>").arg(ct));
}

/**
 * Sets the number of failed tests to \a ct.
 */
void UnitTestDialog::setFailCount(int ct)
{
  this->textLabelFailCt->setText(QString("<font color=\"#0000ff\">%1</font>").arg(ct));
}

/**
 * Sets the number of faulty tests to \a ct.
 */
void UnitTestDialog::setErrorCount(int ct)
{
  this->textLabelErrCt->setText(QString("<font color=\"#0000ff\">%1</font>").arg(ct));
}

/**
 * Sets the number of remaining tests to \a ct.
 */
void UnitTestDialog::setRemainCount(int ct)
{
  this->textLabelRemCt->setText(QString("<font color=\"#0000ff\">%1</font>").arg(ct));
}

#include "UnitTest.cpp"
#include "moc_UnitTest.cpp"

