#ifndef DLGSETTINGSMACROIMP_H
#define DLGSETTINGSMACROIMP_H
#include "DlgSettingsMacro.h"

namespace Gui {
namespace Dialog {

class DlgSettingsMacroImp : public DlgSettingsMacro, public Gui::Dialog::PreferencePage
{ 
    Q_OBJECT

    virtual void ChooseDir();

public:
    DlgSettingsMacroImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettingsMacroImp();

};

} // namespace Dialog
} // namespace Gui

#endif // DLGSETTINGSMACROIMP_H
