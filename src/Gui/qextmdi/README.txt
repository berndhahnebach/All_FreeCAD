What is QextMDI?
================
- is a C++ GUI library extending Qt of Troll Tech
- It works on all systems supported by Qt because it only depends on Qt
- It works with both, Qt-2.2.4, Qt-2.3.x and Qt-3.0.x
- Recommented is Qt-3.0.1 (currently available version)

- programmed by Falk Brettschneider (falk.brettschneider@gmx.de) in 1999-2002

- gives Qt programmers the ability to create "MDI" applications
- consists of a set of C++ classes linked to a dynamic runtime library and
  you can use these classes in your project to get MDI look and style.
- is under the LGPL (see the license), which means you can use it under this 
  license and you can take part on its development, change and improve it.
- QextMDI means: 
    Qt-extension for MDI
    =  ===           ===
- web page: http://www.geocities.com/gigafalk/qextmdi.htm

What does MDI mean here?
========================
- actually: "Multi Document Interface"
- means actually to have several data structures including the possiblity
  to have multiple views on each data structure. 
- QextMDI takes the part of *visualization* for those program architectures,
  means it has nothing to do with the document objects.
- Supported modes of GUI interfaces are:
 * "Childframe mode" (Win32-like)
  - one application mainframe (the one and only application toplevel widget)
  - several child frames, each one capsulates a view on any data
  - child frames are like toplevel widgets but are clipped by their
    mainframe
 * "Toplevel mode" (Unix-like)
  - one application control panel
  - views are all toplevel widgets
 * "TabPage mode"
  - one application mainframe window
  - views are tabbed widgets

Installing the QextMDI library
==============================
- You will need tmake for the Qt 2 version!
- Extract the files
- Set the environment variable QEXTMDIDIR to the location of QextMDI
- Open a command line box and change directory to the QextMDI root directory.

Unix users:
-----------
- If you want KDE2 support:
  Set an environment variable KDE2DIR to the location of your installed KDE 2
  or to the location of KDE 3
- Run the script ./configure in the project's root directory
  (Use the flag -kde2 if you want to enable KDE2 or KDE3 support)
  (Use the flag -release if you want to compile the release version)
  -> It will generate the Makefiles
- Go to the subdirectory ./qextmdi and run make

Win32 users (MSVC5.0 or 6.0):
-----------------------------
- Set the environment variable QTVER to the version number of Qt (e.g. 224 for Qt-2.2.4)
- If you don´t have got an MSVC project file:
      tmake qextmdi.pro -o QextMDI.dsp
- Doubleclick the QextMDI.dsp file in the explorer and get the MSVC project automatically...
- Change some project settings:
      1. Check the output file
      2. Check the library list, if the names of Libs to be linked are right
      3. Change some compiler flags (Pentium optimization, ...)
- Build the library as DLL:
      Just click "Rebuild All"
- Copy the created DLL to one of the standard library folder, e.g.:
      copy lib\*.* c:\winnt\system32


Building the example applications
=================================
  e.g.: "fourChildren"

Unix users:
-----------
    ./configure [-release]
    cd ./examples/fourchildren/fourchildren
    make
    fourChildren
    
    or
    
    ./configure -kde2 [-release]
    cd ./examples/fourchildren/fourchildren
    make
    kFourChildren
    
Win32 users:
------------
    cd .\examples\fourchildren\fourchildren
    tmake fourChildren.pro -o fourChildren.dsp
    Doubleclick fourChildren.dsp in the explorer...
    You can change some project settings in the MSVC environment...
    "Rebuild all"
    "Run"


Using QextMDI in your Qt project
================================
- Read the html files in './doc' what the C++ classes are and how they work.
- You must include the header directory of QextMDI (placed in './include')
  in your project
- You must link your application/library with libqextmdi.so or QextMDI.dll
- You can take a look at the example application 'fourChildren' to see how
  to do this!


Version
=======
- Currently it is version 2.0.2

Feedback
========
- I would welcome people helping me to extend and to enlarge the project,
  and - of course - to improve it!
- Suggestions, comments are also welcome.
- Contact address: falk.brettschneider@gmx.de



Keep the idea of 
taking the best of both worlds!

Falk Brettschneider                             Dresden (Germany), 25/April/2002
