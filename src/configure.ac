#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

## Init stuff
# **************************************************************************
AC_PREREQ(2.57)
AC_INIT(FreeCAD,0.52,[Juergen Riegel],FreeCAD)
AM_INIT_AUTOMAKE([foreign])
#AC_INIT(Main/Main.cpp)
#AM_INIT_AUTOMAKE(FreeCAD, 0.52)
AM_CONFIG_HEADER([config.h])
#AC_CANONICAL_HOST
AC_LANG_CPLUSPLUS

## Default install directory is your home directory
# **************************************************************************
#AC_PREFIX_DEFAULT($PWD/..)
AC_PREFIX_DEFAULT($HOME/FreeCAD)

## Version number
# **************************************************************************
# Shared library versioning
# GENERIC_LIBRARY_VERSION=1:2:0
#                         | | |
#                  +------+ | +---+
#                  |        |     |
#               current:revision:age
#                  |        |     |
#                  |        |     +- increment if interfaces have been added
#                  |        |        set to zero if interfaces have been removed
#                                    or changed
#                  |        +- increment if source code has changed
#                  |           set to zero if current is incremented
#                  +- increment if interfaces have been removed or changed

LIB_CURRENT=2
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
#AC_STDC_HEADERS


## Check for programs
# **************************************************************************
AC_PROG_CXX
AC_PROG_CC
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
AC_PROG_LIBTOOL
AC_PROG_LN_S

#AC_ARG_VAR(DOXYGEN, Doxygen)
#AC_PATH_PROG(DOXYGEN, doxygen, no)

## Defines in config.h
# **************************************************************************
AC_DEFINE_UNQUOTED(HAVE_GETENVIRONMENTVARIABLE, 1, [Define to use GetEnvironmentVariable() instead of getenv()])
AC_DEFINE_UNQUOTED(HAVE_GL_GL_H, 1, [define if the GL header should be included as GL/gl.h])
AC_DEFINE_UNQUOTED(HAVE_QGLFORMAT_EQ_OP, 1, [Define this to 1 if operator==(QGLFormat&, QGLFormat&) is available])
AC_DEFINE_UNQUOTED(HAVE_QGLFORMAT_SETOVERLAY, 1, [Define this to 1 if QGLFormat::setOverlay() is available])
AC_DEFINE_UNQUOTED(HAVE_QGLWIDGET_SETAUTOBUFFERSWAP, 1, [Define this to 1 if QGLWidget::setAutoBufferSwap() is available])
AC_DEFINE_UNQUOTED(HAVE_QT_KEYPAD_DEFINE, 1, [Define this if Qt::Keypad is available])
AC_DEFINE_UNQUOTED(HAVE_QWIDGET_SHOWFULLSCREEN, 1, [Define this if QWidget::showFullScreen() is available])
AC_DEFINE_UNQUOTED(HAVE_SYS_TYPES_H, 1, [Define to 1 if you have the <sys/types.h> header file.])

# Check if you want to use GUI, or not (currently disabled)
# **************************************************************************
#
#AC_ARG_ENABLE([gui],
#  AC_HELP_STRING([--enable-gui], [Enable GUI (you can disable this feature to use FreeCAD in server mode)]),
#  [case $enableval in
#    no | false) fc_set_gui=false ;;
#    *)          fc_set_gui=true ;;
#  esac],
#  [fc_set_gui=true])
#
#if $fc_set_gui; then
#	echo "not yet done"
#fi

## DEFAULT INCLUDE/LIB PATHS
#all_includes="$all_includes -I/usr/include -I/usr/local/include"
#all_libraries="$all_libraries -L/usr/lib -L/usr/local/lib"

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

# checking for WildMagic3
# **************************************************************************

AC_CHECKING([WildMagic3])
AC_ARG_WITH(wildmagic-includes,
	AC_HELP_STRING([--with-wildmagic-includes], [Path of the header files of your WildMagic3 installation]),
	[fc_wm3_incs=$withval],
	[fc_wm3_incs=/usr/include/WildMagic3/Include])

AC_CHECK_FILE($fc_wm3_incs/WildMagic3.h,,
	[AC_MSG_ERROR([
	**** Could not find WildMagic3 header files.
	 Please check your installation. ****
	 ])])

AC_ARG_WITH(wildmagic-libs,
	AC_HELP_STRING([--with-wildmagic-libs], [Path of the library files of your WildMagic3 installation]),
	[fc_wm3_libs=$withval],
	[fc_wm3_libs=/usr/lib])

AC_CHECK_FILE($fc_wm3_incs/Wm3Query.h,fc_wm3_ver=340,fc_wm3_ver=330)

# Set the appropriate library name
AC_MSG_RESULT(["Test Wm3 version"])
if test x"$fc_wm3_ver" = x"330"; then
	fc_wm3_libs="-L$fc_wm3_libs -lWildMagic3"
	fc_wm3_text="Version <= 3.3.0"
