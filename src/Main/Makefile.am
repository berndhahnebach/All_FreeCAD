bin_PROGRAMS=FreeCAD
FreeCAD_SOURCES= \
	MainGui.cpp

# the library search path.
FreeCAD_LDFLAGS = -L../Base -L../App -L../Gui $(all_libraries)

LDADD=\
	-lPAppStdPlugin \
	-lPTKernel \
	-lTKBO \
	-lTKBool \
	-lTKBRep \
	-lTKCAF \
	-lTKCDF \
	-lTKernel \
	-lTKFeat \
	-lTKFillet \
	-lTKGeomAlgo \
	-lTKGeomBase \
	-lTKG2d \
	-lTKG3d \
	-lTKHLR \
	-lTKIGES \
	-lTKMath \
	-lTKOffset \
	-lTKPCAF \
	-lTKPrim \
	-lTKPShape \
	-lTKService \
	-lTKShapeSchema \
	-lTKShHealing \
	-lTKStdSchema \
	-lTKSTEP \
	-lTKSTL \
	-lTKTopAlgo \
	-lTKVRML \
	-lTKV2d \
	-lTKV3d \
	-lTKXCAF \
	-lTKXCAFSchema \
	-lTKXDEIGES \
	-lTKXDESTEP \
	-lTKXSBase \
	-lqui\
	-lqt-mt \
	-lxerces-c \
	-lpython@fc_py_ver@ \
	-lCoin \
	-lSoQt \
	-lFreeCADBase \
	-lFreeCADApp \
	-lFreeCADGui
