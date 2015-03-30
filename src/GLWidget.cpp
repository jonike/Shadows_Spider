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

#include "GLWidget.h"

GLWidget::GLWidget(MainWin &myWinTemp, QSplitter &mySplitVTemp, const QGLWidget *shareWidget, QWidget *parent) : QGLWidget(parent, shareWidget), myWin(myWinTemp), mySplitV(mySplitVTemp)
{
    altTgl = ctrlTgl = shiftTgl = lmbTgl = mmbTgl = rmbTgl = spaceTgl = colorPickTgl = 0;
    wTgl = sTgl = aTgl = dTgl = rTgl = fTgl = vTgl = 0;

    rttVizTgl = 1;
    statsTgl = wireOverTgl = 0;
    mpfTgl = gizSideTgl = gizSpaceTgl = 1;
    rezGateTgl = rezGateTgl_sel = fboReady = 0; //

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
    upTimer->start(8); //update every 8ms - 120 fps
}

void GLWidget::enterEvent(QEvent *)
{
    setFocus();
    activateWindow();
}

void GLWidget::leaveEvent(QEvent *)
{
    altTgl = ctrlTgl = shiftTgl = 0;
    clearFocus();
}

void GLWidget::dragEnterEvent(QDragEnterEvent *e)
{
    dragDrop = 1;

    e->acceptProposedAction();
    emit changed(e->mimeData());
}

void GLWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    dragDrop = 0;
    e->accept();
}

