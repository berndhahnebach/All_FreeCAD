/** \file Libs.cpp
 *  \brief Include all needed libs on Windows
 *  Here all the libs get includet by a #pragma dirctive.
 *  Unfortunatly there is nothin comperable on UNIX, so there
 *  you have to use compiler -l staments, which are somwere deep
 *  in the Makefile.
 */


// === Including of libs: ============================================================================
#ifdef WNT
	// STLport
//#	ifndef __STL_DEBUG
//#		pragma comment(lib,"stlport_vc6.lib")
//#	else
//#		pragma comment(lib,"stlport_vc6_debug.lib")
//#		pragma comment(lib,"stlport_vc6_stldebug.lib")
//#	endif
#	pragma comment(lib,"TKernel.lib")
#	pragma comment(lib,"TKMath.lib")
#	pragma comment(lib,"TKService.lib")
#	pragma comment(lib,"TKGeomAlgo.lib")
#	pragma comment(lib,"TKGeomBase.lib")
#	pragma comment(lib,"TKG2d.lib")
#	pragma comment(lib,"TKG3d.lib")
#	pragma comment(lib,"TKBRep.lib")
#	pragma comment(lib,"TKTopAlgo.lib")
	// OpenCasCade Std IO
#	pragma comment(lib,"TKIGESStd.lib")
#	pragma comment(lib,"TKShHealingStd.lib")
#	pragma comment(lib,"TKSTEPStd.lib")
	// OpenCascade View
#	pragma comment(lib,"TKV2d.lib")
#	pragma comment(lib,"TKV3d.lib")
	// Application Framwork OCAF
#	pragma comment(lib,"TKCAF.lib")
#	pragma comment(lib,"TKCDF.lib")
	// QT extension
#	pragma comment(lib,"qextmdi.lib")

#endif

