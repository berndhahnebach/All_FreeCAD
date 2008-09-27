noinst_LTLIBRARIES=libpropertyeditor.la

BUILT_SOURCES=\
		moc_propertyeditor.cpp \
		moc_propertyeditoritem.cpp \
		moc_propertyeditorlist.cpp 

libpropertyeditor_la_SOURCES=\
		propertyeditor.cpp \
		propertyeditoritem.cpp \
		propertyeditordate.cpp \
		propertyeditorfile.cpp \
		propertyeditorfont.cpp \
		propertyeditorinput.cpp \
		propertyeditorlist.cpp


includedir = @includedir@/Gui/propertyeditor

include_HEADERS=\
		propertyeditor.h \
		propertyeditoritem.h \
		propertyeditordate.h \
		propertyeditorfile.h \
		propertyeditorfont.h \
		propertyeditorinput.h \
		propertyeditorlist.h

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $< -o $(@F)


# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(QT_CXXFLAGS)

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCGui

CLEANFILES = $(BUILT_SOURCES) 