void GLWidget::dropEvent(QDropEvent *e)
{
    const QMimeData *mimeData = e->mimeData();
    QList<QUrl> urlList = mimeData->urls();

    for (int i = 0; i < urlList.size() && i < 10; ++i)
    {
        QString path = urlList[i].toString().remove(0, 8);
        QString ext = path.split(".", QString::SkipEmptyParts).at(1);
        //qDebug() << "dropped an" << ext << "from" << path;

        if (ext == "obj")
            myWin.objAdd(path, "DRAGDROP");
    }

    dragDrop = 0;

    e->acceptProposedAction();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Alt)
    {
        altTgl = 1;
        disableSelRect = 1;
    }

    else if (e->key() == Qt::Key_Control) ctrlTgl = 1;
    else if (e->key() == Qt::Key_Shift) shiftTgl = 1;

    else if (e->key() == Qt::Key_Plus) myWin.gizScale += .5f;
    else if (e->key() == Qt::Key_Minus) myWin.gizScale -= .5f;

    //A-Z
    else if (e->key() == Qt::Key_A) aTgl = 1;
    else if (e->key() == Qt::Key_B) bTgl = 1;
    else if (e->key() == Qt::Key_C) cTgl = 1;
    else if (e->key() == Qt::Key_D) dTgl = 1;
    else if (e->key() == Qt::Key_E) eTgl = 1;
    else if (e->key() == Qt::Key_F) fTgl = 1;
    else if (e->key() == Qt::Key_G) gTgl = 1;
    else if (e->key() == Qt::Key_H) hTgl = 1;
    else if (e->key() == Qt::Key_I) iTgl = 1;
    else if (e->key() == Qt::Key_J) jTgl = 1;
    else if (e->key() == Qt::Key_K) kTgl = 1;
    else if (e->key() == Qt::Key_L) lTgl = 1;
    else if (e->key() == Qt::Key_M) mTgl = 1;
    else if (e->key() == Qt::Key_N) nTgl = 1;
    else if (e->key() == Qt::Key_O) oTgl = 1;
    else if (e->key() == Qt::Key_P) pTgl = 1;
    else if (e->key() == Qt::Key_Q) qTgl = 1;
    else if (e->key() == Qt::Key_R) rTgl = 1;
    else if (e->key() == Qt::Key_S) sTgl = 1;
    else if (e->key() == Qt::Key_T) tTgl = 1;
    else if (e->key() == Qt::Key_U) uTgl = 1;
    else if (e->key() == Qt::Key_V) vTgl = 1;
    else if (e->key() == Qt::Key_W) wTgl = 1;
    else if (e->key() == Qt::Key_X) xTgl = 1;
    else if (e->key() == Qt::Key_Y) yTgl = 1;
    else if (e->key() == Qt::Key_Z) zTgl = 1;
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    if (e->key() == Qt::Key_1) myWin.selMode = "VERT";
    else if (e->key() == Qt::Key_2) myWin.selMode = "EDGE";
    else if (e->key() == Qt::Key_3) myWin.selMode = "FACE";
    else if (e->key() == Qt::Key_4) myWin.selMode = "OBJ";
    else if (e->key() == Qt::Key_5) wireOverTgl_();

    else if (e->key() == Qt::Key_F3) rttVizTgl_();

    else if (e->key() == Qt::Key_F5)
    {
        qDebug() << "debugID = " << debugID;

        debugBool = !debugBool;
        qDebug() << "debugBool = " << debugBool;

//        qDebug() << "entering / exiting paint mode";
//        myWin.setLightsDirty();

    }

    else if (e->key() == Qt::Key_F6)
    {
        myWin.doCubeMap = 1;
    }

    else if (e->key() == Qt::Key_Backslash)
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
        altTgl = 0;

        if (selCamLi->camLiType->val_s != "FPS")
            setCursor(Qt::ArrowCursor);

        if (!lmbTgl)
            disableSelRect = 0;
    }

    else if (e->key() == Qt::Key_Control) ctrlTgl = 0;

    else if (e->key() == Qt::Key_Escape)
    {
        altTgl = 0;
        disableSelRect = 1;
    }

    else if (e->key() == Qt::Key_Shift) shiftTgl = 0;

    else if (e->key() == Qt::Key_Space)
    {
        spaceTgl = 0;
        jumpSwitch();
    }

    // A-Z
    else if (e->key() == Qt::Key_A) aTgl = 0;

    else if (e->key() == Qt::Key_B)
    {
        bTgl = 0;

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->selected)
            {
                myWin.allObj[i]->bb->val_b = !myWin.allObj[i]->bb->val_b;
                myWin.setLightsDirty();
            }
        }

        myWin.attrTable->refreshTable();
    }

    else if (e->key() == Qt::Key_C)
    {
        cTgl = 0;
        myWin.TglCPopWin();
    }

    else if (e->key() == Qt::Key_D)
    {
        dTgl = 0;

        if (selCamLi->camLiType->val_s != "FPS" && !ctrlTgl)
            myWin.pivTgl();
    }

    else if (e->key() == Qt::Key_E)
    {
        eTgl = 0;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.gizShow("R");
    }

    else if (e->key() == Qt::Key_F)
    {
        fTgl = 0;

        if (selCamLi->camLiType->val_s != "FPS" && myWin.selB)
        {
            glm::vec4 sizeByM = myWin.selB->MM * glm::vec4(myWin.selB->bbSize, 0.f);
            glm::vec3 size(sizeByM.x, sizeByM.y, sizeByM.z);
            float dist = glm::sqrt(glm::dot(size, size)) / glm::sin(glm::radians(selCamLi->fov->val_f * .5f));

            selCamLi->setTarg(myWin.selB->piv->val_3, dist * .5f);
        }
    }

    else if (e->key() == Qt::Key_G)
    {
        gTgl = 0;

        selCamLi->gridV = !selCamLi->gridV;
    }

    else if (e->key() == Qt::Key_H) hTgl = 0;
    else if (e->key() == Qt::Key_I) iTgl = 0;
    else if (e->key() == Qt::Key_J) jTgl = 0;
    else if (e->key() == Qt::Key_K) kTgl = 0;
    else if (e->key() == Qt::Key_L) lTgl = 0;
    else if (e->key() == Qt::Key_M) mTgl = 0;

    else if (e->key() == Qt::Key_N)
    {
        nTgl = 0;

        if (myWin.selB == 0)
        {
            for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
                myWin.allObj[i]->nType = 0;
        }

        else
        {
            for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
            {
                if (myWin.allObj[i]->selected)
                {
                    if (myWin.allObj[i]->nType == 0) myWin.allObj[i]->nType = 1; //off
                    else if (myWin.allObj[i]->nType == 1) myWin.allObj[i]->nType = 2; //f
                    else if (myWin.allObj[i]->nType == 2) myWin.allObj[i]->nType = 3; //v
                    else if (myWin.allObj[i]->nType == 3) myWin.allObj[i]->nType = 0; //both
                }
            }
        }
    }

    else if (e->key() == Qt::Key_O) oTgl = 0;
    else if (e->key() == Qt::Key_P) pTgl = 0;

    else if (e->key() == Qt::Key_Q)
    {
        qTgl = 0;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.myGizNull->v->val_b = 0;
    }

    else if (e->key() == Qt::Key_R)
    {
        rTgl = 0;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.gizShow("S");
    }

    else if (e->key() == Qt::Key_S) sTgl = 0;
    else if (e->key() == Qt::Key_T) tTgl = 0;
    else if (e->key() == Qt::Key_U) uTgl = 0;
    else if (e->key() == Qt::Key_V) vTgl = 0;

    else if (e->key() == Qt::Key_W)
    {
        wTgl = 0;

        if (selCamLi->camLiType->val_s != "FPS")
            myWin.gizShow("T");
    }

    else if (e->key() == Qt::Key_X) xTgl = 0;
    else if (e->key() == Qt::Key_Y) yTgl = 0;
    else if (e->key() == Qt::Key_Z) zTgl = 0;
}

bool GLWidget::jumpSwitch()
{
    //JUMP TO OTHER SELECTED CAMLI OR CHANGE LAYOUT
    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->type == "CAMLI" && myWin.allObj[i]->selected && myWin.allObj[i] != selCamLi)
        {
            if (myWin.allObj[i]->camLiType->val_s == "DIR") //switch to persp since cant look through it like spot
            {
                for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
                {
                    if (myWin.allObj[j]->name->val_s == "persp")
                    {
                        if (selCamLi->camLiTypeGet("cam")) //already looking thru a cam, desel
                            return 0;

                        else
                        {
                            myWin.allObj[j]->camLiType->val_s = "PERSP"; //reset to persp if something else
                            myWin.allObj[j]->nearClip->val_f = .001f;
                            myWin.allObj[j]->farClip->val_f = 1000.f;
                            selCamLi = myWin.allObj[j];
                        }
                    }
                }
            }

            else
                selCamLi = myWin.allObj[i];


            resizeGL(width(), height());
            selCamLi->setDirty();

            for (unsigned int j = 0; j < myWin.allCamCombo.size(); ++j)
                myWin.allCamCombo[j]->refresh();

            return 1;
        }
    }

    switchGL_layout();

    return 1;
}

