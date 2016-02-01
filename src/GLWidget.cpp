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

#include "GLWidget.h"

GLWidget::GLWidget(MainWin &myWinTemp, QSplitter &mySplitVTemp, const QGLWidget *shareWidget, QWidget *parent) : QGLWidget(parent, shareWidget), myWin(myWinTemp), mySplitV(mySplitVTemp)
{
    altTgl = ctrlTgl = shiftTgl = lmbTgl = mmbTgl = rmbTgl = spaceTgl = colorPickTgl = false;
    wTgl = sTgl = aTgl = dTgl = rTgl = fTgl = vTgl = false;

    statsTgl = wireOverTgl = false;
    mpfTgl = gizSideTgl = gizSpaceTgl = true;
    rezGateTgl = rezGateTgl_sel = fboReady = false; //

    tick_new = tick_old = tick_diff = 0;

    upWorld = glm::vec3(0.f, 1.f, 0.f);

    QGLFormat format;
    format.setVersion(4, 5);
    format.setProfile(QGLFormat::CompatibilityProfile);
    format.setSwapInterval(0);
    setFormat(format);

    setMouseTracking(1);
    setAcceptDrops(1);

    upTimer = new QTimer(this);
    connect(upTimer, SIGNAL(timeout()), this, SLOT(update()));
    upTimer->setInterval(8); //update every 8ms - 120 fps
    upTimer->start();

    cursorTimer = new QTimer(this);
    connect(cursorTimer, SIGNAL(timeout()), this, SLOT(paintAndCursorDrawHideTimer1()));
    cursorTimer->setInterval(3500);

    auto *clearCanvas_s = new QShortcut(tr("CTRL+SHIFT+C"), this);
    connect(clearCanvas_s, SIGNAL(activated()), this, SLOT(clearCanvas()));

//    chronoPt0 = chrono0.now();
}

void GLWidget::enterEvent(QEvent *)
{
    setFocus();
    activateWindow();
}

void GLWidget::leaveEvent(QEvent *)
{
    altTgl = ctrlTgl = shiftTgl = false;
    clearFocus();
}

void GLWidget::dragEnterEvent(QDragEnterEvent *e)
{
    dragDrop = true;

    e->acceptProposedAction();
    emit changed(e->mimeData());
}

void GLWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    dragDrop = false;
    e->accept();
}

vector<string> mySplitD(string data, string token)
{
    vector<string> output;
    auto pos = string::npos;

    do
    {
        pos = data.find(token);
        output.push_back(data.substr(0, pos));

        if (string::npos != pos)
            data = data.substr(pos + token.size());

    } while (string::npos != pos);

    return output;
}

void GLWidget::dropEvent(QDropEvent *e)
{
    const auto *mimeData = e->mimeData();

    for (int i = 0; i < mimeData->urls().size() && i < 10; ++i)
    {
        auto path = mimeData->urls()[i].toString().toStdString();
        path.erase(0, 8);

        auto ext = myWin.stringSplit(path, ".")[1];

        if (ext == "obj" || ext == "OBJ")
            myWin.objAdd(path, "DRAGDROP");
    }

    dragDrop = false;

    e->acceptProposedAction();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Alt)
    {
        altTgl = true;
        disableSelRect = true;

        clearCursor(paintMode);
    }

    else if (e->key() == Qt::Key_Control)
    {
        ctrlTgl = true;

        //eraser entry point
    }

    else if (e->key() == Qt::Key_Shift) shiftTgl = true;

    else if (e->key() == Qt::Key_Plus) myWin.gizScale += .5f;
    else if (e->key() == Qt::Key_Minus) myWin.gizScale -= .5f;

    //A-Z
    else if (e->key() == Qt::Key_A) aTgl = true;
    else if (e->key() == Qt::Key_B) bTgl = true;
    else if (e->key() == Qt::Key_C) cTgl = true;
    else if (e->key() == Qt::Key_D) dTgl = true;
    else if (e->key() == Qt::Key_E) eTgl = true;
    else if (e->key() == Qt::Key_F) fTgl = true;
    else if (e->key() == Qt::Key_G) gTgl = true;
    else if (e->key() == Qt::Key_H) hTgl = true;

    else if (e->key() == Qt::Key_I)
    {
        iTgl = true;
        myWin.PaintWinTgl(1, 0);
    }

    else if (e->key() == Qt::Key_J) jTgl = true;

    else if (e->key() == Qt::Key_K)
    {
        kTgl = true;
        myWin.PaintWinTgl(1, 1);
    }

    else if (e->key() == Qt::Key_L) lTgl = true;
    else if (e->key() == Qt::Key_M) mTgl = true;
    else if (e->key() == Qt::Key_N) nTgl = true;
    else if (e->key() == Qt::Key_O) oTgl = true;
    else if (e->key() == Qt::Key_P) pTgl = true;
    else if (e->key() == Qt::Key_Q) qTgl = true;
    else if (e->key() == Qt::Key_R) rTgl = true;
    else if (e->key() == Qt::Key_S) sTgl = true;
    else if (e->key() == Qt::Key_T) tTgl = true;
    else if (e->key() == Qt::Key_U) uTgl = true;
    else if (e->key() == Qt::Key_V) vTgl = true;
    else if (e->key() == Qt::Key_W) wTgl = true;
    else if (e->key() == Qt::Key_X) xTgl = true;
    else if (e->key() == Qt::Key_Y) yTgl = true;
    else if (e->key() == Qt::Key_Z) zTgl = true;
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    if (paintMode)
    {
        //placing this BEFORE e->isAutoRepeat lets it exhibit both keyRelease AND keyPress behavior
        auto opacIncr = .005f; // - =
        auto scaleIncr = .04f; // [ ]

        if (ctrlTgl) //eraser
        {
            if (e->key() == Qt::Key_Minus)
            {
                myWin.myGLWidgetSh->selEraser->opac = glm::clamp(myWin.myGLWidgetSh->selEraser->opac - opacIncr, 0.f, 1.f);

                if (myWin.myGLWidgetSh->selEraser->opac < .001f)
                    myWin.myGLWidgetSh->selEraser->opac = 0.f;
            }

            else if (e->key() == Qt::Key_Equal)
                myWin.myGLWidgetSh->selEraser->opac = glm::clamp(myWin.myGLWidgetSh->selEraser->opac + opacIncr, 0.f, 1.f);

            //
            else if (e->key() == Qt::Key_BracketLeft)
                myWin.myGLWidgetSh->selEraser->scale = glm::max(glm::vec3(0.f), myWin.myGLWidgetSh->selEraser->scale - glm::vec3(scaleIncr));

            else if (e->key() == Qt::Key_BracketRight)
                myWin.myGLWidgetSh->selEraser->scale = myWin.myGLWidgetSh->selEraser->scale + scaleIncr;
        }

        else //brush
        {
            if (e->key() == Qt::Key_Minus)
            {
                myWin.myGLWidgetSh->selBrush->opac = glm::clamp(myWin.myGLWidgetSh->selBrush->opac - opacIncr, 0.f, 1.f);

                if (myWin.myGLWidgetSh->selBrush->opac < .001f)
                    myWin.myGLWidgetSh->selBrush->opac = 0.f;
            }

            else if (e->key() == Qt::Key_Equal)
                myWin.myGLWidgetSh->selBrush->opac = glm::clamp(myWin.myGLWidgetSh->selBrush->opac + opacIncr, 0.f, 1.f);

            //
            else if (e->key() == Qt::Key_BracketLeft)
                myWin.myGLWidgetSh->selBrush->scale = glm::max(glm::vec3(0.f), myWin.myGLWidgetSh->selBrush->scale - glm::vec3(scaleIncr));

            else if (e->key() == Qt::Key_BracketRight)
                myWin.myGLWidgetSh->selBrush->scale = myWin.myGLWidgetSh->selBrush->scale + scaleIncr;

//            cout << "opac = " << myWin.myGLWidgetSh->selBrush->opac << endl;
        }
    }

    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_1) myWin.selMode = "VERT";
    else if (e->key() == Qt::Key_2) myWin.selMode = "EDGE";
    else if (e->key() == Qt::Key_3) myWin.selMode = "FACE";
    else if (e->key() == Qt::Key_4) myWin.selMode = "OBJ";
    else if (e->key() == Qt::Key_5) wireOverTgl_swap();

//    else if (e->key() == Qt::Key_ScrollLock)
//    {
//        cout << "media PREV" << endl;
//    }

//    else if (e->key() == Qt::Key_Pause)
//    {
//        cout << "media NEXT" << endl;
//    }

