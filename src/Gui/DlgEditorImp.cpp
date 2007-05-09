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

#include "DlgEditorImp.h"
#include "PrefWidgets.h"
#include "PythonEditor.h"

using namespace Gui;
using namespace Gui::Dialog;

namespace Gui {
namespace Dialog {
struct DlgSettingsEditorP
{
  QVector<QPair<QString, unsigned long> > colormap; // Color map
};
} // namespace Dialog
} // namespace Gui

/* TRANSLATOR Gui::Dialog::DlgSettingsEditorImp */

/**
 *  Constructs a DlgSettingsEditorImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgSettingsEditorImp::DlgSettingsEditorImp( QWidget* parent )
  : PreferencePage( parent )
{
  this->setupUi(this);
  d = new DlgSettingsEditorP();
  QColor col;
  col = Qt::black; 
  unsigned long lText = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Text"), lText));
  col = Qt::cyan; 
  unsigned long lBookmarks = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Bookmark"), lBookmarks));
  col = Qt::red; 
  unsigned long lBreakpnts = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Breakpoint"), lBreakpnts));
  col = Qt::blue; 
  unsigned long lKeywords = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Keyword"), lKeywords));
  col.setRgb(0, 170, 0); 
  unsigned long lComments = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Comment"), lComments));
  col.setRgb(160, 160, 164); 
  unsigned long lBlockCom = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Block comment"), lBlockCom));
  col = Qt::blue; 
  unsigned long lNumbers = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Number"), lNumbers));
  col = Qt::red; 
  unsigned long lStrings = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("String"), lStrings));
  col = Qt::red; 
  unsigned long lCharacter = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Character"), lCharacter));
  col.setRgb(255, 170, 0); 
  unsigned long lClass = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Class name"), lClass));
  col.setRgb(255, 170, 0); 
  unsigned long lDefine = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Define name"), lDefine));
  col.setRgb(160, 160, 164); 
  unsigned long lOperat = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Operator"), lOperat));
  col.setRgb(170, 170, 127); 
  unsigned long lPyOutput = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Python output"), lPyOutput));
  col = Qt::red; 
  unsigned long lPyError = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  d->colormap.push_back(QPair<QString, unsigned long>(QT_TR_NOOP("Python error"), lPyError));
  for (QVector<QPair<QString, unsigned long> >::ConstIterator it = d->colormap.begin(); it != d->colormap.end(); ++it)
    this->ListBox1->insertItem(trUtf8((*it).first));
  pythonSyntax = new PythonSyntaxHighlighter(textEdit1);

  connect(ListBox1, SIGNAL(highlighted ( int )), this, SLOT( onDisplayColor( int ) ));
  connect(ColorBtn, SIGNAL(changed ()), this, SLOT( onChosenColor()));
}

/** Destroys the object and frees any allocated resources */
DlgSettingsEditorImp::~DlgSettingsEditorImp()
{
  // no need to delete child widgets, Qt does it all for us
  delete pythonSyntax;
  delete d;
}

/** Searches for the corresponding color value to \e name in @ref DefColorMap and
 *  assigns it to the color button
 *  @see Gui::ColorButton
 *  @see DefColorMap
 */
void DlgSettingsEditorImp::onDisplayColor(int index)
{
  unsigned long col = d->colormap[index].second;
  ColorBtn->setColor(QColor((col >> 24) & 0xff, (col >> 16) & 0xff, (col >> 8) & 0xff));
}

/** Updates the color map if a color was changed */
void DlgSettingsEditorImp::onChosenColor()
{
  QColor col = ColorBtn->color();
  unsigned long lcol = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  
  int index = ListBox1->currentItem();
  d->colormap[index].second = lcol;
  pythonSyntax->setColor( d->colormap[index].first, col );
}

void DlgSettingsEditorImp::saveSettings()
{
  EnableLineNumber->onSave();
  EnableFolding->onSave();
  tabSize->onSave();
  indentSize->onSave();
  radioTabs->onSave();
  radioSpaces->onSave();

  // Saves the color map
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Editor");
  for (QVector<QPair<QString, unsigned long> >::ConstIterator it = d->colormap.begin(); it != d->colormap.end(); ++it)
    hGrp->SetUnsigned((*it).first.latin1(), (*it).second);

  hGrp->SetASCII( "FontSize", fontSize->currentText().latin1() );
  hGrp->SetASCII( "Font", fontFamily->currentText().latin1() );
}

void DlgSettingsEditorImp::loadSettings()
{
  EnableLineNumber->onRestore();
  EnableFolding->onRestore();
  tabSize->onRestore();
  indentSize->onRestore();
  radioTabs->onRestore();
  radioSpaces->onRestore();

  // Restores the color map
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Editor");
  for (QVector<QPair<QString, unsigned long> >::Iterator it = d->colormap.begin(); it != d->colormap.end(); ++it){
    unsigned long col = hGrp->GetUnsigned((*it).first.ascii(), (*it).second);
    (*it).second = col;
    QColor color;
    color.setRgb((col >> 24) & 0xff, (col >> 16) & 0xff, (col >> 8) & 0xff);
    pythonSyntax->setColor( (*it).first, color );
  }

  // fill up font styles
  //
  QFontDatabase fdb;
  QStringList familyNames = fdb.families( QFontDatabase::Any );
  fontFamily->insertStringList( familyNames );

#ifdef FC_OS_LINUX
  fontSize->setCurrentIndex(10);
#else
  fontSize->setCurrentIndex(5);
#endif

  fontSize->setCurrentText( hGrp->GetASCII( "FontSize", fontSize->currentText().latin1() ).c_str() );
  fontFamily->setCurrentText( hGrp->GetASCII( "Font", "Courier" ).c_str() );

  ListBox1->setCurrentItem(0);
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsEditorImp::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    int pos = fontSize->currentItem();
    retranslateUi(this);
    fontSize->setCurrentItem( pos );

    int index = 0;
    for (QVector<QPair<QString, unsigned long> >::ConstIterator it = d->colormap.begin(); it != d->colormap.end(); ++it)
      this->ListBox1->changeItem(trUtf8((*it).first), index++);
  } else {
    QWidget::changeEvent(e);
  }
}

void DlgSettingsEditorImp::on_fontFamily_activated(const QString& fontFamily)
{
    // select the whole document to apply the new format (that only works on selection)
    QTextCursor cursor = textEdit1->textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontFamily(fontFamily);
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
    cursor.clearSelection();
}

void DlgSettingsEditorImp::on_fontSize_activated(const QString& fontSize)
{
    bool ok;
    int size = fontSize.toInt(&ok);
    if (ok) {
        // Create the new text format applying the given size
        QTextCursor cursor = textEdit1->textCursor();
        QTextCharFormat format = cursor.charFormat();
        QFont font = format.font();
        font.setPointSize(size);
        format.setFont(font);
        // select the whole document and apply the text format
        cursor.setPosition(0);
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(format);
        cursor.clearSelection();
    }
}

#include "moc_DlgEditorImp.cpp"
