#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

## Init stuff
# **************************************************************************
AC_PREREQ(2.57)
AC_INIT(Part,0.184,[Juergen Riegel],Part)
AM_INIT_AUTOMAKE([foreign])
AC_LANG_CPLUSPLUS

## Default install directory is your home directory
# **************************************************************************
#AC_PREFIX_DEFAULT($PWD/..)
AC_PREFIX_DEFAULT($HOME/FreeCAD/Mod/Part)

## Version number
# **************************************************************************
# We begin with 0:0:0
LIB_CURRENT=0
LIB_REVISION=0
LIB_AGE=0
AC_SUBST(LIB_CURRENT)
AC_SUBST(LIB_REVISION)
AC_SUBST(LIB_AGE)

## Required headers
## (mainly for OpenCASCADE 5.2, we need also config.h for cfg's)
# **************************************************************************
AC_LANG([C++])
AC_MSG_CHECKING([ for C++ header files ])
AC_CHECK_HEADERS(istream ostream istream fstream ios iomanip iostream)
AC_CHECK_HEADERS(iomanip.h limits.h values.h float.h)
AC_CHECK_HEADERS(siginfo.h bits/sigset.h bstring.h sys/types.h sys/select.h)
#AC_CHECK_HEADERS(X11/extensions/transovl.h X11/extensions/readdisplay.h)
#AC_CHECK_HEADERS(X11/extensions/multibuf.h)
AC_CHECK_HEADERS(sys/filio.h sys/mman.h libc.h)
AC_CHECK_HEADERS([sstream],,[AC_MSG_ERROR([This header is needed. Bye.])])


## Check for programs
# **************************************************************************
AC_PROG_CXX
AC_PROG_CC
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
AC_PROG_LIBTOOL

## *************************************************************************
##
## Checking for libraries.
##
## *************************************************************************

# checking for Python
# **************************************************************************
AC_CHECKING([Python])
fc_py_ver=`python -c "import sys; print sys.version[[:3]]"`;
if test x$fc_py_ver = x; then
	AC_MSG_ERROR([cannot find python, bye...])
fi;

AC_ARG_WITH(python-includes,
	AC_HELP_STRING([--with-python-includes], [Path of the header files of your Python installation]),
	[fc_py_incs=$withval],
	[fc_py_incs=/usr/include/python$fc_py_ver])

AC_CHECK_FILE($fc_py_incs/Python.h,,
	[AC_CHECK_FILE(/usr/local/include/python$fc_py_ver/Python.h,[fc_py_incs=/usr/local/include/python$fc_py_ver],
	[AC_MSG_ERROR([
	**** Could not find Python header files.
	 Please check your installation. ****
	 ])])])

AC_ARG_WITH(python-libs,
	AC_HELP_STRING([--with-python-libs], [Path of the library files of your Python installation]),
	[fc_py_libs=$withval],
	[fc_py_libs=/usr/lib])

AC_CHECK_FILE($fc_py_libs/libpython$fc_py_ver.so,,
	[AC_CHECK_FILE(/usr/local/lib/libpython$fc_py_ver.so,[fc_py_libs=/usr/local/lib],
	[AC_MSG_ERROR([
	**** Could not find Python library files.
	 Please check your installation. ****
	 ])])])

all_includes="$all_includes -I$fc_py_incs"
all_libraries="$all_libraries -L$fc_py_libs"
AC_MSG_RESULT([Python $fc_py_ver : $fc_py_incs : $fc_py_libs])

# checking for OpenCascade
# **************************************************************************
# Check if CASROOT is set and estimate where the includes and libs could be
if test x"$CASROOT" != x; then
  fc_occ_incs_test="$CASROOT/inc"
  fc_occ_libs_test="$CASROOT/Linux/lib"
else
  fc_occ_incs_test=/usr/include/OCC
  fc_occ_libs_test=/usr/lib/OCC
fi

AC_CHECKING([OpenCascade])
AC_ARG_WITH(OCC-includes,
	AC_HELP_STRING([--with-OCC-includes], [Path of the header files of your OpenCascade installation]),
	[fc_occ_incs=$withval],
	[fc_occ_incs=$fc_occ_incs_test])

AC_CHECK_FILE($fc_occ_incs/Standard.hxx,,
	[AC_MSG_ERROR([
	**** Could not find OpenCascade header files.
	 Please check your installation. ****
	 ])])

AC_ARG_WITH(OCC-libs,
	AC_HELP_STRING([--with-OCC-libs], [Path of the library files of your OpenCascade installation]),
	[fc_occ_libs=$withval],
	[fc_occ_libs=$fc_occ_libs_test])

AC_CHECK_FILE($fc_occ_libs/libTKernel.so,,
	[AC_MSG_ERROR([
	**** Could not find OpenCascade library files.
	 Please check your installation. ****
	 ])])

