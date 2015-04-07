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

#include "MainWin.h"

Attrs::Attrs(MainWin &myWinTemp, QWidget *parent) : QTableWidget(parent), myWin(myWinTemp)
{
    setRowCount(1);
    setColumnCount(2);
    setFrameShadow(QFrame::Raised); //
    setItemDelegate(myWin.myFocus); //
    setAlternatingRowColors(1);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setVisible(0);
    horizontalHeader()->setSectionsMovable(0);
    verticalHeader()->setVisible(0);
    verticalHeader()->setSectionsMovable(0);

    labelF = QFont("DejaVu Sans Mono", 10, 75);
    sepF = QFont("DejaVu Sans Mono", 8, 75);
    valF = QFont("DejaVu Sans Mono", 9, 75);

    selObjN = new QTableWidgetItem;
    setItem(0, 0, selObjN);
    setSpan(0, 0, 1, 2); //sets row 0, col 0 to span both columns

    writeAttrTgl = 1;
    connect(this, &Attrs::itemChanged, this, &Attrs::writeValue);

    viewport()->setMouseTracking(1);
    installEventFilter(this);

    altTgl = ctrlTgl = shiftTgl = qTgl = lmbTgl = mmbTgl = camAttrWrongType = 0;
    tabIdx = 0;

    //CTRL+ALT shortcut and RMB has menu pop where you can reset, save, load etc
//    connect(resetPrefs_, &QAction::triggered, this, &Attrs::resetPrefs);

    //cutTable_init(); //in mainWin
    etcTable_init();
    glTable_init();
    pathTable_init();
}

void Attrs::enterEvent(QEvent *)
{
    setFocus();
    multiCutTgl(0);
}

void Attrs::leaveEvent(QEvent *)
{
    multiCutTgl(1);
}

void Attrs::closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
    if (hint == QAbstractItemDelegate::EditNextItem && currentColumn() == 1)
    {
        hint = QAbstractItemDelegate::NoHint;

        QModelIndex currIdx = currentIndex();
        QModelIndex nextIdx = model()->index(currIdx.row() + 1, 1);

        selectionModel()->select(nextIdx, QItemSelectionModel::Select);
        setCurrentIndex(nextIdx);
        setFocus();
        edit(nextIdx);
    }

    QTableWidget::closeEditor(editor, hint);
}

bool Attrs::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);

        if (ke->key() == Qt::Key_Tab)
        {
            QModelIndex currIdx = currentIndex();
            QModelIndex nextIdx = model()->index(currIdx.row() + 1, 1);

            selectionModel()->select(nextIdx, QItemSelectionModel::Select);
            setCurrentIndex(nextIdx);
            setFocus();
            edit(nextIdx);

            return 1;
        }
    }

    return QWidget::eventFilter(obj, e);
}

void Attrs::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        if (!undoDragAttrs.empty())
        {
            for (unsigned int i = 0; i < undoDragAttrs.size(); ++i)
            {
                for (unsigned int j = 0; j < multiVec.size(); ++j)
                {
                    if (undoDragAttrs[i]->name == multiVec[j]->name)
                    {
                        if (undoDragAttrs[i]->type == "bool")
                        {
                            multiVec[j]->val_b = undoDragAttrs[i]->val_b;

                            if (multiVec[j]->name == "framed")
                            {
                                Qt::WindowFlags framedOrFrameless;

                                if (!multiVec[j]->val_b)
                                    framedOrFrameless = Qt::FramelessWindowHint;

                                myWin.hide();
                                myWin.setWindowFlags(framedOrFrameless);
                                myWin.show();
                            }
                        }

                        else if (undoDragAttrs[i]->type == "float")
                        {
                            multiVec[j]->val_f = undoDragAttrs[i]->val_f;

                            if (multiVec[j]->name == "fov")
                            {
                                for (unsigned int k = 0; k < myWin.allGL.size(); ++k)
                                    myWin.allGL[k]->resizeGL(myWin.allGL[k]->width(), myWin.allGL[k]->height());
                            }
                        }

                        else if (undoDragAttrs[i]->type == "int")
                            multiVec[j]->val_i = undoDragAttrs[i]->val_i;


                        else if (undoDragAttrs[i]->type == "vec3")
                        {
                            //undo pivot
                            for (unsigned int k = 0; k < myWin.allObj.size(); ++k)
                            {
                                if (myWin.allObj[k]->selected)
                                {
                                    float offSetPivotPos = myWin.allObj[k]->piv->val_3[undoDragAttrs[i]->idx_3] - multiVec[j]->val_3[undoDragAttrs[i]->idx_3];

                                    if (multiVec[j]->name == "t") //ALSO MOVE PIVOT
                                        myWin.allObj[k]->piv->val_3[undoDragAttrs[i]->idx_3] = undoDragAttrs[i]->val_3[undoDragAttrs[i]->idx_3] + offSetPivotPos;
                                }
                            }

                            multiVec[j]->val_3[undoDragAttrs[i]->idx_3] = undoDragAttrs[i]->val_3[undoDragAttrs[i]->idx_3];
                        }

                    }
                }
            }

            refreshTable();
            myWin.setLightsDirty();
        }

    }

    else if (e->key() == Qt::Key_Alt) altTgl = 1;
    else if (e->key() == Qt::Key_Control) ctrlTgl = 1;
    else if (e->key() == Qt::Key_Shift) shiftTgl = 1;
    else if (e->key() == Qt::Key_Q) qTgl = 1;

    if (ctrlTgl && e->key() == Qt::Key_Q)
        myWin.quitConfirm();

    if (altTgl && ctrlTgl)
        setCursor(Qt::WhatsThisCursor);

    QTableWidget::keyPressEvent(e);
}

void Attrs::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Alt) altTgl = 0;
    else if (e->key() == Qt::Key_Control) ctrlTgl = 0;
    else if (e->key() == Qt::Key_Shift) shiftTgl = 0;

    setCursor(Qt::ArrowCursor);
}

void Attrs::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = 1;

        QTableWidget::mousePressEvent(e);
    }

    if (e->button() == Qt::MiddleButton)
        mmbTgl = 1;

    if (e->button() == Qt::RightButton)
    {
        QTableWidgetItem *myItem = itemAt(e->pos());

        if (myItem)
        {
            for (unsigned int i = 0; i < multiVec.size(); ++i)
            {
                if (myItem->text() == multiVec[i]->name && multiVec[i]->type == "enum")
                {
                    vector<PopSetup> radPop;

                    QStringList usableList = multiVec[i]->comboList;

                    bool reversePop = 0;

                    if (reversePop)
                    {
                        for (int i = usableList.size(); --i > -1; )
                            radPop.push_back( { usableList[i], "RAD", 75, 50 } );
                    }

                    else
                    {
                        for (int i = 0; i < usableList.size(); ++i)
                            radPop.push_back( { usableList[i], "RAD", 75, 50 } );
                    }

                    myWin.myRadPop->popName = myItem->text();

                    myWin.myRadPop->radius = 0;
                    for (int i = 0; i < usableList.size(); ++i)
                        myWin.myRadPop->radius += 14;

                    myWin.myRadPop->radius = glm::max(100, (int)myWin.myRadPop->radius);

                    myWin.myRadPop->buttonInit(radPop);
                    myWin.myRadPop->startP = myWin.toVec2(QCursor::pos());
                    myWin.myRadPop->newP = myWin.myRadPop->startP;

                    myWin.myRadPop->show();
                }
            }
        }
    }
}

