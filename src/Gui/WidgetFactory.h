/***************************************************************************
                          WidgetFactory.h  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file WidgetFactory.h
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
 

#ifndef __FC_WIDGET_FACTORY_H__
#define __FC_WIDGET_FACTORY_H__

#include "../Base/Factory.h"
#include "../Base/PyExportImp.h"
#include "DlgPreferencesImp.h"

/** The WidgetFactory singleton
  */
class GuiExport FCWidgetFactory : public Base::Factory
{
	public:
		static FCWidgetFactory& Instance(void);
		static void Destruct (void);

    /// produce the widget using the factory
		QWidget* ProduceWidget (const char* sName, QWidget* parent=0) const;
    QWidget* ProducePrefWidget(const char* sName, QWidget* parent, const char* sPref);

	private:
		static FCWidgetFactory* _pcSingleton;

		FCWidgetFactory(){}
		~FCWidgetFactory(){}
};

inline GuiExport FCWidgetFactory& GetWidgetFactory(void)
{
	return FCWidgetFactory::Instance();
}

// --------------------------------------------------------------------

template <class CLASS>
class FCWidgetProducer: public Base::AbstractProducer
{
	public:
		/// Constructor
		FCWidgetProducer ()
		{
			FCWidgetFactory::Instance().AddProducer(typeid(CLASS).name(), this);
		}

		virtual ~FCWidgetProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const
		{ 
			return (void*)(new CLASS);
		}
};

// --------------------------------------------------------------------

template <class CLASS>
class FCPrefPageProducer: public Base::AbstractProducer
{
	public:
		/// Constructor
		FCPrefPageProducer (const QString& caption)  : mCaption(caption)
		{
			FCWidgetFactory::Instance().AddProducer(mCaption.latin1(), this);
			FCDlgPreferencesImp::addPage(caption);
		}

		virtual ~FCPrefPageProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const
		{ 
			return (void*)(new CLASS);
		}

	private:
		QString mCaption;
};

// --------------------------------------------------------------------

/** 
 * The widget factory supplier class
 */
class FCWidgetFactorySupplier
{
  private:
	  // Singleton
	  FCWidgetFactorySupplier();
  	static FCWidgetFactorySupplier *_pcSingleton;

  public:
  	static FCWidgetFactorySupplier &Instance(void);
	  friend FCWidgetFactorySupplier &GetWidgetFactorySupplier(void); 
};

inline FCWidgetFactorySupplier &GetWidgetFactorySupplier(void)
{
  return FCWidgetFactorySupplier::Instance();
}

// ----------------------------------------------------

class FCContainerDialog : public QDialog
{
	public:
		FCContainerDialog( QWidget* templChild );
		~FCContainerDialog();

		QPushButton* buttonOk;
		QPushButton* buttonCancel;

	protected:
		QGridLayout* MyDialogLayout;
};

// ----------------------------------------------------

class FCPyResource : public FCPyObject
{
	/** always start with Py_Header */
	Py_Header;

	public:
		FCPyResource(PyTypeObject *T = &Type);
		~FCPyResource();

		void load(const char* name);
		bool connect(const char* sender, const char* signal, PyObject* cb);

		/// for Construction in python 
		static PyObject *PyMake(PyObject *, PyObject *);

		//---------------------------------------------------------------------
		// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
		//---------------------------------------------------------------------
		PyObject *_getattr(char *attr);				// __getattr__ function
		// getter setter
		int _setattr(char *attr, PyObject *value);	// __setattr__ function

		// methods
		PYFUNCDEF_D(FCPyResource, GetValue);
		PYFUNCDEF_D(FCPyResource, SetValue);
		PYFUNCDEF_D(FCPyResource, Show);
		PYFUNCDEF_D(FCPyResource, Connect);

	private:
		std::vector<class FCSignalConnect*> mySingals;
		QDialog* myDlg;
};

class FCSignalConnect : public QObject
{
	Q_OBJECT

	public:
		FCSignalConnect( FCPyObject* res, PyObject* cb, QObject* sender);
		~FCSignalConnect();

	public slots:
		void onExecute();

	private:
		PyObject* myResource;
		PyObject* myCallback;
		QObject*  mySender;
};

#endif // __FC_WIDGET_FACTORY_H__
