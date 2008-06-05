
lib_LTLIBRARIES=_coin.la _soqt.la

#--------------------------------------------------------------------------------------
# Coin

_coin_la_SOURCES=\
		coin_wrap.cpp \
		coin_header_includes.h

# the library search path.
_coin_la_LDFLAGS = $(sim_ac_coin_ldflags) -module -avoid-version
_coin_la_CPPFLAGS = -fno-strict-aliasing

_coin_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		$(sim_ac_coin_libs)

#--------------------------------------------------------------------------------------
# SoQt

_soqt_la_SOURCES=\
		soqt_wrap.cpp

# the library search path.
_soqt_la_LDFLAGS = $(sim_ac_coin_ldflags) $(sim_ac_soqt_ldflags) $(QT_LIBS) -module -avoid-version
_soqt_la_CPPFLAGS = -fno-strict-aliasing

_soqt_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		$(sim_ac_coin_libs) \
		$(sim_ac_soqt_libs)

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) $(QT_CXXFLAGS) -I$(top_srcdir)/src -I$(top_builddir)/src

libdir = $(prefix)/bin/pivy
datadir = $(prefix)/bin/pivy
data_DATA = __init__.py \
		coin.py \
		sogui.py \
		soqt.py

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		$(data_DATA) \
		AUTHORS \
		LICENSE \
		README \
		CoinBinding.sln \
		CoinPy.vcproj \
		SoQtPy.vcproj


