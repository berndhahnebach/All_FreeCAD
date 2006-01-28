
lib_LTLIBRARIES=libFreeCADBase.la

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
		Base.cpp \
		Base.h \
		BoundBox.cpp \
		BoundBox.h \
		Console.cpp \
		Console.h \
		Documentation.cpp \
		Documentation.h \
		Doxygen.cpp \
		EnvMacros.h \
		Exception.cpp \
		Exception.h \
		Factory.cpp \
		Factory.h \
		FileInfo.cpp \
		FileInfo.h \
		FileTemplate.cpp \
		FileTemplate.h \
		gzstream.cpp \
		gzstream.h \
		Handle.cpp \
		Handle.h \
		InputSource.cpp \
		InputSource.h \
		Interpreter.cpp \
		Interpreter.h \
		Matrix.cpp \
		Matrix.h \
		Observer.cpp \
		Observer.h \
		Parameter.cpp \
		Parameter.h \
		ParameterPy.cpp \
		Persistance.cpp \
		Persistance.h \
		PreCompiled.cpp \
		PreCompiled.h \
		PyExport.cpp \
		PyExport.h \
		PyExportImp.cpp \
		PyExportImp.h \
		PyTools.c \
		PyTools.h \
		Reader.cpp \
		Reader.h \
		Sequencer.cpp \
		Sequencer.h \
		Stream.cpp \
		Stream.h \
		Swap.cpp \
		Swap.h \
		Time.cpp \
		Time.h \
		Type.cpp \
		Type.h \
		Tools2D.cpp \
		Tools2D.h \
		Vector3D.cpp \
		Vector3D.h \
		ViewProj.h \
		XMLTools.h 

libFreeCADBase_la_LIBADD   =  -lpython@fc_py_ver@ -lxerces-c -lz

# set the include path found by configure
INCLUDES= $(all_includes)

# the library version number
libFreeCADBase_la_LDFLAGS = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

EXTRA_DIST = \
		DllMain.cpp \
		FreeCADBase.dsp \
		FreeCADBase.vcproj \
		Libs.cpp

