

// === Incuding of libs: ============================================================================

#include "../../../FCConfig.h"
#ifdef FC_OS_WIN32
# pragma comment(lib,"TKernel.lib")
# pragma comment(lib,"TKMath.lib")
# pragma comment(lib,"TKService.lib")
# pragma comment(lib,"TKGeomAlgo.lib")
# pragma comment(lib,"TKGeomBase.lib")
# pragma comment(lib,"TKG2d.lib")
# pragma comment(lib,"TKG3d.lib")
# pragma comment(lib,"TKBRep.lib")
# pragma comment(lib,"TKTopAlgo.lib")
# pragma comment(lib,"TKPrim.lib")
#	pragma comment(lib,"TKXSBase.lib")
//TKernel.lib TKMath.lib TKService.lib TKGeomAlgo.lib TKGeomBase.lib TKG2d.lib TKG3d.lib TKBRep.lib TKTopAlgo.lib TKPrim.lib TKXSBase.lib

// wild magic library (succesor of MGC)

# if defined(_MSC_VER) && _MSC_VER >= 1400
#   pragma comment(lib,"Wm3Foundation80.lib")
# else
#	  pragma comment(lib,"Wm3Foundation60.lib")
# endif
// GTS
#	pragma comment(lib,"gts-0.7.lib")
//#	pragma comment(lib,"gthread-1.3.lib")
#	pragma comment(lib,"glib-1.3.lib")
//#	pragma comment(lib,"gmodule-1.3.lib")
//#	pragma comment(lib,"gobject-1.3.lib")


// FreeCAD libs
# ifdef FC_DEBUG
#   pragma comment(lib,"FreeCADBaseD.lib")
#   pragma comment(lib,"FreeCADAppD.lib")
# else
#   pragma comment(lib,"FreeCADBase.lib")
#   pragma comment(lib,"FreeCADApp.lib")
# endif
#endif