void GLWidget::fogTglAll()
{
    myWin.fogTgl = !myWin.fogTgl;

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        myWin.allObj[i]->fogTgl = myWin.fogTgl;
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = 1; mmbTgl = 0; rmbTgl = 0;

        if (!altTgl)
        {
            rayP = myWin.toVec2(e->pos()); //
            disableSelRect = 0;

            if (selCamLi->camLiType->val_s != "FPS")
                setCursor(Qt::ArrowCursor);
        }

        if (!selRectPts_color.empty())
            selRectPts_color.clear();
    }

    else if (e->button() == Qt::MiddleButton)
    {
        lmbTgl = 0; mmbTgl = 1; rmbTgl = 0;

        if (!altTgl)
            gizTransType = myWin.myGizmo->hover(myWin.allGL[getGLidx()]);
    }

    else if (e->button() == Qt::RightButton)
    {
        lmbTgl = 0; mmbTgl = 0; rmbTgl = 1;

        if (selCamLi->camLiType->val_s != "FPS")
        {
            if (!altTgl && !ctrlTgl && shiftTgl)
                radPop_GL("addObj");

            if (!altTgl && !ctrlTgl && !shiftTgl)
                radPop_GL("viewChange");
        }
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = 0;

        if (!altTgl)
        {
            //SELOBJ RAYCAST
            singleShot = 0;

            if (selRectPts_color.empty())
            {
                singleShot = 1;
                selRectPts_color.push_back(myWin.toQP(rayP));
            }

            if (!ctrlTgl && !shiftTgl)
                myWin.clearSel(); //

            checkForHits();
        }

        else
            myWin.attrTable->refreshTable();
    }

    else if (e->button() == Qt::MiddleButton)
    {
        mmbTgl = 0;
        gizTransType = "NONE";
        myWin.attrTable->refreshTable();
    }

    else if (e->button() == Qt::RightButton)
    {
        rmbTgl = 0;
        myWin.attrTable->refreshTable();
    }

    if (selCamLi->camLiType->val_s != "FPS")
        setCursor(Qt::ArrowCursor);
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    pOld = pNew;
    pNew = myWin.toVec2(e->pos());
    pD =  pNew - pOld;

    if (myWin.myGizNull->parentTo)
    {
        if (gizTransType == "NONE")
        {
            gizHoverType = myWin.myGizmo->hover(myWin.allGL[GLidx]);
            myWin.myGizmo->resetSetCol(gizHoverType);
        }

        else
            myWin.myGizmo->transform(myWin.allGL[GLidx]);
    }

    if (selCamLi->camLiType->val_s != "FPS")
    {
        if (lmbTgl && altTgl)
        {
            setCursor(Qt::SizeAllCursor);

            if (!(selCamLi->camLiType->val_s == "ORTHO" && !selCamLi->orthoFree->val_b))
            {
                selCamLi->r->val_3.x -= myWin.etcTable->tumbleSpeed->val_f * pD.x;
                selCamLi->r->val_3.y = glm::clamp(selCamLi->r->val_3.y - myWin.etcTable->tumbleSpeed->val_f * pD.y, -89.99f, 89.99f); //no flip
            }

            updateCamAttrs("rotate");
        }

        else if (mmbTgl && (ctrlTgl && shiftTgl))
        {
            selCamLi->r->val_3.z += (float)pD.x;
            updateCamAttrs("rotate");
        }

        else if ((mmbTgl && !myWin.myGizNull->v->val_b) || (mmbTgl && altTgl))
        {
            setCursor(Qt::ClosedHandCursor);

            if (selCamLi->camLiTypeGet("dirOrtho"))
            {
                selCamLi->targO -= selCamLi->rightO * (float)pD.x * myWin.etcTable->panSpeed_ortho->val_f;
                selCamLi->targO += selCamLi->upO * (float)pD.y * myWin.etcTable->panSpeed_ortho->val_f;
            }

            else
            {
                selCamLi->targO -= selCamLi->rightO * (float)pD.x * myWin.etcTable->panSpeed->val_f;
                selCamLi->targO += selCamLi->upO * (float)pD.y * myWin.etcTable->panSpeed->val_f;
            }

            updateCamAttrs("translate");
        }

        else if (rmbTgl && altTgl)
        {
            setCursor(Qt::SizeVerCursor);

            if (selCamLi->camLiTypeGet("dirOrtho"))
            {
                selCamLi->orthoZoom->val_f = glm::max(.001f, selCamLi->orthoZoom->val_f - pD.x * myWin.etcTable->dollySpeed_ortho->val_f); //no flip

                PMupOrtho();
            }

            else
                selCamLi->distO = glm::max(.01f, selCamLi->distO - pD.x * myWin.etcTable->dollySpeed->val_f); //no flip

            updateCamAttrs("zoom");
        }
    }

    else
    {
        glm::vec2 p = myWin.toVec2(mapFromGlobal(QCursor::pos()));
        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

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
        qDebug("Error %s", glewGetErrorString(glewInit()));
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
    PMrtt = glm::ortho(-2.f * aspect, 2.f * aspect, -2.f, 2.f, -10000.f, 10000.f);

    if (fboReady)
        myWin.myPP->resizeTexClearMem(myWin.allGL[GLidx]);
}

