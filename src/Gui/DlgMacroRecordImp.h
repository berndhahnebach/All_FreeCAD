#ifndef DLGMACRORECORDIMP_H
#define DLGMACRORECORDIMP_H
#include "DlgMacroRecord.h"

class DlgMacroRecordImp : public DlgMacroRecord
{ 
    Q_OBJECT

public:
    DlgMacroRecordImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgMacroRecordImp();

public slots:
    void OnTieCommandBar();
    void OnTieToolBar();
    void OnTieKeyboard();

};

#endif // DLGMACRORECORDIMP_H