void Attrs::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        lmbTgl = 0;

    else if (e->button() == Qt::MiddleButton)
    {
        mmbTgl = 0;
        undoDragAttrs.clear();
        setCursor(Qt::ArrowCursor);

        if (refreshRowH_onRelease)
            refreshRowH();

        keepSelAfterMMBRelease();
    }

    else if (e->button() == Qt::RightButton && tableType != "attr")
    {
        if (altTgl && ctrlTgl)
        {
            qDebug() << "show reset settings";
            RMB_menu.exec(mapToGlobal(e->pos()));
        }
    }

    setCursor(Qt::ArrowCursor);

    return QTableWidget::mouseReleaseEvent(e);
}

void Attrs::mouseMoveEvent(QMouseEvent *e)
{
    QTableWidgetItem *myItem = itemAt(e->pos());

    if (myItem && lmbTgl)
        myItem->setSelected(1);

    pOld = pNew;
    pNew = myWin.toVec2(e->pos());
    pD =  pNew - pOld;

    if (mmbTgl && !selectedItems().empty())
    {
        setCursor(Qt::SizeHorCursor);
        vDrag(pD);
    }
}

void Attrs::cutTable_init()
{
    deleteS = make_shared<MultiAttr>();
    deleteS->name = "delete";
    deleteS->type = "QShortcut";
    deleteS->val_s = "DELETE";
    deleteS->cut = new QShortcut(QKeySequence::fromString(deleteS->val_s), &myWin);
    connect(deleteS->cut, SIGNAL(activated()), &myWin, SLOT(delete_()));
    multiCut.push_back(deleteS);

    dupe = make_shared<MultiAttr>();
    dupe->name = "dupe";
    dupe->type = "QShortcut";
    dupe->val_s = "CTRL+D";
    dupe->cut = new QShortcut(QKeySequence::fromString(dupe->val_s), &myWin);
    connect(dupe->cut, SIGNAL(activated()), &myWin, SLOT(dupe()));
    multiCut.push_back(dupe);

    hide = make_shared<MultiAttr>();
    hide->name = "hide";
    hide->type = "QShortcut";
    hide->val_s = "H";
    hide->cut = new QShortcut(QKeySequence::fromString(hide->val_s), &myWin);
    connect(hide->cut, SIGNAL(activated()), &myWin, SLOT(hide()));
    multiCut.push_back(hide);

    unHide = make_shared<MultiAttr>();
    unHide->name = "unHide";
    unHide->type = "QShortcut";
    unHide->val_s = "SHIFT+H";
    unHide->cut = new QShortcut(QKeySequence::fromString(unHide->val_s), &myWin);
    connect(unHide->cut, SIGNAL(activated()), &myWin, SLOT(unHide()));
    multiCut.push_back(unHide);

    invertSel = make_shared<MultiAttr>();
    invertSel->name = "invertSel";
    invertSel->type = "QShortcut";
    invertSel->val_s = "[";
    invertSel->cut = new QShortcut(QKeySequence::fromString(invertSel->val_s), &myWin);
    connect(invertSel->cut, SIGNAL(activated()), &myWin, SLOT(invertSel()));
    multiCut.push_back(invertSel);

    gizSpace = make_shared<MultiAttr>();
    gizSpace->name = "gizSpace";
    gizSpace->type = "QString";
    gizSpace->val_s = "ALT+X";
    gizSpace->cut = new QShortcut(QKeySequence::fromString(gizSpace->val_s), &myWin);
    connect(gizSpace->cut, SIGNAL(activated()), &myWin, SLOT(gizSpaceSwitch()));
    multiCut.push_back(gizSpace);

    parent = make_shared<MultiAttr>();
    parent->name = "parent";
    parent->type = "QString";
    parent->val_s = "P";
    parent->cut = new QShortcut(QKeySequence::fromString(parent->val_s), &myWin);
    connect(parent->cut, SIGNAL(activated()), &myWin, SLOT(parent()));
    multiCut.push_back(parent);

    parentWorld = make_shared<MultiAttr>();
    parentWorld->name = "parentWorld";
    parentWorld->type = "QShortcut";
    parentWorld->val_s = "SHIFT+P";
    parentWorld->cut = new QShortcut(QKeySequence::fromString(parentWorld->val_s), &myWin);
    connect(parentWorld->cut, SIGNAL(activated()), &myWin, SLOT(parentWorld()));
    multiCut.push_back(parentWorld);

    pivCenter = make_shared<MultiAttr>();
    pivCenter->name = "pivCenter";
    pivCenter->type = "QShortcut";
    pivCenter->val_s = "F5";
    pivCenter->cut = new QShortcut(QKeySequence::fromString(pivCenter->val_s), &myWin);
    connect(pivCenter->cut, SIGNAL(activated()), &myWin, SLOT(pivCenter()));
    multiCut.push_back(pivCenter);

    multiCut_stored.resize(multiCut.size());

    for (unsigned int i = 0; i < multiCut.size(); ++i)
        multiCut_stored[i] = multiCut[i]->Clone();
}

