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
#include "Command.h"
#include "Widgets.h"

#include "qextmdi/qextmdimainfrm.h"
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qstring.h>
#include <qslider.h>

// forward declarations
class FCWidgetPrefsHandler;
class QDoubleValidator;
class FCWidgetFactorySupplier;
class QAction;
class FCStackBar;
class FCDockWindow;

/** The widget preference class
 *  If you want to extend a QWidget class to save/restore data
 *  you just have to derive from this class and implement
 *  the both methods @ref restorePreferences() and
 *  @ref savePreferences()
 *  @see FCWidgetPrefsHandler
 *  @see FCWidgetFactorySupplier
 */
class FCWidgetPrefs : public FCParameterGrp::ObserverType
{
  public:
    /** changes the preference name */
    virtual void setEntryName(QString name);
    /** get the widget's name in preferences */
    QString getEntryName() const;
    /** changes the preference path */
    virtual void setParamGrpPath(QString name);
    /** get the widget's preferences path */
    QString getParamGrpPath() const;
    /** return the handler */
    FCWidgetPrefsHandler* getHandler();
    /** install a new handler */
    void installHandler(FCWidgetPrefsHandler*);
    /** get the handle to the parameter group */
    FCParameterGrp::handle getParamGrp();
    /** observers method */
    virtual void OnChange(FCSubject<const char*> &rCaller, const char * sReason);
    /** get the handle to the root parameter group */
    static FCParameterGrp::handle getRootParamGrp();

  protected:
    /** Restore the preferences
     * Must be reimplemented in subclasses
     */
    virtual void restorePreferences() = 0;
    /** Save the preferences
     * Must be reimplemented in subclasses
     */
    virtual void savePreferences()    = 0;
    /// constructor
    FCWidgetPrefs(const char * name = 0, bool bInstall=true);
    /// destructor
    virtual ~FCWidgetPrefs();

    /// changes the preference name
    void setPrefName(QString pref);
    FCParameterGrp::handle hPrefGrp;

  private:
    FCWidgetPrefsHandler* pHandler;
    QString m_sPrefName;
    QString m_sPrefGrp;

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
    virtual void onSave();
    /// restore
    virtual void onRestore();

  signals:
    /// emits this signal when @ref onSave() was called
    void saved();
    /// emits this signal when @ref onRestore() was called
    void restored();

  protected:
    FCWidgetPrefs* pPref;

    //friends
    friend class FCWidgetPrefs;
		friend class FCWidgetPrefsManager;
};

/** Container class for storing several 
 * @ref FCWidgetPrefsHandler objects.
 */
class FCWidgetPrefsManager
{
  public:
    std::vector<FCWidgetPrefsHandler*> getHandlers()
    {
      return m_aHandlers;
    }

		void append(FCWidgetPrefsHandler* handler)
		{
			if (handler)
			{
#ifdef FC_DEBUG
				if (handler->pPref->getParamGrp().IsNull())
					throw;
#endif
				handler->onRestore();
				m_aHandlers.push_back(handler);
			}
		}

  protected:
    std::vector<FCWidgetPrefsHandler*> m_aHandlers;
};

/** The FCEditSpinBox class
 */
class FCPrefSpinBox : public FCFloatSpinBox, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( QString prefEntry READ getEntryName     WRITE setEntryName     )
    Q_PROPERTY( QString prefPath  READ getParamGrpPath  WRITE setParamGrpPath  )

  public:
    FCPrefSpinBox ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCPrefSpinBox();

    // PROPERTIES
    // getters
    QString getEntryName    () const;
    QString getParamGrpPath () const;
    // setters
    void  setEntryName     (QString name);
    void  setParamGrpPath  (QString name);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 * FCLineEdit
 */
class FCLineEdit : public QLineEdit, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( QString prefEntry READ getEntryName     WRITE setEntryName     )
    Q_PROPERTY( QString prefPath  READ getParamGrpPath  WRITE setParamGrpPath  )

  public:
    FCLineEdit ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCLineEdit();

    // PROPERTIES
    // getters
    QString getEntryName    () const;
    QString getParamGrpPath () const;
    // setters
    void  setEntryName     (QString name);
    void  setParamGrpPath  (QString name);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 * FCComboBox
 */
class FCComboBox : public QComboBox, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( QString prefEntry READ getEntryName     WRITE setEntryName     )
    Q_PROPERTY( QString prefPath  READ getParamGrpPath  WRITE setParamGrpPath  )

  public:
    FCComboBox ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCComboBox();

    // PROPERTIES
    // getters
    QString getEntryName    () const;
    QString getParamGrpPath () const;
    // setters
    void  setEntryName     (QString name);
    void  setParamGrpPath  (QString name);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 * FCListBox
 */