else
	fc_wm3_libs="-L$fc_wm3_libs -lWm3Foundation"
	fc_wm3_text="Version >= 3.4.0"
fi

AC_SUBST([WM3_CFLAGS], [-I$fc_wm3_incs])
AC_SUBST([WM3_LIBS], [$fc_wm3_libs])
AC_SUBST([WM3_VERSION], [$fc_wm3_ver])

AC_MSG_RESULT([WildMagic3 ($fc_wm3_text) $WM3_CFLAGS : $WM3_LIBS])

# checking for GTS
# **************************************************************************

fc_gts_incs=`gts-config --cflags`
fc_gts_libs=`gts-config --libs`

AC_SUBST([GTS_CFLAGS], [$fc_gts_incs])
AC_SUBST([GTS_LIBS], [$fc_gts_libs])

#AM_PATH_GLIB_2_0(2.4.0, , AC_MSG_ERROR([GLib not found]), gthread gmodule)
#AM_PATH_GTS(,AC_MSG_ERROR([GTS not found]))
AC_MSG_RESULT([GTS Library $GTS_CFLAGS : $GTS_LIBS])

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

#AC_PATH_PROG(COINCONFIG, [coin-config])
#
#if test "x$COINCONFIG" = x; then
#	AC_ERROR([Could not find coin-config])
#fi
#
#coin_cxxflags=`$COINCONFIG --cxxflags`
#coin_cppflags=`$COINCONFIG --cppflags`
#dnl Strip away any optimization
#coin_cxxflags=`echo \$coin_cxxflags | sed 's/-O[[[:digit:]]]//g'`
#coin_cppflags=`echo \$coin_cppflags | sed 's/-O[[[:digit:]]]//g'`
#coin_ldflags=`$COINCONFIG --ldflags`
#coin_libs=`$COINCONFIG --libs`
#
#AC_SUBST(coin_cxxflags)
#AC_SUBST(coin_cppflags)
#AC_SUBST(coin_ldflags)
#AC_SUBST(coin_libs)

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

#AC_PATH_PROG(SOQTCONFIG, [soqt-config])
#
# if test "x$SOQTCONFIG" = x; then
# 	AC_ERROR([Could not find soqt-config])
# fi
# soqt_cxxflags=`$SOQTCONFIG --cxxflags`
# soqt_cppflags=`$SOQTCONFIG --cppflags`
# dnl Strip away any opti.
# soqt_cxxflags=`echo \$soqt_cxxflags | sed 's/-O[[[:digit:]]]//g'`
# soqt_cppflags=`echo \$soqt_cppflags | sed 's/-O[[[:digit:]]]//g'`
# soqt_ldflags=`$SOQTCONFIG --ldflags`
# soqt_libs=`$SOQTCONFIG --libs`
#
# AC_SUBST(soqt_cxxflags)
# AC_SUBST(soqt_cppflags)
# AC_SUBST(soqt_ldflags)
# AC_SUBST(soqt_libs)
#
# if test x"${with_inventor+set}" = x"set" &&
#    test x"${with_inventor}" != x"no"; then
#   SIM_AC_HAVE_INVENTOR_IFELSE([
#     CPPFLAGS="$sim_ac_inventor_cppflags $CPPFLAGS"
#     LDFLAGS="$sim_ac_inventor_ldflags $LDFLAGS"
#     LIBS="$sim_ac_inventor_libs $LIBS"
#   ], [AC_MSG_ERROR(couldn't compile and link against Open Inventor)])
#   SIM_AC_CONFIGURATION_SETTING([Open Inventor brand], [SGI/TGS Inventor])
# else
#   SIM_AC_HAVE_COIN_IFELSE([
#     CPPFLAGS="$CPPFLAGS $sim_ac_coin_cppflags"
#     LDFLAGS="$LDFLAGS $sim_ac_coin_ldflags"
#     LIBS="$sim_ac_coin_libs $LIBS"
#   ], [AC_MSG_ERROR(couldn't compile and link against Coin)])
#   SIM_AC_CONFIGURATION_SETTING([Open Inventor brand], [Coin])
# fi
#
# Check for SoQt
#
# SIM_AC_HAVE_SOQT_IFELSE([
#     CPPFLAGS="$CPPFLAGS $sim_ac_soqt_cppflags"
#     LDFLAGS="$LDFLAGS $sim_ac_soqt_ldflags"
#
#     LIBS="$sim_ac_coin_libs $LIBS"
#   ], [AC_MSG_ERROR(couldn't compile and link against SoQt)])
#   SIM_AC_CONFIGURATION_SETTING([Open Inventor brand], [SoQt])
#

# Alternative from Coin3D, will use this soon
#SIM_AC_HAVE_SOQT_IFELSE(,AC_MSG_ERROR([SoQt not found]))

# checking for Qt (written by Bastiaan Veelo <Bastiaan.N.Veelo@ntnu.no>)
# **************************************************************************

BNV_HAVE_QT

# check QT_MOC
if test x$QT_MOC = x; then
	AC_MSG_ERROR([Cannot find Qt meta object compiler (moc), bye...])
