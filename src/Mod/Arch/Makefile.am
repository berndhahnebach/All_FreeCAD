#BUILT_SOURCES=\
#		Arch_rc.py

# rules for Qt Resource Compiler:
#%_rc.py: Resources/%.qrc
#	$(PYRCC4) -name $(*F) $< -o $(@F)

# Change data dir from default ($(prefix)/share) to $(prefix)
datadir = $(prefix)/Mod/Arch
data_DATA = \
		Component.py \
		Wall.py \
		Cell.py \
		importIFC.py \
		ifcReader.py \
		Init.py \
		InitGui.py \
        IFC2X3_TC1.exp \
		Arch_rc.py

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		$(data_DATA) \
		CMakeLists.txt \
		arch.dox \
		Resources/Arch.qrc \
		Resources/icons/preferences-arch.svg \
		Resources/icons/Arch_Building.svg \
        Resources/icons/Arch_Cell.svg \
        Resources/icons/Arch_Wall.svg \
        Resources/icons/Arch_Component.svg