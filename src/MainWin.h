/*

Copyright 2015 Aleksander Berg-Jones

This file is part of Shadow's Spider.

Shadow's Spider is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Shadow's Spider is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Shadow's Spider.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef MAINWIN_H
#define MAINWIN_H

#define arraySize(ar) (sizeof(ar) / sizeof(ar[0]))
#define PI 3.14159f
#define versionNum 1.000

#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/compatibility.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QGLWidget>
#include <QDialog>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSizePolicy>
#include <QList>
#include <QGraphicsWidget>
#include <QDebug>
#include <QStackedLayout>
#include <QMenuBar>
#include <QComboBox>
#include <QWheelEvent>
#include <QGraphicsSceneEvent>
#include <QDesktopWidget>
#include <QSplitter>
#include <QShortcut>
#include <QLineEdit>
#include <QModelIndex>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QPushButton>
#include <QColorDialog>
#include <QButtonGroup>
#include <QFileDialog>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QAbstractItemDelegate>
#include <QDesktopServices>
#include <QMimeData>
#include <QProgressBar>
#include <QLabel>

#if __unix__
    #include <curses.h>
#else
    #include <conio.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <random>

using namespace std;

class Anim;
class Attrs;
class CamCombo;
class Combo;
class CPop;
class CPopWin;
class FocusCtrlDel;
class GLWidget;
class GLWidgetSh;
class Gizmo;
class MainWin;
class MenuBarTop;
class MultiAttr;
class Object;
class Outliner;
class Prefs;
class PrefWin;
class PP;
class RadPop;

class MainWin : public QMainWindow
{
    Q_OBJECT
public:
    MainWin(QWidget *parent = 0);
    FocusCtrlDel *myFocus;
    Qt::WindowFlags framelessWin, framedWin;
    QSplitter *gridLay, *hLayU, *hLayD, *vLay;
    QStackedLayout *stackedMain, *stackedAttrTable;
    MenuBarTop *myMenuBar_top;

    shared_ptr<Object> selB, myBB, myTxt, myFSQ, myPivot, myGizNull, myAreaLight, myDirLight, myPointLight, mySpotLight, myVolCone;

    float gamma = 1.f / 2.2f;
    string gizSpace = "world";
    float gizScale = 1.75f;
    QIcon myIconCam, myIconLight;
    int lightCt = 0;

    vector<shared_ptr<CamCombo>> allCamCombo;
    vector<shared_ptr<GLWidget>> allGL;
    vector<shared_ptr<Object>> allObj, allGiz, allGizSide, loadO;

    Anim *myAnim;
    Attrs *attrTable, *etcTable, *glslTable, *cutTable, *pathTable;
    CPopWin *myCPopWin;
    PrefWin *myPrefWin;
    GLWidgetSh *myGLWidgetSh;
    Outliner *myOutliner;
    RadPop *myRadPop;
    QProgressBar *myProgress;
    QWidget *blankW_main;
    Gizmo *myGizmo;
    PP *myPP;

    bool CPopWinTgl, PrefWinTgl, fogTgl, searchB;
    int savedIdx = 2;
    int lastFocusGL = 0;
    QString lastScene, layoutType, myStyle, searchDownType, oldSelB;
    QMenu *ioMenu, *actMenu, *edMenu, *etcMenu, *renderMenu, *rigMenu;
    string selMode = "OBJ";

    bool creatingDynCubeRGB = 0;
    bool doCubeMap = 0;

    QDockWidget *dock_attrTable, *dock_outliner;
    QWidget *dock_attrTable_holder, *attrTable_holder, *dock_outliner_holder;
    QFont menuSmallF;

    GLuint cubeM_specular_32, cubeM_irradiance_32;


    virtual glm::vec2 toVec2(QPoint in) { return glm::vec2(in.x(), in.y()); }
    virtual QPoint toQP(glm::vec2 in) { return QPoint(in.x, in.y); }
    unsigned int countSel();
    int countLights();
    bool tryAddSwitchLight(QString);
    void setLightsDirty();

public slots:
    void progressAdd_init();
    void newScene();
    void openScene();
    void saveScene();
    void saveSceneAs();
    void quitConfirm();
    void startupScene(QString);

    void pivCenter();
    void pivTgl();
    void delete_();
    void dupe();
    void hide();
    void unHide();
    void invertSel();
    void parent();
    void parentWorld();
    void hLayTgl();
    void vLayTgl();
    void gridLayTgl();

    void camLiAdd();
    void objAdd(QString, QString);

    void getChild();
    void getParent();

    void searchUp_intermediate(shared_ptr<Object>);
    bool searchUp(shared_ptr<Object>);

    void pivHide();
    void gizSpaceSwitch();

    void gizShow(string);
    void gizPivAutoShow();
    void PrefWinOpen();
    void TglCPopWin();
    void clearSel();
    void attrTableVizToggle();

    void pivMatchTest();
    glm::vec3 gammaCsel();

    void deferredPrefs();
    void gridInit();
    double GetTickCount2();

protected:
    void addIoMenu(); void addActMenu(); void addEtcMenu(); void addEdMenu(); void addRenderMenu(); void addRigMenu();
    void addAttrTables();
    void addOutliner();
    void centerToScreen(QWidget* widget);

    void camInit();
    void gizInit();
    void objInit();
    void glWidgetInit();

    void restoreZoomedLayout();
};

typedef struct
{
    GLenum face;
    glm::vec3 targ;
    glm::vec3 up;
} CubeShadowTable;

typedef struct
{
    QString name;
    int width, height;
    GLuint fbo1, tex1;
    GLuint64 tex1_64;
    GLuint fbo2, tex2;
    GLuint64 tex2_64;
} AbjNode;

typedef struct
{
    QString type, name;
    glm::vec3 t;
} CamSetup;

typedef struct
{
    QString name;
    glm::vec3 r;
} GridSetup;

typedef struct
{
    QString type, name;
    glm::vec3 Cgiz, r, t;
} GizSetup;

typedef struct
{
    QString type, name;
} LightSetup;

typedef struct
{
    QString attr;
    float attrVal;
} DynSelCamAttrs;

typedef struct
{
    shared_ptr<GLWidget> GL;
    bool loaded;
    GLuint VAO;
} DynVAO;

typedef struct
{
    int idx;
    QSplitter *split;
} FourGrid_idx;

typedef struct
{
    QString cam;
    int idx;
    QSplitter &split;
} GLSetup;

typedef struct
{
    shared_ptr<Object> obj;
    GLuint VBO_P, VBO_UV, VBO_T, VBO_B, VBO_N, VBO_IDX;
} GLSharedData;

typedef struct
{
    QString name, type;
    int sx, sy, offX, offY;
    QPolygon hoverTri;
    glm::vec2 move;
    bool hovered;
} PopSetup;

typedef struct
{
    QPushButton *button;
    QString name;
} TableButtons;

typedef struct
{
    int row, col;
} TableRowCol;

#include "Anim.h"
#include "Attrs.h"
#include "CamCombo.h"
#include "CPop.h"
#include "CPopWin.h"
#include "GLWidget.h"
#include "GLWidgetSh.h"
#include "Gizmo.h"
#include "Object.h"
#include "Outliner.h"
#include "Prefs.h"
#include "PrefWin.h"
#include "PP.h"
#include "RadPop.h"

/* HEADER ONLY */
class Combo : public QComboBox
{
    Q_OBJECT
public:
    Combo(QWidget *parent = 0) : QComboBox(parent) { ; }

protected:
    virtual void wheelEvent(QWheelEvent *e) { e->ignore(); }
};