all_includes="$all_includes -I$fc_occ_incs"
all_libraries="$all_libraries -L$fc_occ_libs"
AC_MSG_RESULT([OpenCascade $fc_occ_incs : $fc_occ_libs])

# checking for xerces-c
# **************************************************************************

AC_CHECKING([xerces-c])
AC_ARG_WITH(xercesc-includes,
	AC_HELP_STRING([--with-xercesc-includes], [Path of the header files of your xerces-c installation]),
	[fc_xer_incs=$withval],
	[fc_xer_incs=/usr/include])

AC_CHECK_FILE($fc_xer_incs/xercesc,,
	[AC_CHECK_FILE(/usr/local/include/xercesc,[fc_xer_incs=/usr/local/include],
	[AC_MSG_ERROR([
	**** Could not find xerces-c header files.
	 Please check your installation. ****
	 ])])])

AC_ARG_WITH(xercesc-libs,
	AC_HELP_STRING([--with-xercesc-libs], [Path of the library files of your xerces-c installation]),
	[fc_xer_libs=$withval],
	[fc_xer_libs=/usr/lib])

AC_CHECK_FILE($fc_xer_libs/libxerces-c.so,,
	[AC_CHECK_FILE(/usr/local/lib/libxerces-c.so,[fc_xer_libs=/usr/local/lib],
	[AC_MSG_ERROR([
	**** Could not find xerces-c library files.
	 Please check your installation. ****
	 ])])])

all_includes="$all_includes -I$fc_xer_incs"
all_libraries="$all_libraries -L$fc_xer_libs"
AC_MSG_RESULT([xerces-c $fc_xer_incs : $fc_xer_libs])

# checking for Coin
# **************************************************************************

AC_CHECKING([Coin])
AC_ARG_WITH(coin-includes,
	AC_HELP_STRING([--with-coin-includes], [Path of the header files of your Coin installation]),
	[fc_coin_incs=$withval],
	[fc_coin_incs=/usr/include])

AC_CHECK_FILE($fc_coin_incs/Inventor,,
	[AC_CHECK_FILE(/usr/local/include/Inventor,[fc_coin_incs=/usr/local/include],
	[AC_MSG_ERROR([
	**** Could not find Coin header files.
	 Please check your installation. ****
	 ])])])

AC_ARG_WITH(coin-libs,
	AC_HELP_STRING([--with-coin-libs], [Path of the library files of your Coin installation]),
	[fc_coin_libs=$withval],
	[fc_coin_libs=/usr/lib])

AC_CHECK_FILE($fc_coin_libs/libCoin.so,,
	[AC_CHECK_FILE(/usr/local/lib/libCoin.so,[fc_coin_libs=/usr/local/lib],
	[AC_MSG_ERROR([
	**** Could not find Coin library files.
	 Please check your installation. ****
	 ])])])

all_includes="$all_includes -I$fc_coin_incs"
all_libraries="$all_libraries -L$fc_coin_libs"
AC_MSG_RESULT([Coin $fc_coin_incs : $fc_coin_libs])

# Alternative from Coin3D, will use this soon
#SIM_AC_HAVE_COIN_IFELSE(,AC_MSG_ERROR([Coin3D not found]))

# checking for SoQt
# **************************************************************************

AC_CHECKING([SoQt])
AC_ARG_WITH(soqt-includes,
	AC_HELP_STRING([--with-soqt-includes], [Path of the header files of your SoQt installation]),
	[fc_soqt_incs=$withval],
	[fc_soqt_incs=/usr/include])

AC_CHECK_FILE($fc_soqt_incs/Inventor/Qt,,
	[AC_CHECK_FILE(/usr/local/include/Inventor/Qt,[fc_soqt_incs=/usr/local/include],
	[AC_MSG_ERROR([
	**** Could not find SoQt header files.
	 Please check your installation. ****
	 ])])])

AC_ARG_WITH(soqt-libs,
	AC_HELP_STRING([--with-soqt-libs], [Path of the library files of your SoQt installation]),
	[fc_soqt_libs=$withval],
	[fc_soqt_libs=/usr/lib])

AC_CHECK_FILE($fc_soqt_libs/libSoQt.so,,
	[AC_CHECK_FILE(/usr/local/lib/libSoQt.so,[fc_soqt_libs=/usr/local/lib],
	[AC_MSG_ERROR([
	**** Could not find SoQt library files.
	 Please check your installation. ****
	 ])])])

all_includes="$all_includes -I$fc_soqt_incs"
all_libraries="$all_libraries -L$fc_soqt_libs"
AC_MSG_RESULT([SoQt $fc_soqt_incs : $fc_soqt_libs])

# Alternative from Coin3D, will use this soon
#SIM_AC_HAVE_SOQT_IFELSE(,AC_MSG_ERROR([SoQt not found]))

# checking for Qt (written by Bastiaan Veelo <Bastiaan.N.Veelo@ntnu.no>)
# **************************************************************************

