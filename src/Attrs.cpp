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

#include "Attrs.h"

Attrs::Attrs(MainWin &myWinTemp, QWidget *parent) : QTableWidget(parent), myWin(myWinTemp)
{
    setRowCount(1);
    setColumnCount(2);
    setFrameShadow(QFrame::Raised);
    setItemDelegate(myWin.myFocus);
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

    writeAttrTgl = true;
    connect(this, &Attrs::itemChanged, this, &Attrs::writeValue);

    viewport()->setMouseTracking(1);
    installEventFilter(this);

    altTgl = ctrlTgl = shiftTgl = lmbTgl = mmbTgl = camAttrWrongType = false;
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

        auto currIdx = currentIndex();
        auto nextIdx = model()->index(currIdx.row() + 1, 1);

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
        auto *ke = static_cast<QKeyEvent*>(e);

        if (ke->key() == Qt::Key_Tab)
        {
            auto currIdx = currentIndex();
            auto nextIdx = model()->index(currIdx.row() + 1, 1);

            selectionModel()->select(nextIdx, QItemSelectionModel::Select);
            setCurrentIndex(nextIdx);
            setFocus();
            edit(nextIdx);

            return true;
        }

        else if (ke->key() == Qt::Key_I || Qt::Key_K)
        {
            if (myWin.myPaintWin->isVisible())
            {
                cout << "I or K is being PRESSED and paintWin is visible" << endl;
                e->ignore(); // no "I" cell highlight while keeping paintWin open

                return false;
            }
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
            for (auto &i : undoDragAttrs)
            {
                for (auto &j : multiVec)
                {
                    if (i->name == j->name)
                    {
                        if (i->type == "bool")
                        {
                            j->val_b = i->val_b;

                            if (j->name == "framed")
                            {
                                Qt::WindowFlags framedOrFrameless;

                                if (!j->val_b)
                                    framedOrFrameless = Qt::FramelessWindowHint;

                                myWin.hide();
                                myWin.setWindowFlags(framedOrFrameless);
                                myWin.show();
                            }
                        }

                        else if (i->type == "float")
                        {
                            j->val_f = i->val_f;

                            if (j->name == "fov")
                                    myWin.myGL->resizeGL(myWin.myGL->width(), myWin.myGL->height());
                        }

                        else if (i->type == "int")
                            j->val_i = i->val_i;


                        else if (i->type == "vec3")
                        {
                            //undo pivot
                            for (auto &k : myWin.allObj)
                            {
                                if (k->selected)
                                {
                                    float offSetPivotPos = k->piv->val_3[i->idx_3] - j->val_3[i->idx_3];

                                    if (j->name == "t") //ALSO MOVE PIVOT
                                        k->piv->val_3[i->idx_3] = i->val_3[i->idx_3] + offSetPivotPos;
                                }
                            }

                            j->val_3[i->idx_3] = i->val_3[i->idx_3];
                        }

                    }
                }
            }

            refreshAndKeepSelAfterMMBRelease();
            myWin.setLightsDirty();
        }
    }

    else if (e->key() == Qt::Key_I)
        myWin.PaintWinTgl(1, 0);

    else if (e->key() == Qt::Key_K)
        myWin.PaintWinTgl(1, 1);

    else if (e->key() == Qt::Key_Alt) altTgl = true;
    else if (e->key() == Qt::Key_Control) ctrlTgl = true;
    else if (e->key() == Qt::Key_Shift) shiftTgl = true;

    if (ctrlTgl && e->key() == Qt::Key_Q)
        myWin.quitConfirm();

    if (altTgl && ctrlTgl)
        setCursor(Qt::WhatsThisCursor);

    QTableWidget::keyPressEvent(e);
}

void Attrs::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_Alt) altTgl = false;
    else if (e->key() == Qt::Key_Control) ctrlTgl = false;
    else if (e->key() == Qt::Key_Shift) shiftTgl = false;

    else if (e->key() == Qt::Key_I)
    {
        if (myWin.myPaintWin->stackedMain->currentIndex() == 0)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_K)
    {
        if (myWin.myPaintWin->stackedMain->currentIndex() == 1)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_QuoteLeft)
        myWin.myPrefWin->hide();

    setCursor(Qt::ArrowCursor);
}

