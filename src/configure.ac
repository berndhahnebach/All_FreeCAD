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
AC_PROG_RANLIB

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
AC_ARG_WITH(OpenCascade-prefix,[  --with-OpenCascade-prefix],
	[fc_occ_prefix=$withval],
	[fc_occ_prefix=/usr])
AC_ARG_WITH(OpenCascade-includes,[  --with-OpenCascade-includes],
	[fc_occ_incs=$withval],
	[fc_occ_incs=$fc_occ_prefix/include/OCC])
AC_ARG_WITH(OpenCascade-libs,[  --with-OpenCascade-libs],
	[fc_occ_libs=$withval],
	[fc_occ_libs=$fc_occ_prefix/lib])

AC_MSG_RESULT([
OpenCascade $fc_occ_ver: $fc_occ_prefix : $fc_occ_incs : $fc_occ_libs
])

# **************************************************************************

all_includes="$all_includes -I$fc_py_incs -I$fc_occ_incs"
all_libraries="$all_libraries -L$fc_py_libs -L$fc_occ_libs"
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
echo "want_gui="$want_gui
if $want_gui; then
	AC_DEFINE_UNQUOTED(_FC_GUI_ENABLED_,1, [Define if you want to use the GUI])
fi


# -DNO_KDE2 -DQT_THREAD_SUPPORT already defined in Config.h
CPPFLAGS="$all_includes  -I/usr/include -I/usr/local/include -I$QTDIR/include  -DBaseExport= -DGuiExport= -DAppExport= -DDocExport=  "
CXX=g++-3.2
CXXFLAGS="$all_libraries -L/usr/lib -L/usr/local/lib -Wno-deprecated -D__DEBUG__ -L$QTDIR/lib -frtti"
LDFLAGS="-L$all_libraries"
AC_SUBST([MOC],[moc])
AC_SUBST([UIC],[uic])
AC_SUBST([fc_py_ver])

AC_CONFIG_FILES([Makefile App/Makefile Base/Makefile Gui/Makefile Main/Makefile])
AC_OUTPUT

