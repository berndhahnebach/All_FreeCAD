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
#include "qextmdi/qextmdimainfrm.h"

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qslider.h>
#include <qtoolbar.h>

// forward declarations
class FCWidgetPrefsHandler;
class QDoubleValidator;
class FCWidgetFactorySupplier;
class QAction;
class QMainWindow;
class FCDockWindow;
class FCCommandManager;

namespace Gui {
namespace DockWnd {
class StackBar;

} // namespace DockWnd
} // namespace Gui

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
    friend class FCWidgetFactory;
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
    /** Appends several items */
    void appendItems(FCParameterGrp::handle, const std::vector<std::string>& item);
    /** Removes several items */
    void removeItems(FCParameterGrp::handle, const std::vector<std::string>& item);
    /** Calls @ref restorePreferences() 
     * So it is possible to call the load routine from outside
     */
    void loadXML();
    /** Calls @ref savePreferences()
     * So it is possible to call the save routine from outside
     */
    void saveXML();
    /** Sets the 'removable' property to b 
     * After switching to a new workbench the customizable
     * widgets of the old workbench (toolbars, command bars and menus 
     * (currently disabled)) normally will be deleted. To avoid this
     * you can set this property to 'false'. The default is 'true'.
     */
    virtual void setRemovable(bool b);
    /** Returns whether the widget can be modified or not */
    bool isRemovable() const;
    virtual void setCanModify(bool b);
    bool canModify() const;
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
    bool                     _bCanModify;
    bool                     _bCanRemovable;
    typedef std::map<FCParameterGrp::handle, std::vector<std::string> > WorkbenchItems;
    // items from other workbenches
    WorkbenchItems _clWbItems;
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
    void setCanModify(bool b);

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
    FCParameterGrp::handle hCommonGrp;
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
    FCCustomWidgetManager(FCCommandManager& rclMgr, Gui::DockWnd::StackBar* pCmdBar);
    ~FCCustomWidgetManager();

    /** Loads the custom widgets depending on the given 
     * workbench from the preferences 
     */
    bool loadCustomWidegts(const char* workbench);
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
    /** Adds new toolbar with its items 
     * After adding the new toolbar it searches for its items in 
     * the preferences. If it has found any items it uses these instead of 
     * the given in 'defIt'. If force is set to true the toolbar takes the
     * given items anyway.
     */
    void addToolBar   (const std::string& type, const std::vector<std::string>& defIt);
    /** Deletes the specified toolbar if it exists */
    void delToolBar(const char* name);
    /** Removes toolbar items */
    void removeToolBarItems (const std::string& type, const std::vector<std::string>& item);
    /** Get the number of toolbars */
    int countToolBars();

    // command bars
    /** Returns the command bar by name
     * If it does not exist it will be created
     */
    FCToolBar* getCmdBar(const char* name);
    /** returns a vector of all command bars */
    std::vector<FCToolBar*> getCmdBars();
    /** Adds new command bar with its items 
     * After adding the new command bar it searches for its items in 
     * the preferences. If it has found any items it uses these instead of 
     * the given in 'defIt'. If force is set to true the toolbar takes the
     * given items anyway.
     */
    void addCmdBar    (const std::string& type, const std::vector<std::string>& defIt);
    /** Deletes the specified command bar if it exists */
    void delCmdBar(const char* name);
    /** Removes command bar items */
    void removeCmdBarItems (const std::string& type, const std::vector<std::string>& item);
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
    /** Adds new menu with its items and its parent */
    void addPopupMenu (const std::string& type, const std::vector<std::string>& defIt, 
                       const char* parent = 0);
    /** Deletes the specified menu if it exists */
    void delPopupMenu(const char* name);
    /** Removes menu items */
    void removeMenuItems (const std::string& type, const std::vector<std::string>& item);
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

  private:
    struct FCCustomWidgetManagerP* d;
};

#endif // __FC_PREF_WIDGETS_H__
