/****************************************************************************
** Meta object code from reading C++ file 'Attrs.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/Attrs.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Attrs.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Attrs_t {
    QByteArrayData data[9];
    char stringdata[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Attrs_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Attrs_t qt_meta_stringdata_Attrs = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Attrs"
QT_MOC_LITERAL(1, 6, 10), // "changeEnum"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 10), // "boolChange"
QT_MOC_LITERAL(4, 29, 9), // "colorPick"
QT_MOC_LITERAL(5, 39, 10), // "writeValue"
QT_MOC_LITERAL(6, 50, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(7, 68, 11), // "multiCutTgl"
QT_MOC_LITERAL(8, 80, 5) // "state"

    },
    "Attrs\0changeEnum\0\0boolChange\0colorPick\0"
    "writeValue\0QTableWidgetItem*\0multiCutTgl\0"
    "state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Attrs[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       3,    0,   42,    2, 0x0a /* Public */,
       4,    0,   43,    2, 0x0a /* Public */,
       5,    1,   44,    2, 0x0a /* Public */,
       7,    1,   47,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, QMetaType::Bool,    8,

       0        // eod
};

void Attrs::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Attrs *_t = static_cast<Attrs *>(_o);
        switch (_id) {
        case 0: _t->changeEnum((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->boolChange(); break;
        case 2: _t->colorPick(); break;
        case 3: _t->writeValue((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->multiCutTgl((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Attrs::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_Attrs.data,
      qt_meta_data_Attrs,  qt_static_metacall, 0, 0}
};


const QMetaObject *Attrs::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Attrs::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Attrs.stringdata))
        return static_cast<void*>(const_cast< Attrs*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int Attrs::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