void GLWidget::PMupOrtho()
{
    //refresh PM because of selCamLi orthoZoom change
    aspect = (float)width() / height();
    aspectSide = myWin.glslTable->gizSideS->val_3.x / myWin.glslTable->gizSideS->val_3.y;

    if (aspect == 0.f) { aspect = 1.f; }
    if (aspectSide == 0.f) { aspectSide = 1.f; }

    selCamLi->PM = glm::ortho(-selCamLi->orthoZoom->val_f * aspect, selCamLi->orthoZoom->val_f * aspect, -selCamLi->orthoZoom->val_f, selCamLi->orthoZoom->val_f, selCamLi->nearClip->val_f, selCamLi->farClip->val_f);
    PMrtt = glm::ortho(-2.f * aspect, 2.f * aspect, -2.f, 2.f, -10000.f, 10000.f);
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
    for (unsigned int i = 0; i < myWin.allGL.size(); ++i)
    {
        if (myWin.allGL[i]->isVisible() && obj == myWin.allGL[i]->selCamLi)
        {
            // UPDATE VM for GIZSIDE
            glm::mat4 R = glm::yawPitchRoll(glm::radians(obj->r->val_3.x), glm::radians(obj->r->val_3.y), 0.f);
            glm::vec3 posG = glm::vec3(R * glm::vec4(0.f, 0.f, myWin.glslTable->gizSideS->val_3.z, 0.f));
            glm::vec3 lookG = glm::normalize(-posG);
            glm::vec3 rightG = glm::cross(lookG, upWorld);
            glm::vec3 upG = glm::cross(rightG, lookG);

            if (obj->camLiType->val_s == "FPS") posG *= -1.f;
            myWin.allGL[i]->VMgizSide = glm::lookAt(posG, glm::vec3(0.f), upG);
        }
    }

    obj->dirtyVM = 0;
}

void GLWidget::fpsCtrls()
{
    float boost = (shiftTgl) ? 1.5f : 1.f;

    if (wTgl) selCamLi->t->val_3 += selCamLi->lookFPS * myWin.etcTable->fpsFBSpeed->val_f * deltaTime * boost;
    if (sTgl) selCamLi->t->val_3 -= selCamLi->lookFPS * myWin.etcTable->fpsFBSpeed->val_f * deltaTime * boost;
    if (aTgl) selCamLi->t->val_3 -= selCamLi->rightFPS * myWin.etcTable->fpsLRSpeed->val_f * deltaTime * boost;
    if (dTgl) selCamLi->t->val_3 += selCamLi->rightFPS * myWin.etcTable->fpsLRSpeed->val_f * deltaTime * boost;
    if (rTgl) selCamLi->t->val_3 += upWorld * myWin.etcTable->fpsUDSpeed->val_f * deltaTime * boost;
    if (fTgl) selCamLi->t->val_3 -= upWorld * myWin.etcTable->fpsUDSpeed->val_f * deltaTime * boost;

    if (wTgl || sTgl || aTgl || dTgl || rTgl || fTgl)
        selCamLi->setDirty(); //VM
}

bool GLWidget::gridMatch(int idx)
{
    bool found = 0;

    QString gridName = myWin.allObj[idx]->name->val_s;
    QString orthoType = selCamLi->orthoType->val_s;

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
    if (hasFocus() || myWin.lastFocusGL == debugID)
    {
        myWin.lastFocusGL = debugID;
        upTimer->setInterval(8);
    }

    else
        upTimer->setInterval(64);

    GLidx = getGLidx();

    if (mpfTgl)
        mpfTimerStart();

    makeCurrent();

    tick_newFPS = myWin.GetTickCount2();
    tick_diffFPS = tick_newFPS - tick_oldFPS;
    deltaTime = tick_diffFPS / 1000.f;

    rezGateTgl_sel = 0; //rez gate on selCamLi

    if (selCamLi->camLiType->val_s == "FPS") fpsCtrls();
    if (!fboReady) fboReady = myWin.myPP->fboPrep(myWin.allGL[GLidx]);

    /* CUBEMAPGEN */
    if (myWin.doCubeMap && myWin.selB)
    {
        myWin.myGLWidgetSh->cubemapGen();
        myWin.doCubeMap = 0;
    }

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        QString type = myWin.allObj[i]->type;
        QString name = myWin.allObj[i]->name->val_s;

        if (myWin.searchUp(myWin.allObj[i]))
        {
            if (type != "RTT" && type != "VOL" && !myWin.allObj[i]->gizSideObj && name != "pivot")
            {
                myWin.allObj[i]->mvpGet(myWin.allGL[GLidx]);

//                if (name == "teapot")
//                {
//                    myWin.allObj[i]->t->val_3.y = myWin.myAnim->dynAnim("cycle") * 10.f;
////                    myWin.allObj[i]->r->val_3.z = myWin.myAnim->dynAnim("accelDecel") * 500.f;
//                    myWin.allObj[i]->shadowCast->val_b = 0;
//                }
            }
        }

        if (myWin.allObj[i]->selected && myWin.allObj[i]->name->val_s == selCamLi->name->val_s)
            rezGateTgl_sel = 1;

        if (myWin.allObj[i]->dupeStenFix != "")
            dupeStenFix(i);

        myWin.allObj[i]->loadVAO(myWin.allGL[GLidx]);
    }

    myWin.myGLWidgetSh->glUseProgram2("pShadow");
    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->type == "CAMLI" && (myWin.allObj[i]->v->val_b || myWin.allObj[i] == selCamLi))
        {
            if (myWin.allObj[i]->dirtyVM)
                VMup(myWin.allObj[i]);

            if (myWin.allObj[i]->dirtyShadow && myWin.allObj[i]->camLiTypeGet("light"))
                myWin.myGLWidgetSh->writeShadow(i);
        }
    }

