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

#ifndef ATTRS_H
#define ATTRS_H

#include "MainWin.h"

class Attrs : public QTableWidget
{
    Q_OBJECT
public:
    MainWin &myWin;
    Attrs(MainWin &, QWidget *parent = 0);

    bool altTgl, ctrlTgl, shiftTgl, qTgl;
    bool lmbTgl, mmbTgl;
    bool setupTgl, writeAttrTgl, camAttrWrongType;
    int useThisRow, lastRow, lastCol, storedStackedIdx, tabIdx;
    bool refreshRowH_onRelease = 0;
    QString changeEnumName;

    glm::vec2 pD, pNew, pOld;

    QString attrType, attrName, tableType;
    QTableWidgetItem *selObjN;
    QMenu RMB_menu;
    QFont labelF, sepF, valF;

    vector<shared_ptr<Object>> selObjs;
    vector<shared_ptr<MultiAttr>> multiVec, multiCut, multiCut_stored, multiEtc, multiEtc_stored, multiGL, multiGL_stored, multiPath, multiPath_stored, prefReset, prefReset_stored, undoDragAttrs;
    vector<TableButtons> myButtons;
    vector<Combo*> myEnumCombo;

    //CUT PREFS
    shared_ptr<MultiAttr> deleteS, dupe, hide, unHide, invertSel, gizSpace, parent, parentWorld, pivCenter;

    //ETC PREFS
    shared_ptr<MultiAttr> framed, selAccuracy;
    shared_ptr<MultiAttr> fontSep, rowH, rowH_sep;
    shared_ptr<MultiAttr> dragSep, dragBoost, dragBoostHSV, dragSlow;
    shared_ptr<MultiAttr> dollySpeed, dollySpeed_ortho, dragManipSpeed, panSpeed, panSpeed_ortho, vSliderSpeed, tumbleSpeed, wheelSpeed, wheelSpeed_ortho;
    shared_ptr<MultiAttr> fpsSep, fpsMouseSpeed, fpsFBSpeed, fpsLRSpeed, fpsUDSpeed, fpsWheelSpeed;

    //GL PREFS
    shared_ptr<MultiAttr> rezX, rezY, popCenterXY, CPopSize, CPopManipSize, gizSide, gizSideS;
    shared_ptr<MultiAttr> gridSep, Cgrid, gridLines, gridSize;
    shared_ptr<MultiAttr> cSep, Csel, CselManip, Ctxt;

    //PATH PREFS
    shared_ptr<MultiAttr> pathGLSL, pathObj, pathTex;


    bool refreshTable();
    void cutTable_init();
    void etcTable_init();
    void glTable_init();
    void pathTable_init();
    void resetOrtho(int);
    string hexFromVec(glm::vec3);
    vector<shared_ptr<Object>> selTemp();
    bool isTransformAttr(QString);
    void refreshRowH();
    void writeActions(QString, QString, unsigned int, unsigned int, int, float, QString);

public slots:
    void changeEnum(QString);
    void boolChange();
    void colorPick();
    void writeValue(QTableWidgetItem *);
    void multiCutTgl(bool state);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    bool eventFilter(QObject *, QEvent *);
    void closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint);

    int rowCt_get(QString);
    void prepItems(QTableWidgetItem *, string);
    void vDrag(glm::vec2);
    void dragUndo_prep();
    void keepSelAfterMMBRelease();
    void resetPrefs(); //

    void tableWidgetItem_add(int);
};

#endif // ATTRS_H
