/** \file Libs.cpp
 *  \brief Include all needed libs on Windows
 *  Here all the libs get includet by a #pragma dirctive.
 *  Unfortunatly there is nothin comperable on UNIX, so there
 *  you have to use compiler -l staments, which are somwere deep
 *  in the Makefile.
 */


// === Incuding of libs: ============================================================================

#include "../../../Config.h"
#ifdef FC_OS_WIN32
#	pragma comment(lib,"TKernel.lib")
# pragma comment(lib,"TKBRep.lib")
#	pragma comment(lib,"TKMath.lib")
#	pragma comment(lib,"TKService.lib")
#	pragma comment(lib,"TKGeomAlgo.lib")
#	pragma comment(lib,"TKGeomBase.lib")
# pragma comment(lib,"TKBool.lib")
#	pragma comment(lib,"TKG2d.lib")
#	pragma comment(lib,"TKG3d.lib")
#	pragma comment(lib,"TKBRep.lib")
#	pragma comment(lib,"TKTopAlgo.lib")
#	pragma comment(lib,"TKPrim.lib")
#	pragma comment(lib,"TKXSBase.lib")
	// Application Framwork OCAF
#	pragma comment(lib,"TKCAF.lib")
#	pragma comment(lib,"TKCDF.lib")

// OpenCasCade Std IO
#	pragma comment(lib,"TKIGES.lib")
#	pragma comment(lib,"TKShHealing.lib")
#	pragma comment(lib,"TKSTEP.lib")


#endif