void Attrs::etcTable_init()
{
    framed = make_shared<MultiAttr>();
    framed->name = "framed";
    framed->type = "bool";
    framed->val_b = 0;
    multiEtc.push_back(framed);

    selAccuracy = make_shared<MultiAttr>();
    selAccuracy->name = "selAccuracy";
    selAccuracy->type = "int";
    selAccuracy->val_i = 30;
    selAccuracy->min = 4;
    multiEtc.push_back(selAccuracy);

    //FONT
    fontSep = make_shared<MultiAttr>();
    fontSep->name = "font";
    fontSep->type = "sep";
    multiEtc.push_back(fontSep);

    rowH = make_shared<MultiAttr>();
    rowH->name = "rowH";
    rowH->type = "int";
    rowH->val_i = 20;
    rowH->min = 10;
    rowH->max = 50;
    multiEtc.push_back(rowH);

    rowH_sep = make_shared<MultiAttr>();
    rowH_sep->name = "rowH_sep";
    rowH_sep->type = "int";
    rowH_sep->val_i = 12;
    rowH_sep->min = 4;
    rowH_sep->max = 50;
    multiEtc.push_back(rowH_sep);

    //DRAG
    dragSep = make_shared<MultiAttr>();
    dragSep->name = "drag";
    dragSep->type = "sep";
    multiEtc.push_back(dragSep);

    dragBoost = make_shared<MultiAttr>();
    dragBoost->name = "dragBoost";
    dragBoost->type = "float";
    dragBoost->val_f = 3.f;
    dragBoost->min = 1.f;
    multiEtc.push_back(dragBoost);

    dragBoostHSV = make_shared<MultiAttr>();
    dragBoostHSV->name = "dragBoostHSV";
    dragBoostHSV->type = "float";
    dragBoostHSV->val_f = 2.f;
    dragBoostHSV->min = 1.f;
    multiEtc.push_back(dragBoostHSV);

    dragSlow = make_shared<MultiAttr>();
    dragSlow->name = "dragSlow";
    dragSlow->type = "float";
    dragSlow->val_f = .2f;
    dragSlow->min = .001f;
    dragSlow->max = 1.f;
    multiEtc.push_back(dragSlow);

    dragManipSpeed = make_shared<MultiAttr>();
    dragManipSpeed->name = "dragManipSpeed";
    dragManipSpeed->type = "float";
    dragManipSpeed->val_f = 4.f;
    dragManipSpeed->min = 1.f;
    multiEtc.push_back(dragManipSpeed);

    panSpeed = make_shared<MultiAttr>();
    panSpeed->name = "panSpeed";
    panSpeed->type = "float";
    panSpeed->val_f = .07f;
    panSpeed->min = .001f;
    multiEtc.push_back(panSpeed);

    panSpeed_ortho = make_shared<MultiAttr>();
    panSpeed_ortho->name = "panSpeed_ortho";
    panSpeed_ortho->type = "float";
    panSpeed_ortho->val_f = .03f;
    panSpeed_ortho->min = .001f;
    multiEtc.push_back(panSpeed_ortho);

    vSliderSpeed = make_shared<MultiAttr>();
    vSliderSpeed->name = "vSliderSpeed";
    vSliderSpeed->type = "float";
    vSliderSpeed->val_f = .075f;
    vSliderSpeed->min = .001f;
    multiEtc.push_back(vSliderSpeed);

    tumbleSpeed = make_shared<MultiAttr>();
    tumbleSpeed->name = "tumbleSpeed";
    tumbleSpeed->type = "float";
    tumbleSpeed->val_f = .5f;
    tumbleSpeed->min = .001f;
    multiEtc.push_back(tumbleSpeed);

    dollySpeed = make_shared<MultiAttr>();
    dollySpeed->name = "dollySpeed";
    dollySpeed->type = "float";
    dollySpeed->val_f = .05f;
    dollySpeed->min = .001f;
    multiEtc.push_back(dollySpeed);

    dollySpeed_ortho = make_shared<MultiAttr>();
    dollySpeed_ortho->name = "dollySpeed_ortho";
    dollySpeed_ortho->type = "float";
    dollySpeed_ortho->val_f = .01f;
    dollySpeed_ortho->min = .001f;
    multiEtc.push_back(dollySpeed_ortho);

    wheelSpeed = make_shared<MultiAttr>();
    wheelSpeed->name = "wheelSpeed";
    wheelSpeed->type = "float";
    wheelSpeed->val_f = .015f;
    wheelSpeed->min = .001f;
    multiEtc.push_back(wheelSpeed);

    wheelSpeed_ortho = make_shared<MultiAttr>();
    wheelSpeed_ortho->name = "wheelSpeed";
    wheelSpeed_ortho->type = "float";
    wheelSpeed_ortho->val_f = .005f;
    wheelSpeed_ortho->min = .001f;
    multiEtc.push_back(wheelSpeed_ortho);

    //FPS
    fpsSep = make_shared<MultiAttr>();
    fpsSep->name = "fps";
    fpsSep->type = "sep";
    multiEtc.push_back(fpsSep);

    fpsMouseSpeed = make_shared<MultiAttr>();
    fpsMouseSpeed->name = "fpsMouseSpeed";
    fpsMouseSpeed->type = "float";
    fpsMouseSpeed->val_f = .3f;
    fpsMouseSpeed->min = .001f;
    multiEtc.push_back(fpsMouseSpeed);

    fpsWheelSpeed = make_shared<MultiAttr>();
    fpsWheelSpeed->name = "fpsWheelSpeed";
    fpsWheelSpeed->type = "float";
    fpsWheelSpeed->val_f = .02f;
    fpsWheelSpeed->min = .001f;
    multiEtc.push_back(fpsWheelSpeed);

    fpsFBSpeed = make_shared<MultiAttr>();
    fpsFBSpeed->name = "fpsFBSpeed";
    fpsFBSpeed->type = "float";
    fpsFBSpeed->val_f = 15.f;
    fpsFBSpeed->min = .001f;
    multiEtc.push_back(fpsFBSpeed);

    fpsLRSpeed = make_shared<MultiAttr>();
    fpsLRSpeed->name = "fpsLRSpeed";
    fpsLRSpeed->type = "float";
    fpsLRSpeed->val_f = 15.f;
    fpsLRSpeed->min = .001f;
    multiEtc.push_back(fpsLRSpeed);

    fpsUDSpeed = make_shared<MultiAttr>();
    fpsUDSpeed->name = "fpsUDSpeed";
    fpsUDSpeed->type = "float";
    fpsUDSpeed->val_f = 10.f;
    fpsUDSpeed->min = .001f;
    multiEtc.push_back(fpsUDSpeed);

    multiEtc_stored.resize(multiEtc.size());

    for (unsigned int i = 0; i < multiEtc.size(); ++i)
        multiEtc_stored[i] = multiEtc[i]->Clone();
}

void Attrs::pathTable_init()
{
    pathGLSL = make_shared<MultiAttr>();
    pathGLSL->name = "pathGLSL";
    pathGLSL->type = "QString";
    pathGLSL->val_s = QDir::currentPath() + "/shaders/";
    multiPath.push_back(pathGLSL);


    pathObj = make_shared<MultiAttr>();
    pathObj->name = "pathObj";
    pathObj->type = "QString";
#if __unix__
    pathObj->val_s = "/home/aleks/Desktop/obj/";
#else
    pathObj->val_s = "C:/users/aleks/desktop/obj/";
#endif
    multiPath.push_back(pathObj);


    pathTex = make_shared<MultiAttr>();
    pathTex->name = "pathTex";
    pathTex->type = "QString";
#if __unix__
    pathTex->val_s = "/home/aleks/Desktop/tex/";
#else
    pathTex->val_s = "C:/users/aleks/desktop/tex/";
#endif
    multiPath.push_back(pathTex);

    multiPath_stored.resize(multiPath.size());

    for (unsigned int i = 0; i < multiPath.size(); ++i)
        multiPath_stored[i] = multiPath[i]->Clone();
}

