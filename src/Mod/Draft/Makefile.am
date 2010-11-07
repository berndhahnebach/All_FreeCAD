
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
		importAirfoilDAT.py \
		Init.py \
		InitGui.py \
		icons.svg \
		macros.py \
		preferences-draft.svg \
		userprefs-base.ui \
        userprefs-import.ui

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
		Languages/draft_se.qm

EXTRA_DIST = \
		$(data_DATA) $(nobase_data_DATA) \
		Languages/draft_de.ts \
		Languages/draft_es.ts \
		Languages/draft_fr.ts \
		Languages/draft_it.ts \
		Languages/draft_se.ts \
		CMakeLists.txt \
		draft.dox