//    myWin.myGLWidgetSh->glUseProgram2("pGBuffer");
//    for (int i = 0; i < myWin.allObj.size(); ++i)
//    {
//        if (!myWin.allObj[i]->selected)
//        {
//            if (myWin.allObj[i]->type == "OBJ" && !myWin.allObj[i]->bb->val_b && myWin.searchUp(myWin.allObj[i]))
//            {
//                if (myWin.allObj[i]->backface->val_b)
//                    glDisable(GL_CULL_FACE);

//                myWin.allObj[i]->render(myWin.allGL[GLidx]);

//                glEnable(GL_CULL_FACE);
//            }
//        }
//    }

    glBindFramebuffer(GL_FRAMEBUFFER, main_node.fbo1);
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
    if (myWin.myFSQ->cubeM->val_s == "atmos") myWin.myGLWidgetSh->glUseProgram2("pAtmos");
    else myWin.myGLWidgetSh->glUseProgram2("pSky");
    myWin.myFSQ->render(myWin.allGL[GLidx]);
    glEnable(GL_DEPTH_TEST);

    if (selCamLi->gridV)
    {
        myWin.myGLWidgetSh->glUseProgram2("pGrid");

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->type == "GRID" && gridMatch(i))
                myWin.allObj[i]->render(myWin.allGL[GLidx]);
        }
    }

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->selected && !myWin.allObj[i]->bb->val_b && myWin.searchUp(myWin.allObj[i]))
        {
            if (myWin.allObj[i]->nType > 0) // nViz
            {
                myWin.myGLWidgetSh->glUseProgram2("pNViz");
                myWin.allObj[i]->render(myWin.allGL[GLidx]);

                myWin.myGLWidgetSh->glUseProgram2("pBase");
                myWin.allObj[i]->render(myWin.allGL[GLidx]);
            }

            else if (myWin.allObj[i]->nType == 0 && myWin.selMode == "OBJ")
            {
                glClearStencil(0);
                glClear(GL_STENCIL_BUFFER_BIT);
                glEnable(GL_STENCIL_TEST); //

                //RENDER MESH
                glStencilFunc(GL_ALWAYS, 1, 0xFF); // -1
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

                myWin.myGLWidgetSh->glUseProgram2("pBase");
                if (myWin.allObj[i]->backface->val_b) glDisable(GL_CULL_FACE);
                myWin.allObj[i]->render(myWin.allGL[GLidx]); //
                glEnable(GL_CULL_FACE);

                //THICK WIRE VERSION
                glStencilFunc(GL_NOTEQUAL, 1, -1);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glLineWidth(7.f);

                glPolygonMode(GL_FRONT, GL_LINE);
                myWin.myGLWidgetSh->glUseProgram2("pStencilHi");
                myWin.allObj[i]->render(myWin.allGL[GLidx]);
                glDisable(GL_STENCIL_TEST);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glLineWidth(1.f);
            }
        }
    }

    myWin.myGLWidgetSh->glUseProgram2("pGiz");
    if (myWin.myPivot->v->val_b)
    {
        glDisable(GL_DEPTH_TEST);
        myWin.myPivot->mvpGet(myWin.allGL[GLidx]);
        myWin.myPivot->render(myWin.allGL[GLidx]);
        glEnable(GL_DEPTH_TEST);
    }

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        QString type = myWin.allObj[i]->type;

        if (type == "CAMLI" && myWin.allObj[i]->selected && myWin.searchUp(myWin.allObj[i]))
        {
            if (myWin.allObj[i]->nType == 0 && !myWin.allObj[i]->bb->val_b && myWin.selMode == "OBJ")
            {
                //wireframe already, change color only
                glm::vec3 Cgiz_temp = myWin.allObj[i]->Cgiz;
                myWin.allObj[i]->Cgiz = myWin.glslTable->Csel->val_3;
                myWin.myGLWidgetSh->glUseProgram2("pGiz");
                myWin.allObj[i]->render(myWin.allGL[GLidx]);
                myWin.allObj[i]->Cgiz = Cgiz_temp;
            }
        }

        else if (type == "CAMLI" || type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_LINE")
        {
            if (!myWin.allObj[i]->gizSideObj && myWin.searchUp(myWin.allObj[i]))
            {
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                myWin.allObj[i]->render(myWin.allGL[GLidx]);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
            }
        }
    }

    if (myWin.myGizNull->v->val_b && myWin.myGizNull->gizType == "R")
    {
        myWin.myGLWidgetSh->glUseProgram2("pGiz_circ");
        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->type == "GIZ_CIRC" || myWin.allObj[i]->type == "GIZ_CIRC_HALF")
            {
                glDisable(GL_DEPTH_TEST);
                myWin.allObj[i]->render(myWin.allGL[GLidx]);
                glEnable(GL_DEPTH_TEST);
            }
        }
    }

    myWin.myGLWidgetSh->glUseProgram2("pBB");
    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->bb->val_b && myWin.searchUp(myWin.allObj[i]))
            myWin.allObj[i]->render(myWin.allGL[GLidx]);
    }

    myWin.myGLWidgetSh->glUseProgram2("pBase");
    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (!myWin.allObj[i]->selected)
        {
            if (myWin.allObj[i]->type == "OBJ" && !myWin.allObj[i]->bb->val_b && myWin.searchUp(myWin.allObj[i]))
            {
                if (myWin.allObj[i]->backface->val_b)
                    glDisable(GL_CULL_FACE);

                myWin.allObj[i]->render(myWin.allGL[GLidx]);

                glEnable(GL_CULL_FACE);
            }
        }
    }

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->type == "CAMLI" && myWin.allObj[i]->camLiType->val_s == "SPOT" && myWin.allObj[i]->volCone->val_b)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glEnable(GL_BLEND);

            glDisable(GL_CULL_FACE);
            glDepthMask(0);

            myWin.myVolCone->parentTo = myWin.allObj[i];
            myWin.myVolCone->s->val_3 = myWin.allObj[i]->volS->val_3;
            myWin.myVolCone->mvpGet(myWin.allGL[GLidx]);
            myWin.myGLWidgetSh->glUseProgram2("pVolumeLight");
            myWin.myVolCone->render(myWin.allGL[GLidx]);

            glDepthMask(1);

            glEnable(GL_CULL_FACE);
        }
    }

    if (gizSideTgl)
    {
        glViewport(0, 0, myWin.glslTable->gizSideS->val_3.x, myWin.glslTable->gizSideS->val_3.y);
        myWin.myGLWidgetSh->glUseProgram2("pGiz_side");

        for (unsigned int i = 0; i < myWin.allGizSide.size(); ++i)
        {
            myWin.allGizSide[i]->mvpGet(myWin.allGL[GLidx]);
            myWin.allGizSide[i]->render(myWin.allGL[GLidx]);
        }

        glViewport(0, 0, width(), height());
        glClear(GL_DEPTH_BUFFER_BIT);
    }