BNV_HAVE_QT

# checking for FreeCAD base libraries
# **************************************************************************

AC_CHECKING([FreeCAD base libs])

AC_ARG_WITH(fc-libs,
	AC_HELP_STRING([--with-fc-libs], [Path of the library files of the FreeCAD base.]),
	[fc_fc_libs=$withval],
	[fc_fc_libs=../../../bin])

AC_CHECK_FILE($fc_fc_libs/libFreeCADBase.so,,
	[AC_MSG_ERROR([
	**** Could not find libFreeCADBase.so library files.
	 Please build FreeCAD first. ****
	 ])])

AC_CHECK_FILE($fc_fc_libs/libFreeCADApp.so,,
	[AC_MSG_ERROR([
	**** Could not find libFreeCADApp.so library files.
	 Please build FreeCAD first. ****
	 ])])

AC_CHECK_FILE($fc_fc_libs/libFreeCADGui.so,,
	[AC_MSG_ERROR([
	**** Could not find libFreeCADGui.so library files.
	 Please build FreeCAD first. ****
	 ])])

#all_libraries="$all_libraries -L../$fc_fc_libs"
all_libraries="$all_libraries -L$fc_fc_libs"

# **************************************************************************

AC_SUBST(all_includes)
AC_SUBST(all_libraries)

# Check if you want to have debug info, or not
# **************************************************************************

AC_ARG_ENABLE([debug],
  AC_HELP_STRING([--enable-debug], [Enable debug information]),
  [case $enableval in
    no | false) fc_set_debug=false ;;
    *)          fc_set_debug=true ;;
  esac],
  [fc_set_debug=true])

if $fc_set_debug; then
	CPPFLAGS="$CPPFLAGS -g -D__DEBUG__ -DFC_DEBUG"
else
	CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

# Check if you want thread support, or not (currently disabled, thread support is forced)
# **************************************************************************

CPPFLAGS="$CPPFLAGS -D_REENTRANT"
LDFLAGS="-lpthread"
fc_set_thread=true

#AC_ARG_ENABLE([thread],
#  AC_HELP_STRING([--enable-thread], [Enable thread support]),
#  [case $enableval in
#    no | false) fc_set_thread=false ;;
#    *)          fc_set_thread=true ;;
#  esac],
#  [fc_set_thread=true])
#if $fc_set_thread; then
#	AC_CHECK_HEADER([pthread.h])
#	AC_CHECK_LIB([pthread],[pthread_create])
#	if test "$ac_cv_header_pthread_h" != yes -o \
#        	"$ac_cv_lib_pthread_pthread_create" != yes; then
#  		AC_MSG_ERROR([POSIX threads (pthreads) not working. Bye.])
#	else
##	 	this is apparently needed for some versions of autoconf/automake
##		LIBS="-lpthread $LIBS"
##		CXXFLAGS="$CXXFLAGS -D_REENTRANT"
#		CPPFLAGS="$CPPFLAGS -D_REENTRANT"
#		LDFLAGS="-lpthread"
#	fi
#else
#	CPPFLAGS="$CPPFLAGS -DNO_FC_THREAD"
#fi

# Check if you want compiler warnings enabled, or not
# **************************************************************************

AC_ARG_ENABLE([warnings],
  AC_HELP_STRING([--enable-warnings], [Enable compiler warnings]),
  [case $enableval in
    no | false) fc_set_warn=false ;;
    *)          fc_set_warn=true ;;
  esac],
  [fc_set_warn=true])
if $fc_set_warn; then
	CPPFLAGS="$CPPFLAGS -Wall"
else
	CPPFLAGS="$CPPFLAGS -w"
fi

# **************************************************************************

# -DQT_THREAD_SUPPORT already defined in Qt test and Config.h
CPPFLAGS="$CPPFLAGS -DBaseExport= -DGuiExport= -DAppExport= -DDocExport=  -DFC_OS_LINUX"
CXX=g++
CXXFLAGS="-Wno-deprecated -frtti"
LDFLAGS="-L$all_libraries"

# Qt tools already defined in Qt test
#AC_SUBST([MOC],[moc])
#AC_SUBST([UIC],[uic])
AC_SUBST([PYTHON],[python])
AC_SUBST([fc_py_ver])

AC_CONFIG_FILES([Makefile App/Makefile Gui/Makefile])
AC_OUTPUT

# Show a configuration report
# **************************************************************************

AC_MSG_NOTICE([

  **************************************************************************

  Part module configuration settings:

  C++ compiler:              $CXX
  Python version:            $fc_py_ver
  Debug info enabled:        $fc_set_debug
  Thread enabled (forced):   $fc_set_thread
  RTTI enabled (forced):     true
  Compiler warnings enabled: $fc_set_warn
  installation prefix:       $prefix

  Now, run 'make' to build Part module.

  **************************************************************************
])

# **************************************************************************

