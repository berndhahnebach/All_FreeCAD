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


#ifndef __FC_PREF_WIDGETS_H__
#define __FC_PREF_WIDGETS_H__

#include "../Base/Parameter.h"
#include "Widgets.h"
#include "Window.h"
#include "SpinBox.h"
#include "FileDialog.h"

#ifndef _PreComp_
# include <qcheckbox.h>
# include <qcombobox.h>
# include <qlineedit.h>
# include <qlistbox.h>
# include <qradiobutton.h>
# include <qslider.h>
#endif

// forward declarations
class QDoubleValidator;
class QAction;
class QMainWindow;
class FCCommandManager;

namespace Gui {
class WidgetFactoryInst;
class PrefWidgetHandler;

/** The preference widget class.
 * If you want to extend a QWidget class to save/restore its data
 * you just have to derive from this class and implement the methods 
 * restorePreferences() and savePreferences().
 *
 * @see PrefWidgetHandler
 * \author Werner Mayer
 */
class GuiExport PrefWidget : public WindowParameter
{
public:
  virtual void setEntryName( const QCString& name );
  QCString entryName() const;

  virtual void setParamGrpPath( const QCString& path );
  QCString paramGrpPath() const;

  void installHandler(PrefWidgetHandler*);
  PrefWidgetHandler* getHandler();

  virtual void OnChange(FCSubject<const char*> &rCaller, const char * sReason);

protected:
  /** Restores the preferences
   * Must be reimplemented in any subclasses.
   */
  virtual void restorePreferences() = 0;
  /** Save the preferences
   * Must be reimplemented in any subclasses.
   */
  virtual void savePreferences()    = 0;

  PrefWidget( bool bInstall=true );
  virtual ~PrefWidget();

private:
  PrefWidgetHandler* pHandler;
  QCString m_sPrefName;
  QCString m_sPrefGrp;

  // friends
  friend class Gui::WidgetFactoryInst;
  friend class PrefWidgetHandler;
};

/** The PrefWidgetHandler class allows you to connect the slots
 * @ref onSave() and @ref onRestore() to signals of your classes.
 * @see PrefWidget
 * \author Werner Mayer
 */
class GuiExport PrefWidgetHandler : public QObject
{
  Q_OBJECT

protected:
  PrefWidgetHandler( PrefWidget* p );

public slots:
  virtual void onSave();
  virtual void onRestore();

signals:
  /** This signal is emitted after onSave() was called */
  void saved();
  /** This signal is emitted after onRestore() was called */
  void restored();

protected:
  /** Pointer to PrefWidget object managed by this class. */
  PrefWidget* pPref; 

  //friends
  friend class PrefWidget;
  friend class PrefWidgetManager;
};

/** Container class for storing several @ref PrefWidgetHandler objects.
 * To restore and save the settings of any widgets in own dialogs you have to derive
 * your dialog class from PrefWidgetManager and call append() e.g. in the dialog's 
 * constructor for each widget you want to enable this mechanism.
 *
 * At destruction time you can call PrefWidgetHandler::onSave() for each registered handler
 * to save the preferences of the used widgets.
 * You also have the possibility to connect the onSave() slot to any signals you prefer.
 *
 * \code
 * class MyDialog : public QDialog, public PrefWidgetManager
 * {
 * public:
 *    MyDialog( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 )
 *    {
 *       // register the widgets' handlers to load and save their preferences
 *       append( _listBox ->getHandler() );
 *       append( _listEdit->getHandler() );
 *    }
 *
 *    ~MyDialog()
 *    {
 *      std::vector<PrefWidgetHandler*> handler(getHandlers());
 *      for ( std::vector<PrefWidgetHandler*>::iterator it = handler.begin(); it != handler.end(); ++it)
 *      {
 *        (*it)->onSave();
 *      }
 *    }
 * ...
 *   PrefListBox*  _listBox;
 *   PrefLineEdit* _lineEdit;
 * };
 * \endcode
 * @see Gui::Dialog::DlgPreferencesImp
 *
 * \author Werner Mayer
 */
class GuiExport PrefWidgetManager
{
public:
  std::vector<PrefWidgetHandler*> getHandlers()
  {
    return m_aHandlers;
  }

  /**
   * Appends the PrefWidgetHandler \a handler and restores the preferences
   * of the widget managed by \a handler.
   */
  void append( PrefWidgetHandler* handler )
  {
    if (handler)
    {
#ifdef FC_DEBUG
      if (handler->pPref->getWindowParameter().IsNull())
        qFatal( "No parameter group specified!" );
#endif
      handler->onRestore();
      m_aHandlers.push_back(handler);
    }
  }

protected:
  /** Vector of all registered handlers. */
  std::vector<PrefWidgetHandler*> m_aHandlers;
};

/** The PrefSpinBox class.
 * \author Werner Mayer
 */
class GuiExport PrefSpinBox : public SpinBox, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefSpinBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefSpinBox();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/** The PrefFloatSpinBox class.
 * \author Werner Mayer
 */
class GuiExport PrefFloatSpinBox : public FloatSpinBox, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefFloatSpinBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefFloatSpinBox();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefLineEdit class.
 * \author Werner Mayer
 */
class GuiExport PrefLineEdit : public QLineEdit, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefLineEdit ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefLineEdit();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefFileChooser class.
 * \author Werner Mayer
 */
class GuiExport PrefFileChooser : public FileChooser, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefFileChooser ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefFileChooser();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefComboBox class.
 * \author Werner Mayer
 */
class GuiExport PrefComboBox : public QComboBox, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefComboBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefComboBox();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefListBox class.
 * \author Werner Mayer
 */
class GuiExport PrefListBox : public QListBox, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefListBox ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
  virtual ~PrefListBox();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefCheckBox class.
 * \author Werner Mayer
 */
class GuiExport PrefCheckBox : public QCheckBox, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefCheckBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefCheckBox();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefRadioButton class.
 * \author Werner Mayer
 */
class GuiExport PrefRadioButton : public QRadioButton, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefRadioButton ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefRadioButton();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefSlider class.
 * \author Werner Mayer
 */
class GuiExport PrefSlider : public QSlider, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefSlider ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefSlider();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

/**
 * The PrefColorButton class.
 * \author Werner Mayer
 */
class GuiExport PrefColorButton : public ColorButton, public PrefWidget
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefColorButton ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefColorButton();

  // PROPERTIES
  // getters
  QCString entryName    () const;
  QCString paramGrpPath () const;
  // setters
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

protected:
  // restore from/save to parameters
  void restorePreferences();
  void savePreferences();
};

} // namespace Gui

#endif // __FC_PREF_WIDGETS_H__