//        if (debugBool)
//            myWin.myGLWidgetSh->paintSlow(myWin.allGL[GLidx]); //

    /* COMP */
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    myWin.myPP->postFX(myWin.allGL[GLidx]); //

    overlay2D();

    tick_oldFPS = tick_newFPS;
}

void GLWidget::dupeStenFix(int idx)
{
    for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
    {
        if (myWin.allObj[idx]->v->val_b && myWin.allObj[idx]->dupeStenFix == myWin.allObj[j]->name->val_s)
        {
            if (myWin.allObj[idx]->MM == myWin.allObj[j]->MM)
                myWin.allObj[j]->v->val_b = 0;

            else
            {
                myWin.allObj[idx]->dupeStenFix = "";
                myWin.allObj[j]->v->val_b = 1;
            }
        }
    }
}

void GLWidget::overlay2D()
{
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    myWin.myGLWidgetSh->glUseProgram2("pGiz_side_over");
    myWin.myFSQ->render(myWin.allGL[GLidx]);

    if (rezGateTgl || rezGateTgl_sel) //REZGATE
    {
        glm::vec2 center(width() / 2, height() / 2);
        glm::vec2 xyHalf(myWin.glslTable->rezX->val_i / 2, myWin.glslTable->rezY->val_i / 2);

        rezGate_LD = toNDC(center - xyHalf, "REG");
        rezGate_RU = toNDC(center + xyHalf, "REG");
    }

    if (rttVizTgl)
    {
        myWin.myGLWidgetSh->glUseProgram2("pRtt");

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->type == "RTT" && myWin.searchUp(myWin.allObj[i]))
            {
                myWin.allObj[i]->mvpGet(myWin.allGL[GLidx]);
                myWin.allObj[i]->render(myWin.allGL[GLidx]);
            }
        }
    }

    if (lmbTgl && !altTgl && !disableSelRect && selCamLi->camLiType->val_s != "FPS") //SELRECT
    {
        selRect_LD = toNDC(rayP, "SELRECT");
        selRect_RU = toNDC(pNew, "SELRECT");

        myWin.myGLWidgetSh->glUseProgram2("pSelRect");

       for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
       {
           if (myWin.allObj[i]->type == "SELRECT")
               myWin.allObj[i]->render(myWin.allGL[GLidx]);
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

            for (unsigned int i = 0; i < myWin.myGLWidgetSh->GLDataSh.size(); ++i)
            {
                if (myWin.myGLWidgetSh->GLDataSh[i].obj.get() == myWin.myTxt.get())
                    glNamedBufferSubData(myWin.myGLWidgetSh->GLDataSh[i].VBO_P, 0, (GLsizei)strlen(myWin.myTxt->txt2D), myWin.myTxt->txt2D);
            }

            myWin.myTxt->txtOrigin = glm::vec2(.8f, -.9f);
            myWin.myTxt->render(myWin.allGL[GLidx]);
        }

        if (gizSpaceTgl) //gizSpace
        {
            if (myWin.gizSpace == "local") myWin.myTxt->txt2D = "l";
            else if (myWin.gizSpace == "world") myWin.myTxt->txt2D = "w";

            for (unsigned int i = 0; i < myWin.myGLWidgetSh->GLDataSh.size(); ++i)
            {
                if (myWin.myGLWidgetSh->GLDataSh[i].obj.get() == myWin.myTxt.get())
                    glNamedBufferSubData(myWin.myGLWidgetSh->GLDataSh[i].VBO_P, 0, (GLsizei)strlen(myWin.myTxt->txt2D), myWin.myTxt->txt2D);
            }

            myWin.myTxt->txtOrigin = glm::vec2(.95f, -.9f);
            myWin.myTxt->render(myWin.allGL[GLidx]);
        }

        if (statsTgl)
        {
            int vertsOnObj = 0;
            int edgesOnObj = 0;
            int facesOnObj = 0;

            for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
            {
                if (myWin.allObj[i]->selected)
                {
                    vertsOnObj += myWin.allObj[i]->vertsOnObj;
    //                edgesOnObj += myWin.allObj[i]->edgesOnObj;
    //                facesOnObj += myWin.allObj[i]->facesOnObj;
                }
            }

            stringstream ss;
            ss << "v " << vertsOnObj << " e " << edgesOnObj << " f " << facesOnObj;

            string selStats = ss.str();
            myWin.myTxt->txt2D = selStats.c_str();

            for (unsigned int i = 0; i < myWin.myGLWidgetSh->GLDataSh.size(); ++i)
            {
                if (myWin.myGLWidgetSh->GLDataSh[i].obj.get() == myWin.myTxt.get())
                    glNamedBufferSubData(myWin.myGLWidgetSh->GLDataSh[i].VBO_P, 0, (GLsizei)strlen(myWin.myTxt->txt2D), myWin.myTxt->txt2D);
            }

            myWin.myTxt->txtOrigin = glm::vec2(.5f, .85f);
            myWin.myTxt->render(myWin.allGL[GLidx]);
        }
    }
}

