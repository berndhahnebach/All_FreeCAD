Changes from 1.0beta3 to 1.0.0 (release)
========================================
- some new bugfixes
- configure script to generate the Makefiles
- KDE2 supported (configure and recompile for KDE2 mode, and QextMdiMainFrm will inherit
  KMainWindow, QextMdiTaskBar will inherit KToolBar.
- another additional decoration for KDE2 added
- switch between decorations (Win32, KDE1, KDE2) of attached views possible at runtime
- example applications improved (fourChildren works in KDE2 mode as well)
- short handbook written
- documentation updated

Changes from 1.0beta2 to 1.0beta3
=================================
- new kind of MDI view possible: call QextMdiMainFrm::addWindow (using flag QextMDI::ToolView)
  to add views as toplevel stay-on-top MDI tool windows
- lots of bugfixes and misbehaviour fixes
- new signals which tell about state changes
- adaptions related to Qt-2.1.1
- class documentation completed
  
Changes from 1.0beta1 to 1.0beta2
=================================
- method interface for adding MDI views changed
- "Window" menu improved: submenu for Dock/Undock and one for window manager operations. 
- class QextMdiChildArea is protected now, there's a througput of some functionality
  from the mainframe to that one.
- the taskbar items can have different names than view titles.  
- class interfaces optimized
- The QextMdiChildView-list of the mainframe can be accessed by a special iterator now.
- TRUE and FALSE is used instead of true and false (compatibility with some compilers)
- toplevel mode is better supported now
- a lot of bugfixes, especially resizing and moving of MDI views
- a set of useful additional methods
- Info: QextMDI works with Qt-2.1 as well
- Info: KDevelop1.1 supports QextMDI by ApplicationWizard
   
Changes from 0.4 to 1.0beta1
============================
- maximizing switches to stacked SDI mode 
  (system buttons and menu moves to the application main menubar)
- minimized views are shown as mini-windows (only the captionbar)
- new "Window" menu functions: 
  "Tile vertically", "Iconify all", "Toplevel mode" and "Childframe mode"
- dragging of MDI views improved
- taskbar can be switched off
- class interfaces optimized
- Qt-1.44 support dropped (because off-topic)
- a lot of bugfixes  

Changes from 0.3 to 0.4
=======================
- Now 2 layout themes: If _OS_WIN32_ is defined, QextMDI will look like MSWindows.
  Otherwise it will look like KDE.
- A childframe window has got a system menu (that's why icon handling changed)
- The "Window" menu has changed completely, MSWindows-like (unix doesn't give a hint).
  4 new functions: switching between "Toplevel mode" and "Childframe mode", "Close All
  Views" and "Close View".
- Interface changed for public QextMdiMainFrm::addWindow(...)
- Serveral bugs fixed: shakin'windows effect, menuitem concept mistake, minor ones...

Changes from 0.2 to 0.3
=======================
- Dynamic taskbar, buttons change size dynamically, button text will be
  abbreviated
- Complete improved focus handling for child widgets of QextMdiChildView
  (every view has its own focus loop, focuses don't get lost during docking/undocking)
- A lot of focus handling bugs are fixed
- Taskbar button bases on QPushButton instead of QToolButton
- No more mouse dragging of view windows into the invisible area
- A lot of methods of QextMdiChildView and QextMdiMainFrm are virtual now
- Some minor bugs are fixed
- Documentation updated and improved
- A second example application "MDIFrameWork" showing how to create a
  complete application in MDI style (as KDevelop Qt-project)
- Source code cleaned up a little bit
- Some things I don't remember...

Changes from 0.1 to 0.2
=======================
- now additionally compatible with Qt1.44 !
- main frame can have a parent
- one can remove child views from MDI system without deleting them
- closeWindow: possible deadlock in signal/slot function stack removed
(now uses an event)
- pixmaps have got transparent background colour
- now example application "fourChildren" changed, cannot crash with X11
error (Qt bug fixed in 2.0.2) anymore
- main frame gives window menu back, one can use it to build the main
menu
- some minor bugs are fixed