//    else if (altTgl && e->key() == Qt::Key_Pause)
//    {
//        cout << "media PAUSE" << endl;
//    }

    else if (e->key() == Qt::Key_F3)
    {
        debug0 = !debug0;
        myWin.attrTable->refreshTable();

        cout << "debug0 : " << debug0 << endl;
    }

    else if (e->key() == Qt::Key_F5)
    {
//        myWin.setLightsDirty();

//        auto pt0 = glm::vec2(0.f, 0.f);
//        auto pt1 = glm::vec2(410.f, 278.f);
//        vector<glm::vec2> myBresenham = bresenham(pt0, pt1);

//        for (auto &i : myBresenham)
//        {
//            cout << "i = " << glm::to_string(i) << endl;
//        }
    }

    else if (e->key() == Qt::Key_F6)
    {
        myWin.doCubeMap = true;
    }

    else if (e->key() == Qt::Key_QuoteLeft)
    {
        if (myWin.myPrefWin->isVisible())
            myWin.myPrefWin->hide();

        else
        {
            auto prefP = myWin.toVec2(QCursor::pos());
            glm::vec2 showPrefP(prefP.x - (myWin.myPrefWin->width() / 2), prefP.y - (myWin.myPrefWin->height() / 2));
            myWin.myPrefWin->move(myWin.toQP(showPrefP));
            myWin.myPrefWin->show();
        }
    }

    else if (e->key() == Qt::Key_Question)
    {
        //cycle cams
        if (selCamLi->camLiType->val_s == "FPS")
            selCamLi->camLiType->val_s = "PERSP";

        else if (selCamLi->camLiType->val_s == "PERSP")
            selCamLi->camLiType->val_s = "ORTHO";

        else if (selCamLi->camLiType->val_s == "ORTHO")
            selCamLi->camLiType->val_s = "FPS";

        //cycle lights (point / spot only)
        else if (selCamLi->camLiType->val_s == "POINT")
            selCamLi->camLiType->val_s = "SPOT";

        else if (selCamLi->camLiType->val_s == "SPOT")
            selCamLi->camLiType->val_s = "POINT";

        changeCamLiType_();
        myWin.attrTable->refreshTable();
    }

    else if (e->key() == Qt::Key_Alt)
    {
        altTgl = false;

        if (selCamLi->camLiType->val_s != "FPS")
        {
            if (paintMode)
                setCursor(Qt::BlankCursor);

            else
                setCursor(Qt::ArrowCursor);
        }

        if (!lmbTgl)
            disableSelRect = false;
    }

    else if (e->key() == Qt::Key_Control)
    {
        ctrlTgl = false;

        clearCursor(paintMode);

//        setCursor(Qt::BlankCursor);
//        setCursor(Qt::ArrowCursor);
    }

    else if (e->key() == Qt::Key_Escape)
    {
        altTgl = false;
        disableSelRect = true;
    }

    else if (e->key() == Qt::Key_Shift) shiftTgl = false;

    else if (e->key() == Qt::Key_Space)
    {
        spaceTgl = false;
        jumpSwitch();
    }

    // A-Z
    else if (e->key() == Qt::Key_A) aTgl = false;

    else if (e->key() == Qt::Key_B)
    {
        bTgl = false;
        bakeNow = true;
    }

    else if (e->key() == Qt::Key_C) cTgl = false;

    else if (e->key() == Qt::Key_D)
    {
        dTgl = false;

        if (selCamLi->camLiType->val_s != "FPS" && !ctrlTgl)
            myWin.pivTgl();
    }

    else if (e->key() == Qt::Key_E)
    {
        eTgl = false;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.gizShow("R");
    }

    else if (e->key() == Qt::Key_F)
    {
        fTgl = false;

        if (selCamLi->camLiType->val_s != "FPS" && myWin.selB)
        {
            auto sizeByM = myWin.selB->MM * glm::vec4(myWin.selB->bbSize, 0.f);
            glm::vec3 size(sizeByM.x, sizeByM.y, sizeByM.z);
            auto dist = glm::sqrt(glm::dot(size, size)) / glm::sin(glm::radians(selCamLi->fov->val_f * .5f));

            selCamLi->setTarg(myWin.selB->piv->val_3, dist * .5f);
        }
    }

    else if (e->key() == Qt::Key_G)
    {
        gTgl = false;

        selCamLi->gridV = !selCamLi->gridV;
    }

    else if (e->key() == Qt::Key_H) hTgl = false;

    else if (e->key() == Qt::Key_I)
    {
        iTgl = false;

        if (myWin.myPaintWin->stackedMain->currentIndex() == 0)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_J) jTgl = false;

    else if (e->key() == Qt::Key_K)
    {
        kTgl = false;

        if (myWin.myPaintWin->stackedMain->currentIndex() == 1)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_L) lTgl = false;
    else if (e->key() == Qt::Key_M) mTgl = false;

    else if (e->key() == Qt::Key_N)
    {
        nTgl = false;
    }

    else if (e->key() == Qt::Key_O) oTgl = false;

    else if (e->key() == Qt::Key_P)
    {
        pTgl = false;

        if (!ctrlTgl)
        {
            if (paintMode)
            {
                clearCursor(paintMode);
                paintMode = false;
                setCursor(Qt::ArrowCursor);
            }

            else
            {
                paintMode = true;
                setCursor(Qt::BlankCursor);
            }
        }
    }

    else if (e->key() == Qt::Key_Q)
    {
        qTgl = false;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.myGizNull->v->val_b = false;

        clearCursor(paintMode);
        paintMode = false;
        setCursor(Qt::ArrowCursor);
    }

    else if (e->key() == Qt::Key_R)
    {
        rTgl = false;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.gizShow("S");
    }

    else if (e->key() == Qt::Key_S) sTgl = false;
    else if (e->key() == Qt::Key_T) tTgl = false;
    else if (e->key() == Qt::Key_U) uTgl = false;
    else if (e->key() == Qt::Key_V) vTgl = false;

    else if (e->key() == Qt::Key_W)
    {
        wTgl = false;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.gizShow("T");
    }

    else if (e->key() == Qt::Key_X) xTgl = false;
    else if (e->key() == Qt::Key_Y) yTgl = false;
    else if (e->key() == Qt::Key_Z) zTgl = false;
}

bool GLWidget::jumpSwitch()
{
    cout << "in jumpSwitch()" << endl;

    //JUMP TO OTHER SELECTED CAMLI OR CHANGE LAYOUT
    for (auto &i : myWin.allObj)
    {
        if (i->type == "CAMLI" && i->selected && i != selCamLi)
        {
            if (i->camLiType->val_s == "DIR") //switch to persp since cant look through it like spot
            {
                for (auto &j : myWin.allObj)
                {
                    if (j->name->val_s == "persp")
                    {
                        if (selCamLi->camLiTypeGet("cam")) //already looking thru a cam, desel
                            return false;

                        else
                        {
                            j->camLiType->val_s = "PERSP"; //reset to persp if something else
                            j->nearClip->val_f = .01f;
                            j->farClip->val_f = 100.f;
                            selCamLi = j;
                        }
                    }
                }
            }

            else
                selCamLi = i;

            resizeGL(width(), height());

            for (auto &j : myWin.allCamCombo)
                j->refresh();

            return true;
        }
    }

    switchGL_layout();

    return true;
}

