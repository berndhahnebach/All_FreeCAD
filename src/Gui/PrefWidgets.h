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
class FCWidgetPrefs
{
  public:
    QString getPrefName();
    QString getDefaultName();
    void setUseSystemParameter();
    void setUseUserParameter();
    FCWidgetPrefsHandler* getHandler();

/**
 * @name restorePreferences
 */
    virtual void restorePreferences() = 0;

/**
 * @name savePreferences
 */
    virtual void savePreferences()    = 0;

  protected:
    FCWidgetPrefs(const char * name = 0);
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

  private:
    FCWidgetPrefsHandler(FCWidgetPrefs* p);

  public slots:
/**
 * @name Save
 */
    void Save();

/**
 * @name Restore
 */
    void Restore();

  signals:
    void saved(const FCstring&);

  private:
    FCWidgetPrefs* pPref;

    //friends
    friend class FCWidgetPrefs;
};

/** The WidgetPrefsManager class
 *  This class manages all available prefereence widgets in FreeCAD. All 
 *  such widgets will be registered here automatically.
 *  @see FCWidgetPrefs
 */
class FCWidgetPrefsManager : public QObject
{
  Q_OBJECT

  private:
	  // Singleton
	  FCWidgetPrefsManager();
  	static FCWidgetPrefsManager *_pcSingleton;

  public:
  	static FCWidgetPrefsManager &Instance(void);

  public:
	  void Attach(const FCstring& name, FCWidgetPrefsHandler* w);
	  void Detach(const FCstring& name, FCWidgetPrefsHandler* w);

  public slots:
    void slotSave(const FCstring& name);

  private:
  #	pragma warning( disable : 4251 )
	  FCmap<FCstring, FCvector<FCWidgetPrefsHandler*> > _sHandlers;
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

    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();

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
    virtual QString mapValueToText(int value);
    virtual int mapTextToValue(bool* ok);
    virtual void valueChange();

  signals:
    void valueFloatChanged(double value);

  private:
    QDoubleValidator* m_pValidator;
    int               m_iAccuracy;
    double            m_fDivisor;
    double            m_fEpsilon;
};
#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QPushButton;
class QSpinBox;

class TestDialog : public QDialog
{ 
    Q_OBJECT

public:
    TestDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~TestDialog();

    FCEditSpinBox* SpinBox1;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

protected:
    QGridLayout* MyDialogLayout;
};

#endif // __FC_PREF_WIDGETS_H__