void Attrs::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = true;

        return QTableWidget::mousePressEvent(e);
    }

    if (e->button() == Qt::MiddleButton)
        mmbTgl = true;

    if (e->button() == Qt::RightButton)
    {
        auto *myItem = itemAt(e->pos());

        if (myItem)
        {
            for (auto &i : multiVec)
            {
                if (myItem->text().toStdString() == i->name && i->type == "enum")
                {
                    vector<PopSetup> radPop;
                    vector<string> usableList = i->comboList;

                    auto reversePop = false;

                    if (reversePop)
                    {
                        for (size_t j = usableList.size(); --j > -1; )
                            radPop.push_back( { usableList[j], "RAD", 75, 50 } );
                    }

                    else
                    {
                        for (auto &j : usableList)
                        radPop.push_back( { j, "RAD", 75, 50 } );
                    }

                    myWin.myRadPop->popName = myItem->text().toStdString();

                    myWin.myRadPop->radius = 0;

                    for (int j = 0; j < usableList.size(); ++j)
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
        lmbTgl = false;

    else if (e->button() == Qt::MiddleButton)
    {
        mmbTgl = false;
        undoDragAttrs.clear();

        if (refreshRowH_onRelease)
            refreshRowH();
    }

    setCursor(Qt::ArrowCursor);

    return QTableWidget::mouseReleaseEvent(e);
}

void Attrs::mouseMoveEvent(QMouseEvent *e)
{
    auto *myItem = itemAt(e->pos());

    if (myItem && lmbTgl)
        myItem->setSelected(1);

    pMouseOld = pMouseNew;
    pMouseNew = myWin.toVec2(e->pos());
    pMouseDiff =  pMouseNew - pMouseOld;

    if (mmbTgl && !selectedItems().empty())
    {
        setCursor(Qt::SizeHorCursor);
        vDrag(pMouseDiff);
    }
}

void Attrs::cutTable_init()
{
    boundingBox = make_shared<MultiAttr>();
    boundingBox->name = "boundingBox";
    boundingBox->type = "QShortcut";
    boundingBox->val_s = "CTRL+B";
    boundingBox->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(boundingBox->val_s)), &myWin);
    connect(boundingBox->cut, SIGNAL(activated()), &myWin, SLOT(boundingBoxTgl()));
    multiCut.push_back(boundingBox);

    deleteS = make_shared<MultiAttr>();
    deleteS->name = "delete";
    deleteS->type = "QShortcut";
    deleteS->val_s = "DELETE";
    deleteS->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(deleteS->val_s)), &myWin);
    connect(deleteS->cut, SIGNAL(activated()), &myWin, SLOT(delete_()));
    multiCut.push_back(deleteS);

    dupe = make_shared<MultiAttr>();
    dupe->name = "dupe";
    dupe->type = "QShortcut";
    dupe->val_s = "CTRL+D";
    dupe->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(dupe->val_s)), &myWin);
    connect(dupe->cut, SIGNAL(activated()), &myWin, SLOT(dupe()));
    multiCut.push_back(dupe);

    hide = make_shared<MultiAttr>();
    hide->name = "hide";
    hide->type = "QShortcut";
    hide->val_s = "H";
    hide->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(hide->val_s)), &myWin);
    connect(hide->cut, SIGNAL(activated()), &myWin, SLOT(hide()));
    multiCut.push_back(hide);

    unHide = make_shared<MultiAttr>();
    unHide->name = "unHide";
    unHide->type = "QShortcut";
    unHide->val_s = "SHIFT+H";
    unHide->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(unHide->val_s)), &myWin);
    connect(unHide->cut, SIGNAL(activated()), &myWin, SLOT(unHide()));
    multiCut.push_back(unHide);

    invertSel = make_shared<MultiAttr>();
    invertSel->name = "invertSel";
    invertSel->type = "QShortcut";
    invertSel->val_s = "CTRL+SHIFT+I";
    invertSel->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(invertSel->val_s)), &myWin);
    connect(invertSel->cut, SIGNAL(activated()), &myWin, SLOT(invertSel()));
    multiCut.push_back(invertSel);

    gizSpace = make_shared<MultiAttr>();
    gizSpace->name = "gizSpace";
    gizSpace->type = "string";
    gizSpace->val_s = "ALT+X";
    gizSpace->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(gizSpace->val_s)), &myWin);
    connect(gizSpace->cut, SIGNAL(activated()), &myWin, SLOT(gizSpaceSwitch()));
    multiCut.push_back(gizSpace);

    parent = make_shared<MultiAttr>();
    parent->name = "parent";
    parent->type = "string";
    parent->val_s = "CTRL+P";
    parent->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(parent->val_s)), &myWin);
    connect(parent->cut, SIGNAL(activated()), &myWin, SLOT(parent()));
    multiCut.push_back(parent);

    parentWorld = make_shared<MultiAttr>();
    parentWorld->name = "parentWorld";
    parentWorld->type = "QShortcut";
    parentWorld->val_s = "SHIFT+P";
    parentWorld->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(parentWorld->val_s)), &myWin);
    connect(parentWorld->cut, SIGNAL(activated()), &myWin, SLOT(parentWorld()));
    multiCut.push_back(parentWorld);

    pivCenter = make_shared<MultiAttr>();
    pivCenter->name = "pivCenter";
    pivCenter->type = "QShortcut";
    pivCenter->val_s = "F5";
    pivCenter->cut = new QShortcut(QKeySequence::fromString(QString::fromStdString(pivCenter->val_s)), &myWin);
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
    framed->val_b = false;
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
    pathGLSL->type = "string";
    pathGLSL->val_s = QDir::currentPath().toStdString();
    pathGLSL->val_s.append("/shaders/");
    multiPath.push_back(pathGLSL);

    pathObj = make_shared<MultiAttr>();
    pathObj->name = "pathObj";
    pathObj->type = "string";
    pathObj->val_s = "C:/users/aleks/desktop/obj/";
    multiPath.push_back(pathObj);

    pathTex = make_shared<MultiAttr>();
    pathTex->name = "pathTex";
    pathTex->type = "string";
    pathTex->val_s = "C:/users/aleks/desktop/tex/";
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

    CPopManipSize = make_shared<MultiAttr>();
    CPopManipSize->name = "CPopManipSize";
    CPopManipSize->type = "int";
    CPopManipSize->val_i = 9;
    CPopManipSize->min = 3.f;
    multiGL.push_back(CPopManipSize);

    gizSide = make_shared<MultiAttr>();
    gizSide->name = "gizSide";
    gizSide->type = "bool";
    gizSide->val_b = true;
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
    gridLines->comboList = { "10", "20", "30", "40", "50" };
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
    for (auto &i : myWin.cutTable->multiCut)
        i->cut->setEnabled(state);
}

