// FCStdExtractor.cpp : Implementation of CFCStdExtractor
#include "stdafx.h"
#include "resource.h"
#include "ThumbFCStd_i.h"
#include <initguid.h>
#include "ThumbFCStd_i.c"
#include "FCStdExtractor.h"
#include <shlobj.h>

#include <iostream>
#include <Base/zipios/zipinputstream.h>
#include <Base/zipios/zipfile.h>
#include <CxImage/xmemfile.h>
#include <CxImage/ximajpg.h>

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_FCStdExtractor, CFCStdExtractor)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        _Module.Init(ObjectMap, hInstance, &LIBID_THUMBFCSTDLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CFCStdExtractor

HRESULT CFCStdExtractor::GetLocation(LPWSTR pszPathBuffer,
        DWORD cchMax, DWORD *pdwPriority,
        const SIZE *prgSize, DWORD dwRecClrDepth,
        DWORD *pdwFlags)
{
    m_bmSize = *prgSize;
    if (*pdwFlags & IEIFLAG_ASYNC)	return E_PENDING; 
    return NOERROR;
}

HRESULT CFCStdExtractor::Load(LPCOLESTR wszFile, DWORD dwMode)
{
    USES_CONVERSION;
    _tcscpy(m_szFile, OLE2T((WCHAR*)wszFile)); 
    return S_OK;	
};
/*
HBITMAP CExtractImageApp::CreateThumbnail(const SIZE bmSize)
{
    HBITMAP hThumb; CBitmap bmpThumb; 
    if(!m_pDoc) return NULL;

    CSize bmDocSize = GetDocSize(); // derived class knows it
    // Create memory DC, create a color bitmap, and draw on it
    CDC memdc;
    memdc.CreateCompatibleDC(NULL);
    bmpThumb.CreateBitmap(bmSize.cx,bmSize.cy,
	    memdc.GetDeviceCaps(PLANES), memdc.GetDeviceCaps(BITSPIXEL),NULL); 
    CBitmap* pOldBm = memdc.SelectObject(&bmpThumb);

    memdc.PatBlt(0,0,bmSize.cx,bmSize.cy,WHITENESS);

    // TODO: You may choose to change the mapping here
    memdc.SetMapMode(MM_ISOTROPIC);
    memdc.SetViewportExt(bmSize.cx,-bmSize.cy);
    memdc.SetWindowExt(bmDocSize.cx,bmDocSize.cy);
    OnDraw(&memdc); //let the derived class to handle it
    memdc.SelectObject(pOldBm); 
    hThumb = (HBITMAP)bmpThumb.Detach(); 
    return hThumb; 
}
*/
#include <qimage.h>
HBITMAP convertToBitmap (const QImage& image)
{
    HDC hDC = CreateCompatibleDC(NULL);

    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = image.width();
    bmi.bmiHeader.biHeight = image.height();
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed  = 0;
    bmi.bmiHeader.biClrImportant = 0;
    bmi.bmiColors[0].rgbBlue = 0;
    bmi.bmiColors[0].rgbGreen;
    bmi.bmiColors[0].rgbRed;
    bmi.bmiColors[0].rgbReserved;

    // make a bitmap to draw to
    uchar *pvBits=NULL;
    HBITMAP hBMP = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&pvBits, NULL, 0);

    SelectObject(hDC, hBMP);

    for(int x=0;x<image.width();++x) {
        for(int y=0;y<image.height();++y) {
            QRgb rgb = image.pixel(x,y);
            ::SetPixel(hDC,x,y,RGB(qRed(rgb),qGreen(rgb),qBlue(rgb)));
        }
    }

    DeleteDC(hDC);

    return hBMP;
}

// IExtractImage::Extract
HRESULT CFCStdExtractor::Extract(HBITMAP* phBmpThumbnail)
{
    return NOERROR; 
    //theApp.LoadDoc(m_szFile);
    //QImage img;
    //img.load("C:\\Temp\\Download\\tmp\\thumb\\Bild1.jpg");
    //m_hPreview = convertToBitmap(img);
    zipios::ZipFile file(m_szFile);
    zipios::ConstEntries files = file.entries();
    zipios::ConstEntryPointer entry = file.getEntry("Thumbnail.jpg");
    if (entry->isValid()) {
        std::istream *str = file.getInputStream(entry);
        std::vector<unsigned char> content;
        unsigned char c;
        while (str->get((char&)c)) {
            content.push_back(c);
        }

        //std::ofstream out("myview.jpg",std::ios::out | std::ios::binary);
        //out.write((char*)&(content[0]),content.size());
        //out.close();

        CxMemFile mem(&(content[0]),content.size());
        CxImageJPG jpg;
        jpg.Decode(&mem);
        m_hPreview = jpg.MakeBitmap();

        *phBmpThumbnail = m_hPreview;
    }

    //m_hPreview = theApp.CreateThumbnail(m_bmSize);
    //::LoadBitmap(_Module.GetModuleInstance(),MAKEINTRESOURCE(IDB_BITMAP2)); 
    //*phBmpThumbnail = m_hPreview;
    return NOERROR; 
}

HRESULT CFCStdExtractor::GetDateStamp(FILETIME *pDateStamp)
{
    FILETIME ftCreationTime,ftLastAccessTime,ftLastWriteTime;
    // open the file and get last write time
    HANDLE hFile = CreateFile(m_szFile,GENERIC_READ,FILE_SHARE_READ,NULL,
        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(!hFile) return E_FAIL;
    GetFileTime(hFile,&ftCreationTime,&ftLastAccessTime,&ftLastWriteTime);
    CloseHandle(hFile);
    *pDateStamp = ftLastWriteTime;
    return NOERROR; 
}

