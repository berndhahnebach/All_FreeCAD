#ifndef DLGDOCTEMPLATESIMP_H
#define DLGDOCTEMPLATESIMP_H
#include "DlgDocTemplates.h"

class DlgDocTemplatesImp : virtual public DlgDocTemplates
{ 
    Q_OBJECT

public:
//#ifdef __linux
   /* ld complains: undefined reference to
      `DlgDocTemplatesImp::DlgDocTemplatesImp(int, QWidget *, char const *, bool, unsigned int)'
      Why...?  */
   DlgDocTemplatesImp(int dummy, QWidget* parent,  const char* name, bool modal, WFlags fl )
     {DlgDocTemplatesImp(parent,name,modal,fl);}
//#endif
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
