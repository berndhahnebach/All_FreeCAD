if OCC_LIBRARY_FILES

lib_LTLIBRARIES=libPart.la Part.la

BUILT_SOURCES=\
		ArcPy.cpp \
		CirclePy.cpp \
		EllipsePy.cpp \
		HyperbolaPy.cpp \
		ParabolaPy.cpp \
		OffsetCurvePy.cpp \
		GeometryPy.cpp \
		LinePy.cpp \
		BezierCurvePy.cpp \
		BSplineCurvePy.cpp \
		PlanePy.cpp \
		ConePy.cpp \
		CylinderPy.cpp \
		SpherePy.cpp \
		ToroidPy.cpp \
		BezierSurfacePy.cpp \
		BSplineSurfacePy.cpp \
		OffsetSurfacePy.cpp \
		SurfaceOfExtrusionPy.cpp \
		SurfaceOfRevolutionPy.cpp \
		PartFeaturePy.cpp \
		TopoShapeCompoundPy.cpp \
		TopoShapeCompSolidPy.cpp \
		TopoShapeEdgePy.cpp \
		TopoShapeFacePy.cpp \
		TopoShapeShellPy.cpp \
		TopoShapeSolidPy.cpp \
		TopoShapeVertexPy.cpp \
		TopoShapeWirePy.cpp \
		TopoShapePy.cpp

libPart_la_BUILT=\
		ArcPy.h \
		CirclePy.h \
		EllipsePy.h \
		HyperbolaPy.h \
		ParabolaPy.h \
		OffsetCurvePy.h \
		GeometryPy.h \
		LinePy.h \
		BezierCurvePy.h \
		BSplineCurvePy.h \
		PlanePy.h \
		ConePy.h \
		CylinderPy.h \
		SpherePy.h \
		ToroidPy.h \
		BezierSurfacePy.h \
		BSplineSurfacePy.h \
		OffsetSurfacePy.h \
		SurfaceOfExtrusionPy.h \
		SurfaceOfRevolutionPy.h \
		PartFeaturePy.h \
		TopoShapeCompoundPy.h \
		TopoShapeCompSolidPy.h \
		TopoShapeEdgePy.h \
		TopoShapeFacePy.h \
		TopoShapeShellPy.h \
		TopoShapeSolidPy.h \
		TopoShapeVertexPy.h \
		TopoShapeWirePy.h \
		TopoShapePy.h

libPart_la_SOURCES=\
		AppPartPy.cpp \
		ArcPyImp.cpp \
		CirclePyImp.cpp \
		EllipsePyImp.cpp \
		HyperbolaPyImp.cpp \
		ParabolaPyImp.cpp \
		OffsetCurvePyImp.cpp \
		GeometryPyImp.cpp \
		LinePyImp.cpp \
		BezierCurvePyImp.cpp \
		BSplineCurvePyImp.cpp \
		PlanePyImp.cpp \
		ConePyImp.cpp \
		CylinderPyImp.cpp \
		SpherePyImp.cpp \
		ToroidPyImp.cpp \
		BezierSurfacePyImp.cpp \
		BSplineSurfacePyImp.cpp \
		OffsetSurfacePyImp.cpp \
		SurfaceOfExtrusionPyImp.cpp \
		SurfaceOfRevolutionPyImp.cpp \
		FeaturePartBoolean.cpp \
		FeaturePartBox.cpp \
		FeaturePartCircle.cpp \
		FeaturePartCurveNet.cpp \
		FeaturePartCommon.cpp \
		FeaturePartCut.cpp \
		FeaturePartFuse.cpp \
		FeaturePartImportBrep.cpp \
		FeaturePartImportIges.cpp \
		FeaturePartImportStep.cpp \
		FeaturePartPolygon.cpp \
		FeaturePartSection.cpp \
		FeatureExtrusion.cpp \
		FeatureFillet.cpp \
		Geometry.cpp \
		ImportStep.cpp \
		PartFeature.cpp \
		PartFeaturePyImp.cpp \
		Part2DObject.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		PrimitiveFeature.cpp \
		PropertyTopoShape.cpp \
		TopoShape.cpp \
		TopoShapeCompoundPyImp.cpp \
		TopoShapeCompSolidPyImp.cpp \
		TopoShapeEdgePyImp.cpp \
		TopoShapeFacePyImp.cpp \
		TopoShapeShellPyImp.cpp \
		TopoShapeSolidPyImp.cpp \
		TopoShapeVertexPyImp.cpp \
		TopoShapeWirePyImp.cpp \
		TopoShapePyImp.cpp

