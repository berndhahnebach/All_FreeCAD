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

#include "WidgetFactory.h"
#include "PrefWidgets.h"
#include "Widgets.h"
#include "DlgPreferencesImp.h"
#include "DlgSettingsImp.h"
#include "DlgSettings3DViewImp.h"
#include "DlgGeneralImp.h"
#include "DlgEditorImp.h"
#include "DlgSettingsMacroImp.h"
#include "DlgOnlineHelpImp.h"

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
    dynamic_cast<QWidget*>(w);
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

template <class CLASS>
FCWidgetProducer<CLASS>::FCWidgetProducer ()
{
	FCWidgetFactory::Instance().AddProducer(typeid(CLASS).name(), this);
}

// ----------------------------------------------------

template <class CLASS>
FCPrefPageProducer<CLASS>::FCPrefPageProducer (const QString& caption) : mCaption(caption)
{
	FCWidgetFactory::Instance().AddProducer(mCaption.latin1(), this);
	FCDlgPreferencesImp::addPage(caption);
}

// ----------------------------------------------------

FCWidgetFactorySupplier* FCWidgetFactorySupplier::_pcSingleton = 0L;

FCWidgetFactorySupplier::FCWidgetFactorySupplier()
{
  // ADD YOUR PREFERENCFE PAGES HERE
  //
  //
	FCDlgPreferencesImp::addGroup( QObject::tr("FreeCAD") );
  new FCPrefPageProducer<FCDlgGeneral>       ( QObject::tr("General"     ) );
  new FCPrefPageProducer<FCDlgEditorSettings>( QObject::tr("Editor"      ) );
  new FCPrefPageProducer<FCDlgSettingsMacro> ( QObject::tr("Macros"      ) );
  new FCPrefPageProducer<FCOnlineHelp>       ( QObject::tr("Online help" ) );
	FCDlgPreferencesImp::addGroup( QObject::tr("Viewer") );
  new FCPrefPageProducer<FCDlgSettings>      ( QObject::tr("Help Viewer" ) );
  new FCPrefPageProducer<FCDlgSettings3DView>( QObject::tr("3D View"     ) );

	// ADD YOUR PREFERENCE WIDGETS HERE
	//
	//
	new FCWidgetProducer<FCPrefSpinBox>;
	new FCWidgetProducer<FCLineEdit>;
	new FCWidgetProducer<FCComboBox>;
	new FCWidgetProducer<FCListBox>;
	new FCWidgetProducer<FCCheckBox>;
	new FCWidgetProducer<FCRadioButton>;
	new FCWidgetProducer<FCSlider>;
	new FCWidgetProducer<FCCmdView>;
	new FCWidgetProducer<FCAccelLineEdit>;
	new FCWidgetProducer<FCColorButton>;

#if QT_VERSION >= 300
	QWidgetFactory::addWidgetFactory(new FCQtWidgetFactory);
#endif
}

FCWidgetFactorySupplier & FCWidgetFactorySupplier::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCWidgetFactorySupplier;
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

PyTypeObject FCPythonResource::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FCPythonResource",		/*tp_name*/
	sizeof(FCPythonResource),/*tp_basicsize*/
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
PyMethodDef FCPythonResource::Methods[] = {
  {"GetValue",       (PyCFunction) sGetValue, Py_NEWARGS},
  {"SetValue",       (PyCFunction) sSetValue, Py_NEWARGS},
  {"Show",           (PyCFunction) sShow,     Py_NEWARGS},
  {"Connect",        (PyCFunction) sConnect,  Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FCPythonResource::Parents[] = {&FCPyObject::Type,&FCPythonResource::Type, NULL};     

//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
FCPythonResource::FCPythonResource(PyTypeObject *T)
 : FCPyObject( T), myDlg(0L)
{
}

PyObject *FCPythonResource::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FCPythonResource();			// Make new Python-able object
  return 0;
}

//--------------------------------------------------------------------------
//  FCPythonResource destructor 
//--------------------------------------------------------------------------
FCPythonResource::~FCPythonResource()
{
	delete myDlg;
}

//--------------------------------------------------------------------------
// FCPyParametrGrp Attributes
//--------------------------------------------------------------------------
PyObject *FCPythonResource::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	_getattr_up(FCPyObject); 						// send to parent
	return 0;
} 

int FCPythonResource::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	return FCPyObject::_setattr(attr, value);	// send up to parent
	return 0;
} 

void FCPythonResource::load(const char* name)
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

PyObject *FCPythonResource::Show(PyObject *args)
{
	if (myDlg)
	{
		myDlg->exec();
	}

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *FCPythonResource::GetValue(PyObject *args)
{
	char *psName;
	char *psProperty;
	if (!PyArg_ParseTuple(args, "ss", &psName, &psProperty))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 

	QString input="";

	if (myDlg)
	{
		QObjectList *l = myDlg->queryList( "QWidget" );
		QObjectListIt it( *l );
		QObject *obj;

		while ( (obj = it.current()) != 0 ) {
				++it;
				if (strcmp(obj->name(), psName) == 0)
				{
					input = obj->property(psProperty).toString();
					break;
				}
		}
		delete l; // delete the list, not the objects
	}

	PyObject *pItem;
	pItem = PyString_FromString(input.latin1());

	return pItem;
}

PyObject *FCPythonResource::SetValue(PyObject *args)
{
	char *psName;
	char *psProperty;
	char *psValue;
	if (!PyArg_ParseTuple(args, "sss", &psName, &psProperty, &psValue))     // convert args: Python->C 
		return NULL;                             // NULL triggers exception 

	if (myDlg)
	{
		QObjectList *l = myDlg->queryList( "QWidget" );
		QObjectListIt it( *l );
		QObject *obj;

		while ( (obj = it.current()) != 0 ) {
				++it;
				if (strcmp(obj->name(), psName) == 0)
				{
					QVariant v(psValue);
					obj->setProperty(psProperty, v);
					break;
				}
		}
		delete l; // delete the list, not the objects
	}

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *my_callback = NULL;

PyObject *FCPythonResource::Connect(PyObject *args)
{
    PyObject *result = NULL;
    PyObject *temp;

    if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
        if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        Py_XINCREF(temp);         /* Add a reference to new callback */
        Py_XDECREF(my_callback);  /* Dispose of previous callback */
        my_callback = temp;       /* Remember new callback */
        /* Boilerplate to return "None" */
        Py_INCREF(Py_None);
        result = Py_None;
    }
    return result;
}
