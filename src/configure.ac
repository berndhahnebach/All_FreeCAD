#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(FreeCAD,cvs,[Juergen Riegel],freecad)
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([autoconfig.h])
#AC_CANONICAL_HOST
AC_LANG_CPLUSPLUS


## REQUIRED HEADERS
##############################
#AC_CHECK_HEADERS([sstream argp.h],,[AC_MSG_ERROR([This header is needed. Bye.])])


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
AC_DEFINE_UNQUOTED(HAVE_SOCAMERA_SETSTEREOMODE, 1, [Define to enable use of Inventor feature])
AC_DEFINE_UNQUOTED(HAVE_SOKEYBOARDEVENT_DELETE, 1, [Define to enable use of Inventor feature])
AC_DEFINE_UNQUOTED(HAVE_SOMOUSEBUTTONEVENT_BUTTON5, 1, [Define to enable use of Inventor feature])
AC_DEFINE_UNQUOTED(HAVE_SOPOLYGONOFFSET, 1, [Define to enable use of the Open Inventor SoPolygonOffset node])
AC_DEFINE_UNQUOTED(HAVE_SYS_TYPES_H, 1, [Define to 1 if you have the <sys/types.h> header file.])
AC_DEFINE_UNQUOTED(SOQT_MAJOR_VERSION, 1, [Define to the major version of SoQt])
AC_DEFINE_UNQUOTED(SOQT_MICRO_VERSION, 2, [Define to the micro version of SoQt])
AC_DEFINE_UNQUOTED(SOQT_MINOR_VERSION, 0, [Define to the minor version of SoQt])
AC_DEFINE_UNQUOTED(SOQT_VERSION, "1.0.2", [Version string for SoQt])
AC_DEFINE_UNQUOTED(SCI_LEXER,1, [Define if you want to use the lexer framework])

## PTHREADS
##############################
#AC_CHECK_HEADER([pthread.h])
#AC_CHECK_LIB([pthread],[pthread_create])
#if test "$ac_cv_header_pthread_h" != yes -o \
#        "$ac_cv_lib_pthread_pthread_create" != yes; then
#  AC_MSG_ERROR([POSIX threads (pthreads) not working. Bye.])
#else
#  # this is apparently needed for some versions of autoconf/automake
#  LIBS="-lpthread $LIBS"
#  CXXFLAGS="$CXXFLAGS -D_REENTRANT"
#fi


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

AC_CHECKING([Python])
fc_py_ver=`python -c "import sys; print sys.version[[:3]]"`;
if test x$fc_py_ver = x; then
	AC_MSG_ERROR([cannot find python, bye...])
fi;

AC_ARG_WITH(python-prefix,[  --with-python-prefix],
	[fc_py_prefix=$withval],
	[fc_py_prefix=/usr])
AC_ARG_WITH(python-includes,[  --with-python-includes],
	[fc_py_incs=$withval],
	[fc_py_incs=$fc_py_prefix/include/python$fc_py_ver])
AC_ARG_WITH(python-libs,[  --with-python-libs],
	[fc_py_libs=$withval],
	[fc_py_libs=$fc_py_prefix/lib])

AC_MSG_RESULT([
Python $fc_py_ver: $fc_py_prefix : $fc_py_incs : $fc_py_libs
])

# **************************************************************************

AC_CHECKING([OpenCascade])
AC_ARG_WITH(OCC-prefix,[  --with-OCC-prefix],
	[fc_occ_prefix=$withval],
	[fc_occ_prefix=/usr])
AC_ARG_WITH(OCC-includes,[  --with-OCC-includes],
	[fc_occ_incs=$withval],
	[fc_occ_incs=$fc_occ_prefix/include/OCC])
AC_ARG_WITH(OCC-libs,[  --with-OCC-libs],
	[fc_occ_libs=$withval],
	[fc_occ_libs=$fc_occ_prefix/lib])

AC_MSG_RESULT([
OpenCascade $fc_occ_ver: $fc_occ_prefix : $fc_occ_incs : $fc_occ_libs
])

# **************************************************************************

AC_CHECKING([xerces-c])
AC_ARG_WITH(XERCES-prefix,[  --with-XERCES-prefix],
	[fc_xerces_prefix=$withval],
	[fc_xerces_prefix=/usr])
