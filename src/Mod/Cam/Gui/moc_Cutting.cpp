/****************************************************************************
** Meta object code from reading C++ file 'Cutting.h'
**
** Created: Mon 14. Jan 13:31:53 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Cutting.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Cutting.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CamGui__Cutting[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x09,
      47,   16,   16,   16, 0x09,
     102,   16,   16,   16, 0x09,
     156,   16,   16,   16, 0x09,
     210,   16,   16,   16, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CamGui__Cutting[] = {
    "CamGui::Cutting\0\0on_CalculcateZLevel_clicked()\0"
    "on_toolpath_calculation_highest_level_button_clicked()\0"
    "on_toolpath_calculation_middle_level_button_clicked()\0"
    "on_toolpath_calculation_lowest_level_button_clicked()\0"
    "on_toolpath_calculation_go_button_clicked()\0"
};

const QMetaObject CamGui::Cutting::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CamGui__Cutting,
      qt_meta_data_CamGui__Cutting, 0 }
};

const QMetaObject *CamGui::Cutting::metaObject() const
{
    return &staticMetaObject;
}

void *CamGui::Cutting::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CamGui__Cutting))
	return static_cast<void*>(const_cast< Cutting*>(this));
    if (!strcmp(_clname, "Ui_Cutting"))
	return static_cast< Ui_Cutting*>(const_cast< Cutting*>(this));
    return QDialog::qt_metacast(_clname);
}

int CamGui::Cutting::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_CalculcateZLevel_clicked(); break;
        case 1: on_toolpath_calculation_highest_level_button_clicked(); break;
        case 2: on_toolpath_calculation_middle_level_button_clicked(); break;
        case 3: on_toolpath_calculation_lowest_level_button_clicked(); break;
        case 4: on_toolpath_calculation_go_button_clicked(); break;
        }
        _id -= 5;
    }
    return _id;
}
