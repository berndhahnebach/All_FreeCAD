
# Change data dir from default ($(prefix)/share) to $(prefix)
datadir = $(prefix)/Mod/Draft
data_DATA = \
		Draft.py \
		draftTools.py \
		draftGui.py \
		WorkingPlane.py \
		importOCA.py \
		importDXF.py \
		importSVG.py \
		Init.py \
		InitGui.py \
		icons.svg \
		preferences-draft.svg \
		userprefs.ui

nobase_data_DATA = \
		draftlibs/dxfColorMap.py \
		draftlibs/dxfImportObjects.py \
		draftlibs/dxfLibrary.py \
		draftlibs/dxfReader.py \
		draftlibs/fcvec.py \
		draftlibs/fcgeo.py \
		draftlibs/__init__.py \
		Languages/draft_de.qm \
		Languages/draft_es.qm \
		Languages/draft_fr.qm \
		Languages/draft_it.qm \
		Languages/draft_se.qm \
		Languages/draft_de.ts \
		Languages/draft_es.ts \
		Languages/draft_fr.ts \
		Languages/draft_it.ts \
		Languages/draft_se.ts

EXTRA_DIST = \
		$(data_DATA) $(nobase_data_DATA) \
		CMakeLists.txt

