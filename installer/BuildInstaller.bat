

C:\Libs\Libs\wix_30\candle.exe -out FreeCADBase.wxobj    FreeCADBase.wxs
C:\Libs\Libs\wix_30\candle.exe -out LibPack.wxobj        LibPack.wxs
C:\Libs\Libs\wix_30\candle.exe -out FreeCADDoc.wxobj     FreeCADDoc.wxs
C:\Libs\Libs\wix_30\candle.exe -out FreeCADModules.wxobj FreeCADModules.wxs
C:\Libs\Libs\wix_30\candle.exe -out FreeCAD.wxobj        FreeCAD.wxs

C:\Libs\Libs\wix_30\light.exe  -ext WixUIExtension -cultures:en-us -out FreeCAD.msi *.wxobj

