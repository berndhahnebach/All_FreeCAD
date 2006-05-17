

// === Including of libs: ============================================================================
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

  // QT extension
# pragma comment(lib,QTLIBNAME)
# pragma comment(lib,INVENTORLIBNAME)
# pragma comment(lib,SOQTLIBNAME)

# ifdef FC_DEBUG
#   pragma comment(lib,"FreeCADBaseD.lib")
#   pragma comment(lib,"FreeCADAppD.lib")
#   pragma comment(lib,"FreeCADGuiD.lib")
# else
#   pragma comment(lib,"FreeCADBase.lib")
#   pragma comment(lib,"FreeCADApp.lib")
#   pragma comment(lib,"FreeCADGui.lib")
# endif

#else
//#error "Dont compile this file on UNIX!"
#endif

