#ifndef DLGMACROEXECUTEIMP_H
#define DLGMACROEXECUTEIMP_H
#include "DlgMacroExecute.h"

class DlgMacroExecuteImp : public DlgMacroExecute
{ 
    Q_OBJECT

public:
    DlgMacroExecuteImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgMacroExecuteImp();

public slots:
    void OnExecute();
    void OnNewFolder();
    void OnRecord();
    void OnNewListItemPicked(QListViewItem*);

};

#endif // DLGMACROEXECUTEIMP_H
