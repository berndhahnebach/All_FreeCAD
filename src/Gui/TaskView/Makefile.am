noinst_LTLIBRARIES=libTaskView.la

BUILT_SOURCES=\
		moc_TaskAppearance.cpp \
		moc_TaskEditControl.cpp \
		moc_TaskView.cpp \
		ui_TaskAppearance.h \
		ui_TaskEditControl.h

libTaskView_la_UI=\
		TaskAppearance.ui \
		TaskEditControl.ui

libTaskView_la_SOURCES=\
		TaskAppearance.cpp \
		TaskEditControl.cpp \
		TaskView.cpp

includedir = @includedir@/Gui/TaskView

include_HEADERS=\
		TaskAppearance.h \
		TaskEditControl.h \
		TaskView.h

EXTRA_DIST = \
		$(libTaskView_la_UI)

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $< -o $(@F)

# rules for Qt User Interface Compiler:
ui_%.h: %.ui
	$(QT_UIC) $< -o $(@F)

# rules for Qt Resource Compiler:
qrc_%.cpp: %.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)


# set the include path found by configure
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src -I$(srcdir)/.. $(QT_CXXFLAGS) $(all_includes)

CLEANFILES = $(BUILT_SOURCES)

