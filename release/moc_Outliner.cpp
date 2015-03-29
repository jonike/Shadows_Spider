/****************************************************************************
** Meta object code from reading C++ file 'Outliner.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/Outliner.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Outliner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Outliner_t {
    QByteArrayData data[8];
    char stringdata[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Outliner_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Outliner_t qt_meta_stringdata_Outliner = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Outliner"
QT_MOC_LITERAL(1, 9, 10), // "changeName"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 38, 9), // "changeSel"
QT_MOC_LITERAL(5, 48, 11), // "disableCuts"
QT_MOC_LITERAL(6, 60, 11), // "fullyExpand"
QT_MOC_LITERAL(7, 72, 13) // "fullyCollapse"

    },
    "Outliner\0changeName\0\0QTreeWidgetItem*\0"
    "changeSel\0disableCuts\0fullyExpand\0"
    "fullyCollapse"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Outliner[] = {

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
       1,    2,   39,    2, 0x0a /* Public */,
       4,    0,   44,    2, 0x0a /* Public */,
       5,    0,   45,    2, 0x0a /* Public */,
       6,    1,   46,    2, 0x0a /* Public */,
       7,    1,   49,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void Outliner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Outliner *_t = static_cast<Outliner *>(_o);
        switch (_id) {
        case 0: _t->changeName((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->changeSel(); break;
        case 2: _t->disableCuts(); break;
        case 3: _t->fullyExpand((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->fullyCollapse((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Outliner::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_Outliner.data,
      qt_meta_data_Outliner,  qt_static_metacall, 0, 0}
};


const QMetaObject *Outliner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Outliner::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Outliner.stringdata))
        return static_cast<void*>(const_cast< Outliner*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int Outliner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
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
