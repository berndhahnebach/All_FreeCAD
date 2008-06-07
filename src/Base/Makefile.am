
lib_LTLIBRARIES=libFreeCADBase.la

BUILT_SOURCES=\
		BaseClassPy.cpp \
		BoundBoxPy.cpp \
		PersistancePy.cpp \
		VectorPy.cpp


libFreeCADBase_la_BUILT=\
		BaseClassPy.h \
		BoundBoxPy.h \
		PersistancePy.h \
		VectorPy.h

libFreeCADBase_la_SOURCES=\
		zipios/backbuffer.h \
		zipios/basicentry.cpp \
		zipios/basicentry.h \
		zipios/collcoll.cpp \
		zipios/collcoll.h \
		zipios/deflateoutputstreambuf.cpp \
		zipios/deflateoutputstreambuf.h \
		zipios/dircoll.cpp \
		zipios/dircoll.h \
		zipios/directory.cpp \
		zipios/directory.h \
		zipios/fcoll.cpp \
		zipios/fcoll.h \
		zipios/fcollexceptions.cpp \
		zipios/fcollexceptions.h \
		zipios/fileentry.cpp \
		zipios/fileentry.h \
		zipios/filepath.cpp \
		zipios/filepath.h \
		zipios/filterinputstreambuf.cpp \
		zipios/filterinputstreambuf.h \
		zipios/filteroutputstreambuf.cpp \
		zipios/filteroutputstreambuf.h \
		zipios/gzipoutputstream.cpp \
		zipios/gzipoutputstream.h \
		zipios/gzipoutputstreambuf.cpp \
		zipios/gzipoutputstreambuf.h \
		zipios/inflateinputstreambuf.cpp \
		zipios/inflateinputstreambuf.h \
		zipios/meta-iostreams.h \
		zipios/outputstringstream.h \
		zipios/simplesmartptr.h \
		zipios/virtualseeker.h \
		zipios/zipfile.cpp \
		zipios/zipfile.h \
		zipios/ziphead.cpp \
		zipios/ziphead.h \
		zipios/zipheadio.cpp \
		zipios/zipheadio.h \
		zipios/zipinputstream.cpp \
		zipios/zipinputstream.h \
		zipios/zipinputstreambuf.cpp \
		zipios/zipinputstreambuf.h \
		zipios/zipios_common.h \
		zipios/zipios-config.h \
		zipios/zipios_defs.h \
		zipios/zipoutputstreambuf.cpp \
		zipios/zipoutputstreambuf.h \
		zipios/zipoutputstream.cpp \
		zipios/zipoutputstream.h \
		PyCXX/Config.hxx \
		PyCXX/cxxextensions.c \
		PyCXX/cxx_extensions.cxx \
		PyCXX/cxxsupport.cxx \
		PyCXX/Exception.hxx \
		PyCXX/Extensions.hxx \
		PyCXX/IndirectPythonInterface.cxx \
		PyCXX/IndirectPythonInterface.hxx \
		PyCXX/Objects.hxx \
		BaseClass.cpp \
		BaseClassPyImp.cpp \
		BoundBoxPyImp.cpp \
		Builder3D.cpp \
		Console.cpp \
		Doxygen.cpp \
		Exception.cpp \
		Factory.cpp \
		FileInfo.cpp \
		FileTemplate.cpp \
		FileTemplate.h \
		gzstream.cpp \
		Handle.cpp \
		InputSource.cpp \
		Interpreter.cpp \
		Matrix.cpp \
		MemDebug.cpp \
		MemDebug.h \
		Parameter.cpp \
		ParameterPy.cpp \
		Persistance.cpp \
		PersistancePyImp.cpp \
		Placement.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		PyExport.cpp \
		PyObjectBase.cpp \
		PyTools.c \
		PyTools.h \
		Reader.cpp \
		Rotation.cpp \
		Sequencer.cpp \
		Stream.cpp \
		Swap.cpp \
		swigpyrun_1.3.25.cpp \
		swigpyrun_1.3.25.h \
		swigpyrun_1.3.33.cpp \
		swigpyrun_1.3.33.h \
		TimeInfo.cpp \
		Type.cpp \
		Tools2D.cpp \
		VectorPyImp.cpp \
		Vector3D.cpp \
		Writer.cpp \
		XMLTools.cpp

includedir = @includedir@/Base
nodist_include_HEADERS=\
		$(libFreeCADBase_la_BUILT)

include_HEADERS=\
		BaseClass.h \
		BoundBox.h \
		Builder3D.h \
		Console.h \
		Exception.h \
		Factory.h \
		FileInfo.h \
		gzstream.h \
		Handle.h \
		InputSource.h \
		Interpreter.h \
		Matrix.h \
		Observer.h \
		Parameter.h \
		Persistance.h \
		Placement.h \
		PyExport.h \
		PyObjectBase.h \
		Reader.h \
		Rotation.h \
		Sequencer.h \
		Stream.h \
		Swap.h \
		TimeInfo.h \
		Type.h \
		Tools2D.h \
		Vector3D.h \
		ViewProj.h \
		Writer.h \
		XMLTools.h

nobase_include_HEADERS = \
		zipios/backbuffer.h \
		zipios/basicentry.h \
		zipios/collcoll.h \
		zipios/deflateoutputstreambuf.h \
		zipios/dircoll.h \
		zipios/directory.h \
		zipios/fcoll.h \
		zipios/fcollexceptions.h \
		zipios/fileentry.h \
		zipios/filepath.h \
		zipios/filterinputstreambuf.h \
		zipios/filteroutputstreambuf.h \
		zipios/gzipoutputstream.h \
		zipios/gzipoutputstreambuf.h \
		zipios/inflateinputstreambuf.h \
		zipios/meta-iostreams.h \
		zipios/outputstringstream.h \
		zipios/simplesmartptr.h \
		zipios/virtualseeker.h \
		zipios/zipfile.h \
		zipios/ziphead.h \
		zipios/zipheadio.h \
		zipios/zipinputstream.h \
		zipios/zipinputstreambuf.h \
		zipios/zipios_common.h \
		zipios/zipios-config.h \
		zipios/zipios_defs.h \
		zipios/zipoutputstreambuf.h \
		zipios/zipoutputstream.h \
		PyCXX/Config.hxx \
		PyCXX/Exception.hxx \
		PyCXX/Extensions.hxx \
		PyCXX/IndirectPythonInterface.hxx \
		PyCXX/Objects.hxx

%.cpp: %.xml
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

libFreeCADBase_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lboost_filesystem \
		-lxerces-c \
		-lz

# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCBase

CLEANFILES = $(BUILT_SOURCES) $(libFreeCADBase_la_BUILT)

# the library version number
libFreeCADBase_la_LDFLAGS = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

EXTRA_DIST = \
		FreeCADBase.vcproj \
		BaseClassPy.xml \
		BoundBoxPy.xml \
		PersistancePy.xml \
		VectorPy.xml

