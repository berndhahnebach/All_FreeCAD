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


#ifndef __FC_WIDGET_FACTORY_H__
#define __FC_WIDGET_FACTORY_H__

#ifndef _PreComp_
# include <qwidgetfactory.h>
# include <qdialog.h>
# include <vector>
#endif

#include "../Base/Factory.h"
#include "../Base/PyExportImp.h"
#include "DlgPreferencesImp.h"

class QPushButton;
class QGridLayout;

namespace Gui {

/** 
 * The widget factory provides methods for the dynamic creation of widgets.
 * To create these widgets once they must be registered to the factory.
 * To register them use WidgetProducer or any subclasses; to register a
 * preference page use PrefPageProducer instead.
 * \author Werner Mayer
 */
class GuiExport WidgetFactoryInst : public Base::Factory
{
public:
  static WidgetFactoryInst& Instance();
  static void Destruct ();

  QWidget* createWidget (const char* sName, QWidget* parent=0) const;
  QWidget* createPrefWidget(const char* sName, QWidget* parent, const char* sPref);

private:
  static WidgetFactoryInst* _pcSingleton;

  WidgetFactoryInst(){}
  ~WidgetFactoryInst(){}
};

inline GuiExport WidgetFactoryInst& WidgetFactory()
{
  return WidgetFactoryInst::Instance();
}

// --------------------------------------------------------------------

/**
 * The WidgetProducer class is a value-based template class that provides 
 * the ability to create widgets dynamically. 
 * \author Werner Mayer
 */
template <class CLASS>
class WidgetProducer: public Base::AbstractProducer
{
public:
  /** 
   * Register a special type of widget to the WidgetFactoryInst.
   */
  WidgetProducer ()
  {
    WidgetFactoryInst::Instance().AddProducer(typeid(CLASS).name(), this);
  }

  virtual ~WidgetProducer (){}

  /** 
   * Creates an instance of the specified widget.
   */
  virtual void* Produce () const
  {
    return (void*)(new CLASS);
  }
};

// --------------------------------------------------------------------

/**
 * The PrefPageProducer class is a value-based template class that provides 
 * the ability to create preference pages dynamically. 
 * \author Werner Mayer
 */
template <class CLASS>
class PrefPageProducer: public Base::AbstractProducer
{
public:
  /** 
   * Register a special type of preference page to the WidgetFactoryInst.
   */
  PrefPageProducer (const QString& caption)  : mCaption(caption)
  {
    WidgetFactoryInst::Instance().AddProducer(mCaption.latin1(), this);
    Gui::Dialog::DlgPreferencesImp::addPage(caption);
  }

  virtual ~PrefPageProducer (){}

  /** 
   * Creates an instance of the specified widget.
   */
  virtual void* Produce () const
  {
    return (void*)(new CLASS);
  }

private:
  QString mCaption;
};

// --------------------------------------------------------------------

/**
 * The widget factory supplier class registers all kinds of
 * preference pages and widgets..
 * \author Werner Mayer
 */
class WidgetFactorySupplier
{
private:
  // Singleton
  WidgetFactorySupplier();
  static WidgetFactorySupplier *_pcSingleton;

public:
  static WidgetFactorySupplier &Instance();
  friend WidgetFactorySupplier &GetWidgetFactorySupplier();
};

inline WidgetFactorySupplier &GetWidgetFactorySupplier()
{
  return WidgetFactorySupplier::Instance();
}

// ----------------------------------------------------

/**
 * The ContainerDialog class acts as a container to embed any kinds of widgets that
 * do not inherit QDialog. This class also provied an "Ok" and a "Cancel" button.
 * At most this class is used to embed widgets which are created from .ui files.
 * \author Werner Mayer
 */
class ContainerDialog : public QDialog
{
public:
  ContainerDialog( QWidget* templChild );
  ~ContainerDialog();

  QPushButton* buttonOk; /**< The Ok button. */
  QPushButton* buttonCancel; /**< The cancel button. */

private:
  QGridLayout* MyDialogLayout;
};

// ----------------------------------------------------

/**
 * The PyResource class provides an interface to create widgets or to load .ui files from Python.
 * \author Werner Mayer
 */
class PyResource : public Base::FCPyObject
{
  // always start with Py_Header
  Py_Header;

public:
  PyResource(PyTypeObject *T = &Type);
  ~PyResource();

  void load(const char* name);
  bool connect(const char* sender, const char* signal, PyObject* cb);

  /// for construction in Python
  static PyObject *PyMake(PyObject *, PyObject *);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++
  //---------------------------------------------------------------------
  PyObject *_getattr(char *attr);             // __getattr__ function
  // getter setter
  int _setattr(char *attr, PyObject *value);  // __setattr__ function

  // methods
  PYFUNCDEF_D(PyResource, value);
  PYFUNCDEF_D(PyResource, setValue);
  PYFUNCDEF_D(PyResource, show);
  PYFUNCDEF_D(PyResource, connect);

private:
  std::vector<class SignalConnect*> mySingals;
  QDialog* myDlg;
};

/**
 * The SignalConnect class provides the abitlity to make a connection
 * between the callback function of a Python object and the slot onExecute().
 * This mechanism is used in the Python/Qt framework.
 * \author Werner Mayer
 */
class SignalConnect : public QObject
{
  Q_OBJECT

public:
  SignalConnect( Base::FCPyObject* res, PyObject* cb, QObject* sender);
  ~SignalConnect();

public slots:
  void onExecute();

private:
  PyObject* myResource;
  PyObject* myCallback;
  QObject*  mySender;
};

/**
 * The QtWidgetFactory class provides the abitlity to use the widget factory 
 * framework of FreeCAD within the framework provided by Qt.
 * \author Werner Mayer
 */
class QtWidgetFactory : public QWidgetFactory
{
public:
  QtWidgetFactory() : QWidgetFactory(){}
  ~QtWidgetFactory(){}

  /**
   * Creates a widget of the type \a className withe the parent \a parent.
   * Fore more details see the documentation to QWidgetFactory.
   */
  QWidget* createWidget( const QString & className, QWidget * parent, const char * name ) const
  {
    QString cname = QString("class %1").arg(className);
    return WidgetFactory().createWidget( cname.latin1(), parent );
  }
};

} // namespace Gui

#endif // __FC_WIDGET_FACTORY_H__
