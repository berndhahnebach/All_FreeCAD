#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(FreeCAD,cvs,[Juergen Riegel],freecad)
#AM_INIT_AUTOMAKE([dist-bzip2 no-texinfo.tex])
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

## !!! TODO !!! Try to find these things below automatically 
## Checks for libraries.
##############################
all_includes="$all_includes -I/usr/local/include/python2.3 -I/usr/local/include"
all_libraries="$all_libraries -L/usr/local/lib"
AC_SUBST(all_includes)
AC_SUBST(all_libraries)


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

AC_CHECKING([Python])
fc_py_ver=`python -c "import sys; print sys.version[[:3]]"`;
if test x$fc_py_ver = x; then
	AC_MSG_ERROR([cannot find python, bye...])
fi;

AC_ARG_WITH(python-prefix,[--with-python-prefix],
	[fc_py_prefix=$withval],
	[fc_py_prefix=/usr])
AC_ARG_WITH(python-includes,[--with-python-includes],
	[fc_py_incs=$withval],
	[fc_py_incs=$fc_py_prefix/include/python$fc_py_ver])
AC_ARG_WITH(python-libs,[--with-python-libs],
	[fc_py_libs=$withval],
	[fc_py_libs=$fc_py_prefix/lib])

AC_MSG_RESULT([
Python $fc_py_ver: $fc_py_prefix : $fc_py_incs : $fc_py_libs
])


# -DNO_KDE2 -DQT_THREAD_SUPPORT already defined in Config.h
CPPFLAGS="-I/usr/include/qt3 -I/home/vaclav/build/OpenCASCADE-5.0/ros/inc -I/usr/include/xercesc -I$fc_py_incs -DBaseExport= -DGuiExport= -DAppExport= -DDocExport=  "
CXX=g++-3.2
CXXFLAGS="-Wno-deprecated -D__DEBUG__"
LDFLAGS="-L/usr/local/lib/OCC-5.0"
AC_SUBST([MOC],[moc])
AC_SUBST([UIC],[uic])
AC_SUBST([fc_py_ver])

#AC_CONFIG_FILES([Makefile App/Makefile Base/Makefile Gui/Makefile Main/Makefile])
AC_CONFIG_FILES([Makefile Base/Makefile])
AC_OUTPUT