fi;
# check QT_UIC
if test x$QT_UIC = x; then
	AC_MSG_ERROR([Cannot find Qt user interface compiler (uic), bye...])
fi;

# **************************************************************************

AC_SUBST(all_includes)
AC_SUBST(all_libraries)

# Check if you want to have log info, or not
# **************************************************************************

AC_ARG_ENABLE([loginfo],
  AC_HELP_STRING([--enable-loginfo], [Enable log information (disabled by default)]),
  [case $enableval in
    no | false) fc_set_loginfo=false ;;
    *)          fc_set_loginfo=true ;;
  esac],
  [fc_set_loginfo=false])

if $fc_set_loginfo; then
	CPPFLAGS="$CPPFLAGS -DFC_DEBUG"
fi

# Check if you want debug information enabled, or not
# **************************************************************************

AC_ARG_ENABLE([debug],
  AC_HELP_STRING([--enable-debug], [Enable debug information (enabled by default)]),
  [case $enableval in
    no | false) fc_set_debug=false ;;
    *)          fc_set_debug=true ;;
  esac],
  [fc_set_debug=true])
if $fc_set_debug; then
	CPPFLAGS="$CPPFLAGS -g -D_DEBUG"
else
	CPPFLAGS="$CPPFLAGS -O2 -DNDEBUG"
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
  AC_HELP_STRING([--enable-warnings], [Enable compiler warnings (enabled by default)]),
  [case $enableval in
    no | false) fc_set_warn=false ;;
    *)          fc_set_warn=true ;;
  esac],
  [fc_set_warn=true])
if $fc_set_warn; then
#	CPPFLAGS="$CPPFLAGS -Wall -W"
	CPPFLAGS="$CPPFLAGS -Wall"
else
	CPPFLAGS="$CPPFLAGS -w"
fi

# Check if you want precompiled headers (PCH) enabled, or not (currently not used)
# **************************************************************************
#
#AC_ARG_ENABLE(pch,
#  [  --enable-pch            enable precompiled headers (gcc 3.4 required)],[
#    case "$enableval" in
#      "yes")
#        MUSECXXFLAGS="$MUSECXXFLAGS -Winvalid-pch"
#        PCH="yes"
#        AC_SUBST(PCH)
#        AC_DEFINE(PCH, 1, [whether or not we use precompiled headers])
#        ;;
#      "no")
#        ;;
#      *)
#        AC_MSG_ERROR([must use --enable-pch(=yes/no) or --disable-pch])
#        ;;
#    esac
#  ])
#AM_CONDITIONAL(PCH, test "$PCH" = "yes")

# **************************************************************************

# -DQT_THREAD_SUPPORT already defined in Qt test and Config.h
CPPFLAGS="$CPPFLAGS -DBaseExport= -DGuiExport= -DAppExport= -DDocExport=  -DFC_OS_LINUX"
CXX=g++
CXXFLAGS="-Wno-deprecated -frtti"
LDFLAGS="-L$all_libraries"

#AC_SUBST(CXXFLAGS)
#AC_SUBST(LDFLAGS)
#AC_SUBST(CFLAGS)
#AC_SUBST(FFLAGS)

# Qt tools already defined in Qt test
#AC_SUBST([MOC],[moc])
#AC_SUBST([UIC],[uic])
AC_SUBST([PYTHON],[python])
AC_SUBST([fc_py_ver])

AC_CONFIG_FILES([Makefile App/Makefile Base/Makefile Gui/Makefile Main/Makefile
Mod/Makefile
Mod/Part/Makefile
Mod/Part/App/Makefile
Mod/Part/Gui/Makefile
Mod/Raytracing/Makefile
Mod/Raytracing/App/Makefile
Mod/Raytracing/Gui/Makefile
Mod/Mesh/Makefile
Mod/Mesh/App/Makefile
Mod/Mesh/Gui/Makefile
Mod/Points/Makefile
Mod/Points/App/Makefile
Mod/Points/Gui/Makefile
Mod/Image/Makefile
Mod/Image/App/Makefile
Mod/Image/Gui/Makefile
Mod/Test/Makefile
Tools/Makefile
Tools/_TEMPLATE_/Makefile
Tools/_TEMPLATE_/App/Makefile
Tools/_TEMPLATE_/Gui/Makefile
debian/Makefile
])

AC_OUTPUT

# Show a configuration report
# **************************************************************************

AC_MSG_NOTICE([

  **************************************************************************

  FreeCAD configuration settings:

  C++ compiler:              $CXX
  Python version:            $fc_py_ver
  Log info enabled:          $fc_set_loginfo
  Debug info enabled:        $fc_set_debug
  Thread enabled (forced):   $fc_set_thread
  RTTI enabled (forced):     true
  Compiler warnings enabled: $fc_set_warn
  installation prefix:       $prefix

  Now, run 'make' to build FreeCAD.

  **************************************************************************
])

# **************************************************************************