void Attrs::resetPrefs()
{
    if (this == myWin.cutTable)
    {
        prefReset = multiCut;
        prefReset_stored = multiCut_stored;
    }

    else if (this == myWin.etcTable)
    {
        prefReset = multiEtc;
        prefReset_stored = multiEtc_stored;
    }

    else if (this == myWin.glslTable)
    {
        prefReset = multiGL;
        prefReset_stored = multiGL_stored;
    }

    else if (this == myWin.pathTable)
    {
        prefReset = multiPath;
        prefReset_stored = multiPath_stored;
    }

    for (auto &i : prefReset)
    {
        for (auto &j : prefReset_stored)
        {
            if (i->name == j->name)
            {
                i->val_b = j->val_b;
                i->val_i = j->val_i;
                i->val_f = j->val_f;
                i->val_s = j->val_s;
                i->val_2 = j->val_2;
                i->val_3 = j->val_3;
                i->val_4 = j->val_4;

                break;
            }
        }
    }

    setCursor(Qt::ArrowCursor);
    refreshTable();
}

int Attrs::rowCt_get(string type)
{
    int realRowCt = 0;

    for (auto &i : multiVec)
    {
        if (tabIdx == i->tab)
        {
            if (i->typeX == type)
            {
                if (i->type == "vec2") realRowCt += 2;
                else if (i->type == "vec3") realRowCt += 3;
                else if (i->type == "vec4") realRowCt += 4;
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
    setupTgl = true;

    if (this == myWin.attrTable && myWin.selB == 0)
    {
        myWin.stackedAttrTable->setCurrentIndex(0); //
        storedStackedIdx = myWin.stackedAttrTable->currentIndex();

        return false;
    }

    else if (this == myWin.attrTable)
    {
        multiVec = myWin.selB->multiObj;
        myWin.stackedAttrTable->setCurrentIndex(1); //
        storedStackedIdx = myWin.stackedAttrTable->currentIndex();

        auto objName_temp = myWin.selB->name->val_s;

        if (myWin.countSel() > 1 && !myWin.myFSQ->selected)
            objName_temp.append("...");

        selObjN->setText(QString::fromStdString(objName_temp));
    }

    else
    {
        selObjN->setFlags(selObjN->flags()^(Qt::ItemIsEditable | Qt::ItemIsSelectable));

        if (this == myWin.cutTable)
        {
            selObjN->setText("cut");
            multiVec = multiCut;
        }

        else if (this == myWin.etcTable)
        {
            selObjN->setText("etc");
            multiVec = multiEtc;
        }

        else if (this == myWin.glslTable)
        {
            selObjN->setText("glsl");
            multiVec = multiGL;
        }

        else if (this == myWin.pathTable)
        {
            selObjN->setText("path");
            multiVec = multiPath;
        }
    }

    if (!myButtons.empty())
        myButtons.clear();

    if (!myEnumCombo.empty())
        myEnumCombo.clear();

    selObjN->setFont(QFont("DejaVu Sans Mono", 13, 75));
    selObjN->setForeground(QBrush(myWin.toQC(glm::vec3(0.f))));
    selObjN->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    selObjN->setData(32, "string"); //TYPE

    int rowCt = rowCt_get("ALL");
    int rowCtO = 0;

    if (this == myWin.attrTable)
    {
        rowCt = (myWin.selB->type == "FBO") ? 0 : rowCt;
        rowCtO = rowCt_get(myWin.selB->type);
    }

    setRowCount(rowCt + rowCtO + 1);

    for (auto &i : multiVec)
    {
        attrType = i->type;
        attrName = i->name;
        attrGrp = i->grp;

        if (tabIdx == i->tab)
        {
            if (this == myWin.attrTable && myWin.selB == myWin.myFSQ)
            {
                //hide transform attrs, show only matching typeX
                if (i->typeX == myWin.selB->type)
                    tableWidgetItem_add(i);
            }

            else
            {
                if (i->typeX == "ALL")
                    tableWidgetItem_add(i);

                if (this == myWin.attrTable)
                {
                    if (i->typeX == myWin.selB->type)
                        tableWidgetItem_add(i);
                }
            }
        }
    }

    useThisRow = 0;
    setupTgl = false;

    return true;
}

void Attrs::tableWidgetItem_add(shared_ptr<MultiAttr> attr)
{
    if (attrType == "bool")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        auto *itemLabel = new QTableWidgetItem(QString::fromStdString(attrName));
        prepItems(itemLabel, "label");

        auto noSelFlag = itemLabel->flags();
        noSelFlag &= ~Qt::ItemIsSelectable;
        itemLabel->setFlags(noSelFlag);

        //VALUE
        auto *itemVal = new QPushButton();
        itemVal->setFlat(1);
        itemVal->setText(QString::number(attr->val_b, 'g', 4));
        itemVal->setFont(valF);

        //SS
        auto pushBG = (useThisRow % 2) ? "#555555" : "#4d4d4d";

        stringstream ss;
        ss << "QPushButton { color: " << "black;" << "background: " << pushBG << "; } QPushButton:pressed { background: " << hexFromVec(myWin.gammaCsel()) << "; }";

        auto colorStyleSheet_dynamic = ss.str();
        itemVal->setStyleSheet(QString::fromStdString(colorStyleSheet_dynamic));

        connect(itemVal, &QPushButton::clicked, this, &Attrs::boolChange);
        setCellWidget(useThisRow, 1, itemVal);
        myButtons.push_back( { itemVal, attrName } );
    }

    else if (attrType == "color")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        auto *itemLabel = new QTableWidgetItem(QString::fromStdString(attrName));
        prepItems(itemLabel, "label");

        auto noSelFlag = itemLabel->flags();
        noSelFlag &= ~Qt::ItemIsSelectable;
        itemLabel->setFlags(noSelFlag);

        //VALUE
        auto *itemVal = new QPushButton();
        itemVal->setFlat(1);

        //SS
        glm::vec3 Cadjusted(pow(attr->val_3.r, myWin.gamma), pow(attr->val_3.g, myWin.gamma), pow(attr->val_3.b, myWin.gamma));
        auto usableCol = hexFromVec(Cadjusted);

        stringstream ss;
        ss << "QPushButton { background: " << usableCol << "; } QPushButton:pressed { background: " << hexFromVec(myWin.gammaCsel()) << "; }";
        auto colorStyleSheet_dynamic = ss.str();
        itemVal->setStyleSheet(QString::fromStdString(colorStyleSheet_dynamic));

        connect(itemVal, &QPushButton::clicked, this, &Attrs::colorPick);
        setCellWidget(useThisRow, 1, itemVal);
        myButtons.push_back( { itemVal, attrName } );
    }

    else if (attrType == "enum")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        auto *itemLabel = new QTableWidgetItem(QString::fromStdString(attrName));
        prepItems(itemLabel, "label");

        auto noSelFlag = itemLabel->flags();
        noSelFlag &= ~Qt::ItemIsSelectable;
        itemLabel->setFlags(noSelFlag);

        //VALUE
        auto *tableCombo = new Combo;
        tableCombo->setItemDelegate(myWin.myFocus);

        if (attrName == "camLiType")
        {
            for (auto &i : attr->comboList)
            {
                auto myType = i;

                if (myType == "FPS" || myType == "ORTHO" || myType == "PERSP")
                    tableCombo->addItem(myWin.myIconCam, QString::fromStdString(i));

                else
                    tableCombo->addItem(myWin.myIconLight, QString::fromStdString(i));
            }
        }

        else
        {
            for (auto &i : attr->comboList)
                tableCombo->addItem(QString::fromStdString(i));
        }

        tableCombo->setFont(labelF);
        tableCombo->setStyleSheet("color:black");
        setCellWidget(useThisRow, 1, tableCombo);

        connect(tableCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeEnumD(QString)));

        tableCombo->setCurrentIndex(tableCombo->findText(QString::fromStdString(attr->val_s)));
        tableCombo->setEditable(1);
        tableCombo->lineEdit()->setAlignment(Qt::AlignHCenter);
        tableCombo->lineEdit()->setFont(labelF); //
        tableCombo->lineEdit()->setStyleSheet(QString::fromStdString(myWin.myStyle)); //
        tableCombo->setEditable(0);

        for (int j = 0; j < tableCombo->count(); ++j)
        {
            tableCombo->setItemData(j, Qt::AlignHCenter, Qt::TextAlignmentRole);
            tableCombo->setItemData(j, QString::fromStdString(attrName), 33); //BASENAME
        }

        myEnumCombo.push_back(tableCombo);
    }

    else if (attrType == "float")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        auto *itemLabel = new QTableWidgetItem(QString::fromStdString(attrName));
        prepItems(itemLabel, "label");

        //VALUE
        stringstream ss;
        ss << fixed << setprecision(3) << attr->val_f;
        auto *itemVal = new QTableWidgetItem(QString::fromStdString(ss.str()));
        prepItems(itemVal, "value");
    }

    else if (attrType == "int")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        auto *itemLabel = new QTableWidgetItem(QString::fromStdString(attrName));
        prepItems(itemLabel, "label");

        auto *itemVal = new QTableWidgetItem(QString::number(attr->val_i, 'g', 4));
        prepItems(itemVal, "value");
    }

    else if (attrType == "string" || attrType == "QShortcut")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

        auto *itemLabel = new QTableWidgetItem(QString::fromStdString(attrName));
        prepItems(itemLabel, "label");

        //VALUE
        auto *itemVal = new QTableWidgetItem(QString::fromStdString(attr->val_s));
        prepItems(itemVal, "value");
    }

    else if (attrType == "sep")
    {
        useThisRow = lastRow + 1;
        setRowHeight(useThisRow, myWin.etcTable->rowH_sep->val_i);

        auto *itemLabel = new QTableWidgetItem(QString::fromStdString(attrName));
        prepItems(itemLabel, "label");
        itemLabel->setFont(sepF); //
        itemLabel->setFlags(itemLabel->flags()^Qt::ItemIsSelectable);

        //get highlight col for label - replace with global highlight col
        itemLabel->setForeground(QBrush(myWin.toQC(myWin.gammaCsel())));

        //VALUE
        auto *itemVal = new QTableWidgetItem();
        prepItems(itemVal, "value");
        itemVal->setFlags(itemVal->flags()^Qt::ItemIsSelectable);
        itemVal->setFlags(itemVal->flags()^Qt::ItemIsEditable);
    }

    else if (attrType == "vec2" || attrType == "vec3" || attrType == "vec4")
    {
        string appendEnding[4] = { "X", "Y", "Z", "W" };

        int usableIter = attrType.back() - '0';

        for (int j = 0; j < usableIter; ++j)
        {
            useThisRow = lastRow + 1;
            setRowHeight(useThisRow, myWin.etcTable->rowH->val_i);

            auto conc = attrName;
            conc.append(appendEnding[j]);

            auto *itemLabel = new QTableWidgetItem(QString::fromStdString(conc));
            prepItems(itemLabel, "label");
            itemLabel->setData(34, j); //VEC IDX

            float usableVal;
            if (usableIter == 2) usableVal = attr->val_2[j];
            if (usableIter == 3) usableVal = attr->val_3[j];
            if (usableIter == 4) usableVal = attr->val_4[j];

            stringstream ss;
            ss << fixed << setprecision(3) << usableVal;

            auto *itemVal = new QTableWidgetItem(QString::fromStdString(ss.str()));
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
        item->setData(32, QString::fromStdString(attrType));
        item->setData(33, QString::fromStdString(attrName));
        item->setData(35, QString::fromStdString(attrGrp));

        setItem(useThisRow, 0, item);
    }

    else if (type == "value")
    {
        item->setFont(valF);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item->setForeground(QBrush(myWin.toQC(glm::vec3(0.f))));
        item->setData(32, QString::fromStdString(attrType));
        item->setData(33, QString::fromStdString(attrName));
        item->setData(35, QString::fromStdString(attrGrp));

        setItem(useThisRow, 1, item);
    }
}

