#ifndef DLGSETTINGSMACROIMP_H
#define DLGSETTINGSMACROIMP_H
#include "DlgSettingsMacro.h"

namespace Gui {
namespace Dialog {

class CDlgSettingsMacroImp : public DlgSettingsMacro, public Gui::Dialog::PreferencePage
{ 
    Q_OBJECT

    virtual void ChooseDir();

public:
    CDlgSettingsMacroImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CDlgSettingsMacroImp();

};

} // namespace Dialog
} // namespace Gui

#endif // DLGSETTINGSMACROIMP_H
