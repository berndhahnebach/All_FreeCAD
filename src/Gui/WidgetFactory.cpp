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
# include <qapplication.h>
# include <qdir.h>
# include <qfileinfo.h>
# include <qlayout.h>
# include <qobjectlist.h>
# include <qpushbutton.h>
#endif

#include "WidgetFactory.h"
#include "PrefWidgets.h"

#include "../App/Application.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"

using Base::Console;
using namespace Gui;


Gui::WidgetFactoryInst* Gui::WidgetFactoryInst::_pcSingleton = NULL;

WidgetFactoryInst& WidgetFactoryInst::instance()
{
  if (_pcSingleton == 0L)
    _pcSingleton = new WidgetFactoryInst;
  return *_pcSingleton;
}

void WidgetFactoryInst::destruct ()
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

/**
 * Creates a widget with the name \a sName which is a child of \a parent.
 * To create an instance of this widget once it must has been registered. 
 * If there is no appropriate widget registered 0 is returned.
 */
QWidget* WidgetFactoryInst::createWidget (const char* sName, QWidget* parent) const
{
  QWidget* w = (QWidget*)Produce(sName);

  // this widget class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    Console().Warning("\"%s\" is not registered\n", sName);
#else
    Console().Log("\"%s\" is not registered\n", sName);
#endif
    return 0;
  }

  try
  {
    const char* cName = dynamic_cast<QWidget*>(w)->className();
#ifdef FC_DEBUG
    Console().Log("Widget of type '%s' created.\n", cName);
#endif
  }
  catch (...)
  {
#ifdef FC_DEBUG
    Console().Error("%s does not inherit from \"QWidget\"\n", sName);
#else
    Console().Log("%s does not inherit from \"QWidget\"\n", sName);
#endif
    delete w;
    return 0;
  }

  // set the parent to the widget
  if (parent)
    w->reparent(parent, QPoint(0,0));

  return w;
}

/**
 * Creates a preference widget with the name \a sName and the preference name \a sPref 
 * which is a child of \a parent.
 * To create an instance of this widget once it must has been registered. 
 * If there is no appropriate widget registered 0 is returned.
 * After creation of this widget its recent preferences are restored automatically.
 */
QWidget* WidgetFactoryInst::createPrefWidget(const char* sName, QWidget* parent, const char* sPref)
{
  QWidget* w = createWidget(sName);
  // this widget class is not registered
  if (!w)
    return 0; // no valid QWidget object

  // set the parent to the widget
  w->reparent(parent, QPoint(0,0));

  try
  {
    dynamic_cast<PrefWidget*>(w)->setEntryName(sPref);
    dynamic_cast<PrefWidget*>(w)->restorePreferences();
  }
  catch (...)
  {
#ifdef FC_DEBUG
    Console().Error("%s does not inherit from \"PrefWidget\"\n", w->className());
#endif
    delete w;
    return 0;
  }

  return w;
}

// ----------------------------------------------------

WidgetFactorySupplier* WidgetFactorySupplier::_pcSingleton = 0L;

WidgetFactorySupplier & WidgetFactorySupplier::instance()
{
  // not initialized?
  if(!_pcSingleton)
  {
    _pcSingleton = new WidgetFactorySupplier;

#if QT_VERSION >= 300
    QWidgetFactory::addWidgetFactory(new QtWidgetFactory);
#endif
  }

  return *_pcSingleton;
}

// ----------------------------------------------------

/* TRANSLATOR Gui::ContainerDialog */

/**
 *  Constructs a ContainerDialog which embeds the child \a templChild.
 *  The dialog will be modal.
 */
ContainerDialog::ContainerDialog( QWidget* templChild )
    : QDialog( qApp->mainWidget(), 0L, true, 0 )