void GLWidget::changeCamLiType_()
{
    if (selCamLi->camLiType->val_s == "FPS")
    {
        setCursor(Qt::BlankCursor);

        selCamLi->nearClip->val_f = .001f;
        selCamLi->farClip->val_f = 1000.f;
        selCamLi->t->val_3 = glm::vec3(5.f);
        selCamLi->lookFPS = glm::normalize(selCamLi->t->val_3);
        selCamLi->r->val_3.x = glm::degrees(float(atan2(selCamLi->lookFPS.z, selCamLi->lookFPS.x) + PI));
        selCamLi->r->val_3.y = glm::degrees(asin(selCamLi->lookFPS.y));

        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
    }

    else if (selCamLi->camLiTypeGet("dirOrtho"))
    {
        setCursor(Qt::ArrowCursor);

        selCamLi->orthoFree->val_b = 1;
        selCamLi->nearClip->val_f = -1000.f;
        selCamLi->farClip->val_f = 1000.f;
        selCamLi->t->val_3 = glm::vec3(5.f);
        selCamLi->targO = glm::vec3(0.f);
        selCamLi->setTarg(selCamLi->targO, 0.f);
    }

    else //persp
    {
        setCursor(Qt::ArrowCursor);

        selCamLi->nearClip->val_f = .001f;
        selCamLi->farClip->val_f = 1000.f;
    }

    resizeGL(width(), height());
    selCamLi->setDirty();
}

void GLWidget::radPop_GL(QString type)
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
        pop.push_back( { "fog", "E", 150, 65, 350, -300 } );
        pop.push_back( { "rtt", "E", 150, 65, 350, -300 } );

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

    myWin.myRadPop->myGL = myWin.allGL[GLidx];
    myWin.myRadPop->show();
}

void GLWidget::mpfTimerStart()
{
    //120fps = 8.333
    //60fps = 16.667
    //30fps = 33.333

    tick_frames++;
    tick_new = myWin.GetTickCount2();
    tick_diff = tick_new - tick_old;

    if (tick_diff >= 1000)
    {
        stringstream ss;
        ss << fixed << setprecision(3) << 1000.f / tick_frames;
        mpf = ss.str();

//        fpsTime = tick_frames * 1000.f / tick_diff;
        mpfTime = 1000.f / tick_frames;
        dTime = mpfTime * myWin.myFSQ->adaptTime->val_f * .01f;

        tick_frames = 0;
        tick_old = tick_new;
    }
}

