
set FREECADLIB=D:\Develop\Projekte\FreeCAD\FreeCADLibPack\FreeCADLibs4

C:\Programme\TortoiseSVN\bin\SubWCRev.exe .. Version.wxi.in Version.wxi

D:\Develop\Libs\wix_30\candle.exe -out FreeCADBase.wxobj    FreeCADBase.wxs
#D:\Develop\Libs\wix_30\candle.exe -out FreeCADDoc.wxobj     FreeCADDoc.wxs
D:\Develop\Libs\wix_30\candle.exe -out FreeCADModules.wxobj FreeCADModules.wxs
D:\Develop\Libs\wix_30\candle.exe -out FreeCAD.wxobj        FreeCAD.wxs

D:\Develop\Libs\wix_30\light.exe -ext WixUIExtension -cultures:en-us -out FreeCAD.msi *.wxobj

mkdir ..\Install
copy FreeCAD.msi ..\Install

readline 