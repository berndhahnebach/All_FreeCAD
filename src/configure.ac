#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(FreeCAD,0.184,[Juergen Riegel],FreeCAD)
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([autoconfig.h])
#AC_CANONICAL_HOST
AC_LANG_CPLUSPLUS

#AC_PREFIX_DEFAULT($PWD/..)
AC_PREFIX_DEFAULT($HOME/FreeCAD)

## Version number
##############################
# We begin with 0:0:0
LIB_CURRENT=0
LIB_REVISION=0
LIB_AGE=0
AC_SUBST(LIB_CURRENT)
AC_SUBST(LIB_REVISION)
AC_SUBST(LIB_AGE)

## REQUIRED HEADERS
##############################
#AC_CHECK_HEADERS([sstream argp.h],,[AC_MSG_ERROR([This header is needed. Bye.])])
AC_CHECK_HEADERS([strstream.h],,[AC_MSG_ERROR([This header is needed. Bye.])])


## CHECKS FOR PROGRAMS
##############################
AC_PROG_CXX
AC_PROG_CC
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
AC_PROG_LIBTOOL

## SET DEFINES
##############################
AC_DEFINE_UNQUOTED(HAVE_GETENVIRONMENTVARIABLE, 1, [Define to use GetEnvironmentVariable() instead of getenv()])
AC_DEFINE_UNQUOTED(HAVE_GL_GL_H, 1, [define if the GL header should be included as GL/gl.h])
AC_DEFINE_UNQUOTED(HAVE_QGLFORMAT_EQ_OP, 1, [Define this to 1 if operator==(QGLFormat&, QGLFormat&) is available])
AC_DEFINE_UNQUOTED(HAVE_QGLFORMAT_SETOVERLAY, 1, [Define this to 1 if QGLFormat::setOverlay() is available])
AC_DEFINE_UNQUOTED(HAVE_QGLWIDGET_SETAUTOBUFFERSWAP, 1, [Define this to 1 if QGLWidget::setAutoBufferSwap() is available])
AC_DEFINE_UNQUOTED(HAVE_QT_KEYPAD_DEFINE, 1, [Define this if Qt::Keypad is available])
AC_DEFINE_UNQUOTED(HAVE_QWIDGET_SHOWFULLSCREEN, 1, [Define this if QWidget::showFullScreen() is available])
AC_DEFINE_UNQUOTED(HAVE_SYS_TYPES_H, 1, [Define to 1 if you have the <sys/types.h> header file.])
AC_DEFINE_UNQUOTED(SCI_LEXER,1, [Define if you want to use the lexer framework])

## DEFAULT INCLUDE/LIB PATHS
all_includes="$all_includes -I/usr/include -I/usr/local/include"
all_libraries="$all_libraries -L/usr/lib -L/usr/local/lib"


#rumor_alsa=no
### ALSA
###############################
#AC_CHECK_HEADER([alsa/asoundlib.h])
#AC_CHECK_LIB([asound],[snd_seq_open])
#if test "$ac_cv_header_alsa_asoundlib_h" = "yes" \
#      -a "$ac_cv_lib_asound_snd_seq_open" = "yes"; then
#  AC_DEFINE(HAVE_ALSA,,[ALSA is usable (for MIDI I/O)])
#  LIBS="-lasound $LIBS"
#  rumor_alsa=yes
#fi

#rumor_oss=no
## OSS
##############################
#AC_CHECK_HEADER([linux/soundcard.h])
#if test "$ac_cv_header_linux_soundcard_h" = yes; then
#  AC_DEFINE(HAVE_OSS,,[OSS is usable (for MIDI I/O)])
#  rumor_oss=yes
#fi

#rumor_guile=no
## GUILE
##############################
#AC_CHECK_HEADER([guile/gh.h])
#AC_CHECK_LIB([guile],[gh_enter])
#if test "$ac_cv_header_guile_gh_h" = "yes" \
#      -a "$ac_cv_lib_guile_gh_enter" = "yes"; then
#  AC_DEFINE(HAVE_GUILE,,[guile is usable (for scripting)])
#  LIBS="-lguile $LIBS"
#  rumor_guile=yes
#fi

##
## Checking for libraries.
##############################

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