class FCListBox : public QListBox, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( QString prefEntry READ getEntryName     WRITE setEntryName     )
    Q_PROPERTY( QString prefPath  READ getParamGrpPath  WRITE setParamGrpPath  )

  public:
    FCListBox ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    virtual ~FCListBox();

    // PROPERTIES
    // getters
    QString getEntryName    () const;
    QString getParamGrpPath () const;
    // setters
    void  setEntryName     (QString name);
    void  setParamGrpPath  (QString name);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 * FCCheckBox
 */
class FCCheckBox : public QCheckBox, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( QString prefEntry READ getEntryName     WRITE setEntryName     )
    Q_PROPERTY( QString prefPath  READ getParamGrpPath  WRITE setParamGrpPath  )

  public:
    FCCheckBox ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCCheckBox();

    // PROPERTIES
    // getters
    QString getEntryName    () const;
    QString getParamGrpPath () const;
    // setters
    void  setEntryName     (QString name);
    void  setParamGrpPath  (QString name);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 * FCRadioButton
 */
class FCRadioButton : public QRadioButton, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( QString prefEntry READ getEntryName     WRITE setEntryName     )
    Q_PROPERTY( QString prefPath  READ getParamGrpPath  WRITE setParamGrpPath  )

  public:
    FCRadioButton ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCRadioButton();

    // PROPERTIES
    // getters
    QString getEntryName    () const;
    QString getParamGrpPath () const;
    // setters
    void  setEntryName     (QString name);
    void  setParamGrpPath  (QString name);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 * FCSlider
 */
class FCSlider : public QSlider, public FCWidgetPrefs
{
    Q_OBJECT

    Q_PROPERTY( QString prefEntry READ getEntryName     WRITE setEntryName     )
    Q_PROPERTY( QString prefPath  READ getParamGrpPath  WRITE setParamGrpPath  )

  public:
    FCSlider ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCSlider();

    // PROPERTIES
    // getters
    QString getEntryName    () const;
    QString getParamGrpPath () const;
    // setters
    void  setEntryName     (QString name);
    void  setParamGrpPath  (QString name);

  protected:
    // restore from/save to parameters
    void restorePreferences();
    void savePreferences();
};

/**
 *  Class to drag a 'QAction' object
 */
class FCActionDrag : public QStoredDrag
{
  public:
    FCActionDrag ( QString action, QWidget * dragSource = 0, const char * name = 0 );
    virtual ~FCActionDrag ();

    static bool canDecode ( const QMimeSource * e );
    static bool decode ( const QMimeSource * e, QString&  action );

  public:
    static std::vector<QString> actions;
};

/**
 * This abstract class is part of the framework providing 
 * methods for a user defined configuration of toolbars and menus
 */
class FCCustomWidget : public FCWidgetPrefs
{
  public:
    /** Returns true if items are set, otherwise false */
    bool hasCustomItems();
    /** Returns a list of set items */
    std::vector<std::string> getItems();
    /** Sets the given items to current */
    void setItems(const std::vector<std::string>& items);
    /** Appends a single item */
    void addItem(const std::string& item);
    /** Calls @ref restorePreferences() 
     * So it is possible to call the load routine from outside
     */
    void loadXML();
    /** Calls @ref savePreferences()
     * So it is possible to call the save routine from outside
     */
    void saveXML();
    virtual void update(FCCommandManager& rclMgr) = 0;
    /** Returns the acive workbench at creation time of this object */
    QString getWorkbench();
    virtual ~FCCustomWidget();

  protected:
    FCCustomWidget(const char* grp, const char * name);
    /// reimplementation
    virtual void restorePreferences();
    /// reimplementation
    virtual void savePreferences();
    /// for internal use only
    void init(const char* grp, const char* name);

    std::vector<std::string> _clItems;
    QString                  _clWorkbench;
};

/**
 *  Toolbar class that knows 'drag and drop'
 */
class FCToolBar : public QToolBar, public FCCustomWidget
{
  Q_OBJECT

  public:
    FCToolBar ( const QString & label, QMainWindow *, QWidget *, bool newLine = FALSE, const char * name = 0, WFlags f = 0, const char* type = "Toolbars" );
    FCToolBar ( QMainWindow * parent = 0, const char * name = 0, const char* type = "Toolbars" );
    virtual ~FCToolBar();
    virtual void clearAll();
    void update(FCCommandManager& rclMgr);

  public:
    static bool isAllowed(QWidget* w);

  protected:
    void dropEvent ( QDropEvent * );
    void dragEnterEvent ( QDragEnterEvent * );
    void dragLeaveEvent ( QDragLeaveEvent * );
    void dragMoveEvent ( QDragMoveEvent * );
    virtual void restorePreferences();
    virtual void savePreferences();
    bool bSaveColor;
};

/**
 *  Menu class that knows 'drag and drop'
 */