void Attrs::glTable_init()
{
    rezX = make_shared<MultiAttr>();
    rezX->name = "rezX";
    rezX->type = "int";
    rezX->val_i = 725;
    rezX->min = 50.f;
    multiGL.push_back(rezX);

    rezY = make_shared<MultiAttr>();
    rezY->name = "rezY";
    rezY->type = "int";
    rezY->val_i = 400;
    rezY->min = 50.f;
    multiGL.push_back(rezY);

    popCenterXY = make_shared<MultiAttr>();
    popCenterXY->name = "popCenterXY";
    popCenterXY->type = "int";
    popCenterXY->val_i = 60;
    popCenterXY->min = 10.f;
    multiGL.push_back(popCenterXY);

    CPopSize = make_shared<MultiAttr>();
    CPopSize->name = "CPopSize";
    CPopSize->type = "int";
    CPopSize->val_i = 150;
    CPopSize->min = 30.f;
    multiGL.push_back(CPopSize);

    CPopManipSize = make_shared<MultiAttr>();
    CPopManipSize->name = "CPopManipSize";
    CPopManipSize->type = "int";
    CPopManipSize->val_i = 9;
    CPopManipSize->min = 3.f;
    multiGL.push_back(CPopManipSize);

    gizSide = make_shared<MultiAttr>();
    gizSide->name = "gizSide";
    gizSide->type = "bool";
    gizSide->val_b = 1;
    multiGL.push_back(gizSide);

    gizSideS = make_shared<MultiAttr>();
    gizSideS->name = "gizSideS";
    gizSideS->type = "vec3";
    gizSideS->val_3 = glm::vec3(40.f, 40.f, 3.f); //
    multiGL.push_back(gizSideS);

    //grid
    gridSep = make_shared<MultiAttr>();
    gridSep->name = "grid";
    gridSep->type = "sep";
    multiGL.push_back(gridSep);

    Cgrid = make_shared<MultiAttr>();
    Cgrid->name = "Cgrid";
    Cgrid->type = "color";
    Cgrid->val_3 = glm::vec3(0.f);
    multiGL.push_back(Cgrid);

    gridLines = make_shared<MultiAttr>();
    gridLines->name = "gridLines";
    gridLines->type = "enum";
    gridLines->val_s = "20"; //
    gridLines->val_i = 20;
    gridLines->comboList << "10" << "20" << "30" << "40" << "50";
    multiGL.push_back(gridLines);

    gridSize = make_shared<MultiAttr>();
    gridSize->name = "gridSize";
    gridSize->type = "float";
    gridSize->val_f = 15.f; //
    multiGL.push_back(gridSize);

    //C
    cSep = make_shared<MultiAttr>();
    cSep->name = "c";
    cSep->type = "sep";
    multiGL.push_back(cSep);

    Csel = make_shared<MultiAttr>();
    Csel->name = "Csel";
    Csel->type = "color";
    Csel->val_3 = glm::vec3(.3f, 1.f, 1.f);
    multiGL.push_back(Csel);

    CselManip = make_shared<MultiAttr>();
    CselManip->name = "Csel manip";
    CselManip->type = "color";
    CselManip->val_3 = glm::vec3(.557f, .333f, .067f);
    multiGL.push_back(CselManip);

    Ctxt = make_shared<MultiAttr>();
    Ctxt->name = "Ctxt";
    Ctxt->type = "color";
    Ctxt->val_3 = glm::vec3(1.f, 1.f, 0.f);
    multiGL.push_back(Ctxt);

    multiGL_stored.resize(multiGL.size());

    for (unsigned int i = 0; i < multiGL.size(); ++i)
        multiGL_stored[i] = multiGL[i]->Clone();
}

void Attrs::multiCutTgl(bool state)
{
    for (unsigned int i = 0; i < myWin.cutTable->multiCut.size(); ++i)
        myWin.cutTable->multiCut[i]->cut->setEnabled(state);
}

void Attrs::resetPrefs()
{
    if (tableType == "cut")
    {
        prefReset = multiCut;
        prefReset_stored = multiCut_stored;
    }

    else if (tableType == "etc")
    {
        prefReset = multiEtc;
        prefReset_stored = multiEtc_stored;
    }

    else if (tableType == "glsl")
    {
        prefReset = multiGL;
        prefReset_stored = multiGL_stored;
    }

    else if (tableType == "path")
    {
        prefReset = multiPath;
        prefReset_stored = multiPath_stored;
    }

    for (unsigned int i = 0; i < prefReset.size(); ++i)
    {
        for (unsigned int j = 0; j < prefReset_stored.size(); ++j)
        {
            if (prefReset[i]->name == prefReset_stored[j]->name)
            {
                prefReset[i]->val_b = prefReset_stored[j]->val_b;
                prefReset[i]->val_i = prefReset_stored[j]->val_i;
                prefReset[i]->val_f = prefReset_stored[j]->val_f;
                prefReset[i]->val_s = prefReset_stored[j]->val_s;
                prefReset[i]->val_2 = prefReset_stored[j]->val_2;
                prefReset[i]->val_3 = prefReset_stored[j]->val_3;
                prefReset[i]->val_4 = prefReset_stored[j]->val_4;

                break;
            }
        }
    }

    setCursor(Qt::ArrowCursor);
    refreshTable();
}

int Attrs::rowCt_get(QString type)
{
    int realRowCt = 0;

    for (unsigned int i = 0; i < multiVec.size(); ++i)
    {
        if (tabIdx == multiVec[i]->tab)
        {
            if (multiVec[i]->typeX == type)
            {
                if (multiVec[i]->type == "vec2") realRowCt += 2;
                else if (multiVec[i]->type == "vec3") realRowCt += 3;
                else if (multiVec[i]->type == "vec4") realRowCt += 4;
                else realRowCt += 1;
            }
        }
    }

    return realRowCt;
}

