Changes from 2.0.1 to 2.0.2
===========================
- another severe focus problem fixed

Changes from 2.0.0 to 2.0.1
===========================
- focus problem hopefully solved
- QextMdiChildView::internalGeometry() fixed
- crash in QextMdiMainFrm::removeWindowFromMdi() fixed
- compiling error with KParts namespace fixed 
  (but the price is it doesn't run on Irix now)

Changes from 2.0.0 beta1 to 2.0.0
=================================
- lots of new bugfixes
- support for accels for view switching

Changes from 1.0.0 to 2.0.0 beta1
=================================
- lots of new bugfixes, especially concerning focus problems
- KDockWidgets of KDE-2.2 integrated, compile on Win32, too
- TabPage MDI mode added, (the third MDI mode!)
- Toplevel MDI mode works without focus problems
- KDE 2 default decoration and KDE 2 laptop decoration added
- optimized and speeded up
- several new state signals and useful custom events
- added faked SDI mode (just maximized view without system buttons)
- dockable tool views (using KDockWidgets)
- internal rewrite of several mechanisms
- KDevelop project file updated to KDevelop-2.0
- example applications improved
- autoconf/automake project files updated to latest version
- HTML class reference updated with latest KDoc 2.0a53
- configure script added, with -kde2 and -release flag