AC_CHECKING([OpenCascade])
AC_ARG_WITH(OCC-includes,
	AC_HELP_STRING([--with-OCC-includes], [Path of the header files of your OpenCascade installation]),
	[fc_occ_incs=$withval],
	[fc_occ_incs=/usr/include/OCC])

AC_CHECK_FILE($fc_occ_incs/Standard.hxx,,
	[AC_MSG_ERROR([
	**** Could not find OpenCascade header files.
	 Please check your installation. ****
	 ])])

AC_ARG_WITH(OCC-libs,
	AC_HELP_STRING([--with-OCC-libs], [Path of the library files of your OpenCascade installation]),
	[fc_occ_libs=$withval],
	[fc_occ_libs=/usr/lib/OCC])

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

# checking for SoQt
# **************************************************************************

AC_CHECKING([SoQt])
AC_ARG_WITH(soqt-includes,
	AC_HELP_STRING([--with-soqt-includes], [Path of the header files of your SoQt installation]),
	[fc_soqt_incs=$withval],
	[fc_soqt_incs=/usr/include])

AC_CHECK_FILE($fc_coin_incs/Inventor/Qt,,
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

# **************************************************************************

AC_SUBST(all_includes)
AC_SUBST(all_libraries)

# Check if you want to use GUI, or not
# **************************************************************************

AC_ARG_ENABLE([gui],
  AC_HELP_STRING([--enable-gui], [Enable GUI (you can disable this feature to use FreeCAD in server mode)]),
  [case $enableval in
    no | false) fc_set_gui=false ;;
    *)          fc_set_gui=true ;;
  esac],
  [fc_set_gui=true])

if $fc_set_gui; then
	echo "not yet done"
fi

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
	CPPFLAGS="$CPPFLAGS -g"
fi

# Check if you want thread support, or not
# **************************************************************************

AC_ARG_ENABLE([thread],
  AC_HELP_STRING([--enable-thread], [Enable thread support]),
  [case $enableval in
    no | false) fc_set_thread=false ;;
    *)          fc_set_thread=true ;;
  esac],
  [fc_set_thread=true])
if $fc_set_thread; then
	AC_CHECK_HEADER([pthread.h])
	AC_CHECK_LIB([pthread],[pthread_create])
	if test "$ac_cv_header_pthread_h" != yes -o \
        	"$ac_cv_lib_pthread_pthread_create" != yes; then
  		AC_MSG_ERROR([POSIX threads (pthreads) not working. Bye.])
	else
#	 	this is apparently needed for some versions of autoconf/automake
#		LIBS="-lpthread $LIBS"
#		CXXFLAGS="$CXXFLAGS -D_REENTRANT"
		CPPFLAGS="$CPPFLAGS -D_REENTRANT"
		LDFLAGS="-lpthread"
	fi
else
	CPPFLAGS="$CPPFLAGS -DNO_FC_THREAD"
fi

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

# -DNO_KDE2 -DQT_THREAD_SUPPORT already defined in Config.h
CPPFLAGS="$CPPFLAGS $all_includes  -I/usr/include -I/usr/local/include -I$QTDIR/include  -DBaseExport= -DGuiExport= -DAppExport= -DDocExport=  -DFC_OS_LINUX"
CXX=g++
CXXFLAGS="$all_libraries -L/usr/lib -L/usr/local/lib -Wno-deprecated -D__DEBUG__ -L$QTDIR/lib -frtti"
LDFLAGS="-L$all_libraries"
AC_SUBST([MOC],[moc])
AC_SUBST([UIC],[uic])
AC_SUBST([PYTHON],[python])
AC_SUBST([fc_py_ver])

AC_CONFIG_FILES([Makefile App/Makefile Base/Makefile Gui/Makefile Main/Makefile])
AC_OUTPUT

echo ""
echo "FreeCAD configuration settings:"
echo "  Python version:            "$fc_py_ver
echo "  Graphical User Interface:  "$fc_set_gui
echo "  Debug info enabled:        "$fc_set_debug
echo "  Thread support enabled:    "$fc_set_thread
echo "  Compiler warnings enabled: "$fc_set_warn
echo ""
echo "Now, run 'make' to build FreeCAD."
echo ""

# **************************************************************************

