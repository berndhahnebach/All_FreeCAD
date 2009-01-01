
# Change data dir from default ($(prefix)/share) to $(prefix)
datadir = $(prefix)/Mod/Draft
data_DATA = \
		Draft.py \
		draftGui.py \
		exportDXF.py \
		importDXF.py \
		importSVG.py \
		Init.py \
		InitGui.py \
		userprefs.ui

nobase_data_DATA = \
		draftlibs/dxfColorMap.py \
		draftlibs/dxfImportObjects.py \
		draftlibs/dxfLibrary.py \
		draftlibs/dxfReader.py \
		draftlibs/fcvec.py \
		draftlibs/sdxf.py \
		draftlibs/__init__.py

EXTRA_DIST = \
		$(data_DATA) $(nobase_data_DATA)

