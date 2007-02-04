/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __TREE_H_
#define __TREE_H_

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#include <Base/PyExport.h>
#include <App/Document.h>
#include <App/Application.h>

#include "Selection.h"
#include "DockWindow.h"

/// Forwards
class FCLabel; 

using namespace std;

namespace Gui {
class Document;
class TreeView;
class ViewProviderDocumentObject;

struct GUIDDefs {
    char  *Guid;
    char  *Name;
};

/** The link between the Tree and a document.
 *  Every (shown) document in the Application get it 
 *  associated DocItem which controls the visibility 
 *  and the functions of the Document.
 *  \author Jürgen Riegel
 */
class ObjectItem : public Q3ListViewItem
{
public:
  /// Constructor
  ObjectItem( Q3ListViewItem* parent,Gui::ViewProviderDocumentObject* pcViewProvider);
  virtual ~ObjectItem();

  /// This is the class Id to distinguish from QListViewItem itself or from other QListViewItem-subclasses. 
  int rtti () const { return 3100; }
  /// Opens the Leafs and generate them.
  void setOpen( bool );
  /// Im not realy sure whats this method do ;-).
  void setup();

  void highlightFeature(bool);

  void selectFeature(bool);

  void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );

  /// Delivers the pixmap that is shown.
//    const QPixmap *pixmap( int i ) const;
  /// Sets the pixmap that will be shown.
//    void setPixmap( QPixmap *p );

  void update(void);

  void buildUp(void);

  bool testStatus(void);
  void displayStatusInfo();

  QColor BaseColor, TextColor, HighlightColor;

  friend class DocItem;

protected:
  void activate (); 

  bool bHighlight;
  bool bSelected;

  int FeatStatus;


  Gui::ViewProviderDocumentObject*  _pcViewProvider;
};


/** The link between the Tree and a document.
 *  Every (shown) document in the Application get it 
 *  associated DocItem which controls the visibility 
 *  and the functions of the Document.
 *  \author Jürgen Riegel
 */
class DocItem : public Q3ListViewItem//, public App::Document::ObserverType
{
public:
  /// Constructor
  DocItem( Q3ListViewItem* parent,Q3ListViewItem * after,Gui::Document* doc);
  virtual ~DocItem();

  /// This is the class Id to distinguish from QListViewItem itself or from other QListViewItem-subclasses. 
  int rtti () const { return 3000; }
  /// Opens the Leafs and generate them.
  void setOpen( bool );
  /// Im not realy sure whats this method do ;-).
  void setup();

  /// Delivers the pixmap that is shown.
//    const QPixmap *pixmap( int i ) const;
  /// Sets the pixmap that will be shown.
//    void setPixmap( QPixmap *p );

  /** Adds a view provider to the document item.
   * If this view provider is already added nothing happens.
   */
  void addViewProviderDocumentObject(ViewProviderDocumentObject*);
  /** Removes a view provider from the document item.
   * If this view provider is not added nothing happens.
   */
  void removeViewProviderDocumentObject(ViewProviderDocumentObject*);

  void highlightFeature(const char*, bool);

  void selectFeature(const char*, bool);
  void clearSelection(void);
  void isSelectionUptodate(void);

  void update(void);

  void buildUp(void);

  bool testStatus(void);

  Gui::Document*  getDocument(void){return _pcDocument;}

  void rename(void);

  /// Observer message from the App doc
//  virtual void OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason);
  void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );


protected:
  void activate (); 

  bool bHighlight;
  bool bSelected;


  Gui::Document*  _pcDocument;
  std::map<std::string,ObjectItem*> FeatMap;
};
 

 


/** 
 *  \author Jürgen Riegel
 */
class TreeView :public Gui::DockWindow, public Gui::SelectionSingleton::ObserverType
{
  Q_OBJECT

public:
  TreeView(Gui::Document*  pcDocument,QWidget *parent=0);
  // App_Tree();

  ~TreeView();

  bool onMsg(const char* pMsg);

  virtual const char *getName(void) const {return "TreeView";}

    /// Observer message from the Selection
  virtual void OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason);


  //void InitCascade(Handle(TDocStd_Document) hDoc);
  friend class DocItem;
  friend class ObjectItem;

  /// get called when the document is changed or updated
  virtual void onUpdate(void);
  /// get called when the document is renamed
  virtual void onRename(Gui::Document *);


	/// This method get called when a new doc appears
	DocItem *  NewDoc(Gui::Document*);
	/// This method get called when a new doc will be deleted
  void DeleteDoc(Gui::Document*);

  Q3ListViewItem* getMainItem(void) const {return _pcMainItem;}

  void testStatus(void);

public Q_SLOTS:

  void onSelectionChanged(void);
  void onItem(Q3ListViewItem*);

protected:

  void contextMenuEvent ( QContextMenuEvent * e );
  void changeEvent(QEvent *e);

  Q3ListView*  _pcListView;
  Q3ListViewItem*  _pcMainItem;

  static QPixmap *pcDocumentPixmap;

  map<Gui::Document*,DocItem*> DocMap;

  bool bFromOutside;

private:
  Q3ListViewItem* _lastDocItem;
};

} // namespace Gui

#endif // __TREE_H_

