#ifndef DLGDOCTEMPLATESIMP_H
#define DLGDOCTEMPLATESIMP_H
#include "DlgDocTemplates.h"

class DlgDocTemplatesImp : public DlgDocTemplates
{ 
    Q_OBJECT

public:
    DlgDocTemplatesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgDocTemplatesImp();

public slots:
    virtual void ChoseFile();
    virtual void Validate();
    virtual void ViewChange(QIconViewItem*);
    virtual void EditFile();
	virtual void IconDoubleClick(QIconViewItem*);
};

#endif // DLGDOCTEMPLATESIMP_H
