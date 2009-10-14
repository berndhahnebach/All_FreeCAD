
lib_LTLIBRARIES=libFreeCADBase.la

BUILT_SOURCES=\
		BaseClassPy.cpp \
		BoundBoxPy.cpp \
		MatrixPy.cpp \
		PersistencePy.cpp \
		PlacementPy.cpp \
		RotationPy.cpp \
		VectorPy.cpp

if HAVE_SWIG_FOUND
BUILT_SOURCES+=swigpyrun.h
endif


libFreeCADBase_la_BUILT=\
		BaseClassPy.h \
		BoundBoxPy.h \
		MatrixPy.h \
		PersistencePy.h \
		PlacementPy.h \
		RotationPy.h \
		VectorPy.h

libFreeCADBase_la_SOURCES=\
		Base64.cpp \
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
		GeometryPyCXX.cpp \
		gzstream.cpp \
		Handle.cpp \
		InputSource.cpp \
		Interpreter.cpp \
		Matrix.cpp \
		MatrixPyImp.cpp \
		MemDebug.cpp \
		MemDebug.h \
		Parameter.cpp \
		ParameterPy.cpp \
		Persistence.cpp \
		PersistencePyImp.cpp \
		Placement.cpp \
		PlacementPyImp.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		PyExport.cpp \
		PyObjectBase.cpp \
		PyTools.c \
		PyTools.h \
		Reader.cpp \
		Rotation.cpp \
		RotationPyImp.cpp \
		Sequencer.cpp \
		Stream.cpp \
		Swap.cpp \
		swigpyrun.inl \
		swigpyrun.cpp \
		swigpyrun_1.3.25.cpp \
		swigpyrun_1.3.25.h \
		swigpyrun_1.3.33.cpp \
		swigpyrun_1.3.33.h \
		swigpyrun_1.3.36.cpp \
		swigpyrun_1.3.36.h \
		swigpyrun_1.3.38.cpp \
		swigpyrun_1.3.38.h \
		TimeInfo.cpp \
		Type.cpp \
		Tools2D.cpp \
		Uuid.cpp \
		VectorPyImp.cpp \
		Vector3D.cpp \
		Writer.cpp \
		XMLTools.cpp

includedir = @includedir@/Base
nodist_include_HEADERS=\
		$(libFreeCADBase_la_BUILT)

include_HEADERS=\
		Base64.h \
		BaseClass.h \
		BoundBox.h \
		Builder3D.h \
		Console.h \
		Exception.h \
		Factory.h \
		FileInfo.h \
		fdstream.hpp \
		GeometryPyCXX.h \
		gzstream.h \
		Handle.h \
		InputSource.h \
		Interpreter.h \
		Matrix.h \
		Observer.h \
		Parameter.h \
		Persistence.h \
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
		Uuid.h \
		Vector3D.h \
		ViewProj.h \
		Writer.h \
		XMLTools.h

if HAVE_SWIG_FOUND
swigpyrun.h:
	$(SWIG) -python -external-runtime
endif

%.cpp: %.xml $(top_srcdir)/src/Tools/generateTemplates/templateClassPyExport.py
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

libFreeCADBase_la_LIBADD   = \
		@BOOST_FILESYSTEM_LIB@ @BOOST_SYSTEM_LIB@ @ZIPIOS_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lz

# set the include path found by configure
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes)

CLEANFILES = $(BUILT_SOURCES) $(libFreeCADBase_la_BUILT)

# the library version number
libFreeCADBase_la_LDFLAGS = $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libFreeCADBase_la_CPPFLAGS = -DHAVE_SWIG=$(HAVE_SWIG)


EXTRA_DIST = \
		CMakeLists.txt \
		BaseClassPy.xml \
		BoundBoxPy.xml \
		MatrixPy.xml \
		PersistencePy.xml \
		PlacementPy.xml \
		RotationPy.xml \
		VectorPy.xml

# -------------------------------------------------------------------------------
# Debian based systems offer packages for PyCXX and zipios++ but other Linux
# distributions possibly not. Thus, we must prepare a switch for this Makefile
# to use the local or system-wide packages which will also affect the 'make dist'
# target to create source tarballs without the local PyCXX & zipios++ sources. 
# -------------------------------------------------------------------------------

