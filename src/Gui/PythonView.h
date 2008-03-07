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


#ifndef GUI_PYTHONVIEW_H
#define GUI_PYTHONVIEW_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include "MDIView.h"
#include "Window.h"

namespace Gui {

class LineMarkerP;
class LineMarker: public QWidget
{
    Q_OBJECT

public:
    LineMarker(QWidget* parent=0);
    virtual ~LineMarker();

    void setTextEdit(QTextEdit *edit);

protected:
    void paintEvent ( QPaintEvent * );

private:
    QTextEdit *edit;
};

/**
 * A special view class which sends the messages from the application to
 * the Python editor and embeds the editor in a window.
 * \author Werner Mayer
 */
class PythonViewP;
class GuiExport PythonView : public MDIView, public WindowParameter
{
    Q_OBJECT

public:
    PythonView(QWidget* parent);
    ~PythonView();

    void OnChange(Base::Subject<const char*> &rCaller,const char* rcReason);

    const char *getName(void) const {return "PythonView";}
    void onUpdate(void){};

    bool onMsg(const char* pMsg,const char** ppReturn);
    bool onHasMsg(const char* pMsg) const;

    bool canClose(void);

    /** @name Standard actions of the editor */
    //@{
    bool open   (const QString &f);
    bool saveAs ();
    void cut    ();
    void copy   ();
    void paste  ();
    void undo   ();
    void redo   ();
    void run    ();
    void print  ();
    void printPdf();
    //@}

    QStringList undoActions() const;
    QStringList redoActions() const;
    QString fileName() const;

protected:
    void focusInEvent(QFocusEvent* e);

private Q_SLOTS:
    void checkTimestamp();
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void undoAvailable(bool);
    void redoAvailable(bool);

private:
    void setCurrentFileName(const QString &fileName);
    bool saveFile();

private:
    PythonViewP* d;
};

} // namespace Gui

#endif // GUI_PYTHONVIEW_H