void GLWidget::tabletEvent(QTabletEvent *e)
{
    if (e->pointerType() == QTabletEvent::Eraser)
        penOrientation = "ERASER";

    else if (e->pointerType() == QTabletEvent::Pen)
        penOrientation = "PEN";

    tabletPressure = e->pressure();
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = true; mmbTgl = false; rmbTgl = false;

        if (!altTgl)
        {
            rayP = myWin.toVec2(e->pos()); //

            if (paintMode)
                disableSelRect = true;

            else
                disableSelRect = false;

            if (selCamLi->camLiType->val_s != "FPS")
            {
                if (paintMode)
                    setCursor(Qt::BlankCursor);

                else
                    setCursor(Qt::ArrowCursor); //
            }
        }

        if (!selRectPts_color.empty())
            selRectPts_color.clear();
    }

    else if (e->button() == Qt::MiddleButton)
    {
        lmbTgl = false; mmbTgl = true; rmbTgl = false;

        if (!altTgl)
            gizTransType = myWin.myGizmo->hover(activeGL);

        clearCursor(paintMode);
    }

    else if (e->button() == Qt::RightButton)
    {
        lmbTgl = false; mmbTgl = false; rmbTgl = true;

        if (selCamLi->camLiType->val_s != "FPS")
        {
            if (!altTgl && !ctrlTgl && shiftTgl)
                radPop_GL("addObj");

            if (!altTgl && !ctrlTgl && !shiftTgl)
                radPop_GL("viewChange");
        }

        clearCursor(paintMode);
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = false;

        if (!altTgl && !paintMode)
        {
            //SELOBJ RAYCAST
            singleShotRC = false;

            if (selRectPts_color.empty())
            {
                singleShotRC = true;
                selRectPts_color.push_back(rayP);
            }

            if (!ctrlTgl && !shiftTgl)
                myWin.clearSel(); //

            checkForHits();
        }

        else if (!altTgl && paintMode && penOrientation == "ERASER")
        {
            firstPress = 1;
        }

        else if (!altTgl && paintMode && penOrientation == "PEN")
        {
            //CLEAR BG1
            glBindFramebuffer(GL_FRAMEBUFFER, brushBGN.fbo1);
            glViewport(0, 0, brushBGN.width, brushBGN.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.f, 0.f, 0.f, 0.f);

            //COPY PAINT DRAG TO BG2
            glDisable(GL_BLEND);
            glBindFramebuffer(GL_FRAMEBUFFER, brushBGN.fbo2);
            glViewport(0, 0, brushBGN.width, brushBGN.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.f, 0.f, 0.f, 0.f);

            myWin.myGLWidgetSh->glUseProgram2("pCopyTex");

            copyTgt = 5; //brushN2
            myWin.myFSQ->render(activeGL);

//            cout << "resetting debug paint settings 0" << endl;
            firstPress = 1;
        }

        else
            myWin.attrTable->refreshTable();
    }

    else if (e->button() == Qt::MiddleButton)
    {
        mmbTgl = false;
        gizTransType = "NONE";
        myWin.attrTable->refreshTable();
    }

    else if (e->button() == Qt::RightButton)
    {
        rmbTgl = false;
        myWin.attrTable->refreshTable();
    }

    if (selCamLi->camLiType->val_s != "FPS")
    {
        if (paintMode)
            setCursor(Qt::BlankCursor);

        else
            setCursor(Qt::ArrowCursor);
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    pMouseOld = pMouseNew;
    pMouseNew = myWin.toVec2(e->pos());
    pMouseDiff =  pMouseNew - pMouseOld;

    //store pMouseNew into pMouseNew_stored
    //each frame check if the the above (pMouseNew) == pMouseNew_stored
    //if they ARE equal, that means the mouse hasn't moved any and the cursor should be hidden
//    pMouseNew_stored = pMouseNew;
//    pMouseNew_stored = 0;

    if (myWin.myGizNull->parentTo)
    {
        if (gizTransType == "NONE")
        {
            gizHoverType = myWin.myGizmo->hover(activeGL);
            myWin.myGizmo->resetSetCol(gizHoverType);
        }

        else
            myWin.myGizmo->transform(activeGL);
    }

    if (paintMode)
    {
        if (paintCursorResizeTgl)
        {
            auto paintCursorScaleSpeed = .005f;

            myWin.myGLWidgetSh->selBrush->scale = clamp(myWin.myGLWidgetSh->selBrush->scale + pMouseDiff.x * paintCursorScaleSpeed, glm::vec3(0.25f), glm::vec3(1.f));
        }
    }

    if (selCamLi->camLiType->val_s != "FPS")
    {
        if (lmbTgl && altTgl)
        {
            setCursor(Qt::SizeAllCursor);

            if (!(selCamLi->camLiType->val_s == "ORTHO" && !selCamLi->orthoFree->val_b))
            {
                selCamLi->r->val_3.x -= myWin.etcTable->tumbleSpeed->val_f * pMouseDiff.x;
                selCamLi->r->val_3.y = glm::clamp(selCamLi->r->val_3.y - myWin.etcTable->tumbleSpeed->val_f * pMouseDiff.y, -89.99f, 89.99f); //no flip
            }

            updateCamAttrs("rotate");
        }

        else if (mmbTgl && ctrlTgl && shiftTgl)
        {
            selCamLi->r->val_3.z += (float)pMouseDiff.x;
            updateCamAttrs("rotate");
        }

        else if ((mmbTgl && !myWin.myGizNull->v->val_b) || (mmbTgl && altTgl))
        {
            setCursor(Qt::ClosedHandCursor);

            if (selCamLi->camLiTypeGet("dirOrtho"))
            {
                selCamLi->targO -= selCamLi->rightO * (float)pMouseDiff.x * myWin.etcTable->panSpeed_ortho->val_f;
                selCamLi->targO += selCamLi->upO * (float)pMouseDiff.y * myWin.etcTable->panSpeed_ortho->val_f;
            }

            else
            {
                selCamLi->targO -= selCamLi->rightO * (float)pMouseDiff.x * myWin.etcTable->panSpeed->val_f;
                selCamLi->targO += selCamLi->upO * (float)pMouseDiff.y * myWin.etcTable->panSpeed->val_f;
            }

            updateCamAttrs("translate");
        }

        else if (rmbTgl && altTgl)
        {
            setCursor(Qt::SizeVerCursor);

            if (selCamLi->camLiTypeGet("dirOrtho"))
            {
                selCamLi->orthoZoom->val_f = glm::max(.001f, selCamLi->orthoZoom->val_f - pMouseDiff.x * myWin.etcTable->dollySpeed_ortho->val_f); //no flip

                PMupOrtho();
            }

            else
                selCamLi->distO = glm::max(.01f, selCamLi->distO - pMouseDiff.x * myWin.etcTable->dollySpeed->val_f); //no flip

            updateCamAttrs("zoom");
        }
    }

    else
    {
        auto p = myWin.toVec2(mapFromGlobal(QCursor::pos()));
        QCursor::setPos(mapToGlobal(myWin.toQP(glm::vec2(width() / 2, height() / 2))));

        selCamLi->r->val_3.x += float(width() / 2 - p.x) * myWin.etcTable->fpsMouseSpeed->val_f ;
        selCamLi->r->val_3.y -= float(height() / 2 - p.y) * myWin.etcTable->fpsMouseSpeed->val_f;

        selCamLi->setDirty(); //VM
    }
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    if (selCamLi->camLiType->val_s == "FPS")
        selCamLi->t->val_3 += selCamLi->lookFPS * (float)e->delta() * myWin.etcTable->fpsWheelSpeed->val_f;

    else if (selCamLi->camLiTypeGet("dirOrtho"))
    {
        selCamLi->orthoZoom->val_f = glm::max(.001f, selCamLi->orthoZoom->val_f - (float)e->delta() * myWin.etcTable->wheelSpeed_ortho->val_f); //no flip

        PMupOrtho();
    }

    else
        selCamLi->distO = glm::max(.01f, selCamLi->distO - (float)e->delta() * myWin.etcTable->wheelSpeed->val_f); //no flip

    updateCamAttrs("zoom");
}

void GLWidget::initializeGL()
{
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
        cout << "Error with glewInit : " << glewGetErrorString(glewInit()) << endl;
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    aspect = (0.f) ? 1.f : (float)w / h;
    aspectSide = (0.f) ? 1.f : myWin.glslTable->gizSideS->val_3.x / myWin.glslTable->gizSideS->val_3.y;

    if (selCamLi->camLiTypeGet("dirOrtho"))
        selCamLi->PM = glm::ortho(-selCamLi->orthoZoom->val_f * aspect, selCamLi->orthoZoom->val_f * aspect, -selCamLi->orthoZoom->val_f, selCamLi->orthoZoom->val_f, selCamLi->nearClip->val_f, selCamLi->farClip->val_f);

    else
        selCamLi->PM = glm::perspective(glm::radians(selCamLi->fov->val_f), aspect, selCamLi->nearClip->val_f, selCamLi->farClip->val_f);

    PMinv_cube = glm::inverse(glm::perspective(glm::radians(selCamLi->fov->val_f), aspect, selCamLi->nearClip->val_f, selCamLi->farClip->val_f));
    PMgizSide = glm::perspective(glm::radians(50.f), aspectSide, .001f, 1000.f);

    aspectXYZ.x = (aspect > 1.f) ? aspect : 1.f;
    aspectXYZ.y = (aspect > 1.f) ? 1.f : aspect;
    aspectXYZ.z = 1.f;

    if (fboReady)
        myWin.myPP->resizeTexClearMem(activeGL);

    selCamLi->setDirty();
}

void GLWidget::PMupOrtho()
{
    //refresh PM because of selCamLi orthoZoom change
    aspect = (float)width() / height();
    aspectSide = myWin.glslTable->gizSideS->val_3.x / myWin.glslTable->gizSideS->val_3.y;

    if (aspect == 0.f)
        aspect = 1.f;

    if (aspectSide == 0.f)
        aspectSide = 1.f;

    selCamLi->PM = glm::ortho(-selCamLi->orthoZoom->val_f * aspect, selCamLi->orthoZoom->val_f * aspect, -selCamLi->orthoZoom->val_f, selCamLi->orthoZoom->val_f, selCamLi->nearClip->val_f, selCamLi->farClip->val_f);
}

void GLWidget::VMup(shared_ptr<Object> obj)
{
    glm::mat4 R = glm::yawPitchRoll(glm::radians(obj->r->val_3.x), glm::radians(obj->r->val_3.y), 0.f);

    if (obj->camLiType->val_s == "FPS")
    {
        obj->piv->val_3 = obj->t->val_3;

        obj->lookFPS = glm::vec3(R * glm::vec4(0.f, 0.f, 1.f, 0.f));
        obj->upFPS = glm::vec3(R * glm::vec4(0.f, 1.f, 0.f, 0.f));
        obj->rightFPS = glm::cross(obj->lookFPS, obj->upFPS);

        obj->VM = glm::lookAt(obj->t->val_3, obj->t->val_3 + obj->lookFPS, obj->upFPS);
    }

    else if (obj->camLiType->val_s != "DIR")
    {
        obj->t->val_3 = glm::vec3(R * glm::vec4(0.f, 0.f, obj->distO, 0.f)) + obj->targO;
        obj->piv->val_3 = obj->t->val_3;

        obj->lookO = glm::normalize(obj->targO - obj->t->val_3);
        obj->rightO = glm::normalize(glm::cross(obj->lookO, upWorld));
        obj->upO = glm::cross(obj->rightO, obj->lookO);

        obj->VM = glm::lookAt(obj->t->val_3, obj->targO, obj->upO);
        obj->VM *= glm::rotate(glm::mat4(), glm::radians(obj->r->val_3.z), glm::vec3(0.f, 0.f, 1.f));
    }

    //update VMgizSide if this object is the selcam for a visible GL
    for (auto &i : myWin.allGL)
    {
        if (i->isVisible() && obj == i->selCamLi)
        {
            // UPDATE VM for GIZSIDE
            auto R = glm::yawPitchRoll(glm::radians(obj->r->val_3.x), glm::radians(obj->r->val_3.y), 0.f);
            auto posG = glm::vec3(R * glm::vec4(0.f, 0.f, myWin.glslTable->gizSideS->val_3.z, 0.f));
            auto lookG = glm::normalize(-posG);
            auto rightG = glm::cross(lookG, upWorld);
            auto upG = glm::cross(rightG, lookG);

            if (obj->camLiType->val_s == "FPS") posG *= -1.f;
            i->VMgizSide = glm::lookAt(posG, glm::vec3(0.f), upG);
        }
    }

    obj->dirtyVM = false;
    myWin.myGLWidgetSh->UBO_light_needsUp = true;

    if (myWin.myFSQ->clearBehav->val_s == "OnVMup")
    {
        cout << "baking (and clearing) canvas on VMup" << endl;

        bakeNow = true;
//        clearCanvas();
    }
}

