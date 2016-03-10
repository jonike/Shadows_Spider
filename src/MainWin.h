/*

Copyright 2015 Aleks Berg-Jones

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
#include <gtx/color_space.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QGLWidget>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QHeaderView>
#include <QSizePolicy>
#include <QStackedLayout>
#include <QMenuBar>
#include <QComboBox>
#include <QWheelEvent>
#include <QDesktopWidget>
#include <QSplitter>
#include <QShortcut>
#include <QLineEdit>
#include <QModelIndex>
#include <QTimer>
#include <QPushButton>
#include <QFileDialog>
#include <QProgressBar>
#include <QTabletEvent>
#include <QStyledItemDelegate>
#include <QAbstractItemDelegate>
#include <QMimeData>
#include <QDebug>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <chrono>
#include <random>

using namespace std;

class Anim;
class Attrs;
class CamCombo;
class Combo;
class CPop;
class FocusCtrlDel;
class GLWidget;
class GLWidgetSh;
class Gizmo;
class MainWin;
class MenuBarTop;
class MultiAttr;
class Object;
class Outliner;
class LayerSel;
class PaintWin;
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
    QIcon myIconCam, myIconLight;
    QProgressBar *myProgress;
    QWidget *blankW_main;
    QMenu *ioMenu, *actMenu, *edMenu, *etcMenu, *renderMenu, *rigMenu;
    QDockWidget *dock_attrTable, *dock_outliner;
    QWidget *dock_attrTable_holder, *attrTable_holder, *dock_outliner_holder;
    QFont menuSmallF;

    shared_ptr<Object> selB, myBB, myTxt, myFSQ, myPivot, myGizNull, myAreaLight, myDirLight, myPointLight, mySpotLight, myVolCone, paintStroke;

    float gamma = 1.f / 2.2f;
    string gizSpace = "world";
    float gizScale = 1.75f;
    int lightCt = 0;

    vector<shared_ptr<CamCombo>> allCamCombo;
    vector<shared_ptr<GLWidget>> allGL;
    vector<shared_ptr<Object>> allObj, allGiz, allGizSide, newObj;

    MenuBarTop *myMenuBar_top;
    Anim *myAnim;
    Attrs *attrTable, *etcTable, *glslTable, *cutTable, *pathTable;
    PaintWin *myPaintWin;
    LayerSel *objectLay, *typeLay, *mapLay, *layerLay;
    PrefWin *myPrefWin;
    GLWidgetSh *myGLWidgetSh;
    Outliner *myOutliner;
    RadPop *myRadPop;
    Gizmo *myGizmo;
    PP *myPP;
    GLuint cubeM_specular_32, cubeM_irradiance_32;

    bool searchB;
    int savedIdx = 2;
    int ID_lastFocused = 0;
    bool isTyping = false;

    string lastScene, oldSelB, layoutType, myStyle;
    string selMode = "OBJ";

    int brushSize = 10;
    int brushHard = 100;

    virtual glm::vec2 toVec2(QPoint in) { return glm::vec2(in.x(), in.y()); }
    virtual QPoint toQP(glm::vec2 in) { return QPoint(in.x, in.y); }
    virtual QColor toQC(glm::vec3 in) { return QColor::fromRgbF(in.r, in.g, in.b); }

    unsigned int countSel();
    int countLights();
    bool tryAddSwitchLight(string);
    void setLightsDirty();
    string stringToUpper(string);
    vector<string> stringSplit(string, string);

    void MainWin::rgb2HSV_d();

public slots:
    void progressAdd_init();
    void newScene();
    void openScene();
    void saveScene();
    void saveSceneAs();
    void quitConfirm();
    void startupScene(string);

    void pivCenter();
    void pivTgl();
    void boundingBoxTgl();
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
    void objAdd(string, string);

    void getChild();
    void getParent();

    void searchUp_intermediate(shared_ptr<Object>);
    bool searchUp(shared_ptr<Object>);

    void pivHide();
    void gizSpaceSwitch();

    void gizShow(string);
    void gizPivAutoShow();

    void refreshAllPaintTables();
    void PaintWinTgl(bool, int);

    void clearSel();
    void attrTableVizToggle();

    void pivMatchTest();
    glm::vec3 gammaCsel();

    void deferredPrefs();
    void gridInit();

protected:
    void keyReleaseEvent(QKeyEvent *);

    void addIoMenu(); void addActMenu(); void addEtcMenu(); void addEdMenu(); void addRenderMenu(); void addRigMenu();
    void addAttrTables();
    void addOutliner();
    void centerToScreen(QWidget* widget);
    void camInit();
    void gizInit();
    void objInit();
    void glWidgetInit();
    void restoreZoomedLayout();
    void loadSS();
};

typedef struct
{
    string name;
    int width, height;
    GLuint fbo1, tex1_32;
    GLuint64 tex1_64;
    GLuint fbo2, tex2_32;
    GLuint64 tex2_64;
    GLuint DS_32;
    GLuint64 DS_64;
    int idx;
    unsigned int ID;
} AbjNode;

typedef struct
{
    string type, name;
    glm::vec3 t;
} CamSetup;

typedef struct
{
    string name;
    glm::vec3 r;
} GridSetup;

typedef struct
{
    string type, name;
    glm::vec3 Cgiz, r, t;
} GizSetup;

typedef struct
{
    string type, name;
} LightSetup;

typedef struct
{
    string attr;
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
    string cam;
    int splitIdx, ID;
    QSplitter &split;
} GLSetup;

typedef struct
{
    shared_ptr<Object> obj;
    GLuint VBO_P, VBO_UV, VBO_T, VBO_N, VBO_IDX;
} GLSharedData;

typedef struct
{
    glm::vec3 targ, up;
} CubemapDirections;

typedef struct
{
    string name, type;
    int sx, sy, offX, offY;
    QPolygon hoverTri;
    glm::vec2 move;
    bool hovered;
} PopSetup;

typedef struct
{
    QPushButton *button;
    string name;
} TableButtons;

typedef struct
{
    int row, col;
} TableRowCol;

typedef struct
{
    string name;
    GLuint pro;
} Pro;

typedef struct
{
    string name, type, flat;
    unsigned int ID;
    vector<AbjNode> layer;
} Map;

typedef struct
{
    string name;
    glm::vec3 scale;
    float opac;
    unsigned int ID;
    glm::vec2 coord;
} Brush;

typedef struct
{
    glm::vec2 coord;
    glm::vec3 scale;
    float opac;
} Stroke;

typedef struct
{
    string name;
    unsigned int ID;
} NameID;

typedef struct
{
    NameID x, y;
} TwoNameID;

typedef struct
{
    string type;
    NameID map, layer;
    vector<TwoNameID> typeMap, mapLayer;
} TexSel;

typedef struct
{
    glm::vec2 mouse, UV;
} MouseToUV;

typedef struct
{
    unsigned int dwSize;
    unsigned int dwFlags;
    unsigned int dwHeight;
    unsigned int dwWidth;
    unsigned int dwPitchOrLinearSize;
    unsigned int dwDepth;
    unsigned int dwMipMapCount;
    unsigned int dwReserved1[11];

    struct
    {
        unsigned int dwSize;
        unsigned int dwFlags;
        unsigned int dwFourCC;
        unsigned int dwRGBBitCount;
        unsigned int dwRBitMask;
        unsigned int dwGBitMask;
        unsigned int dwBBitMask;
        unsigned int dwAlphaBitMask;
    } sPixelFormat;

    struct
    {
        unsigned int dwCaps1;
        unsigned int dwCaps2;
        unsigned int dwDDSX;
        unsigned int dwReserved;
    } sCaps;

    unsigned int dwReserved2;

} ddsHeader;

struct lightUBO
{
    glm::vec4 Cl_type, falloff, DirRot, P_WS;
	glm::mat4 ShadowCoord;
};

#include "Anim.h"
#include "Attrs.h"
#include "CamCombo.h"
#include "CPop.h"
#include "GLWidget.h"
#include "GLWidgetSh.h"
#include "Gizmo.h"
#include "Object.h"
#include "Outliner.h"
#include "LayerSel.h"
#include "PaintWin.h"
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

    string typeX = "ALL";
    string grp, name, type, val_s;

    bool repop = true;
    bool val_b;
    int val_i, idx_3;
    int tab = 0;
    int idx;
    float val_f;
    float min = -9999.f;
    float max = 9999.f;
    glm::vec2 val_2;
    glm::vec3 val_3;
    glm::vec4 val_4;
    vector<string> comboList;
    QShortcut *cut;

    shared_ptr<Object> obj;

//protected:
    virtual MultiAttr* CloneImpl() const { return(new MultiAttr(*this)); }

//    virtual void set(glm::vec3) { val_3 = val; if (name == "t") obj->piv->val_3 += val; }
//    void set(float, float, float) { val_3 = glm::vec3(x, y, z); if (name == "t") obj->piv->val_3 += glm::vec3(x, y, z); }
};

template <typename T>
class make_vector
{
public:
  typedef make_vector<T> my_type;
  my_type& operator<< (const T& val)
  {
    data_.push_back(val);
    return *this;
  }

  operator std::vector<T>() const
  {
    return data_;
  }
private:
  std::vector<T> data_;
};


#endif // MAINWIN_H
