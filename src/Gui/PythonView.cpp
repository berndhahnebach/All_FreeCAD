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

#include "PythonView.h"
#include "PythonEditor.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "FileDialog.h"
#include "Macro.h"

#include <Base/Parameter.h>

using namespace Gui;
namespace Gui {
class PythonViewP {
public:
    LineMarker* lineMarker;
    QTextEdit* textEdit;
    QString fileName;
    QTimer*  activityTimer;
    uint timeStamp;
};
}

/* TRANSLATOR Gui::PythonView */

/**
 *  Constructs a PythonView which is a child of 'parent', with the
 *  name 'name'.
 */
PythonView::PythonView(QWidget* parent)
    : MDIView(0,parent,0), WindowParameter( "Editor" )
{
    d = new PythonViewP;

    // create the editor first
    d->textEdit = new PythonEditor();
    d->textEdit->setLineWrapMode( QTextEdit::NoWrap );
    d->lineMarker = new LineMarker();
    d->lineMarker->setTextEdit(d->textEdit);

    // Create the layout containing the workspace and a tab bar
    QFrame* hbox = new QFrame(this);
    hbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(1);
    layout->addWidget(d->lineMarker);
    layout->addWidget(d->textEdit);
    hbox->setLayout(layout);
    setCentralWidget(hbox);

    setCurrentFileName(QString());
    d->textEdit->setFocus();

    setIcon( Gui::BitmapFactory().pixmap("python_small") );

    ParameterGrp::handle hPrefGrp = getWindowParameter();
    hPrefGrp->Attach( this );
    hPrefGrp->NotifyAll();

    d->activityTimer = new QTimer(this);
    connect(d->activityTimer, SIGNAL(timeout()),
            this, SLOT(checkTimestamp()) );
    connect(d->textEdit->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));
}

/** Destroys the object and frees any allocated resources */
PythonView::~PythonView()
{
    d->activityTimer->stop();
    delete d->activityTimer;
    delete d;
    getWindowParameter()->Detach( this );
}

void PythonView::OnChange( Base::Subject<const char*> &rCaller,const char* rcReason )
{
    ParameterGrp::handle hPrefGrp = getWindowParameter();
    if (strcmp(rcReason, "EnableLineNumber") == 0) {
        bool show = hPrefGrp->GetBool( "EnableLineNumber", true );
        if ( show )
            d->lineMarker->show();
        else
            d->lineMarker->hide();
    }
}

void PythonView::checkTimestamp()
{
    QFileInfo fi(d->fileName);
    uint timeStamp =  fi.lastModified().toTime_t();
    if ( timeStamp != d->timeStamp ) {
        switch( QMessageBox::question( this, tr("Modified file"), 
                tr("%1.\n\nThis has been modified outside of the source editor. Do you want to reload it?").arg( d->fileName ),
                QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) )
        {
            case QMessageBox::Yes:
                // updates time stamp and timer
                open( d->fileName );
                return;
            case QMessageBox::No:
                d->timeStamp = timeStamp;
                break;
        }
    }

    d->activityTimer->start( 3000, true );
}

/**
 * Runs the action specified by \a pMsg.
 */
bool PythonView::onMsg(const char* pMsg,const char** ppReturn)
{
    if (strcmp(pMsg,"Save")==0){
        saveFile();
        return true;
    } else if (strcmp(pMsg,"Run")==0){
        run();
        return true;
    } else if (strcmp(pMsg,"SaveAs")==0){
        saveAs();
        return true;
    } else if (strcmp(pMsg,"Cut")==0){
        cut();
        return true;
    } else if (strcmp(pMsg,"Copy")==0){
        copy();
        return true;
    } else if (strcmp(pMsg,"Paste")==0){
        paste();
        return true;
    } else if (strcmp(pMsg,"Undo")==0){
        undo();
        return true;
    } else if (strcmp(pMsg,"Redo")==0){
        redo();
        return true;
    } else if (strcmp(pMsg,"ViewFit")==0){
        // just ignore this
        return true;
    }

    return false;
}

/**
 * Checks if the action \a pMsg is available. This is for enabling/disabling
 * the corresponding buttons or menu items for this action.
 */
bool PythonView::onHasMsg(const char* pMsg) const
{
    if (strcmp(pMsg,"Run")==0)  return true;
    if (strcmp(pMsg,"SaveAs")==0)  return true;
    if (strcmp(pMsg,"Print")==0) return true;
    if (strcmp(pMsg,"PrintPdf")==0) return true;
    if (strcmp(pMsg,"Save")==0) { 
        return d->textEdit->document()->isModified();
    } else if (strcmp(pMsg,"Cut")==0) {
        bool canWrite = !d->textEdit->isReadOnly();
        return (canWrite && (d->textEdit->hasSelectedText()));
    } else if (strcmp(pMsg,"Copy")==0) {
        return ( d->textEdit->hasSelectedText() );
    } else if (strcmp(pMsg,"Paste")==0) {
        QClipboard *cb = QApplication::clipboard();
        QString text;

        // Copy text from the clipboard (paste)
        text = cb->text();

        bool canWrite = !d->textEdit->isReadOnly();
        return ( !text.isEmpty() && canWrite );
    } else if (strcmp(pMsg,"Undo")==0) {
        return d->textEdit->isUndoAvailable ();
    } else if (strcmp(pMsg,"Redo")==0) {
        return d->textEdit->isRedoAvailable ();
    }

    return false;
}