void GLWidget::updateCamAttrs(QString type)
{
    selCamLi->setDirty();

    if (myWin.selB && myWin.selB == selCamLi)
    {
        myWin.attrTable->writeAttrTgl = 0; //dont refresh whole attr table, change indy cell text instead
        vector<DynSelCamAttrs> updateAttrs;

        if (type == "rotate")
        {
            updateAttrs.push_back( {"rX", selCamLi->r->val_3.x} );
            updateAttrs.push_back( {"rY", selCamLi->r->val_3.y} );
            updateAttrs.push_back( {"rZ", selCamLi->r->val_3.z} );

            updateAttrs.push_back( {"tX", selCamLi->t->val_3.x} );
            updateAttrs.push_back( {"tY", selCamLi->t->val_3.y} );
        }

        else if (type == "translate")
        {
            updateAttrs.push_back( {"tX", selCamLi->t->val_3.x} );
            updateAttrs.push_back( {"tY", selCamLi->t->val_3.y} );
        }

        else if (type == "zoom")
        {
            updateAttrs.push_back( {"orthoZoom", selCamLi->orthoZoom->val_f} );
            updateAttrs.push_back( {"tX", selCamLi->t->val_3.x} );
            updateAttrs.push_back( {"tY", selCamLi->t->val_3.y} );
            updateAttrs.push_back( {"tZ", selCamLi->t->val_3.z} );
        }

        for (unsigned int i = 0; i < updateAttrs.size(); ++i)
        {
            QList<QTableWidgetItem *> findItem = myWin.attrTable->findItems(updateAttrs[i].attr, Qt::MatchExactly);

            if (!findItem.empty())
            {
                stringstream ss;
                ss << fixed << setprecision(3) << updateAttrs[i].attrVal;
                myWin.attrTable->item(myWin.attrTable->row(findItem[0]), 1)->setText(QString::fromStdString(ss.str()));
            }
        }

        myWin.attrTable->writeAttrTgl = 1;
    }
}

void GLWidget::getPtsBetweenRect()
{
    if (!selRectPts_color.empty())
        selRectPts_color.clear();

    int bigX, smallX, bigY, smallY;

    bigX = glm::max(rayP.x, pNew.x);
    smallX = min(rayP.x, pNew.x);
    bigY = glm::max(rayP.y, pNew.y);
    smallY = min(rayP.y, pNew.y);

    if (bigY - smallY > bigX - smallX) //bigger y diff
    {
        for (int y = smallY; y < bigY; ++y)
        {
            for (int x = smallX; x < bigX; ++x)
            {
                selRectPts_color.push_back( {x, y} );

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
                selRectPts_color.push_back( {x, y} );

                continue;
            }
        }
    }
}

bool GLWidget::checkForHits() //READPIXELS
{
    colorPickTgl = 1;
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
    {
        QString type = myWin.allObj[j]->type;
        if ((type == "CAMLI" || type == "OBJ") && myWin.searchUp(myWin.allObj[j]))
        {
            if (myWin.allObj[j]->bb->val_b)
                myWin.myGLWidgetSh->glUseProgram2("pBB");

            else if (myWin.allObj[j]->type == "CAMLI")
                myWin.myGLWidgetSh->glUseProgram2("pGiz");

            else if (myWin.allObj[j]->type == "OBJ")
                myWin.myGLWidgetSh->glUseProgram2("pBase");

            myWin.allObj[j]->mvpGet(myWin.allGL[GLidx]);
            myWin.allObj[j]->render(myWin.allGL[GLidx]);
        }
    }

    glDisable(GL_DEPTH_TEST);

    glFlush();
    glFinish();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    selRectPts_usable = selRectPts_color;
    bool foundNonOBJ = 0;

    for (QVector<QPoint>::iterator it = selRectPts_usable.begin(); it != selRectPts_usable.end();)
    {
        unsigned char data[4];
        glReadPixels((*it).x(), height() - (*it).y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int pickedID = data[0] +  data[1] * 256 + data[2] * 256 * 256;

        if (pickedID != 0x00ffffff)
        {
            int r = (pickedID & 0x000000FF) >> 0;
            int g = (pickedID & 0x0000FF00) >> 8;
            int b = (pickedID & 0x00FF0000) >> 16;

            glm::vec3 compareID(r / 255.f, g / 255.f, b / 255.f);

            for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
            {
                if (myWin.allObj[j]->Crand == compareID)
                {
                    if (myWin.allObj[j]->selectable)
                        matchFoundRaycast(j);
                }
            }
        }

        if (singleShot)
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

    colorPickTgl = 0;

    myWin.gizPivAutoShow(); //
    myWin.myOutliner->refreshOutliner(1);
    myWin.attrTable->refreshTable();

    return foundNonOBJ;
}

void GLWidget::matchFoundRaycast(int idx)
{
    if (myWin.allObj[idx]->selected && ctrlTgl)
        myWin.allObj[idx]->selected = 0;

    if (!ctrlTgl && myWin.searchUp(myWin.allObj[idx]))
    {
        QString type = myWin.allObj[idx]->type;

        myWin.allObj[idx]->selected = 1;
        myWin.selB = myWin.allObj[idx]; //

        if (type == "GIZ_CIRC" || type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_LINE")
            myWin.allObj[idx]->parentTo->parentTo->selected = 1; //keep obj selected w/ manips
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

        for (unsigned int i = 0; i < myWin.allCamCombo.size(); ++i)
        {
            if (myWin.allCamCombo[i]->myGL.get() == this)
                myWin.allCamCombo[i]->myGL.get()->resize(0, 350);
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

int GLWidget::getGLidx()
{
    int index = 999;

    for (unsigned int i = 0; i < myWin.allGL.size(); ++i)
    {
        if (myWin.allGL[i].get() == this)
        {
            index = i;

            break;
        }
    }

    return index;
}
