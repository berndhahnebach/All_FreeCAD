dnl @synopsis AC_CXX_HAVE_STL
dnl
dnl If the compiler supports the Standard Template Library, define HAVE_STL.
dnl
dnl @version $Id: acinclude.m4,v 1.2 2006/02/24 00:09:19 wmayer Exp $
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_HAVE_STL],
[AC_CACHE_CHECK(whether the compiler supports Standard Template Library,
ac_cv_cxx_have_stl,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <list>
#include <deque>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[list<int> x; x.push_back(5);
list<int>::iterator iter = x.begin(); if (iter != x.end()) ++iter; return 0;],
 ac_cv_cxx_have_stl=yes, ac_cv_cxx_have_stl=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_stl" = yes; then
  AC_DEFINE(HAVE_STL,,[define if the compiler supports Standard Template Library])
fi
])
dnl @synopsis AC_CXX_HAVE_STD
dnl
dnl If the compiler supports ISO C++ standard library (i.e., can include the
dnl files iostream, map, iomanip and cmath}), define HAVE_STD.
dnl
dnl @version $Id: acinclude.m4,v 1.2 2006/02/24 00:09:19 wmayer Exp $
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_HAVE_STD],
[AC_CACHE_CHECK(whether the compiler supports ISO C++ standard library,
ac_cv_cxx_have_std,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <iostream>
#include <map>
#include <iomanip>
#include <cmath>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[return 0;],
 ac_cv_cxx_have_std=yes, ac_cv_cxx_have_std=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_std" = yes; then
  AC_DEFINE(HAVE_STD,,[define if the compiler supports ISO C++ standard library])
fi
])
dnl @synopsis AC_CXX_NAMESPACES
dnl
dnl If the compiler can prevent names clashes using namespaces, define
dnl HAVE_NAMESPACES.
dnl
dnl @version $Id: acinclude.m4,v 1.2 2006/02/24 00:09:19 wmayer Exp $
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether the compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
fi
])
dnl @synopsis AC_CXX_HAVE_SSTREAM
dnl
dnl If sstream (part of Standard C++ Library) exists
dnl define HAVE_SSTREAM.
dnl
dnl @version ac_cxx_have_std.m4 Tue Mar 28 18:20:26 CEST 2000
dnl @author Thomas Sondergaard thomass@deltadata.dk
dnl
AC_DEFUN([AC_CXX_HAVE_SSTREAM],
[AC_CACHE_CHECK(for sstream,
ac_cv_cxx_have_sstream,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <sstream>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[return 0;],
 ac_cv_cxx_have_sstream=yes, ac_cv_cxx_have_sstream=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_sstream" = yes; then
  AC_DEFINE(HAVE_SSTREAM,1,[define if the compiler supports sstream])
fi
])
dnl @synopsis AC_CXX_HAVE_STD_IOSTREAM
dnl
dnl If std iostream (part of Standard C++ Library) exists
dnl define HAVE_STD_IOSTREAM.
dnl
dnl @version ac_cxx_have_std.m4 Tue Mar 28 18:20:26 CEST 2000
dnl @author Thomas Sondergaard thomass@deltadata.dk
dnl
AC_DEFUN([AC_CXX_HAVE_STD_IOSTREAM],
[AC_CACHE_CHECK(for std iostream,
ac_cv_cxx_have_std_iostream,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
#include <sstream>
#include <streambuf>
#include <ios>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif
],[return 0;],
 ac_cv_cxx_have_std_iostream=yes, ac_cv_cxx_have_std_iostream=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_std_iostream" = yes; then
  AC_DEFINE(HAVE_STD_IOSTREAM,,[define if the compiler has std compliant iostream library])
fi
])
dnl @synopsis FREECAD_AC_HAVE_QT
dnl
dnl @summary Search for Trolltech's Qt GUI framework.
dnl
dnl Checks for the Qt 4.x library and its tools uic, moc and rcc.
dnl
dnl The following variables are exported:
dnl
dnl   QT_DIR
dnl   QT_CXXFLAGS
dnl   QT_LIBS
dnl   QT_MOC
dnl   QT_UIC
dnl   QT_RCC
dnl
AC_DEFUN([FREECAD_AC_HAVE_QT],
[
AC_MSG_CHECKING(for Qt)

QT_DIR=xxx
AC_ARG_WITH([qt-dir],
             AC_HELP_STRING([--with-qt-dir=DIR], [give prefix location of Qt 4.x]),
             [QTPATHS="$withval"], [QTPATHS="/usr/share/qt4"])

for x in $QTPATHS; do
    if test -d $x ; then
       QT_DIR="$x"
    fi
done
dnl If the path was not specified we also check for the QTDIR system variable
if test $QT_DIR = xxx ; then
    if test -d ${QTDIR} ; then
       QT_DIR="${QTDIR}"
    else
       AC_MSG_ERROR(Could not locate Qt 4.x)
    fi
fi
AC_MSG_RESULT($QT_DIR)
AC_MSG_CHECKING(for host)
host=`uname -a` # AC_CANONICAL_HOST is broken at the time of this writing.
case $host in
  MINGW32*)
    AC_MSG_RESULT(MinGW)
    QT_LIBS="-L$QT_DIR/lib -lopengl32 -lglu32 -lgdi32 -luser32 -lmingw32 -mthreads -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc -Wl,-s -Wl,-s -Wl,-subsystem,windows"
    QT_CXXFLAGS="-I$QT_DIR/include -I$QT_DIR/include/QtCore -I$QT_DIR/include/QtGui -I$QT_DIR/include/QtOpenGL -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT -DQT_NEEDS_QMAIN -frtti -fexceptions"
    QT_BIN="$QT_DIR/bin"
    ;;
  *)
    AC_MSG_RESULT(Linux)
    AC_PATH_XTRA
    QT_LIBS="-Wl,-rpath,$QT_DIR/lib -L$QT_DIR/lib $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS -lGLU -lGL -lpthread"
    QT_CXXFLAGS="-I$QT_DIR/include -I$QT_DIR/include/Qt3Support -I$QT_DIR/include/QtGui -I$QT_DIR/include/QtCore -I$QT_DIR/include/QtOpenGL $X_CFLAGS -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -DQT_THREAD_SUPPORT"
    QT_BIN="$QT_DIR/bin"
    ;;
esac

QT_MOC="$QT_BIN/moc"
QT_UIC="$QT_BIN/uic"
QT_RCC="$QT_BIN/rcc"

# Now we check whether we can actually build a Qt app.
cat > myqt.h << EOF
#include <QObject>
class Test : public QObject
{
Q_OBJECT
public:
  Test() {}
  ~Test() {}
public slots:
  void receive() {}
signals:
  void send();
};
EOF

cat > myqt.cpp << EOF
#include "myqt.h"
#include <QApplication>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

AC_MSG_CHECKING(for moc)
bnv_try_1="$QT_MOC myqt.h -o moc_myqt.cpp"
AC_TRY_EVAL(bnv_try_1)
if test x"$ac_status" != x0; then
   AC_MSG_ERROR(failed)
fi
AC_MSG_RESULT(yes)
AC_MSG_CHECKING(for correct functioning of moc)
bnv_try_2="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_myqt.o moc_myqt.cpp"
AC_TRY_EVAL(bnv_try_2)
if test x"$ac_status" != x0; then
   AC_MSG_ERROR(failed)
fi
AC_MSG_RESULT(yes)
AC_MSG_CHECKING(for compiling and linking with Qt)
bnv_try_3="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o myqt.o myqt.cpp"
AC_TRY_EVAL(bnv_try_3)
if test x"$ac_status" != x0; then
   AC_MSG_ERROR(failed)
fi
nv_try_4="$CXX $QT_LIBS $LIBS -o myqt myqt.o moc_myqt.o"
AC_TRY_EVAL(bnv_try_4)
if test x"$ac_status" != x0; then
   AC_MSG_ERROR(failed)
fi
AC_MSG_RESULT(yes)

dnl AC_MSG_CHECKING(for mkoctfile)
dnl AC_TRY_EVAL(mkoctfile)
dnl if test x"$ac_status" != x0; then
dnl    AC_MSG_ERROR(mkoctfile is not in the path)
dnl fi
dnl AC_MSG_RESULT(yes)
rm -f moc_myqt.cpp myqt.h myqt.cpp myqt.o myqt moc_myqt.o

dnl check QT_MOC
if test x$QT_MOC = x; then
    AC_MSG_ERROR([Cannot find Qt meta object compiler (moc), bye...])
fi;
dnl check QT_UIC
if test x$QT_UIC = x; then
    AC_MSG_ERROR([Cannot find Qt user interface compiler (uic), bye...])
fi;
dnl check QT_RCC
if test x$QT_RCC = x; then
    AC_MSG_ERROR([Cannot find Qt resource compiler (rcc), bye...])
fi;

AC_SUBST(QT_DIR)
AC_SUBST(QT_CXXFLAGS)
AC_SUBST(QT_LIBS)
AC_SUBST(QT_UIC)
AC_SUBST(QT_MOC)
AC_SUBST(QT_RCC)
])
dnl @synopsis FREECAD_AC_HAVE_BOOST
dnl
dnl @summary Search for boost header and library files.
dnl
dnl
AC_DEFUN([FREECAD_AC_HAVE_BOOST],
[
AC_MSG_CHECKING(for boost)

AC_ARG_WITH(boost-includes,
	AC_HELP_STRING([--with-boost-includes], [Path to the boost header files]),
	[fc_boost_incs=$withval], [fc_boost_incs=/usr/include])

AC_ARG_WITH(boost-libs,
	AC_HELP_STRING([--with-boost-libs], [Path to the boost library files]),
	[fc_boost_libs=$withval], [fc_boost_libs=/usr/lib])

fc_boost_ac_save_cppflags=$CPPFLAGS
fc_boost_ac_save_ldflags=$LDFLAGS
fc_boost_ac_save_libs=$LIBS
CPPFLAGS="$CPPFLAGS -I$fc_boost_incs"
LDFLAGS="$LDFLAGS -L$fc_boost_libs"
LIBS="-lboost_program_options"

fc_cv_lib_boost_avail=no
AC_TRY_LINK([#include <boost/program_options.hpp>],
	[namespace po = boost::program_options;
	 po::options_description generic("Generic options");
	 generic.add_options()
	     ("version,v", "print version string")
	     ("help", "produce help message");
	], [fc_cv_lib_boost_avail=yes
	AC_MSG_RESULT(yes)],
	AC_MSG_ERROR(failed))

CPPFLAGS=$fc_boost_ac_save_cppflags
LDFLAGS=$fc_boost_ac_save_ldflags
LIBS=$fc_boost_ac_save_libs

all_includes="$all_includes -I$fc_boost_incs"
all_libraries="$all_libraries -L$fc_boost_libs"
])