bool Attrs::refreshTable()
{
    setRowCount(1);

    lastRow = useThisRow = 0;
    setupTgl = 1;

    if (tableType == "attr" && myWin.selB == 0)
    {
        myWin.stackedAttrTable->setCurrentIndex(0); //
        storedStackedIdx = myWin.stackedAttrTable->currentIndex();

        return 0;
    }

    else if (tableType == "attr")
    {
        multiVec = myWin.selB->multiObj;
        myWin.stackedAttrTable->setCurrentIndex(1); //
        storedStackedIdx = myWin.stackedAttrTable->currentIndex();

        QString objName_temp = myWin.selB->name->val_s;

        if (myWin.countSel() > 1 && !myWin.myFSQ->selected)
            objName_temp += "...";

        selObjN->setText(objName_temp);
    }

    if (tableType != "attr")
    {
        selObjN->setText(tableType);
        selObjN->setFlags(selObjN->flags()^(Qt::ItemIsEditable | Qt::ItemIsSelectable));

        if (tableType == "cut") multiVec = multiCut;
        else if (tableType == "etc") multiVec = multiEtc;
        else if (tableType == "glsl") multiVec = multiGL;
        else if (tableType == "path") multiVec = multiPath;
    }

    if (!myButtons.empty())
        myButtons.clear();

    if (!myEnumCombo.empty())
        myEnumCombo.clear();

    selObjN->setFont(QFont("DejaVu Sans Mono", 13, 75));
    selObjN->setForeground(QBrush(Qt::black));
    selObjN->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    selObjN->setData(32, "QString"); //TYPE

    int rowCt = rowCt_get(0);
    int rowCtO = 0;

    if (tableType == "attr")
    {
        rowCt = (myWin.selB->type == "FBO") ? 0 : rowCt;
        rowCtO = rowCt_get(myWin.selB->type);
    }

    setRowCount(rowCt + rowCtO + 1);

    for (unsigned int i = 0; i < multiVec.size(); ++i)
    {
        attrType = multiVec[i]->type;
        attrName = multiVec[i]->name;

        if (tabIdx == multiVec[i]->tab)
        {
            if (tableType == "attr" && myWin.selB == myWin.myFSQ)
            {
                //hide transform attrs, show only matching typeX
                if (multiVec[i]->typeX == myWin.selB->type)
                    tableWidgetItem_add(i);
            }

            else
            {
                if (multiVec[i]->typeX == 0)
                    tableWidgetItem_add(i);

                if (tableType == "attr")
                {
                    if (multiVec[i]->typeX == myWin.selB->type)
                        tableWidgetItem_add(i);
                }
            }
        }
    }

    useThisRow = 0;
    setupTgl = 0;

    return 1;
}

void Attrs::tableWidgetItem_add(int idx)
{
    if (attrType == "bool")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName);
        prepItems(itemLabel, "label");

        Qt::ItemFlags noSelFlag = itemLabel->flags();
        noSelFlag &= ~Qt::ItemIsSelectable;
        itemLabel->setFlags(noSelFlag);

        //VALUE
        QPushButton *itemVal = new QPushButton();
        itemVal->setFlat(1);
        itemVal->setText(QString::number(multiVec[idx]->val_b, 'g', 4));
        itemVal->setFont(valF);

        //SS
        string pushBG = (useThisRow % 2) ? "#555555" : "#4d4d4d";

        stringstream ss;
        ss << "QPushButton { color: " << "black;" << "background: " << pushBG << "; } QPushButton:pressed { background: " << hexFromVec(myWin.gammaCsel()) << "; }";

        QString colorStyleSheet_dynamic = QString::fromStdString(ss.str());
        itemVal->setStyleSheet(colorStyleSheet_dynamic);

        connect(itemVal, &QPushButton::clicked, this, &Attrs::boolChange);
        setCellWidget(useThisRow, 1, itemVal);
        myButtons.push_back( {itemVal, attrName} );
    }

    else if (attrType == "color")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName);
        prepItems(itemLabel, "label");

        Qt::ItemFlags noSelFlag = itemLabel->flags();
        noSelFlag &= ~Qt::ItemIsSelectable;
        itemLabel->setFlags(noSelFlag);

        //VALUE
        QPushButton *itemVal = new QPushButton();
        itemVal->setFlat(1);

        //SS
        glm::vec3 Cadjusted(pow(multiVec[idx]->val_3.r, myWin.gamma), pow(multiVec[idx]->val_3.g, myWin.gamma), pow(multiVec[idx]->val_3.b, myWin.gamma));
        string usableCol = hexFromVec(Cadjusted);

        stringstream ss;
        ss << "QPushButton { background: " << usableCol << "; } QPushButton:pressed { background: " << hexFromVec(myWin.gammaCsel()) << "; }";
        QString colorStyleSheet_dynamic = QString::fromStdString(ss.str());
        itemVal->setStyleSheet(colorStyleSheet_dynamic);

        connect(itemVal, &QPushButton::clicked, this, &Attrs::colorPick);
        setCellWidget(useThisRow, 1, itemVal);
        myButtons.push_back( {itemVal, attrName} );
    }

    else if (attrType == "enum")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName);
        prepItems(itemLabel, "label");

        Qt::ItemFlags noSelFlag = itemLabel->flags();
        noSelFlag &= ~Qt::ItemIsSelectable;
        itemLabel->setFlags(noSelFlag);

        //VALUE
        Combo *tableCombo = new Combo;
        tableCombo->setItemDelegate(myWin.myFocus);

        if (attrName == "camLiType")
        {
            for (int i = 0; i < multiVec[idx]->comboList.size(); ++i)
            {
                QString myType = multiVec[idx]->comboList[i];

                if (myType == "FPS" || myType == "ORTHO" || myType == "PERSP")
                    tableCombo->addItem(myWin.myIconCam, multiVec[idx]->comboList[i]);

                else
                    tableCombo->addItem(myWin.myIconLight, multiVec[idx]->comboList[i]);
            }
        }

        else
            tableCombo->addItems(multiVec[idx]->comboList); //

        tableCombo->setFont(labelF);
        tableCombo->setStyleSheet("color:black");
        setCellWidget(useThisRow, 1, tableCombo);

        connect(tableCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeEnum(QString)));

        tableCombo->setCurrentIndex(tableCombo->findText(multiVec[idx]->val_s));
        tableCombo->setEditable(1);
        tableCombo->lineEdit()->setAlignment(Qt::AlignHCenter);
        tableCombo->lineEdit()->setFont(labelF); //
        tableCombo->lineEdit()->setStyleSheet(myWin.myStyle); //
        tableCombo->setEditable(0);

        for (int j = 0; j < tableCombo->count(); ++j)
        {
            tableCombo->setItemData(j, Qt::AlignHCenter, Qt::TextAlignmentRole);
            tableCombo->setItemData(j, attrName, 33); //BASENAME
        }

        myEnumCombo.push_back(tableCombo);

