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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "MainWin.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    MainWin &myWin;
    QSplitter &mySplitV;
    GLWidget(MainWin &, QSplitter &, const QGLWidget *shareWidget = 0, QWidget *parent = 0);

    bool lmbTgl, mmbTgl, rmbTgl, altTgl, ctrlTgl, shiftTgl, spaceTgl;
    bool aTgl, bTgl, cTgl, dTgl, eTgl, fTgl, gTgl, hTgl, iTgl, jTgl, kTgl, lTgl, mTgl, nTgl, oTgl, pTgl, qTgl, rTgl, sTgl, tTgl, uTgl, vTgl, wTgl, xTgl, yTgl, zTgl;
    bool colorPickTgl, disableSelRect, gizSideTgl, gizSpaceTgl, mpfTgl, rezGateTgl, rezGateTgl_sel, singleShot, statsTgl, wireOverTgl, rttVizTgl;
    float aspect, aspectSide, NDC_x, NDC_y, rayIntersectDist;

    glm::mat4 VMgizSide, PM, PMgizSide, PMinv_cube, PMrtt;
    glm::vec2 pNew, pOld, pD, rayP;
    glm::vec2 selRect_LD, selRect_RU, rezGate_LD, rezGate_RU;
    glm::vec3 upWorld, from_OBB, to_OBB;

    //giz
    string gizHoverType = "NONE";
    string gizTransType = "NONE";
    glm::vec3 gizN, gizMouseDown, giz_rayOrigin, giz_rayDir;
    glm::vec3 gizHoverCheckX, gizHoverCheckY, gizHoverCheckZ;

    string mpf;
    QVector<QPoint> selRectPts_color, selRectPts_usable;
    FourGrid_idx splitIdx;
    shared_ptr<Object> selCamLi;

    int debugID;
    bool debugBool = 0;
    int GLidx;

    //timer
    double tick_new, tick_old, tick_diff, tick_newFPS, tick_oldFPS, tick_diffFPS;
    unsigned int tick_frames = 0;
    double lastTime, currentTime;
    float deltaTime, deltaTime_resize, fpsTime, mpfTime;
    QTimer *upTimer;
    float dTime = 0.f;

    //FBO
    bool fboReady;
    int currLum = 0;

    AbjNode gbuf_node, deferred_node;
    GLuint gbuf0_32, gbuf1_32, gbuf2_32, gbuf3_32, gbuf4_32, gbuf5_32, gbuf_DS_32;
    GLuint64 gbuf0_64, gbuf1_64, gbuf2_64, gbuf3_64, gbuf4_64, gbuf5_64, gbuf_DS_64;


    GLuint simp_sky_32, simp_DS_32, simp_Giz_32;
    GLuint64 simp_sky_64, simp_DS_64, simp_Giz_64;
    GLuint rttGaussIn32, tempGauss;
    GLuint64 rttGaussIn64, downSamp_64, extractHLfromSrc_64;
    AbjNode bloomC_node, bloom_node, bloom_gauss_node[6], down_node[6], fxaa_node, lumaAdapt[2], lumaInit, simp_node, ssao_node, ssao_gauss_node, tonemap_node, tonemap_exposure_node;
    glm::vec2 texelSize;

    bool storedPM = 0;
    glm::mat4 PMd, PMstored;

    //DRAG DROP
    bool dragDrop = 0;


    void resizeGL(int, int);
    void changeCamLiType_();
    void VMup(shared_ptr<Object>);
    void overlay2D();
    int getGLidx();
    glm::vec2 toNDC(glm::vec2, string);
    bool jumpSwitch();
    void dupeStenFix(int);

    virtual void gizSideTgl_() { gizSideTgl = !gizSideTgl; }
    virtual void gizSpaceTgl_() { gizSpaceTgl = !gizSpaceTgl; }
    virtual void mpfTgl_() { mpfTgl = !mpfTgl; }
    virtual void rezGateTgl_() { rezGateTgl = !rezGateTgl; }
    virtual void rttVizTgl_() { rttVizTgl = !rttVizTgl; }
    virtual void statsTgl_() { statsTgl = !statsTgl; }
    virtual void wireOverTgl_() { wireOverTgl = !wireOverTgl; }

public slots:

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void initializeGL();
    void paintGL();

    void dragEnterEvent(QDragEnterEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *);

    void mpfTimerStart();

    bool checkForHits();
    void matchFoundRaycast(int);

    void switchGL_layout();
    void getIndexIntoSplit();
    void updateCamAttrs(QString);
    void getPtsBetweenRect();
    void fpsCtrls();
    void radPop_GL(QString);
    void PMupOrtho();
    bool gridMatch(int idx);

signals:
    void changed(const QMimeData *mimeData = 0);

};


#endif // GLWIDGET_H