AC_ARG_WITH(XERCES-includes,[  --with-XERCES-includes],
	[fc_xerces_incs=$withval],
	[fc_xerces_incs=$fc_xerces_prefix/include/])
AC_ARG_WITH(XERCES-libs,[  --with-XERCES-libs],
	[fc_xerces_libs=$withval],
	[fc_xerces_libs=$fc_xerces_prefix/lib])

AC_MSG_RESULT([
Xerces-c $fc_xerces_ver: $fc_xerces_prefix : $fc_xerces_incs : $fc_xerces_libs
])

# **************************************************************************

AC_CHECKING([Coin])
AC_ARG_WITH(coin-prefix,[  --with-coin-prefix],
	[fc_coin_prefix=$withval],
	[fc_coin_prefix=/usr])
AC_ARG_WITH(coin-includes,[  --with-coin-includes],
	[fc_coin_incs=$withval],
	[fc_coin_incs=$fc_coin_prefix/include/])
AC_ARG_WITH(coin-libs,[  --with-coin-libs],
	[fc_coin_libs=$withval],
	[fc_coin_libs=$fc_coin_prefix/lib])

AC_MSG_RESULT([
Coin $fc_coin_ver: $fc_coin_prefix : $fc_coin_incs : $fc_coin_libs
])

# **************************************************************************

AC_CHECKING([SoQt])
AC_ARG_WITH(soqt-prefix,[  --with-soqt-prefix],
	[fc_soqt_prefix=$withval],
	[fc_soqt_prefix=/usr])
AC_ARG_WITH(soqt-includes,[  --with-soqt-includes],
	[fc_soqt_incs=$withval],
	[fc_soqt_incs=$fc_soqt_prefix/include/])
AC_ARG_WITH(soqt-libs,[  --with-soqt-libs],
	[fc_soqt_libs=$withval],
	[fc_soqt_libs=$fc_soqt_prefix/lib])

AC_MSG_RESULT([
SoQt $fc_soqt_prefix : $fc_soqt_incs : $fc_soqt_libs
])

# **************************************************************************

all_includes="$all_includes -I$fc_py_incs -I$fc_occ_incs -I$fc_xerces_incs -I$fc_coin_incs -I$fc_soqt_incs"
all_libraries="$all_libraries -L$fc_py_libs -L$fc_occ_libs -L$fc_xerces_libs -L$fc_coin_libs -L$fc_soqt_libs"
AC_SUBST(all_includes)
AC_SUBST(all_libraries)

# **************************************************************************
# Check if you want to use GUI, or not
want_gui=true
AC_ARG_ENABLE([gui],
  AC_HELP_STRING([--enable-gui], [Enable GUI (you can disable this feature to use FreeCAD as server)]),
  [case $enableval in
    no | false) want_gui=false ;;
    *)          want_gui=true ;;
  esac],
  [want_gui=true])
#echo "want_gui="$want_gui
if $want_gui; then
	CPPFLAGS="-D_FC_GUI_ENABLED_"
fi

# **************************************************************************
# Check if you want thread support, or not
want_thread=true
AC_ARG_ENABLE([thread],
  AC_HELP_STRING([--enable-thread], [Enable Thread support]),
  [case $enableval in
    no | false) want_thread=false ;;
    *)          want_thread=true ;;
  esac],
  [want_thread=true])
#echo "want_gui="$want_gui
if $want_thread; then
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"
	LDFLAGS="-lpthread"
fi


# -DNO_KDE2 -DQT_THREAD_SUPPORT already defined in Config.h
CPPFLAGS="$CPPFLAGS $all_includes  -I/usr/include -I/usr/local/include -I$QTDIR/include  -DBaseExport= -DGuiExport= -DAppExport= -DDocExport=  -DFC_OS_LINUX"
CXX=g++
CXXFLAGS="$all_libraries -L/usr/lib -L/usr/local/lib -Wno-deprecated -D__DEBUG__ -L$QTDIR/lib -frtti"
LDFLAGS="-L$all_libraries"
AC_SUBST([MOC],[moc])
AC_SUBST([UIC],[uic])
AC_SUBST([fc_py_ver])

AC_CONFIG_FILES([Makefile App/Makefile Base/Makefile Gui/Makefile Main/Makefile])
AC_OUTPUT

echo ""
echo "Now, run 'make' to build FreeCAD."
echo ""

# **************************************************************************

