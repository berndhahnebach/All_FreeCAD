

fc_copy_to_mod_path("Draft" 
	draftlibs/dxfColorMap.py
	draftlibs/dxfImportObjects.py
	draftlibs/dxfLibrary.py
	draftlibs/dxfReader.py
	draftlibs/fcgeo.py
	draftlibs/fcvec.py
	draftlibs/__init__.py
	Init.py
	InitGui.py
	Draft.py
	draftGui.py
	importDXF.py
	importOCA.py
	importSVG.py
	icons.svg
	preferences-draft.svg
	userprefs.ui
)


install(
		 FILES
				 draftlibs/dxfColorMap.py
				 draftlibs/dxfImportObjects.py
				 draftlibs/dxfLibrary.py
				 draftlibs/dxfReader.py
				 draftlibs/fcvec.py
				 draftlibs/fcgeo.py
				 draftlibs/__init__.py
		 DESTINATION
		 		 Mod/Draft/draftlibs)

install(
		 FILES
				 Draft.py
				 draftGui.py
				 importOCA.py
				 importDXF.py
				 importSVG.py
				 Init.py
				 InitGui.py
				 icons.svg
				 preferences-draft.svg
				 userprefs.ui
		 DESTINATION
		 		 Mod/Draft)