# PyCXX stuff
if ENABLE_PYCXX_ZIPIOS
PYCXXSRC=../CXX
EXTRA_DIST += \
		$(PYCXXSRC)/Config.hxx \
		$(PYCXXSRC)/Exception.hxx \
		$(PYCXXSRC)/Extensions.hxx \
		$(PYCXXSRC)/IndirectPythonInterface.hxx \
		$(PYCXXSRC)/Objects.hxx \
		$(PYCXXSRC)/Version.hxx \
		$(PYCXXSRC)/WrapPython.h \
		$(PYCXXSRC)/COPYRIGHT
else
PYCXXSRC=/usr/share/@PYTHON_LIB@/CXX
endif

nodist_libFreeCADBase_la_SOURCES=\
		$(PYCXXSRC)/cxxextensions.c \
		$(PYCXXSRC)/cxx_extensions.cxx \
		$(PYCXXSRC)/cxxsupport.cxx \
		$(PYCXXSRC)/IndirectPythonInterface.cxx

# zipios++ stuff
if ENABLE_PYCXX_ZIPIOS
ZIPIOS_SRC=../zipios++
nodist_libFreeCADBase_la_SOURCES += \
		$(ZIPIOS_SRC)/basicentry.cpp \
		$(ZIPIOS_SRC)/collcoll.cpp \
		$(ZIPIOS_SRC)/deflateoutputstreambuf.cpp \
		$(ZIPIOS_SRC)/dircoll.cpp \
		$(ZIPIOS_SRC)/directory.cpp \
		$(ZIPIOS_SRC)/fcoll.cpp \
		$(ZIPIOS_SRC)/fcollexceptions.cpp \
		$(ZIPIOS_SRC)/fileentry.cpp \
		$(ZIPIOS_SRC)/filepath.cpp \
		$(ZIPIOS_SRC)/filterinputstreambuf.cpp \
		$(ZIPIOS_SRC)/filteroutputstreambuf.cpp \
		$(ZIPIOS_SRC)/gzipoutputstream.cpp \
		$(ZIPIOS_SRC)/gzipoutputstreambuf.cpp \
		$(ZIPIOS_SRC)/inflateinputstreambuf.cpp \
		$(ZIPIOS_SRC)/zipfile.cpp \
		$(ZIPIOS_SRC)/ziphead.cpp \
		$(ZIPIOS_SRC)/zipheadio.cpp \
		$(ZIPIOS_SRC)/zipinputstream.cpp \
		$(ZIPIOS_SRC)/zipinputstreambuf.cpp \
		$(ZIPIOS_SRC)/zipoutputstreambuf.cpp \
		$(ZIPIOS_SRC)/zipoutputstream.cpp

EXTRA_DIST += \
		$(ZIPIOS_SRC)/backbuffer.h \
		$(ZIPIOS_SRC)/basicentry.h \
		$(ZIPIOS_SRC)/collcoll.h \
		$(ZIPIOS_SRC)/deflateoutputstreambuf.h \
		$(ZIPIOS_SRC)/dircoll.h \
		$(ZIPIOS_SRC)/directory.h \
		$(ZIPIOS_SRC)/fcoll.h \
		$(ZIPIOS_SRC)/fcollexceptions.h \
		$(ZIPIOS_SRC)/fileentry.h \
		$(ZIPIOS_SRC)/filepath.h \
		$(ZIPIOS_SRC)/filterinputstreambuf.h \
		$(ZIPIOS_SRC)/filteroutputstreambuf.h \
		$(ZIPIOS_SRC)/gzipoutputstream.h \
		$(ZIPIOS_SRC)/gzipoutputstreambuf.h \
		$(ZIPIOS_SRC)/inflateinputstreambuf.h \
		$(ZIPIOS_SRC)/meta-iostreams.h \
		$(ZIPIOS_SRC)/outputstringstream.h \
		$(ZIPIOS_SRC)/simplesmartptr.h \
		$(ZIPIOS_SRC)/virtualseeker.h \
		$(ZIPIOS_SRC)/zipfile.h \
		$(ZIPIOS_SRC)/ziphead.h \
		$(ZIPIOS_SRC)/zipheadio.h \
		$(ZIPIOS_SRC)/zipinputstream.h \
		$(ZIPIOS_SRC)/zipinputstreambuf.h \
		$(ZIPIOS_SRC)/zipios_common.h \
		$(ZIPIOS_SRC)/zipios-config.h \
		$(ZIPIOS_SRC)/zipios_defs.h \
		$(ZIPIOS_SRC)/zipoutputstreambuf.h \
		$(ZIPIOS_SRC)/zipoutputstream.h \
		$(nodist_libFreeCADBase_la_SOURCES)
endif