//        for (int j = 0; j < myEnumCombo.size(); ++j)
//            qDebug() << "myEnumCombo[0] 33 basename = " << myEnumCombo[j]->itemData(0, 33).toString();
    }

    else if (attrType == "float")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName);
        prepItems(itemLabel, "label");

        //VALUE
        stringstream ss;
        ss << fixed << setprecision(3) << multiVec[idx]->val_f;
        QTableWidgetItem *itemVal = new QTableWidgetItem(QString::fromStdString(ss.str()));
        prepItems(itemVal, "value");
    }

    else if (attrType == "int")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName);
        prepItems(itemLabel, "label");

        QTableWidgetItem *itemVal = new QTableWidgetItem(QString::number(multiVec[idx]->val_i, 'g', 4));
        prepItems(itemVal, "value");
    }

    else if (attrType == "QString" || attrType == "QShortcut")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName);
        prepItems(itemLabel, "label");

        //VALUE
        QTableWidgetItem *itemVal = new QTableWidgetItem(multiVec[idx]->val_s);
        prepItems(itemVal, "value");
    }

    else if (attrType == "sep")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH_sep->val_i);

        //get highlight col for label - replace with global highlight col
        glm::vec3 Cadj = myWin.gammaCsel();
        QColor Chigh = QColor::fromRgbF(Cadj.r, Cadj.g, Cadj.b);

        QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName);
        prepItems(itemLabel, "label");
        itemLabel->setFont(sepF); //
        itemLabel->setForeground(QBrush(Chigh));
        itemLabel->setFlags(itemLabel->flags()^Qt::ItemIsSelectable);

        //VALUE
        QTableWidgetItem *itemVal = new QTableWidgetItem();
        prepItems(itemVal, "value");
        itemVal->setFlags(itemVal->flags()^Qt::ItemIsSelectable);
        itemVal->setFlags(itemVal->flags()^Qt::ItemIsEditable);
    }

    else if (attrType == "vec2" || attrType == "vec3" || attrType == "vec4")
    {
        QString appendEnding[4] = {"X", "Y", "Z", "W"};

        int usableIter = attrType.toStdString().back() - '0';

        for (int j = 0; j < usableIter; ++j)
        {
            useThisRow = lastRow + 1;
            setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

            QTableWidgetItem *itemLabel = new QTableWidgetItem(attrName + appendEnding[j]);
            prepItems(itemLabel, "label");
            itemLabel->setData(34, j); //VEC IDX

            float usableVal;
            if (usableIter == 2) usableVal = multiVec[idx]->val_2[j];
            if (usableIter == 3) usableVal = multiVec[idx]->val_3[j];
            if (usableIter == 4) usableVal = multiVec[idx]->val_4[j];

            stringstream ss;
            ss << fixed << setprecision(3) << usableVal;

            QTableWidgetItem *itemVal = new QTableWidgetItem(QString::fromStdString(ss.str()));
            prepItems(itemVal, "value");
            itemVal->setData(34, j); //VEC IDX
        }
    }
}

void Attrs::prepItems(QTableWidgetItem *item, string type)
{
    if (type == "label")
    {
        item->setFont(labelF);

        if (this != myWin.attrTable)
            item->setTextAlignment(Qt::AlignCenter| Qt::AlignVCenter);

        else
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        item->setFlags(item->flags()^Qt::ItemIsEditable);
        item->setData(32, attrType);
        item->setData(33, attrName);

        setItem(useThisRow, 0, item);
    }

    else if (type == "value")
    {
        item->setFont(valF);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setForeground(QBrush(Qt::black));
        item->setData(32, attrType);
        item->setData(33, attrName);

        setItem(useThisRow, 1, item);
    }
}

vector<shared_ptr<Object>> Attrs::selTemp()
{
    vector<shared_ptr<Object>> temp;

    for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
    {
        if (tableType == "attr")
        {
            if (myWin.allObj[j]->selected)
                temp.push_back(myWin.allObj[j]);
        }

        else //need 1 obj to iterate...use any nondeletable
        {
            if (myWin.allObj[j]->name->val_s == "top")
            {
                temp.push_back(myWin.allObj[j]);

                continue; //
            }
        }
    }

    return temp;
}

void Attrs::boolChange()
{
    for (unsigned int i = 0; i < myButtons.size(); ++i)
    {
        if (myButtons[i].button == sender())
        {
            vector<shared_ptr<Object>> boolChangeTemp = selTemp();

            for (unsigned int j = 0; j < boolChangeTemp.size(); ++j)
            {
                if (tableType == "attr")
                    multiVec = boolChangeTemp[j]->multiObj;

                for (unsigned int k = 0; k < multiVec.size(); ++k)
                {
                    if (myButtons[i].name == multiVec[k]->name)
                    {
                        multiVec[k]->val_b = !multiVec[k]->val_b;

                        if (multiVec[k]->name == "framed")
                        {
                            Qt::WindowFlags framedOrFrameless;

                            if (!multiVec[k]->val_b)
                                framedOrFrameless = Qt::FramelessWindowHint;

                            myWin.setWindowFlags(framedOrFrameless);
                            myWin.show();
                        }

                        else if (multiVec[k]->name == "fxaaBlur")
                        {
                            for (unsigned int l = 0; l < myWin.allGL.size(); ++l)
                                myWin.myPP->resizeTexClearMem(myWin.allGL[l]);
                        }

                        else if (multiVec[k]->name == "gizSide")
                        {
                            for (unsigned int l = 0; l < myWin.allGL.size(); ++l)
                                myWin.allGL[l].get()->gizSideTgl_();
                        }

                        else if (multiVec[k]->name == "orthoFree" && multiVec[k]->val_b == 0)
                            resetOrtho(j);

                        else if (multiVec[k]->name == "bb" || multiVec[k]->name == "shadowCast" || multiVec[k]->name == "v")
                            myWin.setLightsDirty();
                    }
                }
            }
        }
    }

    refreshTable();
    myWin.myOutliner->refreshOutliner(1);
}

void Attrs::colorPick()
{
    for (unsigned int i = 0; i < myButtons.size(); ++i)
    {
        if (myButtons[i].button == sender())
        {
            for (unsigned int j = 0; j < multiVec.size(); ++j)
            {
                if (myButtons[i].name == multiVec[j]->name)
                {
                    glm::vec3 Cldr = glm::clamp(multiVec[j]->val_3, 0.f, 1.f);
                    QColor toHSV = QColor::fromRgbF(Cldr.r, Cldr.g, Cldr.b).toHsv();

                    myWin.myCPopWin->myCPop->hue = toHSV.hueF();
                    myWin.myCPopWin->myCPop->sat = toHSV.saturationF();
                    myWin.myCPopWin->myCPop->val = toHSV.valueF();
                    myWin.myCPopWin->myCPop->changedManip = 1;

                    if (!myWin.CPopWinTgl) // if !visible
                        myWin.TglCPopWin();

                    myWin.myCPopWin->myCPop->drawHueRect();
                    myWin.myCPopWin->myCPop->update();

                    myWin.myCPopWin->myCPop->targetAttr = multiVec[j];
                }
            }
        }
    }

    refreshTable();
}

string Attrs::hexFromVec(glm::vec3 glmCol)
{
    glmCol = glm::clamp(glmCol, 0.f, 1.f);

    int r_i = int(glmCol.r * 255 + .5);
    int g_i = int(glmCol.g * 255 + .5);
    int b_i = int(glmCol.b * 255 + .5);

    QColor *qCol = new QColor(r_i, g_i, b_i);

    return qCol->name().toStdString();
}

