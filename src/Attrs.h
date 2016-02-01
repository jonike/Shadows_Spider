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

#ifndef ATTRS_H
#define ATTRS_H

#include "MainWin.h"

class Attrs : public QTableWidget
{
    Q_OBJECT
public:
    MainWin &myWin;
    Attrs(MainWin &, QWidget *parent = 0);

    QTableWidgetItem *selObjN;
    QFont labelF, sepF, valF;

    bool altTgl, ctrlTgl, shiftTgl;
    bool lmbTgl, mmbTgl;
    bool setupTgl, writeAttrTgl, camAttrWrongType;
    bool refreshRowH_onRelease = false;

    int useThisRow, lastRow, lastCol, storedStackedIdx, tabIdx;
    string attrType, attrName, attrGrp, changeEnumName;
    glm::vec2 pMouseDiff, pMouseNew, pMouseOld;

    vector<shared_ptr<Object>> selObjs;
    vector<shared_ptr<MultiAttr>> multiVec, multiCut, multiCut_stored, multiEtc, multiEtc_stored, multiGL, multiGL_stored, multiPath, multiPath_stored, prefReset, prefReset_stored, undoDragAttrs;
    vector<TableButtons> myButtons;
    vector<Combo*> myEnumCombo;

    //CUT PREFS
    shared_ptr<MultiAttr> boundingBox, deleteS, dupe, hide, unHide, invertSel, gizSpace, parent, parentWorld, pivCenter;

    //ETC PREFS
    shared_ptr<MultiAttr> framed, selAccuracy;
    shared_ptr<MultiAttr> fontSep, rowH, rowH_sep;
    shared_ptr<MultiAttr> dragSep, dragBoost, dragBoostHSV, dragSlow;
    shared_ptr<MultiAttr> dollySpeed, dollySpeed_ortho, dragManipSpeed, panSpeed, panSpeed_ortho, vSliderSpeed, tumbleSpeed, wheelSpeed, wheelSpeed_ortho;
    shared_ptr<MultiAttr> fpsSep, fpsMouseSpeed, fpsFBSpeed, fpsLRSpeed, fpsUDSpeed, fpsWheelSpeed;

    //GL PREFS
    shared_ptr<MultiAttr> rezX, rezY, popCenterXY, CPopManipSize, gizSide, gizSideS;
    shared_ptr<MultiAttr> gridSep, Cgrid, gridLines, gridSize;
    shared_ptr<MultiAttr> cSep, Csel, CselManip, Ctxt;

    //PATH PREFS
    shared_ptr<MultiAttr> pathGLSL, pathObj, pathTex;

    bool refreshTable();
    void cutTable_init();
    void etcTable_init();
    void glTable_init();
    void pathTable_init();

    string hexFromVec(glm::vec3);
    bool isTransformAttr(string);
    void refreshRowH();
    void resetOrtho(shared_ptr<Object>);
    vector<shared_ptr<Object>> selTemp();
    void writeActions(string, string, shared_ptr<Object>, shared_ptr<MultiAttr>, int, float, string);
    virtual void keyboardSearch(const QString &) {};

public slots:
    void changeEnum(string);
    void changeEnumD(QString);
    void boolChange();
    void colorPick();
    void writeValue(QTableWidgetItem *);
    void multiCutTgl(bool);

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

    void dragUndo_prep();
    void prepItems(QTableWidgetItem *, string);
    void refreshAndKeepSelAfterMMBRelease();
    void resetPrefs();
    int rowCt_get(string);
    void tableWidgetItem_add(shared_ptr<MultiAttr>);
    void vDrag(glm::vec2);
};

#endif // ATTRS_H
