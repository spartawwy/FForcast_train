/****************************************************************************
** Meta object code from reading C++ file 'train_dlg.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../train_dlg.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'train_dlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TrainDlg_t {
    QByteArrayData data[11];
    char stringdata[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TrainDlg_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TrainDlg_t qt_meta_stringdata_TrainDlg = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 33),
QT_MOC_LITERAL(2, 43, 0),
QT_MOC_LITERAL(3, 44, 22),
QT_MOC_LITERAL(4, 67, 12),
QT_MOC_LITERAL(5, 80, 11),
QT_MOC_LITERAL(6, 92, 13),
QT_MOC_LITERAL(7, 106, 10),
QT_MOC_LITERAL(8, 117, 7),
QT_MOC_LITERAL(9, 125, 5),
QT_MOC_LITERAL(10, 131, 6)
    },
    "TrainDlg\0OnTblHangonOrdersRowDoubleClicked\0"
    "\0OnScrollTrainTimeMoved\0OnStartTrain\0"
    "OnStopTrain\0OnMoveToNextK\0OnNextStep\0"
    "OnTrade\0OnBuy\0OnSell\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TrainDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x0a,
       3,    1,   62,    2, 0x0a,
       4,    0,   65,    2, 0x0a,
       5,    0,   66,    2, 0x0a,
       6,    0,   67,    2, 0x0a,
       7,    0,   68,    2, 0x0a,
       8,    0,   69,    2, 0x0a,
       9,    0,   70,    2, 0x0a,
      10,    0,   71,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TrainDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TrainDlg *_t = static_cast<TrainDlg *>(_o);
        switch (_id) {
        case 0: _t->OnTblHangonOrdersRowDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->OnScrollTrainTimeMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->OnStartTrain(); break;
        case 3: _t->OnStopTrain(); break;
        case 4: _t->OnMoveToNextK(); break;
        case 5: _t->OnNextStep(); break;
        case 6: _t->OnTrade(); break;
        case 7: _t->OnBuy(); break;
        case 8: _t->OnSell(); break;
        default: ;
        }
    }
}

const QMetaObject TrainDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TrainDlg.data,
      qt_meta_data_TrainDlg,  qt_static_metacall, 0, 0}
};


const QMetaObject *TrainDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TrainDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TrainDlg.stringdata))
        return static_cast<void*>(const_cast< TrainDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int TrainDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
