/***************************************************************************
                          PrefWidgets.h  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file PrefWidgets.h
 *  \brief A set of FreeCAD widgets
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
 

#ifndef __FC_PREF_WIDGETS_H__
#define __FC_PREF_WIDGETS_H__

#include "../App/Application.h"
#include "../Base/Parameter.h"

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qstring.h>

// forward declarations
class FCWidgetPrefsHandler;
class QDoubleValidator;
class FCWidgetFactorySupplier;

/** The widget preference class
 *  If you want to extend a QWidget class to save/restore data
 *  you just have to derive from this class and implement
 *  the both methods @ref restorePreferences() and
 *  @ref savePreferences()
 *  @see FCWidgetPrefsHandler
 *  @see FCWidgetFactorySupplier
 */
class FCWidgetPrefs : public FCObserver
{
  public:
    /// get the widget's name in preferences
    QString getPrefName();
    /// use system parameter
    void setUseSystemParameter();
    /// use user parameter
    void setUseUserParameter();
    /// return the handler
    FCWidgetPrefsHandler* getHandler();
    /// install a new handler
    void installHandler(FCWidgetPrefsHandler*);
    /// get the handle to the parameter group
    FCParameterGrp::handle getParamGrp();
    /// observers method
    virtual void OnChange(FCSubject &rCaller);

  protected:
    /// restore the preferences
    virtual void restorePreferences() = 0;
    /// save the preferences
    virtual void savePreferences()    = 0;
    /// constructor
    FCWidgetPrefs(const char * name = 0);
    /// destructor
    virtual ~FCWidgetPrefs();

    FCParameterGrp::handle hPrefGrp;

  private:
    // only FCWidgetFactorySupplier has the permission to change
    void setPrefName(QString pref);

    FCWidgetPrefsHandler* pHandler;
    QString m_sPrefGrp;
    QString m_sPrefName;

    // friends
    friend class FCWidgetFactorySupplier;
    friend class FCWidgetPrefsHandler;
};

/** The preference handler class
 *  This class allows you to connect the slots
 *  @ref Save() and @ref Restore() 
 *  to signals of your classes.
 *  @see FCWidgetPrefs
 */
class FCWidgetPrefsHandler : public QObject
{
  Q_OBJECT

  protected:
    FCWidgetPrefsHandler(FCWidgetPrefs* p);

  public slots:
    /// save
    virtual void save();
    /// restore
    virtual void restore();

  protected:
    FCWidgetPrefs* pPref;

    //friends
    friend class FCWidgetPrefs;
};

/** The FCEditSpinBox class
 */
class FCEditSpinBox : public QSpinBox, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( int    accuracy   READ getAccuracy      WRITE setAccuracy      )
    Q_PROPERTY( double valueFloat READ getValueFloat    WRITE setValueFloat    )
    Q_PROPERTY( double valueMax   READ getMaxValueFloat WRITE setMaxValueFloat )
    Q_PROPERTY( double valueMin   READ getMinValueFloat WRITE setMinValueFloat )

  public:
    FCEditSpinBox ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCEditSpinBox();

    // PROPERTIES
    // getters
    int    getAccuracy      () const;
    double getValueFloat    () const;
    double getMinValueFloat () const;
    double getMaxValueFloat () const;
    // setters
    void  setAccuracy      (int    accur);
    void  setValueFloat    (double value);
    void  setMinValueFloat (double value);
    void  setMaxValueFloat (double value);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
    virtual QString mapValueToText(int value);
    virtual int mapTextToValue(bool* ok);
    virtual void valueChange();
    void stepChange();

  signals:
    void valueFloatChanged(double value);

  private:
    QDoubleValidator* m_pValidator;
    int               m_iAccuracy;
    double            m_fDivisor;
    double            m_fEpsilon;
};

/**
 *
 */
class FCLineEdit : public QLineEdit, public FCWidgetPrefs
{
    Q_OBJECT

  public:
    FCLineEdit ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCLineEdit();

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 *
 */
class FCComboBox : public QComboBox, public FCWidgetPrefs
{
    Q_OBJECT

  public:
    FCComboBox ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCComboBox();

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 *
 */
class FCListBox : public QListBox, public FCWidgetPrefs
{
    Q_OBJECT

  public:
    FCListBox ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    virtual ~FCListBox();

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 *
 */
class FCCheckBox : public QCheckBox, public FCWidgetPrefs
{
    Q_OBJECT

  public:
    FCCheckBox ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCCheckBox();

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 *
 */
class FCRadioButton : public QRadioButton, public FCWidgetPrefs
{
    Q_OBJECT

  public:
    FCRadioButton ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCRadioButton();

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

#endif // __FC_PREF_WIDGETS_H__
