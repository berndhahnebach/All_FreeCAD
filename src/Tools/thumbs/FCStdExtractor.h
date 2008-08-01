// FCStdExtractor.h : Declaration of the CFCStdExtractor

#ifndef FC_STD_EXTRACTOR_H
#define FC_STD_EXTRACTOR_H

#include "resource.h"               // main symbols
#include <shlguid.h>
#include <AtlCom.h>
#include <shlobj.h>
#include "IExtractImage.h"

/////////////////////////////////////////////////////////////////////////////
// CFCStdExtractor
class ATL_NO_VTABLE CFCStdExtractor : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CFCStdExtractor, &CLSID_FCStdExtractor>,
    public IPersistFile,
    public IExtractImage2
{
public:
    CFCStdExtractor()
    {
    }

DECLARE_REGISTRY_RESOURCEID(IDR_THUMBSCB)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CFCStdExtractor)
    COM_INTERFACE_ENTRY(IPersistFile)
    COM_INTERFACE_ENTRY(IExtractImage)
    COM_INTERFACE_ENTRY(IExtractImage2)
END_COM_MAP()

// IExtractImage
public:

    STDMETHOD(GetLocation)(LPWSTR pszPathBuffer,
                           DWORD cchMax,
                           DWORD *pdwPriority,
                           const SIZE *prgSize,
                           DWORD dwRecClrDepth,
                           DWORD *pdwFlags);
    STDMETHOD(Extract)(HBITMAP*);
    // IExtractImage2
    STDMETHOD(GetDateStamp)(FILETIME *pDateStamp);

    // IPersistFile
    STDMETHOD(Load)(LPCOLESTR wszFile, DWORD dwMode);

    STDMETHOD(GetClassID)(LPCLSID clsid)
    { MessageBox(0,"GetClassID",0,0);
    return E_NOTIMPL;	}

    STDMETHOD(IsDirty)(VOID)
    { MessageBox(0,"IsDirty",0,0);
    return E_NOTIMPL; }

    STDMETHOD(Save)(LPCOLESTR, BOOL)
    { MessageBox(0,"Save",0,0);
    return E_NOTIMPL; }

    STDMETHOD(SaveCompleted)(LPCOLESTR)
    { MessageBox(0,"SaveCompleted",0,0);
    return E_NOTIMPL; }

    STDMETHOD(GetCurFile)(LPOLESTR FAR*)
    { MessageBox(0,"GetCurFile",0,0);
    return E_NOTIMPL; }

private:
    SIZE m_bmSize;
    HBITMAP m_hPreview;
    TCHAR m_szFile[500];
};

#endif //FC_STD_EXTRACTOR_H
