/***************************************************************************
                          WidgetFactory.cpp  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file WidgetFactory.cpp
 *  \brief Produce widgets
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/



#include "PreCompiled.h"
#ifndef _PreComp_
#	include <list>
#	include <string>
#	include <vector>
#	include <qlistbox.h>
#	include <qobjectlist.h>
#endif

#include "WidgetFactory.h"
#include "PrefWidgets.h"

#include "../Base/Console.h"
#include "../Base/Exception.h"

#if QT_VERSION >= 300
#	include <qwidgetfactory.h>
# ifdef FC_OS_WIN32
#		pragma comment(lib,"qui.lib")
#		pragma warning( disable : 4098 )
#	endif
#endif


FCWidgetFactory* FCWidgetFactory::_pcSingleton = NULL;

FCWidgetFactory& FCWidgetFactory::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new FCWidgetFactory;
  return *_pcSingleton;
}

void FCWidgetFactory::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

QWidget* FCWidgetFactory::ProduceWidget (const char* sName, QWidget* parent) const
{
	QWidget* w = (QWidget*)Produce(sName);

  // this widget class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    GetConsole().Warning("\"%s\" is not registered\n", sName);
#else
    GetConsole().Log("\"%s\" is not registered\n", sName);
#endif
    return NULL;
  }

  try
  {
    const char* cName = dynamic_cast<QWidget*>(w)->className();
#ifdef FC_DEBUG
    GetConsole().Log("Widget of type '%s' created.\n", cName);
#endif
  }
  catch (...)
  {
#ifdef FC_DEBUG
    GetConsole().Error("%s does not inherit from \"QWidget\"\n", sName);
#else
    GetConsole().Log("%s does not inherit from \"QWidget\"\n", sName);
#endif
		delete w;
		return NULL;
  }

  // set the parent to the widget 
	if (parent)
	  w->reparent(parent, QPoint(0,0));

  return w;
}

QWidget* FCWidgetFactory::ProducePrefWidget(const char* sName, QWidget* parent, const char* sPref)
{
  QWidget* w = ProduceWidget(sName);
  // this widget class is not registered
  if (!w)
    return NULL; // no valid QWidget object

  // set the parent to the widget 
  w->reparent(parent, QPoint(0,0));

  try
  {
    dynamic_cast<FCWidgetPrefs*>(w)->setPrefName(sPref);
    dynamic_cast<FCWidgetPrefs*>(w)->restorePreferences();
  }
  catch (...)
  {
#ifdef FC_DEBUG
    GetConsole().Error("%s does not inherit from \"FCWidgetPrefs\"\n", w->className());
#endif
		delete w;
		return NULL;
  }

  return w;
}

// ----------------------------------------------------

#if QT_VERSION >= 300
class FCQtWidgetFactory : public QWidgetFactory
{
	public:
		FCQtWidgetFactory() : QWidgetFactory(){}
		~FCQtWidgetFactory(){}

		QWidget* createWidget( const QString & className, QWidget * parent, const char * name ) const
		{
			QString cname = QString("class %1").arg(className);
			return GetWidgetFactory().ProduceWidget(cname.latin1(), parent);
		}
};
#endif

// ----------------------------------------------------

FCWidgetFactorySupplier* FCWidgetFactorySupplier::_pcSingleton = 0L;

FCWidgetFactorySupplier & FCWidgetFactorySupplier::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCWidgetFactorySupplier;

#if QT_VERSION >= 300
	QWidgetFactory::addWidgetFactory(new FCQtWidgetFactory);
#endif
	}

  return *_pcSingleton;
}

// ----------------------------------------------------

FCContainerDialog::FCContainerDialog( QWidget* templChild )
    : QDialog( 0L, 0L, true, 0 )

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

FCContainerDialog::~FCContainerDialog()
{
}

// ----------------------------------------------------

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FCPyResource::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCPyResource",		/*tp_name*/
	sizeof(FCPyResource),/*tp_basicsize*/
	0,						/*tp_itemsize*/
	/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 			/*tp_print*/
	__getattr, 				/*tp_getattr*/
	__setattr, 				/*tp_setattr*/
	0,						/*tp_compare*/
	__repr,					/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef FCPyResource::Methods[] = {
  {"GetValue",       (PyCFunction) sGetValue, Py_NEWARGS},
  {"SetValue",       (PyCFunction) sSetValue, Py_NEWARGS},
  {"Show",           (PyCFunction) sShow,     Py_NEWARGS},
  {"Connect",        (PyCFunction) sConnect,  Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCPyResource::Parents[] = {&FCPyObject::Type,&FCPyResource::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
FCPyResource::FCPyResource(PyTypeObject *T)
 : FCPyObject( T), myDlg(0L)
{
}

PyObject *FCPyResource::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FCPyResource();			// Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
//  FCPyResource destructor 
//--------------------------------------------------------------------------
FCPyResource::~FCPyResource()
{
	delete myDlg;
	for (std::vector<FCSignalConnect*>::iterator it = mySingals.begin(); it != mySingals.end(); ++it)
	{
		FCSignalConnect* sc = *it;
		delete sc;
	}
}

//--------------------------------------------------------------------------
// FCPyParametrGrp Attributes
//--------------------------------------------------------------------------
PyObject *FCPyResource::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	_getattr_up(FCPyObject); 						// send to parent
	return 0;
} 

int FCPyResource::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	return FCPyObject::_setattr(attr, value);	// send up to parent
	return 0;
} 

void FCPyResource::load(const char* name)
{
	QString fn = name;
	QFileInfo fi(fn);

	if (!fi.exists())
		throw FCException(QString("Cannot find file %1").arg(fn));

#if QT_VERSION < 300
	throw FCException("Qt version is too old!\n");
#else
	QWidget* w=NULL;
	try{
		w = QWidgetFactory::create(fn);
	}catch(...){
		throw FCException("Cannot create resource");
	}

	if (w->inherits("QDialog"))
	{
		myDlg = (QDialog*)w;
	}
	else
	{
		myDlg = new FCContainerDialog(w);
	}
#endif
}

PyObject *FCPyResource::Show(PyObject *args)
{
	if (myDlg)
	{
		myDlg->exec();
	}

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *FCPyResource::GetValue(PyObject *args)
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

		while ( (obj = it.current()) != 0 ) {
				++it;
				if (strcmp(obj->name(), psName) == 0)
				{
					v = obj->property(psProperty);
					break;
				}
		}
		delete l; // delete the list, not the objects
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

PyObject *FCPyResource::SetValue(PyObject *args)
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

		while ( (obj = it.current()) != 0 ) {
				++it;
				if (strcmp(obj->name(), psName) == 0)
				{
					obj->setProperty(psProperty, v);
					break;
				}
		}
		delete l; // delete the list, not the objects
	}

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *FCPyResource::Connect(PyObject *args)
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

bool FCPyResource::connect(const char* sender, const char* signal, PyObject* cb)
{
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
		FCSignalConnect* sc = new FCSignalConnect(this, cb, objS);
		mySingals.push_back(sc);
		QObject::connect(objS, sigStr.latin1(), sc, SLOT ( onExecute() )  );

		return true;
	}
	else
		return false;
}

// ----------------------------------------------------

FCSignalConnect::FCSignalConnect( FCPyObject* res, PyObject* cb, QObject* sender)
 : myResource(res), myCallback(cb), mySender(sender)
{
}

FCSignalConnect::~FCSignalConnect()
{
	Py_XDECREF(myCallback);  /* Dispose of callback */
}

void FCSignalConnect::onExecute()
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
