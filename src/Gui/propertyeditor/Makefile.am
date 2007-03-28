noinst_LTLIBRARIES=libpropertyeditor.la

BUILT_SOURCES=\
		moc_propertyeditor.cpp \
		moc_propertyeditorfile.cpp \
		moc_propertyeditorfont.cpp \
		moc_propertyeditorinput.cpp \
		moc_propertyeditoritem.cpp 

libpropertyeditor_la_SOURCES=\
		propertyeditor.cpp \
		propertyeditor.h \
		propertyeditoritem.cpp \
		propertyeditoritem.h \
		propertyeditordate.cpp \
		propertyeditordate.h \
		propertyeditorfile.cpp \
		propertyeditorfile.h \
		propertyeditorfont.cpp \
		propertyeditorfont.h \
		propertyeditorinput.cpp \
		propertyeditorinput.h \
		propertyeditorlist.cpp \
		propertyeditorlist.h


# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $(srcdir)/$< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $(srcdir)/$< -o $(@F)


# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(QT_CXXFLAGS)

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCGui

CLEANFILES = $(BUILT_SOURCES) 
