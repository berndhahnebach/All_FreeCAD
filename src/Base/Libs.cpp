/** \file Libs.cpp
 *  \brief Include all needed libs on Windows
 *  Here all the libs get includet by a #pragma dirctive.
 *  Unfortunatly there is nothin comperable on UNIX, so there
 *  you have to use compiler -l staments, which are somwere deep
 *  in the Makefile.
 */


// === Incuding of libs: ============================================================================
#ifdef WNT
#	pragma comment(lib,"python21.lib")
	// STLport
#	ifndef __STL_DEBUG
#		pragma comment(lib,"stlport_vc6.lib")
#	else
#		pragma comment(lib,"stlport_vc6_debug.lib")
#		pragma comment(lib,"stlport_vc6_stldebug.lib")
#	endif

#endif