void GLWidget::fpsCtrls()
{
    auto boost = (shiftTgl) ? 1.5f : 1.f;

    if (wTgl) selCamLi->t->val_3 += selCamLi->lookFPS * myWin.etcTable->fpsFBSpeed->val_f * deltaTime * boost;
    if (sTgl) selCamLi->t->val_3 -= selCamLi->lookFPS * myWin.etcTable->fpsFBSpeed->val_f * deltaTime * boost;
    if (aTgl) selCamLi->t->val_3 -= selCamLi->rightFPS * myWin.etcTable->fpsLRSpeed->val_f * deltaTime * boost;
    if (dTgl) selCamLi->t->val_3 += selCamLi->rightFPS * myWin.etcTable->fpsLRSpeed->val_f * deltaTime * boost;
    if (rTgl) selCamLi->t->val_3 += upWorld * myWin.etcTable->fpsUDSpeed->val_f * deltaTime * boost;
    if (fTgl) selCamLi->t->val_3 -= upWorld * myWin.etcTable->fpsUDSpeed->val_f * deltaTime * boost;

    if (wTgl || sTgl || aTgl || dTgl || rTgl || fTgl)
        selCamLi->setDirty(); //VM
}

bool GLWidget::gridMatch(shared_ptr<Object> obj)
{
    auto found = false;
    auto gridName = obj->name->val_s;
    auto orthoType = selCamLi->orthoType->val_s;

    if (selCamLi->camLiType->val_s != "ORTHO" || selCamLi->orthoFree->val_b)
    {
        if (gridName == "grid_top")
            found = 1;

        else if (gridName == "grid_topB")
            found = 1;
    }

    else
    {
        if ((gridName == "grid_front" && (orthoType == "BACK" || orthoType == "FRONT"))
                || (gridName == "grid_left" && (orthoType == "LEFT" || orthoType == "RIGHT"))
                || (gridName == "grid_top" && (orthoType == "BOTTOM" || orthoType == "TOP")))

            found = 1;

        else if (   (gridName == "grid_frontB" && (orthoType == "BACK" || orthoType == "FRONT"))
                    || (gridName == "grid_leftB" && (orthoType == "LEFT" || orthoType == "RIGHT"))
                    || (gridName == "grid_topB" && (orthoType == "BOTTOM" || orthoType == "TOP")) )

            found = 1;
    }

    return found;
}