nodist_include_HEADERS=\
		$(libPart_la_BUILT)

include_HEADERS=\
		FeaturePartBoolean.h \
		FeaturePartBox.h \
		FeaturePartCircle.h \
		FeaturePartCurveNet.h \
		FeaturePartCommon.h \
		FeaturePartCut.h \
		FeaturePartFuse.h \
		FeaturePartImportBrep.h \
		FeaturePartImportIges.h \
		FeaturePartImportStep.h \
		FeaturePartPolygon.h \
		FeaturePartSection.h \
		FeatureExtrusion.h \
		FeatureFillet.h \
		Geometry.h \
		ImportStep.h \
		PartFeature.h \
		Part2DObject.h \
		PrimitiveFeature.h \
		PropertyTopoShape.h \
		TopoShape.h


# the library search path.
libPart_la_LDFLAGS = -L../../../Base -L../../../App -L/usr/X11R6/lib -L$(OCC_LIB) $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libPart_la_CPPFLAGS = -DAppPartExport=

libPart_la_LIBADD   = \
		@BOOST_FILESYSTEM_LIB@ @BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lTKernel \
		-lTKFillet \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKMesh \
		-lTKSTEP \
		-lTKIGES \
		-lTKSTL \
		-lTKShHealing \
		-lTKXSBase \
		-lTKBool \
		-lTKBO \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKOffset \
		-lTKPrim

%.cpp: %.xml
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

#--------------------------------------------------------------------------------------
# Loader of libPart

Part_la_SOURCES=\
		AppPart.cpp

# the library search path.
Part_la_LDFLAGS = $(libPart_la_LDFLAGS) -module -avoid-version
Part_la_CPPFLAGS = $(libPart_la_CPPFLAGS)

Part_la_LIBADD   = \
		$(libPart_la_LIBADD) \
		-lPart

Part_la_DEPENDENCIES = libPart.la

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) -I$(OCC_INC)

endif

includedir = @includedir@/Mod/Part/App
libdir = $(prefix)/Mod/Part

CLEANFILES = $(BUILT_SOURCES) $(libPart_la_BUILT)

EXTRA_DIST = \
		OpenCascadeAll.h \
		ArcPy.xml \
		CirclePy.xml \
		EllipsePy.xml \
		HyperbolaPy.xml \
		ParabolaPy.xml \
		OffsetCurvePy.xml \
		GeometryPy.xml \
		LinePy.xml \
		BezierCurvePy.xml \
		BSplineCurvePy.xml \
		PlanePy.xml \
		ConePy.xml \
		CylinderPy.xml \
		SpherePy.xml \
		ToroidPy.xml \
		BezierSurfacePy.xml \
		BSplineSurfacePy.xml \
		OffsetSurfacePy.xml \
		SurfaceOfExtrusionPy.xml \
		SurfaceOfRevolutionPy.xml \
		PartFeaturePy.xml \
		TopoShapePy.xml \
		TopoShapeCompSolidPy.xml \
		TopoShapeCompoundPy.xml \
		TopoShapeEdgePy.xml \
		TopoShapeFacePy.xml \
		TopoShapeShellPy.xml \
		TopoShapeSolidPy.xml \
		TopoShapeVertexPy.xml \
		TopoShapeWirePy.xml \
		AppPart.vcproj