class MenuBarTop : public QMenuBar
{
public:
    MainWin &myWin;
    MenuBarTop(MainWin &myWinTemp, QWidget *parent = 0) : QMenuBar(parent), myWin(myWinTemp) { setFont(myWin.menuSmallF); }

    glm::vec2 mpos;

protected:
    virtual void mousePressEvent(QMouseEvent *e) { mpos = myWin.toVec2(e->pos()); return QMenuBar::mousePressEvent(e); }
    virtual void mouseMoveEvent(QMouseEvent *e) { if (e->buttons() && Qt::LeftButton) myWin.move(e->globalX() - mpos.x, e->globalY() - mpos.y); }

public slots:
};

class FocusCtrlDel : public QStyledItemDelegate
{
public:
    FocusCtrlDel(QObject *parent = 0) : QStyledItemDelegate(parent) { ; }

    virtual void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
    {
        QStyledItemDelegate::initStyleOption(option, index);

        if (option->state & QStyle::State_HasFocus)
            option->state = option->state & ~QStyle::State_HasFocus;
    }
};

class MultiAttr
{
public:
    MultiAttr() { ; }
    shared_ptr<MultiAttr> Clone() const { return(shared_ptr<MultiAttr>(CloneImpl())); }

    QString name, type, typeX, val_s;
    bool repop = 1;
    bool val_b;
    int val_i, idx_3;
    int tab = 0;
    float val_f;
    float min = -9999.f;
    float max = 9999.f;
    glm::vec2 val_2;
    glm::vec3 val_3;
    glm::vec4 val_4;
    QStringList comboList;
    QShortcut *cut;

    shared_ptr<Object> obj;

//protected:
    virtual MultiAttr* CloneImpl() const { return(new MultiAttr(*this)); }

//    virtual void set(glm::vec3) { val_3 = val; if (name == "t") obj->piv->val_3 += val; }
//    void set(float, float, float) { val_3 = glm::vec3(x, y, z); if (name == "t") obj->piv->val_3 += glm::vec3(x, y, z); }

};


#endif // MAINWIN_H
