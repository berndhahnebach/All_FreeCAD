#ifndef DLGSETTINGSMACROIMP_H
#define DLGSETTINGSMACROIMP_H
#include "DlgSettingsMacro.h"

class FCDlgSettingsMacro : public DlgSettingsMacro
{ 
    Q_OBJECT

    virtual void ChooseDir();

public:
    FCDlgSettingsMacro( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgSettingsMacro();

};

#endif // DLGSETTINGSMACROIMP_H
