dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

dnl Init stuff
dnl **************************************************************************
dnl release version number info
m4_define([FREECAD_MAJOR], [0])
m4_define([FREECAD_MINOR], [5])
m4_define([FREECAD_MICRO], [4])

AC_INIT(FreeCAD,[FREECAD_MAJOR.FREECAD_MINOR.FREECAD_MICRO],[werner.wm.mayer@gmx.de], FreeCAD)
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])
AC_LANG_CPLUSPLUS

dnl Default install directory is your home directory
dnl **************************************************************************
AC_PREFIX_DEFAULT($HOME/FreeCAD)

dnl Version number
dnl **************************************************************************
dnl Shared library versioning
dnl GENERIC_LIBRARY_VERSION=1:2:0
dnl                         | | |
dnl                  +------+ | +---+
dnl                  |        |     |
dnl               current:revision:age
dnl                  |        |     |
dnl                  |        |     +- increment if interfaces have been added
dnl                  |        |        set to zero if interfaces have been removed
dnl                  |        |        or changed
dnl                  |        +- increment if source code has changed
dnl                  |           set to zero if current is incremented
dnl                  +- increment if interfaces have been added, removed or changed

LIB_CURRENT=2
LIB_REVISION=0
LIB_AGE=0
AC_SUBST(LIB_CURRENT)
AC_SUBST(LIB_REVISION)
AC_SUBST(LIB_AGE)