void GLWidget::paintGL()
{
//    chronoPt1 = chrono0.now();
//    cout << "chrono diff = " << chrono::duration_cast<chrono::seconds>(chronoPt1 - chronoPt0).count() << endl;s
//    cout << "chrono diff = " << chrono::duration_cast<chrono::milliseconds>(chronoPt1 - chronoPt0).count() << endl;
//    cout << "chrono diff = " << chrono::duration_cast<chrono::microseconds>(chronoPt1 - chronoPt0).count() << endl;
//    cout << "chrono diff = " << chrono::duration_cast<chrono::nanoseconds>(chronoPt1 - chronoPt0).count() << endl;

    if (myWin.layerLay->getNewOrder)
    {
        myWin.layerLay->reorder("layer");
        myWin.layerLay->getNewOrder = false;
    }

    if (hasFocus() || myWin.ID_lastFocused == ID_GLWidget)
    {
        myWin.ID_lastFocused = ID_GLWidget;
        upTimer->setInterval(8);
    }

    else
        upTimer->setInterval(64);

    activeGL = getActiveGL();

    if (mpfTgl)
        mpfTimerStart();

    makeCurrent();

    tick_newFPS = GetTickCount();
    tick_diffFPS = tick_newFPS - tick_oldFPS;
    deltaTime = tick_diffFPS / 1000.f;

    rezGateTgl_sel = 0; //rez gate on selCamLi

    if (selCamLi->camLiType->val_s == "FPS")
        fpsCtrls();

    if (!fboReady)
        fboReady = myWin.myPP->fboPrep(activeGL);

    if (myWin.doCubeMap && myWin.selB) //CUBEMAPGEN
    {
        myWin.myGLWidgetSh->cubemapGen();
        myWin.doCubeMap = false;
    }

    for (auto &i : myWin.allObj)
    {
        auto type = i->type;
        auto name = i->name->val_s;

        if (myWin.searchUp(i))
        {
            if (type != "VOL" && !i->gizSideObj && name != "pivot")
            {
                i->mvpGet(activeGL);

//                if (name == "torus") //teapot
//                {
//                    i->r->val_3.y = myWin.myAnim->dynAnim("cycle") * 50.f;
//                    i->r->val_3.x = myWin.myAnim->dynAnim("cycle") * 200.f;
//                    i->t->val_3.y = myWin.myAnim->dynAnim("cycle");
//                    i->t->val_3.y = myWin.myAnim->dynAnim("cycle") * 3.f + 7.f;
//                    i->shadowCast->val_b = false;
//                }
            }
        }

        if (i->selected && i->name->val_s == selCamLi->name->val_s)
            rezGateTgl_sel = true;

        if (i->dupeStenFix != "999")
            dupeStenFix_check(i);

        i->loadVAO(activeGL);
    }

    myWin.myGLWidgetSh->glUseProgram2("pShadow");

    for (auto &i : myWin.allObj)
    {
        if (i->type == "CAMLI" && (i->v->val_b || i == selCamLi))
        {
            if (i->dirtyVM)
                VMup(i);

            if (i->dirtyShadow && i->camLiTypeGet("light"))
                myWin.myGLWidgetSh->writeShadow(i);
        }
    }

    if (myWin.myGLWidgetSh->UBO_light_needsUp) //
        myWin.myGLWidgetSh->UBO_update();

    glBindFramebuffer(GL_FRAMEBUFFER, bgN.fbo1);
    glViewport(0, 0, width(), height());
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    myWin.myGLWidgetSh->glUseProgram2("pSky");
    myWin.myFSQ->render(activeGL);
    glEnable(GL_DEPTH_TEST);

    if (selCamLi->gridV)
    {
        myWin.myGLWidgetSh->glUseProgram2("pGrid");

        for (auto &i : myWin.allObj)
        {
            if (i->type == "GRID" && gridMatch(i))
                i->render(activeGL);
        }
    }

    myWin.myGLWidgetSh->glUseProgram2("pGiz");
    if (gizSideTgl)
    {
        glViewport(0, 0, myWin.glslTable->gizSideS->val_3.x, myWin.glslTable->gizSideS->val_3.y);

        for (auto &i : myWin.allGizSide)
        {
            i->mvpGet(activeGL);
            i->render(activeGL);
        }

        glViewport(0, 0, width(), height());
    }

    if (myWin.myPivot->v->val_b)
    {
        myWin.myPivot->mvpGet(activeGL);
        myWin.myPivot->render(activeGL);
    }

    for (auto &i : myWin.allObj)
    {
        auto type = i->type;

        if (type == "CAMLI" && i->selected && myWin.searchUp(i))
        {
            if (!i->bb->val_b && myWin.selMode == "OBJ")
            {
                //wireframe already, change color only
                auto Cgiz_temp = i->Cgiz;
                i->Cgiz = myWin.glslTable->Csel->val_3;
                i->render(activeGL);
                i->Cgiz = Cgiz_temp;
            }
        }

        else if (type == "CAMLI" || type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_LINE")
        {
            if (!i->gizSideObj && myWin.searchUp(i))
            {
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                i->render(activeGL);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
            }
        }
    }

    if (myWin.myGizNull->v->val_b && myWin.myGizNull->gizType == "R")
    {
        myWin.myGLWidgetSh->glUseProgram2("pGiz_circ");

        for (auto &i : myWin.allObj)
        {
            if (i->type == "GIZ_CIRC" || i->type == "GIZ_CIRC_HALF")
            {
                glDisable(GL_DEPTH_TEST);
                i->render(activeGL);
                glEnable(GL_DEPTH_TEST);
            }
        }
    }

    myWin.myGLWidgetSh->glUseProgram2("pBB");
    for (auto &i : myWin.allObj)
    {
        if (i->bb->val_b && myWin.searchUp(i))
            i->render(activeGL);
    }

    for (auto &i : myWin.allObj)
    {
        if (i->type == "OBJ" && !i->bb->val_b && myWin.searchUp(i))
        {
            if (wireOverTgl)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                myWin.myGLWidgetSh->glUseProgram2("pWireframe");
                i->render(activeGL);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }

    for (auto &i : myWin.allObj)
    {
        if (i->selected && i->type == "OBJ" && !i->bb->val_b && myWin.searchUp(i))
        {
            if (myWin.selMode == "OBJ")
            {
                glClearStencil(0);
                glClear(GL_STENCIL_BUFFER_BIT);
                glEnable(GL_STENCIL_TEST);

                //RENDER MESH
                glStencilFunc(GL_ALWAYS, 1, 0xFF); // -1
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

                myWin.myGLWidgetSh->glUseProgram2("pStencilGeo");

                if (i->twoSided->val_s != "OFF")
                    glDisable(GL_CULL_FACE);

                i->render(activeGL);
                glEnable(GL_CULL_FACE);

                //THICK WIRE VERSION
                glStencilFunc(GL_NOTEQUAL, 1, -1);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glLineWidth(7.f);

                glPolygonMode(GL_FRONT, GL_LINE);
                myWin.myGLWidgetSh->glUseProgram2("pStencilHi");
                i->render(activeGL);
                glDisable(GL_STENCIL_TEST);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glLineWidth(1.f);
            }
        }
    }

    //REV DEPTH
    glBindFramebuffer(GL_FRAMEBUFFER, depthRevN.fbo1);
    glViewport(0, 0, depthRevN.width, depthRevN.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDisable(GL_BLEND);

    myWin.myGLWidgetSh->glUseProgram2("pDepthRev");

    for (auto &i : myWin.allObj)
    {
        if (i->type == "OBJ" && !i->bb->val_b && myWin.searchUp(i))
        {
            if (i->twoSided->val_s != "OFF")
                glDisable(GL_CULL_FACE);

            i->render(activeGL);
            glEnable(GL_CULL_FACE);
        }
    }

    /* G BUFFER */
    glBindFramebuffer(GL_FRAMEBUFFER, gBufN.fbo1);
    glViewport(0, 0, gBufN.width, gBufN.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);

    myWin.myGLWidgetSh->glUseProgram2("pGBuffer");

    for (auto &i : myWin.allObj)
    {
        if (i->type == "OBJ" && !i->bb->val_b && myWin.searchUp(i))
        {
            if (i->twoSided->val_s != "OFF")
                glDisable(GL_CULL_FACE);

            i->render(activeGL);
            glEnable(GL_CULL_FACE);
        }
    }

    if (myWin.myGLWidgetSh->brushOutlineUpB)
        brushOutlineUp();

    /* PAINT */
    prevCursorShape = cursor().shape();

    P_currF = myWin.toVec2(QCursor::pos());
    paintAndCursorDrawHideTimer0(); //cursor fade out after X seconds
    P_prevF = P_currF;

    /* COMP */
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    myWin.myPP->postFX(activeGL);
    overlay2D();

    tick_oldFPS = tick_newFPS;
}

void GLWidget::paintAndCursorDrawHideTimer0() //this is a REVERSE timer that counts UP
{
    if (P_currF == P_prevF && cursorTimer->remainingTime() == -1) //cursor in same spot and not currently updating
    {
        cursorTimer->start();
    }

    else if (P_currF != P_prevF) //moving
    {
        cursorTimer->stop();

        if (paintMode)
        {
            paintCursorResizeTgl = paintCursorResize_request();

            if (paintCursorResizeTgl || (!altTgl && !mmbTgl && !rmbTgl))
                paintSomething0(); //outputs to brushN2.rgb
        }

        else
        {
            if (prevCursorShape == 10) // Qt::BlankCursor
                setCursor(Qt::ArrowCursor);
        }
    }
}

void GLWidget::paintAndCursorDrawHideTimer1()
{
//    cout << "static for 2 sec from cursorTimer" << endl;

    if (paintMode)
        clearCursor(paintMode); // TODO: fade out as well

    else
        setCursor(Qt::BlankCursor);
}

bool GLWidget::paintCursorResize_request()
{
    if (rmbTgl && ctrlTgl && shiftTgl)
    {
        if (paintCursorResizeTgl == 0) //store inital P
            paintCursorResize_p = toNDC(pMouseOld, "SELRECT");

        return true;
    }

    else
        return false;
}

void GLWidget::blendModeDebug(string type)
{
    //two squares, overlapping each other at upper right / lower left

    glEnable(GL_BLEND);
//    glDisable(GL_CULL_FACE);
//    glDisable(GL_DEPTH_TEST);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //

    if (debug0)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    else
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    /* ...PS BLENDING MODES... */

    //NORMAL
    if (type == "normal")
        glBlendEquation(GL_FUNC_ADD);

    /* DARKEN */
    else if (type == "darken")
        glBlendEquation(GL_DARKEN_NV);

    else if (type == "multiply")
        glBlendEquation(GL_MULTIPLY_NV);

    else if (type == "color_burn")
        glBlendEquation(GL_COLORBURN_NV);

    else if (type == "linear_burn")
        glBlendEquation(GL_LINEARBURN_NV);

    /* LIGHTEN */
    else if (type == "lighten")
        glBlendEquation(GL_LIGHTEN_NV);

    else if (type == "screen")
        glBlendEquation(GL_SCREEN_NV);

    else if (type == "color_dodge")
        glBlendEquation(GL_COLORDODGE_NV);

    else if (type == "linear_dodge")
        glBlendEquation(GL_LINEARDODGE_NV);

    /* CONTRAST */
    else if (type == "overlay")
        glBlendEquation(GL_OVERLAY_NV);

    else if (type == "soft_light")
        glBlendEquation(GL_SOFTLIGHT_NV);

    else if (type == "hard_light")
        glBlendEquation(GL_HARDLIGHT_NV);

    else if (type == "vivid_light")
        glBlendEquation(GL_VIVIDLIGHT_NV);

    else if (type == "linear_light")
        glBlendEquation(GL_LINEARLIGHT_NV);

    else if (type == "pin_light")
        glBlendEquation(GL_PINLIGHT_NV);

    else if (type == "hard_mix")
        glBlendEquation(GL_HARDMIX_NV);

    /* INVERSION */
    else if (type == "difference")
        glBlendEquation(GL_DIFFERENCE_NV);

    else if (type == "exclusion")
        glBlendEquation(GL_EXCLUSION_NV);

    /* COMPONENT */
    else if (type == "hue")
        glBlendEquation(GL_HSL_HUE_NV);

    else if (type == "saturation")
        glBlendEquation(GL_HSL_SATURATION_NV);

    else if (type == "color")
        glBlendEquation(GL_HSL_COLOR_NV);

    else if (type == "luminosity")
        glBlendEquation(GL_HSL_LUMINOSITY_NV);

    glBindFramebuffer(GL_FRAMEBUFFER, brushN.fbo1);
    glViewport(0, 0, brushN.width, brushN.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    myWin.myGLWidgetSh->glUseProgram2("pBlendMode");

    blendModeD = 0;
    myWin.paintStroke->s->val_3 = glm::vec3(.5f);
    myWin.paintStroke->t->val_3 = glm::vec3(-.15f, .2f, 0.f);
    myWin.paintStroke->mvpGet(activeGL);
    myWin.paintStroke->render(activeGL);

    blendModeD = 1;
    myWin.paintStroke->s->val_3 = glm::vec3(.5f);
//    myWin.paintStroke->s->val_3 = glm::vec3(1.f, .5f, 0.f);
    myWin.paintStroke->t->val_3 = glm::vec3(.15f, -.2f, 0.f);
    myWin.paintStroke->mvpGet(activeGL);
    myWin.paintStroke->render(activeGL);

//    glDisable(GL_BLEND);
//    glEnable(GL_DEPTH_TEST);
    glBlendEquation(GL_FUNC_ADD);
}

void GLWidget::paintSomething0()
{
    glm::vec2 widthHeight(width(), height());

    auto pt0 = toNDC(pMouseOld, "SELRECT") * widthHeight; //-1..1
    auto pt1 = toNDC(pMouseNew, "SELRECT") * widthHeight; //-1..1

    strokes_cursor.clear();

    vector<glm::vec2> bresenham_cursor = bresenham(pt0, pt1, 500);
    auto distDrag = glm::distance(pt0, pt1);

    if (penOrientation == "PEN") //CURSOR - BRUSH
    {
        for (auto &i : bresenham_cursor)
            strokes_cursor.push_back( { i, myWin.myGLWidgetSh->selBrush->scale, myWin.myGLWidgetSh->selBrush->opac } );

        paintSomething1("BRUSH_CURSOR");

        if (lmbTgl) //STROKE - BRUSH
        {
            if (firstPress || distDrag > distDragPaint)
            {
                strokes_brush.clear();

//                vector<glm::vec2> bresenham_brush = bresenham(pt0, pt1, 1);
//                vector<glm::vec2> bresenham_brush = bresenham(pt0, pt1, 10);
                vector<glm::vec2> bresenham_brush = bresenham(pt0, pt1, 50);
//                vector<glm::vec2> bresenham_brush = bresenham(pt0, pt1, 100); //

                for (auto &i : bresenham_brush)
                    strokes_brush.push_back( { i, myWin.myGLWidgetSh->selBrush->scale, myWin.myGLWidgetSh->selBrush->opac } );

                paintSomething1("BRUSH");
                firstPress = 0;
            }
        }
    }

    else if (penOrientation == "ERASER") //CURSOR - ERASER
    {
        for (auto &i : bresenham_cursor)
            strokes_cursor.push_back( { i, myWin.myGLWidgetSh->selEraser->scale, myWin.myGLWidgetSh->selEraser->opac } );

        paintSomething1("ERASER_CURSOR");

        if (lmbTgl) //STROKE - ERASER
        {
            if (firstPress || distDrag > 10.f)
            {
                strokes_eraser.clear();

                vector<glm::vec2> bresenham_eraser = bresenham(pt0, pt1, 2);

                for (auto &i : bresenham_eraser)
                    strokes_eraser.push_back( { i, myWin.myGLWidgetSh->selEraser->scale, myWin.myGLWidgetSh->selEraser->opac } );

                paintSomething1("ERASER");
                firstPress = 0;
            }
        }
    }

    if (bakeNow && myWin.selB)
    {
        bakeSomething();

        if (myWin.myFSQ->clearBehav->val_s != "Manual")
            clearCanvas();
    }
}

void GLWidget::paintSomething1(string type)
{
    /* PAINT A STROKE */
    paintType = type;

    if (type == "BRUSH")
    {
        brushRGB = glm::vec3(1.f);
        brushA = myWin.myGLWidgetSh->selBrush->opac;
        brushRGBA = glm::vec4(brushRGB * glm::vec3(brushA), brushA);

        //***************************** STROKE *******************************
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBindFramebuffer(GL_FRAMEBUFFER, brushTempN.fbo1);
        glViewport(0, 0, brushTempN.width, brushTempN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pPaintStroke");

        for (auto &i : strokes_brush)
        {
            myWin.paintStroke->s->val_3 = i.scale * tabletPressure;
            myWin.paintStroke->t->val_3 = glm::vec3(i.coord, 0.f);
            myWin.paintStroke->mvpGet(activeGL);
            myWin.paintStroke->render(activeGL);
        }

        //***************************** BLEND BG1 AND STROKE *******************************
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        glBindFramebuffer(GL_FRAMEBUFFER, brushN.fbo1);
        glViewport(0, 0, brushN.width, brushN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pCopyTex");

        copyTgt = 2; //bgN.tex1
        myWin.myFSQ->render(activeGL);

        copyTgt = 0; //brushTempN1
        myWin.myFSQ->render(activeGL);

        //***************************** COPY TO BG1 (held stroke) *******************************
        glDisable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, brushBGN.fbo1);
        glViewport(0, 0, brushBGN.width, brushBGN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pCopyTex");
        copyTgt = 4; //brushN.tex1
        myWin.myFSQ->render(activeGL);

        //***************************** HARDWARE BLEND STROKE AND BG2 *******************************
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        glBindFramebuffer(GL_FRAMEBUFFER, brushN.fbo2);
        glViewport(0, 0, brushN.width, brushN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pCopyTex");

        copyTgt = 3; //bgN.tex2
        myWin.myFSQ->render(activeGL);

        copyTgt = 4; //brushN1
        myWin.myFSQ->render(activeGL);
    }

    else if (type == "ERASER")
    {
        brushRGBA = glm::vec4(1.f);

        //***************************** STROKE *******************************
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glBindFramebuffer(GL_FRAMEBUFFER, eraserN.fbo1);
        glViewport(0, 0, eraserN.width, eraserN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pPaintStroke");

        for (auto &i : strokes_eraser)
        {
            myWin.paintStroke->s->val_3 = i.scale * tabletPressure;
            myWin.paintStroke->t->val_3 = glm::vec3(i.coord, 0.f);
            myWin.paintStroke->mvpGet(activeGL);
            myWin.paintStroke->render(activeGL);
        }

        //ERASE
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, eraserN.fbo2);
        glViewport(0, 0, eraserN.width, eraserN.height);

        myWin.myGLWidgetSh->glUseProgram2("pEraseMix");
        myWin.myFSQ->render(activeGL);

        //COPY TO BG2
        glBindFramebuffer(GL_FRAMEBUFFER, brushBGN.fbo2);
        glViewport(0, 0, brushBGN.width, brushBGN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pCopyTex");

        copyTgt = 12; //eraserN.tex2
        myWin.myFSQ->render(activeGL);

        //COPY TO BRUSHN2
        glBindFramebuffer(GL_FRAMEBUFFER, brushN.fbo2);
        glViewport(0, 0, brushN.width, brushN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pCopyTex");

        copyTgt = 12; //eraserN.tex2
        myWin.myFSQ->render(activeGL);
    }

    else if (type == "BRUSH_CURSOR" || type == "ERASER_CURSOR")
    {
        brushRGB = glm::vec3(1.f);
        brushA = .2f;
        brushRGBA = glm::vec4(brushRGB * glm::vec3(brushA), brushA);

        glBindFramebuffer(GL_FRAMEBUFFER, cursorN.fbo1);
        glViewport(0, 0, cursorN.width, cursorN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        myWin.myGLWidgetSh->glUseProgram2("pPaintStroke");

        if (paintCursorResizeTgl)
        {
            myWin.paintStroke->s->val_3 = strokes_cursor[0].scale;
            myWin.paintStroke->t->val_3 = glm::vec3(paintCursorResize_p, 0.f);
            myWin.paintStroke->mvpGet(activeGL);
            myWin.paintStroke->render(activeGL);
        }

        else
        {
            for (auto &i : strokes_cursor)
            {
                myWin.paintStroke->s->val_3 = i.scale;
                myWin.paintStroke->t->val_3 = glm::vec3(i.coord, 0.f);
                myWin.paintStroke->mvpGet(activeGL);
                myWin.paintStroke->render(activeGL);
            }
        }
    }
}

void GLWidget::brushOutlineUp()
{
    //edge detect ALL brushes
    //upload sobel silh into brush.FBO2

    //take the alpha of a brush (always solid white) INTO ITS OWN TEXTURE !!!! - brushTemp1
    //gaussian blur THE ALPHA CHANNEL - alphaGauss2
    //sobel the blurred tex

    for (auto &i : myWin.myGLWidgetSh->allMaps)
    {
        if (i.type == "BRUSH")
        {
            sobelMap = i;

            //ALPHA INTO ITS OWN CHANNEL
            glBindFramebuffer(GL_FRAMEBUFFER, brushTempN.fbo1);
            glViewport(0, 0, brushTempN.width, brushTempN.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.f, 0.f, 0.f, 0.f);

            myWin.myGLWidgetSh->glUseProgram2("pAlphaAsRGBA");
            myWin.myFSQ->render(activeGL);

            //GAUSSIAN BLUR
            glMakeTextureHandleNonResidentARB(alphaGaussN.tex2_64);
            myWin.myGLWidgetSh->glUseProgram2("pGauss");
            //alphaGaussN.tex2_64 = glGetTextureHandleARB(myWin.myPP->gaussianBlur2(brushTempN.tex1_32, alphaGaussN, 0));
            alphaGaussN.tex2_64 = glGetTextureHandleARB(myWin.myPP->gaussianBlur2(brushTempN.tex1_32, alphaGaussN, 1));
            glMakeTextureHandleResidentARB(alphaGaussN.tex2_64);

            //EDGE DETECT
            glBindFramebuffer(GL_FRAMEBUFFER, i.layer[0].fbo2);
            glViewport(0, 0, i.layer[0].width, i.layer[0].height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.f, 0.f, 0.f, 0.f);

            myWin.myGLWidgetSh->glUseProgram2("pEdgeDetect");
//            edgeDetect_mode = 0; // sobel
            edgeDetect_mode = 1; // freiChen
            myWin.myFSQ->render(activeGL);
        }
    }

    myWin.myGLWidgetSh->brushOutlineUpB = false;
}

void GLWidget::clearCursor(bool inPaintMode)
{
    if (inPaintMode) // don't unneccesarily clear if not in paint mode
    {
        glBindFramebuffer(GL_FRAMEBUFFER, cursorN.fbo1);
        glViewport(0, 0, cursorN.width, cursorN.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);
    }
}

void GLWidget::clearCanvas()
{
    GLint resetCol[] = { 0, 0, 0, 0 };

    glClearNamedFramebufferiv(brushN.fbo2, GL_COLOR, 0, resetCol);
    glClearNamedFramebufferiv(brushBGN.fbo1, GL_COLOR, 0, resetCol);
    glClearNamedFramebufferiv(brushBGN.fbo2, GL_COLOR, 0, resetCol);
}

Map GLWidget::getCurrPaintLayer()
{
    for (auto &i : myWin.myGLWidgetSh->allMaps)
    {
        //match the type
        if (myWin.selB->texSel.type == i.type)
        {
            auto myType = myWin.selB->texSel.type;
            string myTypeVal;

            if (myType == "ALBEDO") myTypeVal = myWin.selB->albedoM->val_s;
            else if (myType == "ALPHA") myTypeVal = myWin.selB->alphaM->val_s;
            else if (myType == "ANISO") myTypeVal = myWin.selB->anisoM->val_s;
            else if (myType == "LENS") myTypeVal = myWin.selB->lensM->val_s;
            else if (myType == "METALLIC") myTypeVal = myWin.selB->metallicM->val_s;
            else if (myType == "RUFF") myTypeVal = myWin.selB->ruffM->val_s;
            else if (myType == "SSS") myTypeVal = myWin.selB->sssM->val_s;

            if (i.name == myTypeVal)
                return i;
        }
    }

    return { "", "", "", 0  };
}

void GLWidget::bakeSomething()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //
    glEnable(GL_BLEND);

    myLayerIdx = getCurrPaintLayer(); //get correct selLayer

    //1 - write current FBO1.selLayer to copyTex
    glBindFramebuffer(GL_FRAMEBUFFER, myLayerIdx.layer[0].fbo2);
    glViewport(0, 0, myLayerIdx.layer[0].width, myLayerIdx.layer[0].height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    myWin.myGLWidgetSh->glUseProgram2("pCopyTex");
    copyTgt = 10;
    myWin.myFSQ->render(activeGL);

    //2
    glBindFramebuffer(GL_FRAMEBUFFER, myLayerIdx.layer[0].fbo1);
    glViewport(0, 0, myLayerIdx.layer[0].width, myLayerIdx.layer[0].height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    myWin.myGLWidgetSh->glUseProgram2("pPaintProj");

    if (!myWin.selB->bb->val_b && myWin.searchUp(myWin.selB))
        myWin.selB->render(activeGL);

    //3 - REGEN MIPS
    glGenerateTextureMipmap(myLayerIdx.layer[0].tex1_32);

    bakeNow = false;
}

vector<glm::vec2> GLWidget::bresenham(glm::vec2 pt0, glm::vec2 pt1, int incr)
{
    vector<glm::vec2> myVec;
    glm::vec2 widthHeight(width(), height());

    bool steep = (fabs(pt1.y - pt0.y) > fabs(pt1.x - pt0.x));

    if (steep)
    {
        swap(pt0.x, pt0.y);
        swap(pt1.x, pt1.y);
    }

    if (pt0.x > pt1.x)
    {
        swap(pt0.x, pt1.x);
        swap(pt0.y, pt1.y);
    }

    auto dx = pt1.x - pt0.x;
    auto dy = fabs(pt1.y - pt0.y);
    auto error = dx / 2.f;

    auto ystep = (pt0.y < pt1.y) ? incr : -incr;
    int y = pt0.y;
    //    int maxX = pt1.x;
    int maxX = pt1.x + 1;

    for (int x = pt0.x; x < maxX; x += incr)
    {
        if (steep)
            myVec.push_back(glm::vec2(y, x) / widthHeight);

        else
            myVec.push_back(glm::vec2(x, y) / widthHeight);

        error -= dy;

        if (error < 0)
        {
            y += ystep;
            error += dx;
        }
    }

    return myVec; // CONSIDER DIVIDING BY WIDTH_HEIGHT HERE
}

void GLWidget::dupeStenFix_check(shared_ptr<Object> obj)
{
    for (auto &i : myWin.allObj)
    {
        if (obj->v->val_b && obj->dupeStenFix == i->name->val_s)
        {
            if (obj->MM == i->MM)
                i->v->val_b = false;

            else
            {
                obj->dupeStenFix = "999";
                i->v->val_b = true;
            }
        }
    }
}

void GLWidget::overlay2D()
{
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    if (rezGateTgl || rezGateTgl_sel) //REZGATE
    {
        glm::vec2 center(width() / 2, height() / 2);
        glm::vec2 xyHalf(myWin.glslTable->rezX->val_i / 2, myWin.glslTable->rezY->val_i / 2);

        rezGate_LD = toNDC(center - xyHalf, "REG");
        rezGate_RU = toNDC(center + xyHalf, "REG");
    }

    if (!mmbTgl & lmbTgl && !altTgl && !disableSelRect && selCamLi->camLiType->val_s != "FPS") //SELRECT
    {
        selRect_LD = toNDC(rayP, "SELRECT");
        selRect_RU = toNDC(pMouseNew, "SELRECT");

        myWin.myGLWidgetSh->glUseProgram2("pSelRect");

       for (auto &i : myWin.allObj)
       {
           if (i->type == "SELRECT")
               i->render(activeGL);
       }

        getPtsBetweenRect();
    }

    //TXT
    if (mpfTgl || gizSpaceTgl || statsTgl)
    {
        myWin.myGLWidgetSh->glUseProgram2("pTxt");

        if (mpfTgl)
        {
            myWin.myTxt->txt2D = mpf.c_str();

            for (auto &i : myWin.myGLWidgetSh->GLDataSh)
            {
                if (i.obj == myWin.myTxt)
                    glNamedBufferSubData(i.VBO_P, 0, (GLsizei)strlen(myWin.myTxt->txt2D), myWin.myTxt->txt2D);
            }

            myWin.myTxt->txtOrigin = glm::vec2(.8f, -.9f);
            myWin.myTxt->render(activeGL);
        }

        if (gizSpaceTgl) //gizSpace
        {
            if (myWin.gizSpace == "local") myWin.myTxt->txt2D = "l";
            else if (myWin.gizSpace == "world") myWin.myTxt->txt2D = "w";

            for (auto &i : myWin.myGLWidgetSh->GLDataSh)
            {
                if (i.obj == myWin.myTxt)
                    glNamedBufferSubData(i.VBO_P, 0, (GLsizei)strlen(myWin.myTxt->txt2D), myWin.myTxt->txt2D);
            }

            myWin.myTxt->txtOrigin = glm::vec2(.95f, -.9f);
            myWin.myTxt->render(activeGL);
        }

        if (statsTgl)
        {
            auto vertsOnObj = 0; auto edgesOnObj = 0; auto facesOnObj = 0;

            for (auto &i : myWin.allObj)
            {
                if (i->selected)
                {
                    vertsOnObj += i->vertsOnObj;
    //                edgesOnObj += i->edgesOnObj;
    //                facesOnObj += i->facesOnObj;
                }
            }

            stringstream ss;
            ss << "v " << vertsOnObj << " e " << edgesOnObj << " f " << facesOnObj;

            auto selStats = ss.str();
            myWin.myTxt->txt2D = selStats.c_str();

            for (auto &i : myWin.myGLWidgetSh->GLDataSh)
            {
                if (i.obj == myWin.myTxt)
                    glNamedBufferSubData(i.VBO_P, 0, (GLsizei)strlen(myWin.myTxt->txt2D), myWin.myTxt->txt2D);
            }

            myWin.myTxt->txtOrigin = glm::vec2(.5f, .85f);
            myWin.myTxt->render(activeGL);
        }
    }
}

void GLWidget::changeCamLiType_()
{
    cout << "in changeCamLiType_()" << endl;

    if (selCamLi->camLiType->val_s == "FPS")
    {
        setCursor(Qt::BlankCursor);

        selCamLi->nearClip->val_f = .01f;
        selCamLi->farClip->val_f = 100.f;
        selCamLi->t->val_3 = glm::vec3(5.f);
        selCamLi->lookFPS = glm::normalize(selCamLi->t->val_3);
        selCamLi->r->val_3.x = glm::degrees(float(atan2(selCamLi->lookFPS.z, selCamLi->lookFPS.x) + PI));
        selCamLi->r->val_3.y = glm::degrees(asin(selCamLi->lookFPS.y));

        QCursor::setPos(mapToGlobal(myWin.toQP(glm::vec2(width() / 2, height() / 2))));
    }

    else if (selCamLi->camLiTypeGet("dirOrtho"))
    {
        setCursor(Qt::ArrowCursor);

        selCamLi->orthoFree->val_b = true;
        selCamLi->nearClip->val_f = -1000.f;
        selCamLi->farClip->val_f = 1000.f;
        selCamLi->t->val_3 = glm::vec3(5.f);
        selCamLi->targO = glm::vec3(0.f);
        selCamLi->setTarg(selCamLi->targO, 0.f);
    }

    else //persp
    {
        setCursor(Qt::ArrowCursor);

        selCamLi->nearClip->val_f = .01f;
        selCamLi->farClip->val_f = 100.f;
    }

    resizeGL(width(), height());
}

void GLWidget::radPop_GL(string type)
{
    vector<PopSetup> pop;

    if (type == "addObj")
    {
        pop.push_back( { "cube", "RAD", 75, 50 } );
        pop.push_back( { "cyl", "RAD", 75, 50 } );
        pop.push_back( { "plane", "RAD", 75, 50 } );
        pop.push_back( { "sphere", "RAD", 75, 50 } );
        pop.push_back( { "teapot", "RAD", 75, 50 } );
        pop.push_back( { "torus", "RAD", 75, 50 } );

        pop.push_back( { "camLi", "W", 150, 65, -350, -300 } );
    }

    else if (type == "viewChange")
    {
        pop.push_back( { "top", "RAD", 75, 50 } );
        pop.push_back( { "front", "RAD", 75, 50 } );
        pop.push_back( { "left", "RAD", 75, 50 } );
        pop.push_back( { "persp", "RAD", 75, 50 } );

        //per-obj tgls
        pop.push_back( { "gizSpace", "S", 150, 50 } );
        pop.push_back( { "mpf", "S", 150, 50 } );
        pop.push_back( { "rez", "S", 150, 50 } );
        pop.push_back( { "stats", "S", 150, 50 } );

        //layouts
        pop.push_back( { "hLay", "W", 150, 65, -350, -300 } );
        pop.push_back( { "gridLay", "W", 150, 65, -350, -300 } );
        pop.push_back( { "vLay", "W", 150, 65, -350, -300 } );
    }

    myWin.myRadPop->popName = type;
    myWin.myRadPop->buttonInit(pop);
    myWin.myRadPop->startP = myWin.toVec2(QCursor::pos());
    myWin.myRadPop->newP = myWin.myRadPop->startP;

    myWin.myRadPop->myGL = activeGL;
    myWin.myRadPop->show();
}

void GLWidget::mpfTimerStart()
{
    //120fps = 8.333
    //60fps = 16.667
    //30fps = 33.333

    tick_frames++;
    tick_new = GetTickCount();
    tick_diff = tick_new - tick_old;

    if (tick_diff >= 1000)
    {
        stringstream ss;
        ss << fixed << setprecision(3) << 1000.f / tick_frames;
        mpf = ss.str();

        mpfTime = 1000.f / tick_frames;
        dTime = mpfTime * myWin.myFSQ->adaptTime->val_f * .01f;

        tick_frames = 0;
        tick_old = tick_new;
    }
}

void GLWidget::updateCamAttrs(string type)
{
    selCamLi->setDirty();

    if (myWin.selB && myWin.selB == selCamLi)
    {
        myWin.attrTable->writeAttrTgl = false; //dont refresh whole attr table, change indy cell text instead
        vector<DynSelCamAttrs> updateAttrs;

        if (type == "rotate")
        {
            updateAttrs.push_back( { "rX", selCamLi->r->val_3.x } );
            updateAttrs.push_back( { "rY", selCamLi->r->val_3.y } );
            updateAttrs.push_back( { "rZ", selCamLi->r->val_3.z } );

            updateAttrs.push_back( { "tX", selCamLi->t->val_3.x } );
            updateAttrs.push_back( { "tY", selCamLi->t->val_3.y } );
        }

        else if (type == "translate")
        {
            updateAttrs.push_back( { "tX", selCamLi->t->val_3.x } );
            updateAttrs.push_back( { "tY", selCamLi->t->val_3.y } );
        }

        else if (type == "zoom")
        {
            updateAttrs.push_back( { "orthoZoom", selCamLi->orthoZoom->val_f } );
            updateAttrs.push_back( { "tX", selCamLi->t->val_3.x } );
            updateAttrs.push_back( { "tY", selCamLi->t->val_3.y } );
            updateAttrs.push_back( { "tZ", selCamLi->t->val_3.z } );
        }

        for (auto &i : updateAttrs)
        {
            for (int j = 0; j < myWin.attrTable->findItems(QString::fromStdString(i.attr), Qt::MatchExactly).size(); ++j)
            {
                auto *findItem = myWin.attrTable->findItems(QString::fromStdString(i.attr), Qt::MatchExactly)[0];

                stringstream ss;
                ss << fixed << setprecision(3) << i.attrVal;

                myWin.attrTable->item(myWin.attrTable->row(findItem), 1)->setText(QString::fromStdString(ss.str()));
            }
        }

        myWin.attrTable->writeAttrTgl = true;
    }
}

void GLWidget::getPtsBetweenRect()
{
    if (!selRectPts_color.empty())
        selRectPts_color.clear();

    int bigX, smallX, bigY, smallY;

    bigX = glm::max(rayP.x, pMouseNew.x);
    smallX = min(rayP.x, pMouseNew.x);
    bigY = glm::max(rayP.y, pMouseNew.y);
    smallY = min(rayP.y, pMouseNew.y);

    if (bigY - smallY > bigX - smallX) //bigger y diff
    {
        for (int y = smallY; y < bigY; ++y)
        {
            for (int x = smallX; x < bigX; ++x)
            {
                selRectPts_color.push_back( { x, y } );

                continue;
            }
        }
    }

    else if (bigX - smallX > bigY - smallY) //bigger x diff
    {
        for (int x = smallX; x < bigX; ++x)
        {
            for (int y = smallY; y < bigY; ++y)
            {
                selRectPts_color.push_back( { x, y } );

                continue;
            }
        }
    }
}

bool GLWidget::checkForHits() //READPIXELS
{
    colorPickTgl = true;
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    myWin.myGLWidgetSh->glUseProgram2("pBB"); //gets Crand

    for (auto &i : myWin.allObj)
    {
        if ((i->type == "CAMLI" || i->type == "OBJ") && myWin.searchUp(i))
        {
            i->mvpGet(activeGL);
            i->render(activeGL);
        }
    }

    glDisable(GL_DEPTH_TEST);

    glFlush();
    glFinish();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    selRectPts_usable = selRectPts_color;
    auto foundNonOBJ = false;

    for (auto it = selRectPts_usable.begin(); it != selRectPts_usable.end();)
    {
        unsigned char data[4];
        glReadPixels((*it).x, height() - (*it).y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int pickedID = data[0] +  data[1] * 256 + data[2] * 256 * 256;

        if (pickedID != 0x00ffffff)
        {
            int r = (pickedID & 0x000000FF) >> 0;
            int g = (pickedID & 0x0000FF00) >> 8;
            int b = (pickedID & 0x00FF0000) >> 16;

            glm::vec3 compareID(r / 255.f, g / 255.f, b / 255.f);

            for (auto &i : myWin.allObj)
            {
                if (i->Crand == compareID)
                {
                    if (i->selectable)
                        matchFoundRaycast(i);
                }
            }
        }

        if (singleShotRC)
            ++it;

        else
        {
            if (it + myWin.etcTable->selAccuracy->val_i <= selRectPts_usable.end())
                advance(it, myWin.etcTable->selAccuracy->val_i);

            else
                ++it;
        }
    }

    //cleanup
    glEnable(GL_DEPTH_TEST);

    colorPickTgl = false;

    myWin.gizPivAutoShow(); //
    myWin.myOutliner->refreshOutliner(1);
    myWin.attrTable->refreshTable();

    return foundNonOBJ;
}

void GLWidget::matchFoundRaycast(shared_ptr<Object> obj)
{
    if (obj->selected && ctrlTgl)
        obj->selected = false;

    if (!ctrlTgl && myWin.searchUp(obj))
    {
        auto type = obj->type;

        obj->selected = true;
        myWin.selB = obj; //

        if (type == "GIZ_CIRC" || type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_LINE")
            obj->parentTo->parentTo->selected = 1; //keep obj selected w/ manips
    }
}

void GLWidget::getIndexIntoSplit()
{
    if (myWin.layoutType == "gridLay")
    {
        for (int i = 0; i < myWin.hLayU->count(); ++i)
        {
            if (&mySplitV == myWin.hLayU->widget(i))
            {
                splitIdx.split = myWin.hLayU;
                splitIdx.idx = i;

                break;
            }
        }

        for (int i = 0; i < myWin.hLayD->count(); ++i)
        {
            if (myWin.hLayD->widget(i) == &mySplitV)
            {
                splitIdx.split = myWin.hLayD;
                splitIdx.idx = i;

                break;
            }
        }
    }

    else if (myWin.layoutType == "hLay")
    {
        for (int i = 0; i < myWin.hLayU->count(); ++i)
        {
            if (myWin.hLayU->widget(i) == &mySplitV)
            {
                splitIdx.idx = i;

                break;
            }
        }
    }

    else if (myWin.layoutType == "vLay")
    {
        for (int i = 0; i < myWin.vLay->count(); ++i)
        {
            if (myWin.vLay->widget(i) == &mySplitV)
            {
                splitIdx.idx = i;

                break;
            }
        }
    }
}

void GLWidget::switchGL_layout()
{
    getIndexIntoSplit();

    if (myWin.stackedMain->currentIndex() == 1)
    {
        if (myWin.layoutType == "gridLay")
            splitIdx.split->insertWidget(splitIdx.idx, &mySplitV);

        else if (myWin.layoutType == "hLay")
            myWin.hLayU->insertWidget(splitIdx.idx, &mySplitV);

        else if (myWin.layoutType == "vLay")
            myWin.vLay->insertWidget(splitIdx.idx, &mySplitV);

        mySplitV.show();

        for (auto &i : myWin.allCamCombo)
        {
            if (i->myGL.get() == this)
                i->myGL->resize(0, 350);
        }

        myWin.stackedMain->insertWidget(1, myWin.blankW_main);
        myWin.stackedMain->setCurrentIndex(2);
        myWin.savedIdx = 2;
     }

    else //zoom
    {
        myWin.blankW_main->setParent(0);
        myWin.stackedMain->insertWidget(1, &mySplitV);
        myWin.stackedMain->setCurrentIndex(1);
        myWin.savedIdx = 1;
    }
}

glm::vec2 GLWidget::toNDC(glm::vec2 pt, string mode)
{
    if (mode == "SELRECT")
    {
        pt.x = (2.f * pt.x / width()) - 1.f;
        pt.y = (2.f * pt.y / height()) - 1.f;
        pt.y *= -1.f;
    }

    else
    {
        pt.x = pt.x / width();
        pt.y = pt.y / height();
    }

    return pt;
}

shared_ptr<GLWidget> GLWidget::getActiveGL()
{
    auto myActiveGL = myWin.allGL[0]; //dummy

    for (auto &i : myWin.allGL)
    {
        if (i.get() == this)
        {
            myActiveGL = i;

            break;
        }
    }

    return myActiveGL;
}
