
set FREECADLIB    = D:\Develop\Projekte\FreeCAD\FreeCADLibPack\FreeCADLibs4

SubWCRev .. Version.wxi.in Version.wxi

candle -out FreeCADBase.wxobj    FreeCADBase.wxs
candle -out LibPack4.wxobj       LibPack4.wxs
rem candle -out FreeCADDoc.wxobj     FreeCADDoc.wxs
candle -out FreeCADModules.wxobj FreeCADModules.wxs
candle -out FreeCAD.wxobj        FreeCAD.wxs

light  -ext WixUIExtension -cultures:en-us -out FreeCAD.msi *.wxobj

SubWCRev .. CopyRelease.bat.in CopyRelease.bat
call CopyRelease.bat
del *.wxobj CopyRelease.bat Version.wxi FreeCAD.msi