vector<shared_ptr<Object>> Attrs::selTemp()
{
    vector<shared_ptr<Object>> temp;

    for (auto &i : myWin.allObj)
    {
        if (this == myWin.attrTable)
        {
            if (i->selected)
                temp.push_back(i);
        }

        else //need 1 obj to iterate...use any nondeletable
        {
            if (i->name->val_s == "top")
            {
                temp.push_back(i);

                continue; //
            }
        }
    }

    return temp;
}

void Attrs::boolChange()
{
    for (auto &i : myButtons)
    {
        if (i.button == sender())
        {
            auto boolChangeTemp = selTemp();

            for (auto &j : boolChangeTemp)
            {
                if (this == myWin.attrTable)
                    multiVec = j->multiObj;

                for (auto &k : multiVec)
                {
                    if (i.name == k->name)
                    {
                        k->val_b = !k->val_b;

                        if (k->name == "framed")
                        {
                            Qt::WindowFlags framedOrFrameless;

                            if (!k->val_b)
                                framedOrFrameless = Qt::FramelessWindowHint;

                            myWin.setWindowFlags(framedOrFrameless);
                            myWin.show();
                        }

                        else if (k->name == "fxaaBlur")
                            myWin.myPP->resizeTexClearMem();

                        else if (k->name == "gizSide")
                            myWin.myGL->gizSideTgl_swap();

                        else if (k->name == "orthoFree" && k->val_b == false)
                            resetOrtho(j);

                        else if (k->name == "bb" || k->name == "shadowCast" || k->name == "v")
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
    for (auto &i : myButtons)
    {
        if (i.button == sender())
        {
            for (auto &j : multiVec)
            {
                if (i.name == j->name)
                {
                    cout << "j->name = " << j->name << endl;

                    auto Cldr = glm::hsvColor(glm::clamp(j->val_3, 0.f, 1.f));

//                    myWin.myPaintWin->myCPop->hue = Cldr[0];
                    myWin.myPaintWin->myCPop->hue = Cldr[0] / 360.f; //divide by 360 to get 0-1 hue...
                    myWin.myPaintWin->myCPop->sat = Cldr[1];
                    myWin.myPaintWin->myCPop->val = Cldr[2];

                    myWin.myPaintWin->myCPop->changedManip = true;

                    myWin.myPaintWin->myCPop->drawHueRect();
                    myWin.myPaintWin->myCPop->update();

                    myWin.myPaintWin->myCPop->targetAttr = j; //changes the attr color box of the vec3...
                }
            }
        }
    }

    refreshTable();
}

string Attrs::hexFromVec(glm::vec3 glmCol)
{
    glmCol = glm::clamp(glmCol, 0.f, 1.f);

    auto r_i = int(glmCol.r * 255 + .5);
    auto g_i = int(glmCol.g * 255 + .5);
    auto b_i = int(glmCol.b * 255 + .5);

    return QColor(r_i, g_i, b_i).name().toStdString();
}

void Attrs::changeEnumD(QString text)
{
    changeEnum(text.toStdString());
}

void Attrs::changeEnum(string text)
{
    if (!setupTgl)
    {
        if (changeEnumName == "")
            changeEnumName = qobject_cast<QComboBox *>(sender())->itemData(0, 33).toString().toStdString();

        if (this == myWin.glslTable)
        {
            if (changeEnumName == "gridLines")
            {
                myWin.glslTable->gridLines->val_s = text;
                myWin.glslTable->gridLines->val_i = QString::fromStdString(text).toInt();

                for (auto &i : myWin.allObj)
                {
                    if (i->type == "GRID")
                    {
                        i->deletable = true;
                        i->selected = true;
                    }
                }

                //rebuild grid
                myWin.delete_(); // TO DO : GRID ONLY !!!!! USE TEMP SEL HERE
                myWin.gridInit();
            }
        }

        else // attrTable
        {
            for (auto &i : myWin.allObj)
            {
                if (i->selected || changeEnumName == "cube_specM")
                {
                    for (auto &j : i->multiObj)
                    {
                        if (changeEnumName == j->name)
                        {
//                            cout << "match = " << changeEnumName << myWin.allObj[i]->multiObj[j]->name << endl;

                            j->val_s = text;

                            if (changeEnumName == "camLiType")
                            {
                                i->targO = glm::vec3(0.f);
                                i->setTarg(i->targO, 0.f);

                                if (text == "DIR")
                                {
                                    i->t->val_3 = glm::vec3(0.f, 8.f, .1f);
                                    i->r->val_3 = glm::vec3(-45.f, 0.f, 0.f);

                                    i->RM = i->rotOrderUse(i->rotOrder->val_s);
                                }

                                if (myWin.myGL->selCamLi == myWin.selB)
                                    myWin.myGL->changeCamLiType_();

                                myWin.lightCt = myWin.countLights();
                                myWin.myGLWidgetSh->addDeleteShadows("refresh");
                                myWin.setLightsDirty();
                                myWin.myGLWidgetSh->UBO_light_needsUp = true; //
                            }

                            else if (changeEnumName == "cube_specM")
                            {
                                for (auto &k : myWin.myGLWidgetSh->allMaps)
                                {
                                    if (text == k.name)
                                    {
                                        if (k.type == "CUBE_SPEC")
                                            myWin.cubeM_specular_32 = k.layer[0].tex1_32;

                                        else if (k.type == "CUBE_IRRAD")
                                            myWin.cubeM_irradiance_32 = k.layer[0].tex1_32;
                                    }

                                    myWin.myFSQ->cubeM->val_s = text;
                                }
                            }

                            else if (changeEnumName == "orthoType")
                            {
                                i->camLiType->val_s = "ORTHO";
                                i->orthoFree->val_b = false;
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

void Attrs::resetOrtho(shared_ptr<Object> obj)
{
    CamSetup myCamSetup[] =
    {
        { "back", "ORTHO", { 0.f, 0.f, -5.f } },
        { "bottom", "ORTHO", { 0.f, -5.f, .002f } }, //
        { "front", "ORTHO", { 0.f, 0.f, 5.f } },
        { "left", "ORTHO", { 5.f, 0.f, 0.f } },
        { "right", "ORTHO", { -5.f, 0.f, 0.f } },
        { "top", "ORTHO", { 0.f, 5.f, .002f } }, //
    };

    for (auto &i : myCamSetup)
    {
        if (obj->orthoType->val_s == myWin.stringToUpper(i.name))
        {
            obj->t->val_3 = i.t;
            obj->upO = glm::vec3(0.f, 1.f, 0.f);
            obj->setTarg(glm::vec3(0.f), 0.f);

            continue;
        }
    }
}

void Attrs::refreshAndKeepSelAfterMMBRelease()
{
    vector<TableRowCol> selItems_saved;
    vector<QTableWidgetItem *> selItems;

    for (auto &i : selectedItems())
        selItems.push_back(i);

    for (auto &i : selItems)
        selItems_saved.push_back( { i->row(), i->column() } );

    refreshTable();

    for (auto &i : selItems_saved)
    {
        auto id = model()->index(i.row, i.col);
        selectionModel()->select(id, QItemSelectionModel::Select);
    }

    selItems_saved.clear();
    undoDragAttrs.clear();
    mmbTgl = false;
    setCursor(Qt::ArrowCursor);
}

bool Attrs::isTransformAttr(string name)
{
    if (name == "t" || name == "r" || name == "s")
        return true;

    return false;
}

void Attrs::refreshRowH()
{
    myWin.cutTable->refreshTable();
    myWin.glslTable->refreshTable();
    myWin.pathTable->refreshTable();
    myWin.attrTable->refreshTable();

    refreshRowH_onRelease = false;
}

void Attrs::writeActions(string type, string name, shared_ptr<Object> obj, shared_ptr<MultiAttr> attr, int idxV, float val, string valText)
{
    if (type == "float")
    {
        attr->val_f = val;

        if (name == "gridSize")
        {
            for (auto &i : myWin.allObj)
            {
                if (i->type == "GRID")
                    i->s->val_3 = glm::vec3(attr->val_f);
            }
        }

        else if (name == "fov" || name == "orthoZoom" || name == "nearClip" || name == "farClip")
            myWin.myGL->resizeGL(myWin.myGL->width(), myWin.myGL->height());

        else if (name == "nearShadow" || name == "farShadow")
            obj->dirtyShadow = true;

        else if (name == "lInten" || name == "lSpotI" || name == "lSpotO")
            myWin.myGLWidgetSh->UBO_light_needsUp = true;

        else if (name == "edgeThr")
            myWin.myGLWidgetSh->brushOutlineUpB = true;
    }

    else if (type == "int")
    {
        attr->val_i = val;

        if (name == "CPopManipSize")
        {
            myWin.myPaintWin->myCPop->changedManip = true;
            myWin.myPaintWin->myCPop->update();
        }

        else if (name == "rowH" || name == "rowH_sep")
        {
            if (mmbTgl) refreshRowH_onRelease = true;
            else refreshRowH();
        }
    }

    else if (type == "QShortcut")
    {
        attr->val_s = valText;
        attr->cut->setKey(QKeySequence::fromString(QString::fromStdString(attr->val_s)));
    }

    else if (type == "string")
    {
        obj->rename(valText);

        if (obj->type == "CAMLI")
        {
            for (auto &i : myWin.allCamCombo)
                i->refresh();
        }

        myWin.myOutliner->refreshOutliner(1);
    }

    else if (type == "vec2")
    {
        attr->val_2[idxV] = val;
    }

    else if (type == "vec3")
    {
        attr->val_3[idxV] = val;

        if (name == "t" || name == "r" || name == "s")
        {
            obj->AABB_toWS();
        }

        if (name == "t")
        {
            for (auto &i : multiVec)
            {
                if (i->name == "piv")
                {
                    //float pivVal = i->val_3[idxV];
                    //float tVal = i->val_3[idxV];
                    //float textVal = item->text().toFloat();
                    //float offsetVal = pivVal - tVal;
                    //cout() << "piv / t / text / offset = " << pivVal << tVal << textVal << offsetVal << endl;

                    //                    i->val_3[idxV] = item->text().toFloat(); // !!!
                    //i->val_3[idxV] = offsetVal;
                }
            }
        }

        if (obj->type == "CAMLI")
            obj->setDirty();

        else if (isTransformAttr(name) && obj->type == "OBJ")
            myWin.setLightsDirty();
    }

    else if (type == "vec4")
    {
        attr->val_4[idxV] = val;
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

        for (auto &i : selObjs)
        {
            for (auto *singleItem : selectedItems())
            {
                if (this == myWin.attrTable)
                    multiVec = i->multiObj;

                for (auto &j : multiVec)
                {
                    auto name = j->name;
                    auto grp = j->grp;

                    if ((QString::fromStdString(name) == singleItem->data(33) && QString::fromStdString(grp) == singleItem->data(35) ) || singleItem->row() == 0)
                    {
                        writeActions(singleItem->data(32).toString().toStdString(), name, i, j, singleItem->data(34).toInt(), glm::clamp(item->text().toFloat(), j->min, j->max), singleItem->text().toStdString());

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

    vector<QTableWidgetItem *> selItems;

    for (auto &i : selectedItems()) // check if both col are selected
    {
        auto notADupe = true;

        for (auto &j : selectionModel()->selectedRows())
        {
            if (i->row() == j.row() && i->column() == 0)
                notADupe = 1;
        }

        if (notADupe)
            selItems.push_back(i);
    }

    myWin.attrTable->writeAttrTgl = false;
    selObjs = selTemp();

    for (auto &i : selObjs)
    {
        if (this == myWin.attrTable)
            multiVec = i->multiObj;

        for (auto &j : multiVec)
        {
            for (auto *singleItem : selItems)
            {
                auto attrNameVD = singleItem->data(33).toString().toStdString();
                auto attrTypeVD = singleItem->data(32).toString().toStdString();
                auto attrGrpVD = singleItem->data(35).toString().toStdString();

                if (j->name == attrNameVD && j->grp == attrGrpVD)
                {
                    float vSlide, vSlideVal;

                    if (ctrlTgl) vSlide = vSliderSpeed->val_f * myWin.etcTable->dragBoost->val_f;
                    else if (altTgl) vSlide = vSliderSpeed->val_f * myWin.etcTable->dragSlow->val_f;
                    else vSlide = vSliderSpeed->val_f;

                    if (attrNameVD == "r") vSlide *= 10.f;
                    else if (attrTypeVD == "float") vSlide *= .3f;
                    else if (attrTypeVD == "int") vSlide *= 5.f;
                    else if (attrNameVD == "t" || attrNameVD == "pivot") vSlide *= .2f;

                    if (this == myWin.attrTable)
                    {
                        if (attrTypeVD == "int") vSlideVal = glm::clamp(j->val_i + vSlide * diff.x, j->min, j->max);
                        else if (attrTypeVD == "float") vSlideVal = glm::clamp(j->val_f + vSlide * diff.x, j->min, j->max);
                        else if (attrTypeVD == "vec2") vSlideVal = glm::clamp(j->val_2[singleItem->data(34).toInt()] + vSlide * diff.x, j->min, j->max);
                        else if (attrTypeVD == "vec3") vSlideVal = glm::clamp(j->val_3[singleItem->data(34).toInt()] + vSlide * diff.x, j->min, j->max);
                        else if (attrTypeVD == "vec4") vSlideVal = glm::clamp(j->val_4[singleItem->data(34).toInt()] + vSlide * diff.x, j->min, j->max);
                    }

                    else
                        vSlideVal = glm::clamp(item(singleItem->row(), 1)->text().toFloat() + vSlide * diff.x, j->min, j->max);

                    if (i == myWin.selB || this != myWin.attrTable)
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

                    writeActions(attrTypeVD, attrNameVD, i, j, singleItem->data(34).toInt(), vSlideVal, singleItem->text().toStdString());
                }
            }
        }
    }

    selObjs.clear();

    myWin.attrTable->writeAttrTgl = true;
}

void Attrs::dragUndo_prep()
{
    for (auto &i : selectedItems())
    {
        for (auto &j : multiVec)
        {
            if (i->data(33) == QString::fromStdString(j->name))
            {
                auto at = j->Clone();

                if (i->data(32) == "vec3")
                    at->idx_3 = i->data(34).toInt();

                undoDragAttrs.push_back(at);
            }
        }
    }
}