/** Checking on close state. */
bool PythonView::canClose(void)
{
    if ( !d->textEdit->document()->isModified() )
        return true;
    switch( QMessageBox::question(this, tr("Unsaved document"), 
                                    tr("The document has been modified.\n"
                                       "Do you want to save your changes?"),
                                     QMessageBox::Yes|QMessageBox::Default, QMessageBox::No, 
                                     QMessageBox::Cancel|QMessageBox::Escape))
    {
        case QMessageBox::Yes:
            return saveFile();
        case QMessageBox::No:
            return true;
        case QMessageBox::Cancel:
            return false;
        default:
            return false;
    }
}

/**
 * Saves the content of the editor to a file specified by the appearing file dialog.
 */
bool PythonView::saveAs(void)
{
    QString fn = FileDialog::getSaveFileName(QString::null, "FreeCAD macro (*.FCMacro);;Python (*.py)", 
                                               this, QObject::tr("Save Macro"));
    if (fn.isEmpty())
        return false;
    setCurrentFileName(fn);
    return saveFile();
}

/**
 * Opens the file \a fileName.
 */
bool PythonView::open(const QString& fileName)
{
    if (!QFile::exists(fileName))
        return false;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return false;

    d->textEdit->setPlainText(file.readAll());
    file.close();

    QFileInfo fi(fileName);
    d->timeStamp =  fi.lastModified().toTime_t();
    d->activityTimer->start( 3000, true );	

    setCurrentFileName(fileName);
    return true;
}

/**
 * Runs the opened script in the macro manager.
 */
void PythonView::run(void)
{
    Application::Instance->macroManager()->run(Gui::MacroManager::File,d->fileName.latin1());
}

/**
 * Copies the selected text to the clipboard and deletes it from the text edit.
 * If there is no selected text nothing happens.
 */
void PythonView::cut(void)
{
    d->textEdit->cut();
}

/**
 * Copies any selected text to the clipboard.
 */
void PythonView::copy(void)
{
    d->textEdit->copy();
}

/**
 * Pastes the text from the clipboard into the text edit at the current cursor position. 
 * If there is no text in the clipboard nothing happens.
 */
void PythonView::paste(void)
{
    d->textEdit->paste();
}

/**
 * Undoes the last operation.
 * If there is no operation to undo, i.e. there is no undo step in the undo/redo history, nothing happens.
 */
void PythonView::undo(void)
{
    d->textEdit->undo();
}

/**
 * Redoes the last operation.
 * If there is no operation to undo, i.e. there is no undo step in the undo/redo history, nothing happens.
 */
void PythonView::redo(void)
{
    d->textEdit->redo();
}

/**
 * Shows the printer dialog.
 */
void PythonView::print()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        d->textEdit->document()->print(&printer);
    }
}

/**
 * Prints the document into a Pdf file.
 */
void PythonView::printPdf()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export PDF"), QString(), tr("PDF file (*.pdf)"));
    if (!filename.isEmpty()) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);
        d->textEdit->document()->print(&printer);
    }
}

void PythonView::setCurrentFileName(const QString &fileName)
{
    d->fileName = fileName;
    d->textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled[*]";
    else
        shownName = fileName + "[*]";
    setWindowTitle(shownName);
    setWindowModified(false);
}

/**
 * Saves the contents to a file.
 */
bool PythonView::saveFile()
{
    if (d->fileName.isEmpty())
        return saveAs();

    QFile file(d->fileName);
    if (!file.open(QFile::WriteOnly))
        return false;
    QTextStream ts(&file);
    ts.setCodec(QTextCodec::codecForName("UTF-8"));
    ts << d->textEdit->document()->toPlainText();
    file.close();
    d->textEdit->document()->setModified(false);

    QFileInfo fi(d->fileName);
    d->timeStamp =  fi.lastModified().toTime_t();
    return true;
}

/**
 * \Todo: Get the undo history.
 */
QStringList PythonView::undoActions() const
{
    return QStringList();
}

/**
 * \Todo: Get the redo history.
 */
QStringList PythonView::redoActions() const
{
    return QStringList();
}

// ---------------------------------------------------------

LineMarker::LineMarker(QWidget* parent)
    : QWidget(parent)
{
    setFixedWidth( fontMetrics().width( QString("0000") ) );
}

LineMarker::~LineMarker()
{
}

void LineMarker::setTextEdit(QTextEdit *edit)
{
    this->edit = edit;
    connect( edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ),
	     this, SLOT( update() ) );
    connect( edit->verticalScrollBar(), SIGNAL(valueChanged(int) ),
	     this, SLOT( update() ) );
}

void LineMarker::paintEvent( QPaintEvent* )
{
    QAbstractTextDocumentLayout *layout = edit->document()->documentLayout();
    int contentsY = edit->verticalScrollBar()->value();
    qreal pageBottom = contentsY + edit->viewport()->height();
    const QFontMetrics fm = fontMetrics();
    const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
    int lineCount = 1;

    QPainter p(this);

    for ( QTextBlock block = edit->document()->begin();
	  block.isValid(); block = block.next(), ++lineCount ) {

        const QRectF boundingRect = layout->blockBoundingRect( block );

        QPointF position = boundingRect.topLeft();
        if ( position.y() + boundingRect.height() < contentsY )
            continue;
        if ( position.y() > pageBottom )
            break;

        const QString txt = QString::number( lineCount );
        p.drawText( width() - fm.width(txt), qRound( position.y() ) - contentsY + ascent, txt );
    }
}

#include "moc_PythonView.cpp"