dnl Required headers
dnl (mainly for OpenCASCADE 5.2, we need also config.h for cfg's)
dnl **************************************************************************
AC_LANG([C++])
AC_MSG_CHECKING([ for C++ header files ])
AC_CHECK_HEADERS(istream ostream istream fstream ios iomanip iostream)
AC_CHECK_HEADERS(iomanip.h limits.h values.h float.h)
AC_CHECK_HEADERS(siginfo.h bits/sigset.h bstring.h sys/types.h sys/select.h)
AC_CHECK_HEADERS(sys/filio.h sys/mman.h libc.h)
AC_CHECK_HEADERS([sstream],,[AC_MSG_ERROR([This header is needed. Bye.])])

AC_CXX_HAVE_STD_IOSTREAM

dnl Checking for programs
dnl **************************************************************************
AC_PROG_CXX
AC_PROG_CC
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_LN_S

dnl Defines in config.h
dnl **************************************************************************
AC_DEFINE_UNQUOTED(HAVE_GETENVIRONMENTVARIABLE, 1, [Define to use GetEnvironmentVariable() instead of getenv()])
AC_DEFINE_UNQUOTED(HAVE_GL_GL_H, 1, [define if the GL header should be included as GL/gl.h])
AC_DEFINE_UNQUOTED(HAVE_QGLFORMAT_EQ_OP, 1, [Define this to 1 if operator==(QGLFormat&, QGLFormat&) is available])
AC_DEFINE_UNQUOTED(HAVE_QGLFORMAT_SETOVERLAY, 1, [Define this to 1 if QGLFormat::setOverlay() is available])
AC_DEFINE_UNQUOTED(HAVE_QGLWIDGET_SETAUTOBUFFERSWAP, 1, [Define this to 1 if QGLWidget::setAutoBufferSwap() is available])
AC_DEFINE_UNQUOTED(HAVE_QT_KEYPAD_DEFINE, 1, [Define this if Qt::Keypad is available])
AC_DEFINE_UNQUOTED(HAVE_QWIDGET_SHOWFULLSCREEN, 1, [Define this if QWidget::showFullScreen() is available])
AC_DEFINE_UNQUOTED(HAVE_SYS_TYPES_H, 1, [Define to 1 if you have the <sys/types.h> header file.])
AC_DEFINE_UNQUOTED(USE_STD_IOSTREAM, 1, [Define to 1 to build zipios++ sources with iostream.])

dnl Check if you want to use GUI, or not (currently disabled)
dnl **************************************************************************
dnl
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
#
#if test "x$use_glx" = "xyes"; then
#  AM_CONDITIONAL(FREECAD_BUILD_GUI, true)
#else
#  AM_CONDITIONAL(FREECAD_BUILD_GUI, false)
#fi
#if FREECAD_BUILD_GUI
# ... in Makefile.am
#endif

## DEFAULT INCLUDE/LIB PATHS
#all_includes="$all_includes -I/usr/include -I/usr/local/include"
#all_libraries="$all_libraries -L/usr/lib -L/usr/local/lib"

dnl *************************************************************************
dnl
dnl Checking for libraries.
dnl
dnl *************************************************************************

dnl checking for zlib
dnl **************************************************************************
AC_CHECK_LIB(z, inflate, [cv_libz=yes], [cv_libz=no])

if test "$cv_libz" != "yes"; then
  AC_MSG_ERROR([
  **** Cannot find the zlib library. ****
  ])
fi

dnl checking for Python
dnl **************************************************************************
AC_MSG_CHECKING([for Python])
fc_py_ver=`python -c "import sys; print sys.version[[:3]]"`;
if test x$fc_py_ver = x; then
	AC_MSG_ERROR([
	**** Cannot find Python interpreter. ****
  ])
fi;

AC_ARG_WITH(python-includes,
	AC_HELP_STRING([--with-python-includes], [Path of the header files of your Python installation]),
	[fc_py_incs=$withval],
	[fc_py_incs=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc()"`])

AC_ARG_WITH(python-libs,
	AC_HELP_STRING([--with-python-libs], [Path of the library files of your Python installation]),
	[fc_py_libs=$withval],
	[fc_py_libs=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_config_var('LIBDIR')"`])

fc_py_ac_save_cppflags=$CPPFLAGS
fc_py_ac_save_ldflags=$LDFLAGS
fc_py_ac_save_libs=$LIBS
CPPFLAGS="$CPPFLAGS -I$fc_py_incs"
LDFLAGS="$LDFLAGS -L$fc_py_libs"
LIBS="-lpython$fc_py_ver"

fc_cv_lib_py_avail=no
AC_CHECK_HEADER(Python.h,[
AC_MSG_CHECKING([whether libpython$fc_py_ver is available])
AC_TRY_LINK([#include "Python.h"],
	[Py_Initialize(); Py_Finalize();],
	[fc_cv_lib_py_avail=yes
	AC_MSG_RESULT(yes)],
	AC_MSG_RESULT(no))])

CPPFLAGS=$fc_py_ac_save_cppflags
LDFLAGS=$fc_py_ac_save_ldflags
LIBS=$fc_py_ac_save_libs

if test x"$fc_cv_lib_py_avail" = xyes; then
	all_includes="$all_includes -I$fc_py_incs"
	all_libraries="$all_libraries -L$fc_py_libs"

	AC_SUBST([PYTHON],[python])
	AC_SUBST([PYTHON_LIB], [python$fc_py_ver])
else
	AC_MSG_ERROR([
	**** Cannot find Python devel files. ****
	])
fi

dnl checking for xerces-c
dnl **************************************************************************
AC_MSG_CHECKING([for xerces-c])
AC_ARG_WITH(xercesc-includes,
	AC_HELP_STRING([--with-xercesc-includes], [Path of the header files of your xerces-c installation]),
	[fc_xer_incs=$withval],
	[fc_xer_incs=/usr/include])

AC_CHECK_FILE($fc_xer_incs/xercesc,,
	[AC_CHECK_FILE(/usr/local/include/xercesc,[fc_xer_incs=/usr/local/include])])

AC_ARG_WITH(xercesc-libs,
	AC_HELP_STRING([--with-xercesc-libs], [Path of the library files of your xerces-c installation]),
	[fc_xer_libs=$withval],
	[fc_xer_libs=/usr/lib])

fc_xer_ac_save_cppflags=$CPPFLAGS
fc_xer_ac_save_ldflags=$LDFLAGS
fc_xer_ac_save_libs=$LIBS
CPPFLAGS="$CPPFLAGS -I$fc_xer_incs"
LDFLAGS="$LDFLAGS -L$fc_xer_libs"
LIBS="-lxerces-c"

fc_cv_lib_xer_avail=no
AC_CHECK_HEADER(xercesc/framework/XMLBuffer.hpp,[
AC_MSG_CHECKING([whether xerces lib is available])
AC_TRY_LINK([#include <xercesc/framework/XMLBuffer.hpp>],
	[XERCES_CPP_NAMESPACE_USE
	 XMLBuffer buf;],
	[fc_cv_lib_xer_avail=yes
	AC_MSG_RESULT(yes)],
	AC_MSG_RESULT(no))])

CPPFLAGS=$fc_xer_ac_save_cppflags
LDFLAGS=$fc_xer_ac_save_ldflags
LIBS=$fc_xer_ac_save_libs

if test x"$fc_cv_lib_xer_avail" = xyes; then
	all_includes="$all_includes -I$fc_xer_incs"
	all_libraries="$all_libraries -L$fc_xer_libs"
else
	AC_MSG_ERROR([
	**** Cannot find xerces devel files. ****
   ])
fi

dnl checking for Qt (Linux part written by Bastiaan Veelo <Bastiaan.N.Veelo@ntnu.no>)
dnl **************************************************************************

case `uname -s` in 
CYGWIN*) 
AC_MSG_CHECKING([for Qt])

fc_qt_incs=""
fc_qt_ldflags=""
fc_qt_libs=""
fc_qt_dir=""
AC_PATH_PROG(PKGCONFIG, pkg-config)
if test x"$PKGCONFIG" != x; then
	fc_qt_incs=`$PKGCONFIG qt-mt --cflags-only-I`
	fc_qt_ldflags=`$PKGCONFIG qt-mt --libs-only-L`
	fc_qt_libs=-lqt-mt
fi

if test x"$fc_qt_incs" = x; then
	if test x"$QTDIR" != x; then
		fc_qt_incs=-I/usr/include/qt3
		fc_qt_ldflags=-L$QTDIR/lib
		fc_qt_libs=-lqt-mt
	fi
fi

fc_qt_ac_save_cppflags=$CPPFLAGS
fc_qt_ac_save_ldflags=$LDFLAGS
fc_qt_ac_save_libs=$LIBS
CPPFLAGS="$CPPFLAGS $fc_qt_incs"
LDFLAGS="$LDFLAGS $fc_qt_ldflags"
LIBS="$fc_qt_libs"

fc_cv_lib_qt_avail=no
AC_CHECK_HEADER(qapplication.h,[
AC_MSG_CHECKING([whether Qt lib is available])
AC_TRY_LINK([#include <qapplication.h>],
	[
	 qDebug("App is running...");],
	[fc_cv_lib_qt_avail=yes
	AC_MSG_RESULT(yes)],
	AC_MSG_RESULT(no))])

CPPFLAGS=$fc_qt_ac_save_cppflags
LDFLAGS=$fc_qt_ac_save_ldflags
LIBS=$fc_qt_ac_save_libs

if test x"$fc_cv_lib_qt_avail" = xyes; then
  QT_CXXFLAGS="$fc_qt_incs"
  QT_LIBS="$fc_qt_ldflags $fc_qt_libs"
else
	AC_MSG_ERROR([
	**** Cannot find Qt devel files. ****
   ])
fi

if test x"$QTDIR" != x; then
	fc_ac_path=$QTDIR/bin:$PATH
	AC_PATH_PROG(QT_UIC, uic, false, $fc_ac_path)
	AC_PATH_PROG(QT_MOC, moc, false, $fc_ac_path)
else
if test x"$QT_UIC" != xfalse; then
	AC_MSG_WARN([QTDIR environment variable not set -- this might be an indication of a problem])
fi
fi

;;
*) BNV_HAVE_QT ;;
esac

dnl check QT_MOC
if test x$QT_MOC = x; then
	AC_MSG_ERROR([Cannot find Qt meta object compiler (moc), bye...])
fi;
dnl check QT_UIC
if test x$QT_UIC = x; then
	AC_MSG_ERROR([Cannot find Qt user interface compiler (uic), bye...])
fi;

dnl checking for Coin
dnl **************************************************************************

SIM_AC_HAVE_COIN_IFELSE(,AC_MSG_ERROR([
	**** Cannot find Coin devel files. ****
   ]))

AC_SUBST([sim_ac_coin_includedir])
AC_SUBST([sim_ac_coin_cppflags])
AC_SUBST([sim_ac_coin_ldflags])
AC_SUBST([sim_ac_coin_libs])

dnl checking for SoQt
dnl **************************************************************************

SIM_AC_HAVE_SOQT_IFELSE(,AC_MSG_ERROR([
	**** Cannot find SoQt devel files. ****
   ]))

AC_SUBST([sim_ac_soqt_includedir])
AC_SUBST([sim_ac_soqt_cppflags])
AC_SUBST([sim_ac_soqt_ldflags])
AC_SUBST([sim_ac_soqt_libs])

dnl **************************************************************************
dnl checking for optional libraies
dnl **************************************************************************

dnl checking for OpenCascade
dnl **************************************************************************
dnl Check if CASROOT is set and estimate where the includes and libs could be
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

AC_ARG_WITH(OCC-libs,
	AC_HELP_STRING([--with-OCC-libs], [Path of the library files of your OpenCascade installation]),
	[fc_occ_libs=$withval],
	[fc_occ_libs=$fc_occ_libs_test])

fc_occ_ac_save_cppflags=$CPPFLAGS
fc_occ_ac_save_ldflags=$LDFLAGS
fc_occ_ac_save_libs=$LIBS
CPPFLAGS="$CPPFLAGS -I$fc_occ_incs"
LDFLAGS="$LDFLAGS -L$fc_occ_libs"
LIBS="-lTKernel"

fc_cv_lib_occ_avail=no
AC_CHECK_HEADER(Standard.hxx,[
AC_MSG_CHECKING([whether OCC libs are available])
AC_TRY_LINK([#define HAVE_IOSTREAM 1
	#include <gp_Pnt.hxx>],
	[gp_Pnt pt(0,0,0);],
	[fc_cv_lib_occ_avail=yes
	AC_MSG_RESULT(yes)],
	AC_MSG_RESULT(no))])

CPPFLAGS=$fc_occ_ac_save_cppflags
LDFLAGS=$fc_occ_ac_save_ldflags
LIBS=$fc_occ_ac_save_libs

if test x"$fc_cv_lib_occ_avail" = xyes; then
	AC_SUBST([OCC_INC], [$fc_occ_incs])
	AC_SUBST([OCC_LIB], [$fc_occ_libs])
else
	AC_MSG_WARN([
	**** Cannot find OpenCASCADE devel files.
	Modules that depend on this library cannot be built. ****
  ])
fi

AM_CONDITIONAL(OCC_LIBRARY_FILES, test x"$fc_cv_lib_occ_avail" = xyes)

dnl checking for WildMagic3
dnl **************************************************************************
AC_CHECKING([WildMagic3])
AC_ARG_WITH(wildmagic-includes,
	AC_HELP_STRING([--with-wildmagic-includes], [Path of the header files of your WildMagic3 installation]),
	[fc_wm3_incs=$withval],
	[fc_wm3_incs=/usr/include/WildMagic3/Include])

AC_ARG_WITH(wildmagic-libs,
	AC_HELP_STRING([--with-wildmagic-libs], [Path of the library files of your WildMagic3 installation]),
	[fc_wm3_libs=$withval],
	[fc_wm3_libs=/usr/lib])

AC_CHECK_FILE($fc_wm3_incs/Wm3Query.h,fc_wm3_ver=340,fc_wm3_ver=330)

dnl Set the appropriate library name
if test x"$fc_wm3_ver" = x"330"; then
	fc_wm3_lib="WildMagic3"
	fc_wm3_txt="Version <= 3.3.0"
else
	fc_wm3_lib="Wm3Foundation"
	fc_wm3_txt="Version >= 3.4.0"
fi

fc_wm3_ac_save_cppflags=$CPPFLAGS
fc_wm3_ac_save_ldflags=$LDFLAGS
fc_wm3_ac_save_libs=$LIBS
CPPFLAGS="$CPPFLAGS -I$fc_wm3_incs"
LDFLAGS="$LDFLAGS -L$fc_wm3_libs"
LIBS="-l$fc_wm3_lib"

fc_cv_lib_wm3_avail=no
AC_CHECK_HEADER(Wm3Math.h,[
AC_MSG_CHECKING([whether WildMagic lib is available])
AC_TRY_LINK([#include <Wm3Math.h>],
	[double val = Wm3::Math<double>::Cos(0.0);],
	[fc_cv_lib_wm3_avail=yes
	AC_MSG_RESULT(yes)],
	AC_MSG_RESULT(no))])

CPPFLAGS=$fc_wm3_ac_save_cppflags
LDFLAGS=$fc_wm3_ac_save_ldflags
LIBS=$fc_wm3_ac_save_libs

if test x"$fc_cv_lib_wm3_avail" = xyes; then
	AC_SUBST([WM3_CFLAGS], [-I$fc_wm3_incs])
	AC_SUBST([WM3_LDFLAGS], [-L$fc_wm3_libs])
	AC_SUBST([WM3_LIBS], [-l$fc_wm3_lib])
	AC_SUBST([WM3_VERSION], [$fc_wm3_ver])
else
	AC_MSG_WARN([
	**** Cannot find WildMagic devel files.
	Modules that depend on this library cannot be built. ****
  ])
fi

AM_CONDITIONAL(WM3_LIBRARY_FILES, test x"$fc_cv_lib_wm3_avail" = xyes)

dnl checking for GTS
dnl **************************************************************************

AC_PATH_PROG(GTSCONFIG, gts-config, false, $PATH)
if test x"$GTSCONFIG" != xfalse; then
	fc_gts_incs=`gts-config --cflags`
	fc_gts_libs=`gts-config --libs`

	AC_SUBST([GTS_CFLAGS], [$fc_gts_incs])
	AC_SUBST([GTS_LIBS], [$fc_gts_libs])
else
	AC_MSG_WARN([
	**** Cannot find GTS library files.
	Modules that depend on this library cannot be built. ****
  ])
fi

AM_CONDITIONAL(GTS_LIBRARY_FILES, test x"$GTSCONFIG" != xfalse)

dnl **************************************************************************

AC_SUBST(all_includes)
AC_SUBST(all_libraries)

dnl Check if you want to have log info, or not
dnl **************************************************************************

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

dnl Check if you want debug information enabled, or not
dnl **************************************************************************

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

dnl Check if you want thread support, or not (currently disabled, thread support is forced)
dnl **************************************************************************

CPPFLAGS="$CPPFLAGS -D_REENTRANT"
LDFLAGS="$LDFLAGS -lpthread"
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

dnl Check if you want compiler warnings enabled, or not
dnl **************************************************************************

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

dnl Check if you want precompiled headers (PCH) enabled, or not (currently not used)
dnl **************************************************************************
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

dnl **************************************************************************

CXX=g++
CXXFLAGS="-Wno-deprecated -frtti"

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

dnl Show a configuration report
dnl **************************************************************************

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

dnl **************************************************************************

