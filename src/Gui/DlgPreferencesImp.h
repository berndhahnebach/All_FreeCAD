#ifndef DLGPREFERENCESIMP_H
#define DLGPREFERENCESIMP_H
#include "DlgPreferences.h"

class DlgPreferencesImp : public DlgPrefernces
{ 
    Q_OBJECT

public:
    DlgPreferencesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgPreferencesImp();

};

#endif // DLGPREFERENCESIMP_H