{
  setCaption( templChild->name() );
  setName( templChild->name() );

  setSizeGripEnabled( TRUE );
  MyDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "MyDialogLayout");

  buttonOk = new QPushButton( this, "buttonOk" );
  buttonOk->setText( tr( "&OK" ) );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );

  MyDialogLayout->addWidget( buttonOk, 1, 0 );
  QSpacerItem* spacer = new QSpacerItem( 210, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  MyDialogLayout->addItem( spacer, 1, 1 );

  buttonCancel = new QPushButton( this, "buttonCancel" );
  buttonCancel->setText( tr( "&Cancel" ) );
  buttonCancel->setAutoDefault( TRUE );

  MyDialogLayout->addWidget( buttonCancel, 1, 2 );

  templChild->reparent(this, QPoint());

  MyDialogLayout->addMultiCellWidget( templChild, 0, 0, 0, 2 );
  resize( QSize(307, 197).expandedTo(minimumSizeHint()) );

  // signals and slots connections
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/** Destroys the object and frees any allocated resources */
ContainerDialog::~ContainerDialog()
{
}

// ----------------------------------------------------

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject PyResource::Type = {
                                    PyObject_HEAD_INIT(&PyType_Type)
                                    0,                    /*ob_size*/
                                    "PyResource",         /*tp_name*/
                                    sizeof(PyResource),   /*tp_basicsize*/
                                    0,                    /*tp_itemsize*/
                                    /* methods */
                                    PyDestructor,         /*tp_dealloc*/
                                    0,                    /*tp_print*/
                                    __getattr,            /*tp_getattr*/
                                    __setattr,            /*tp_setattr*/
                                    0,                    /*tp_compare*/
                                    __repr,               /*tp_repr*/
                                    0,                    /*tp_as_number*/
                                    0,                    /*tp_as_sequence*/
                                    0,                    /*tp_as_mapping*/
                                    0,                    /*tp_hash*/
                                    0,                    /*tp_call */
                                  };

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef PyResource::Methods[] = {
                                        {"GetValue",       (PyCFunction) svalue,    Py_NEWARGS},
                                        {"SetValue",       (PyCFunction) ssetValue, Py_NEWARGS},
                                        {"Show",           (PyCFunction) sshow,     Py_NEWARGS},
                                        {"Connect",        (PyCFunction) sconnect,  Py_NEWARGS},

                                        {NULL, NULL}      /* Sentinel */
                                      };

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PyResource::Parents[] = {&PyObjectBase::Type,&PyResource::Type, NULL};

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
PyResource::PyResource(PyTypeObject *T)
    : PyObjectBase( T), myDlg(0L)
{
}

PyObject *PyResource::PyMake(PyObject *ignored, PyObject *args) // Python wrapper
{
  //return new FCPyResource();      // Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
//  FCPyResource destructor
//--------------------------------------------------------------------------
PyResource::~PyResource()
{
  delete myDlg;
  for (std::vector<SignalConnect*>::iterator it = mySingals.begin(); it != mySingals.end(); ++it)
  {
    SignalConnect* sc = *it;
    delete sc;
  }
}

//--------------------------------------------------------------------------
// FCPyParametrGrp Attributes
//--------------------------------------------------------------------------
PyObject *PyResource::_getattr(char *attr)        // __getattr__ function: note only need to handle new state
{
  _getattr_up(PyObjectBase);            // send to parent
  return 0;
}

int PyResource::_setattr(char *attr, PyObject *value)   // __setattr__ function: note only need to handle new state
{
  return PyObjectBase::_setattr(attr, value); // send up to parent
  return 0;
}

/**
 * Loads an .ui file with the name \a name. If the .ui file cannot be found or the QWidgetFactory
 * cannot create an instance an exception is thrown. If the created resource does not inherit from
 * QDialog an instance of ContainerDialog is created to embed it.
 */
void PyResource::load( const char* name )
{
  QString fn = name;
  QFileInfo fi(fn);

  // checks whether it's a relative path
  if ( fi.isRelative() )
  {
    QString cwd = QDir::currentDirPath ();
    QString home= QDir(App::GetApplication().GetHomePath()).path();

    // search in cwd and home path for the file
    //
    // file does not reside in cwd, check home path now
    if ( !fi.exists() )
    {
      if ( cwd == home )
      {
        throw Base::Exception(QString("Cannot find file %1").arg( fi.absFilePath() ));
      }
      else
      {
        fi.setFile( QDir(home), fn );

        if ( !fi.exists() )
        {
          throw Base::Exception(QString("Cannot find file %1 neither in %2 nor in %3").
                      arg( fn ).arg( cwd ).arg( home ) );
        }
        else
        {
          fn = fi.absFilePath(); // file resides in FreeCAD's home directory
        }
      }
    }
  }
  else
  {
    if ( !fi.exists() )
      throw Base::Exception(QString("Cannot find file %1").arg( fn ));
  }

  QWidget* w=0;
  try{
    w = QWidgetFactory::create( fn, 0, qApp->mainWidget() );
  }catch(...){
    throw Base::Exception("Cannot create resource");
  }

  if ( !w )
    throw Base::Exception("Invalid widget.");

  if (w->inherits("QDialog"))
  {
    myDlg = (QDialog*)w;
  }
  else
  {
    myDlg = new ContainerDialog(w);
  }
}

/**
 * Makes a connection between the sender widget \a sender and its signal \a signal
 * of the created resource and Python callback function \a cb.
 * If the sender widget does not exist or no resource has been loaded this methode returns FALSE, 
 * otherwise it returns TRUE.
 */
bool PyResource::connect(const char* sender, const char* signal, PyObject* cb)
{
  if ( !myDlg )
    return false;

  QObject* objS=0L;
  QObjectList *l = myDlg->queryList( "QWidget" );
  QObjectListIt it( *l );
  QObject *obj;
  QString sigStr = QString("2%1").arg(signal);

  while ( (obj = it.current()) != 0 ) {
    ++it;
    if (strcmp(obj->name(), sender) == 0)
    {
      objS = obj;
      break;
    }
  }

  delete l; // delete the list, not the objects

  if (objS)
  {
    SignalConnect* sc = new SignalConnect(this, cb, objS);
    mySingals.push_back(sc);
    return QObject::connect(objS, sigStr.latin1(), sc, SLOT ( onExecute() )  );
  }
  else
    qWarning( "'%s' does not exist.\n", sender );

  return false;
}

/**
 * Searches for the sender, the signal and the callback function to connect with
 * in the argument object \a args. In the case it fails 0 is returned.
 */
PyObject *PyResource::connect(PyObject *args)
{
  char *psSender;
  char *psSignal;

  PyObject *result = NULL;
  PyObject *temp;

  if (PyArg_ParseTuple(args, "ssO:set_callback", &psSender, &psSignal, &temp))
  {
    if (!PyCallable_Check(temp))
    {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }

    Py_XINCREF(temp);         /* Add a reference to new callback */
    std::string sSender = psSender;
    std::string sSignal = psSignal;

    if ( !connect(psSender, psSignal, temp) )
    {
      // no signal object found => dispose the callback object
      Py_XDECREF(temp);  /* Dispose of callback */
    }

    /* Boilerplate to return "None" */
    Py_INCREF(Py_None);
    result = Py_None;
  }

  return result;
}

/**
 * If any resouce has been loaded this methods shows it as a modal dialog.
 */
PyObject *PyResource::show(PyObject *args)
{
  if (myDlg)
  {
    // small trick to get focus
    myDlg->showMinimized();

#ifdef Q_WS_X11
    // On X11 this may not work. For further information see QWidget::showMaximized
    //
    // workaround for X11
    myDlg->hide();
    myDlg->show();
#endif

    myDlg->showNormal();
    myDlg->exec();
  }

  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * Searches for a widget and its value in the argument object \a args
 * to returns its value as Python object.
 * In the case it fails 0 is returned.
 */
PyObject *PyResource::value(PyObject *args)
{
  char *psName;
  char *psProperty;
  if (!PyArg_ParseTuple(args, "ss", &psName, &psProperty))     // convert args: Python->C
    return NULL;                             // NULL triggers exception

  QVariant v;
  if (myDlg)
  {
    QObjectList *l = myDlg->queryList( "QWidget" );
    QObjectListIt it( *l );
    QObject *obj;

    bool fnd = false;
    while ( (obj = it.current()) != 0 ) {
      ++it;
      if (strcmp(obj->name(), psName) == 0)
      {
        fnd = true;
        v = obj->property(psProperty);
        break;
      }
    }
    delete l; // delete the list, not the objects

    if ( !fnd )
      qWarning( "'%s' not found.\n", psName );
  }

  PyObject *pItem;
  switch (v.type())
  {
  case QVariant::StringList:
    {
      QStringList str = v.toStringList();
      int nSize = str.count();
      PyObject* slist = PyList_New(nSize);
      for (int i=0; i<nSize;++i)
      {
        PyObject* item = PyString_FromString(str[i].latin1());
        PyList_SetItem(slist, i, item);
      }
    } break;
  case QVariant::CString:
    break;
  case QVariant::String:
    pItem = PyString_FromString(v.toString().latin1());
    break;
  case QVariant::Double:
    pItem = PyFloat_FromDouble(v.toDouble());
    break;
  case QVariant::Bool:
    pItem = PyInt_FromLong(v.toBool() ? 1 : 0);
    break;
  case QVariant::UInt:
    pItem = PyLong_FromLong(v.toUInt());
    break;
  case QVariant::Int:
    pItem = PyInt_FromLong(v.toInt());
    break;
  default:
    pItem = PyString_FromString("");
    break;
  }

  return pItem;
}

/**
 * Searches for a widget, its value name and the new value in the argument object \a args
 * to set even this new value.
 * In the case it fails 0 is returned.
 */
PyObject *PyResource::setValue(PyObject *args)
{
  char *psName;
  char *psProperty;
  PyObject *psValue;
  if (!PyArg_ParseTuple(args, "ssO", &psName, &psProperty, &psValue))     // convert args: Python->C
    return NULL;                             // NULL triggers exception

  QVariant v;
  if ( PyString_Check(psValue) )
  {
    v = QString(PyString_AsString(psValue));
  }
  else if ( PyInt_Check(psValue) )
  {
    int val = PyInt_AsLong(psValue);
    v = val;
  }
  else if ( PyLong_Check(psValue) )
  {
    unsigned int val = PyLong_AsLong(psValue);
    v = val;
  }
  else if ( PyFloat_Check(psValue) )
  {
    v = PyFloat_AsDouble(psValue);
  }
  else if ( PyList_Check(psValue) )
  {
    QStringList str;
    int nSize = PyList_Size(psValue);
    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(psValue, i);
      if (!PyString_Check(item))
        continue;

      char* pItem = PyString_AsString(item);

      str.append(pItem);
    }

    v = str;
  }
  else
  {
    PyErr_SetString(PyExc_AssertionError, "Unsupported type");
    return NULL;
  }

  if (myDlg)
  {
    QObjectList *l = myDlg->queryList( "QWidget" );
    QObjectListIt it( *l );
    QObject *obj;

    bool fnd = false;
    while ( (obj = it.current()) != 0 ) {
      ++it;
      if (strcmp(obj->name(), psName) == 0)
      {
        fnd = true;
        obj->setProperty(psProperty, v);
        break;
      }
    }
    delete l; // delete the list, not the objects

    if ( !fnd )
      qWarning( "'%s' not found.\n", psName );
  }

  Py_INCREF(Py_None);
  return Py_None;
}

// ----------------------------------------------------

SignalConnect::SignalConnect( Base::PyObjectBase* res, PyObject* cb, QObject* sender)
  : myResource(res), myCallback(cb), mySender(sender)
{
}

SignalConnect::~SignalConnect()
{
  Py_XDECREF(myCallback);  /* Dispose of callback */
}

/**
 * Calls the callback function of the connected Python object.
 */
void SignalConnect::onExecute()
{
  int arg;
  PyObject *arglist;
  PyObject *result;

  arg = 123;

  /* Time to call the callback */
  arglist = Py_BuildValue("(O)", myResource);
  result = PyEval_CallObject(myCallback, arglist);
  Py_DECREF(arglist);
}

#include "moc_WidgetFactory.cpp"