class FCPopupMenu : public QPopupMenu, public FCCustomWidget
{
  Q_OBJECT

  public:
    FCPopupMenu ( QWidget * parent=0, const char * name=0, const char* menu = 0 );
    virtual ~FCPopupMenu();
    void update(FCCommandManager& rclMgr);
    virtual void OnChange(FCSubject<const char*> &rCaller, const char * sReason);

  protected:
    void dropEvent ( QDropEvent * );
    void dragEnterEvent ( QDragEnterEvent * );
    void dragLeaveEvent ( QDragLeaveEvent * );
    void dragMoveEvent ( QDragMoveEvent * );
    void mouseMoveEvent ( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    virtual void restorePreferences();
    virtual void savePreferences();
    QString parent;
    bool    bAllowDrag;
};

/**
 * Class that manages the construction/destruction of all customizable objects.
 * At destruction of a custom widget its content will be written to preferences.
 * At construction the content will be restored.
 * @see FCToolBar, FCPopupMenu and FCDockWindow
 */
class FCCustomWidgetManager
{
  public:
    FCCustomWidgetManager(FCCommandManager& rclMgr, FCStackBar* pCmdBar);
    ~FCCustomWidgetManager();

    /** Initializes the custom widgets depending on the given workbench */
    bool init(const char* workbench);
    /** Updates the custom widgets depending on the given workbench */
    bool update(const char* workbench);
    /** Clears all custom widgets and reload it from preferences */
    bool update();

    // toolbars
    /** Returns the toolbar by name
     * If it does not exist it will be created
     */
    FCToolBar* getToolBar(const char* name);
    /** returns a vector of all toolbars */
    std::vector<FCToolBar*> getToolBars();
    /** Deletes the specified toolbar if it exists */
    void delToolBar(const char* name);
    /** Get the number of toolbars */
    int countToolBars();

    // command bars
    /** Returns the command bar by name
     * If it does not exist it will be created
     */
    FCToolBar* getCmdBar(const char* name);
    /** returns a vector of all command bars */
    std::vector<FCToolBar*> getCmdBars();
    /** Deletes the specified command bar if it exists */
    void delCmdBar(const char* name);
    /** Get the number of command bars */
    int countCmdBars();

    // menus
    /** Returns the menu bar by name
     * If it does not exist it will be created. If parent is not NULL
     * the menu with the name 'parent' is the father. Otherwise
     * the menu will be put in the application's menu bar.
     */
    FCPopupMenu* getPopupMenu(const char* name, const char* parent = 0);
    /** returns a vector of all menus */
    std::vector<FCPopupMenu*> getPopupMenus();
    /** Deletes the specified menu if it exists */
    void delPopupMenu(const char* name);
    /** Get the number of menus */
    int countPopupMenus();

    // dockable windows
    /** Returns the dock window bar by name
     * If it does not exist it returns NULL
     */
    FCDockWindow* getDockWindow(const char* name);
    /** returns a vector of all dock windows */
    std::vector<FCDockWindow*> getDockWindows();
    /** Deletes the specified dock window if it exists */
    void delDockWindow(const char* name);
    /** Adds a new dock window */
		void addDockWindow(const char* name,FCDockWindow *pcDocWindow, const char* sCompanion = NULL,
                       KDockWidget::DockPosition pos = KDockWidget::DockRight, int percent = 50);

    /** Adds new menu with its items and its parent */
    void addPopupMenu (const std::string& type, const std::vector<std::string>& defIt, const char* parent = 0);
    /** Adds new menu with its parent The items are set before creation of the menu.
     * @see addItem(const std::string&)
     */
    void addPopupMenu (const std::string& type, const char* parent = 0);
    /** Adds new tool bar with its items */
    void addToolBar   (const std::string& type, const std::vector<std::string>& defIt);
    /** Adds new toolbar The items are set before creation of the menu.
     * @see addItem(const std::string&)
     */
    void addToolBar   (const std::string& type);
    /** Adds new command bar with its items */
    void addCmdBar    (const std::string& type, const std::vector<std::string>& defIt);
    /** Adds new command bar The items are set before creation of the menu.
     * @see addItem(const std::string&)
     */
    void addCmdBar    (const std::string& type);
    /** Adds a single item to temporary buffer. The item is not yet assigned
     * to a concrete custom widget at this time 
     */
    void addItem      (const std::string& item);

  protected:
    std::map <std::string,FCPopupMenu*> _clPopupMenus;
    std::map <std::string,FCToolBar*>   _clToolbars;
    std::map <std::string,FCToolBar*>   _clCmdbars;
	  std::map <std::string,FCDockWindow*>    _clDocWindows;
    std::vector<std::string>            _clDefaultItems;
    FCCommandManager&                   _clCmdMgr;
  	FCStackBar*                         _pclStackBar;
};

#endif // __FC_PREF_WIDGETS_H__