void Attrs::changeEnum(QString text)
{
    if (!setupTgl)
    {
        if (changeEnumName == "")
            changeEnumName = qobject_cast<QComboBox *>(sender())->itemData(0, 33).toString();

        if (this == myWin.glslTable)
        {
            if (changeEnumName == "gridLines")
            {
                myWin.glslTable->gridLines->val_s = text;
                myWin.glslTable->gridLines->val_i = text.toInt();

                for (unsigned int k = 0; k < myWin.allObj.size(); ++k)
                {
                    if (myWin.allObj[k]->type == "GRID")
                    {
                        myWin.allObj[k]->deletable = 1;
                        myWin.allObj[k]->selected = 1;
                    }
                }

                //rebuild grid
                myWin.delete_();
                myWin.gridInit();
            }
        }

        else // attrTable
        {
            for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
            {
                if (myWin.allObj[i]->selected || changeEnumName == "cubeM")
                {
                    for (unsigned int j = 0; j < myWin.allObj[i]->multiObj.size(); ++j)
                    {
                        if (changeEnumName == myWin.allObj[i]->multiObj[j]->name)
                        {
                            //qDebug() << "match = " << changeEnumName << myWin.allObj[i]->multiObj[j]->name;
                            myWin.allObj[i]->multiObj[j]->val_s = text;

                            if (changeEnumName == "camLiType")
                            {
                                myWin.allObj[i]->targO = glm::vec3(0.f);
                                myWin.allObj[i]->setTarg(myWin.allObj[i]->targO, 0.f);

                                if (text == "DIR")
                                {
                                    myWin.allObj[i]->t->val_3 = glm::vec3(0.f, 8.f, .1f);
                                    myWin.allObj[i]->r->val_3 = glm::vec3(-45.f, 0.f, 0.f);

                                    myWin.allObj[i]->RM = myWin.allObj[i]->rotOrderUse(myWin.allObj[i]->rotOrder->val_s);
                                }

                                for (unsigned int k = 0; k < myWin.allGL.size(); ++k)
                                {
                                    if (myWin.allGL[k]->isVisible() && myWin.allGL[k]->selCamLi == myWin.selB)
                                        myWin.allGL[k]->changeCamLiType_();
                                }

                                myWin.lightCt = myWin.countLights();
                                myWin.myGLWidgetSh->addDeleteShadows("refresh");
                                myWin.setLightsDirty();
                            }

                            else if (changeEnumName == "cubeM")
                            {
                                for (unsigned int k = 0; k < myWin.myGLWidgetSh->allTex.size(); ++k)
                                {
                                    if (text == myWin.myGLWidgetSh->allTex[k].name)
                                    {
                                        if (myWin.myGLWidgetSh->allTex[k].type == "CUBE")
                                            myWin.cubeM_specular_32 = myWin.myGLWidgetSh->allTex[k].tex_32;

                                        else if (myWin.myGLWidgetSh->allTex[k].type == "CUBE_IRRAD")
                                            myWin.cubeM_irradiance_32 = myWin.myGLWidgetSh->allTex[k].tex_32;
                                    }

                                    myWin.myFSQ->cubeM->val_s = text;
                                }
                            }

                            else if (changeEnumName == "orthoType")
                            {
                                myWin.allObj[i]->camLiType->val_s = "ORTHO";
                                myWin.allObj[i]->orthoFree->val_b = 0;
                                resetOrtho(i);
                            }
                        }
                    }
                }
            }
        }

        changeEnumName = "";
        refreshTable();
    }
}

void Attrs::resetOrtho(int idx)
{
    CamSetup myCamSetup[] =
    {
        { "back", "ORTHO", {0.f, 0.f, -5.f} },
        { "bottom", "ORTHO", {0.f, -5.f, .002f} }, //
        { "front", "ORTHO", {0.f, 0.f, 5.f} },
        { "left", "ORTHO", {5.f, 0.f, 0.f} },
        { "right", "ORTHO", {-5.f, 0.f, 0.f} },
        { "top", "ORTHO", {0.f, 5.f, .002f} }, //
    };

    for (unsigned int i = 0; i < arraySize(myCamSetup); ++i)
    {
        if (myWin.allObj[idx]->orthoType->val_s == myCamSetup[i].name.toUpper())
        {
            myWin.allObj[idx]->t->val_3 = myCamSetup[i].t;
            myWin.allObj[idx]->upO = glm::vec3(0.f, 1.f, 0.f);
            myWin.allObj[idx]->setTarg(glm::vec3(0.f), 0.f);

            continue;
        }
    }
}

void Attrs::keepSelAfterMMBRelease()
{
    vector<TableRowCol> selItems_saved;
    QList<QTableWidgetItem*> selItems = selectedItems();

    for (int i = 0; i < selItems.size(); ++i)
        selItems_saved.push_back( {selItems[i]->row(), selItems[i]->column()} );

    refreshTable();

    for (unsigned int i = 0; i < selItems_saved.size(); ++i)
    {
        QModelIndex id = model()->index(selItems_saved[i].row, selItems_saved[i].col);
        selectionModel()->select(id, QItemSelectionModel::Select);
    }

    selItems_saved.clear();
}

bool Attrs::isTransformAttr(QString name)
{
    if (name == "t" || name == "r" || name == "s")
        return 1;

    return 0;
}

void Attrs::refreshRowH()
{
    myWin.cutTable->refreshTable();
    myWin.glslTable->refreshTable();
    myWin.pathTable->refreshTable();
    myWin.attrTable->refreshTable();

    refreshRowH_onRelease = 0;
}

void Attrs::writeActions(QString type, QString name, unsigned int idxO, unsigned int idxM, int idxV, float val, QString valText)
{
    if (type == "float")
    {
        multiVec[idxM]->val_f = val;

        if (name == "gridSize")
        {
            for (unsigned int k = 0; k < myWin.allObj.size(); ++k)
            {
                if (myWin.allObj[k]->type == "GRID")
                    myWin.allObj[k]->s->val_3 = glm::vec3(multiVec[idxM]->val_f);
            }
        }

        else if (name == "fov" || name == "orthoZoom" || name == "nearClip" || name == "farClip")
        {
            for (unsigned int k = 0; k < myWin.allGL.size(); ++k)
                myWin.allGL[k]->resizeGL(myWin.allGL[k]->width(), myWin.allGL[k]->height());
        }
    }

    else if (type == "int")
    {
        multiVec[idxM]->val_i = val;

        if (name == "CPopSize")
        {
            myWin.myCPopWin->resize(multiVec[idxM]->val_i, multiVec[idxM]->val_i);
            myWin.myCPopWin->myCPop->update();
        }

        else if (name == "CPopManipSize")
        {
            myWin.myCPopWin->myCPop->changedManip = 1;
            myWin.myCPopWin->myCPop->update();
        }

        else if (name == "rowH" || name == "rowH_sep")
        {
            if (mmbTgl) refreshRowH_onRelease = 1;
            else refreshRowH();
        }
    }

    else if (type == "QShortcut")
    {
        multiVec[idxM]->val_s = valText;
        multiVec[idxM]->cut->setKey(QKeySequence::fromString(multiVec[idxM]->val_s));
    }

    else if (type == "QString")
    {
        selObjs[idxO]->rename(valText);

        if (selObjs[idxO]->type == "CAMLI")
        {
            for (unsigned int l = 0; l < myWin.allCamCombo.size(); ++l)
                myWin.allCamCombo[l]->refresh();
        }

        myWin.myOutliner->refreshOutliner(1);
    }

    else if (type == "vec2")
    {
        multiVec[idxM]->val_2[idxV] = val;
    }

    else if (type == "vec3")
    {
        multiVec[idxM]->val_3[idxV] = val;

        if (name == "t")
        {
            for (unsigned int k = 0; k < multiVec.size(); ++k)
            {
                if (multiVec[k]->name == "piv")
                {
                    //float pivVal = multiVec[k]->val_3[idxV];
                    //float tVal = multiVec[j]->val_3[idxV];
                    //float textVal = item->text().toFloat();
                    //float offsetVal = pivVal - tVal;
                    //qDebug() << "piv / t / text / offset = " << pivVal << tVal << textVal << offsetVal;

//                    multiVec[k]->val_3[idxV] = item->text().toFloat(); // !!!
                    //multiVec[k]->val_3[idxV] = offsetVal;
                }
            }
        }

        if (selObjs[idxO]->type == "CAMLI")
            selObjs[idxO]->setDirty();

        else if (isTransformAttr(name) && selObjs[idxO]->type == "OBJ")
            myWin.setLightsDirty();
    }

    else if (type == "vec4")
    {
        multiVec[idxM]->val_4[idxV] = val;
    }
}

