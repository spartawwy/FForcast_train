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
    QByteArrayData data[19];
    char stringdata[295];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TrainDlg_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TrainDlg_t qt_meta_stringdata_TrainDlg = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 11),
QT_MOC_LITERAL(2, 21, 0),
QT_MOC_LITERAL(3, 22, 15),
QT_MOC_LITERAL(4, 38, 33),
QT_MOC_LITERAL(5, 72, 31),
QT_MOC_LITERAL(6, 104, 22),
QT_MOC_LITERAL(7, 127, 12),
QT_MOC_LITERAL(8, 140, 11),
QT_MOC_LITERAL(9, 152, 9),
QT_MOC_LITERAL(10, 162, 10),
QT_MOC_LITERAL(11, 173, 5),
QT_MOC_LITERAL(12, 179, 6),
QT_MOC_LITERAL(13, 186, 21),
QT_MOC_LITERAL(14, 208, 9),
QT_MOC_LITERAL(15, 218, 18),
QT_MOC_LITERAL(16, 237, 26),
QT_MOC_LITERAL(17, 264, 9),
QT_MOC_LITERAL(18, 274, 19)
    },
    "TrainDlg\0OnStepTimer\0\0DoTblPosDbClick\0"
    "OnTblHangonOrdersRowDoubleClicked\0"
    "OnTblConditionsRowDoubleClicked\0"
    "OnScrollTrainTimeMoved\0OnStartTrain\0"
    "OnStopTrain\0OnControl\0OnNextStep\0OnBuy\0"
    "OnSell\0OnCloseAllUnfrozenPos\0cur_quote\0"
    "SaveStopProfitLoss\0std::vector<PositionAtom>&\0"
    "pos_atoms\0OnAddConditionOrder\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TrainDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a,
       3,    1,   90,    2, 0x0a,
       4,    1,   93,    2, 0x0a,
       5,    1,   96,    2, 0x0a,
       6,    1,   99,    2, 0x0a,
       7,    0,  102,    2, 0x0a,
       8,    0,  103,    2, 0x0a,
       9,    0,  104,    2, 0x0a,
      10,    0,  105,    2, 0x0a,
      11,    0,  106,    2, 0x0a,
      12,    0,  107,    2, 0x0a,
      13,    0,  108,    2, 0x0a,
      14,    0,  109,    2, 0x0a,
      15,    1,  110,    2, 0x0a,
      18,    0,  113,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Double,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,

       0        // eod
};

void TrainDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TrainDlg *_t = static_cast<TrainDlg *>(_o);
        switch (_id) {
        case 0: _t->OnStepTimer(); break;
        case 1: _t->DoTblPosDbClick((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->OnTblHangonOrdersRowDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->OnTblConditionsRowDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->OnScrollTrainTimeMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->OnStartTrain(); break;
        case 6: _t->OnStopTrain(); break;
        case 7: _t->OnControl(); break;
        case 8: _t->OnNextStep(); break;
        case 9: _t->OnBuy(); break;
        case 10: _t->OnSell(); break;
        case 11: _t->OnCloseAllUnfrozenPos(); break;
        case 12: { double _r = _t->cur_quote();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 13: _t->SaveStopProfitLoss((*reinterpret_cast< std::vector<PositionAtom>(*)>(_a[1]))); break;
        case 14: _t->OnAddConditionOrder(); break;
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
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