void Attrs::writeValue(QTableWidgetItem *item)
{
    if (setupTgl)
    {
        lastRow = item->row();
        lastCol = item->column();
    }

    else if (writeAttrTgl)
    {
        selObjs = selTemp();

        for (unsigned int i = 0; i < selObjs.size(); ++i)
        {
            foreach (QTableWidgetItem *singleItem, selectedItems())
            {
                if (tableType == "attr")
                    multiVec = selObjs[i]->multiObj;

                for (unsigned int j = 0; j < multiVec.size(); ++j)
                {
                    QString name = multiVec[j]->name;

                    if (name == singleItem->data(33) || singleItem->row() == 0)
                    {
                        writeActions(singleItem->data(32).toString(), name, i, j, singleItem->data(34).toInt(), glm::clamp(item->text().toFloat(), multiVec[j]->min, multiVec[j]->max), singleItem->text());

                        break;
                    }
                }
            }
        }

        selObjs.clear();

        if (!mmbTgl)
            refreshTable();
    }
}

void Attrs::vDrag(glm::vec2 diff)
{
    if (undoDragAttrs.empty())
        dragUndo_prep();

    QList<QTableWidgetItem*> selItems = selectedItems();
    QModelIndexList fullySelectedRows = selectionModel()->selectedRows();

    for (int i = 0; i < selItems.length(); ++i) //check if both columns are selected
    {
        for (int j = 0; j < fullySelectedRows.length(); ++j)
        {
            if (selItems[i]->row() == fullySelectedRows[j].row() && selItems[i]->column() == 0)
                selItems.removeAt(i);
        }
    }

    myWin.attrTable->writeAttrTgl = 0;
    selObjs = selTemp();

    for (unsigned int i = 0; i < selObjs.size(); ++i)
    {
        if (tableType == "attr")
            multiVec = selObjs[i]->multiObj;

        for (unsigned int j = 0; j < multiVec.size(); ++j)
        {
            foreach (QTableWidgetItem *singleItem, selItems)
            {
                QString attrNameVD = singleItem->data(33).toString();
                QString attrTypeVD = singleItem->data(32).toString();

                if (multiVec[j]->name == attrNameVD)
                {
                    float vSlide, vSlideVal;

                    if (ctrlTgl) vSlide = vSliderSpeed->val_f * myWin.etcTable->dragBoost->val_f;
                    else if (altTgl) vSlide = vSliderSpeed->val_f * myWin.etcTable->dragSlow->val_f;
                    else vSlide = vSliderSpeed->val_f;

                    if (attrNameVD == "r") vSlide *= 10.f;
                    else if (attrTypeVD == "float") vSlide *= .3f;
                    else if (attrTypeVD == "int") vSlide *= 5.f;
                    else if (attrNameVD == "t" || attrNameVD == "pivot") vSlide *= .2f;

                    if (tableType == "attr")
                    {
                        if (attrTypeVD == "int") vSlideVal = glm::clamp(multiVec[j]->val_i + vSlide * diff.x, multiVec[j]->min, multiVec[j]->max);
                        else if (attrTypeVD == "float") vSlideVal = glm::clamp(multiVec[j]->val_f + vSlide * diff.x, multiVec[j]->min, multiVec[j]->max);
                        else if (attrTypeVD == "vec2") vSlideVal = glm::clamp(multiVec[j]->val_2[singleItem->data(34).toInt()] + vSlide * diff.x, multiVec[j]->min, multiVec[j]->max);
                        else if (attrTypeVD == "vec3") vSlideVal = glm::clamp(multiVec[j]->val_3[singleItem->data(34).toInt()] + vSlide * diff.x, multiVec[j]->min, multiVec[j]->max);
                        else if (attrTypeVD == "vec4") vSlideVal = glm::clamp(multiVec[j]->val_4[singleItem->data(34).toInt()] + vSlide * diff.x, multiVec[j]->min, multiVec[j]->max);
                    }

                    else
                        vSlideVal = glm::clamp(item(singleItem->row(), 1)->text().toFloat() + vSlide * diff.x, multiVec[j]->min, multiVec[j]->max);

                    if (selObjs[i] == myWin.selB || tableType != "attr")
                    {
                        if (attrTypeVD == "int")
                            item(singleItem->row(), 1)->setText(QString::number((int)vSlideVal, 'g', '4'));

                        else
                        {
                            stringstream drag_ss;
                            drag_ss << fixed << setprecision(3) << vSlideVal;
                            item(singleItem->row(), 1)->setText(QString::fromStdString(drag_ss.str()));
                        }
                    }

                    writeActions(attrTypeVD, attrNameVD, i, j, singleItem->data(34).toInt(), vSlideVal, singleItem->text());
                }
            }
        }
    }

    selObjs.clear();

    myWin.attrTable->writeAttrTgl = 1;
}

void Attrs::dragUndo_prep()
{
    for (int i = 0; i < selectedItems().size(); ++i)
    {
        for (unsigned int j = 0; j < multiVec.size(); ++j)
        {
            if (selectedItems()[i]->data(33) == multiVec[j]->name)
            {
                shared_ptr<MultiAttr> at = multiVec[j]->Clone();

                if (selectedItems()[i]->data(32) == "vec3")
                    at->idx_3 = selectedItems()[i]->data(34).toInt();

                undoDragAttrs.push_back(at);
            }
        }
    }
}
