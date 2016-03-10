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

    int prevCursorShape;

    bool lmbTgl, mmbTgl, rmbTgl, altTgl, ctrlTgl, shiftTgl, spaceTgl;
    bool aTgl, bTgl, cTgl, dTgl, eTgl, fTgl, gTgl, hTgl, iTgl, jTgl, kTgl, lTgl, mTgl, nTgl, oTgl, pTgl, qTgl, rTgl, sTgl, tTgl, uTgl, vTgl, wTgl, xTgl, yTgl, zTgl;
    bool colorPickTgl, disableSelRect, gizSideTgl, gizSpaceTgl, mpfTgl, rezGateTgl, rezGateTgl_sel, singleShotRC, statsTgl, wireOverTgl;

    float aspect, aspectSide, NDC_x, NDC_y, rayIntersectDist;

    glm::mat4 VMgizSide, PM, PMgizSide, PMinv_cube;
    glm::vec2 pMouseNew, pMouseNew_stored, pMouseOld, pMouseDiff, rayP, texelSize;
    glm::vec2 selRect_LD, selRect_RU, rezGate_LD, rezGate_RU;
    glm::vec3 upWorld, from_OBB, to_OBB, aspectXYZ;
    vector<Stroke> strokes_brush, strokes_eraser, strokes_brush_cursor, strokes_eraser_cursor, strokes_cursor;

    string pGBufferDyn, BOIT_name_D;

    //giz
    string gizHoverType = "NONE";
    string gizTransType = "NONE";
    glm::vec3 gizN, gizMouseDown, giz_rayOrigin, giz_rayDir, gizHoverCheckX, gizHoverCheckY, gizHoverCheckZ;

    glm::vec3 brushRGB;
    glm::vec4 brushRGBA;
    float brushA;

    string mpf;
    vector<glm::vec2> selRectPts_color, selRectPts_usable;

    FourGrid_idx splitIdx;
    shared_ptr<Object> selCamLi;
    shared_ptr<GLWidget> activeGL;

    int ID_GLWidget;
    int blendModeD = 999;
    int copyTgt = 0;
    bool debug0 = false;
    bool debug1 = false;
    bool moveDist_paint = false;
    bool firstPress = true;
    bool doEraserDebug = true;
    bool paintCursorResizeTgl = false;
    float distDragPaint = 15.f;
    glm::vec2 paintCursorResize_p;
    glm::vec2 P_currF, P_prevF;
    Map myLayerIdx, sobelMap;

    //timer
    unsigned int tick_frames = 0;
    double tick_new, tick_old, tick_diff, tick_newFPS, tick_oldFPS, tick_diffFPS;
    float deltaTime, mpfTime;
    QTimer *upTimer, *cursorTimer;
    float dTime = 0.f;

//    chrono::high_resolution_clock chrono0;
//    chrono::time_point<chrono::system_clock> chronoPt0, chronoPt1;

    //FBO
    bool fboReady;
    int currLum = 0;

    GLuint gBuf0_32, gBuf1_32, gBuf2_32, gBuf3_32, gBuf4_32, gBuf5_32, gBuf6_32, gBuf7_32, gBuf_DS_32;
    GLuint64 gBuf0_64, gBuf1_64, gBuf2_64, gBuf3_64, gBuf4_64, gBuf5_64, gBuf6_64, gBuf7_64, gBuf_DS_64;

    GLuint rttGaussIn32, tempGauss;
    GLuint64 rttGaussIn64, downSamp_64, extractHLfromSrc_64;

    AbjNode gBufN, bloomCN, bloomN, bloomGaussN[6], deferredN, depthRevN, downN[6], fxaaN, lumaAdaptN[2], lumaInitN, bgN, ssaoN, ssaoGaussN, ssrN, tonemapN, tonemapExpN, alphaGaussN;
    AbjNode brushN, brushBGN, brushTempN, eraserN, cursorN, sobelN;

    bool bakeNow = false;
    glm::mat4 PMd, PMstored;

    float tabletPressure;
    string penOrientation = "PEN";
    string paintType = "NONE";
    bool paintMode = false;
    bool dragDrop = false;
    int edgeDetect_mode = 0;

    void resizeGL(int, int);
    void changeCamLiType_();
    void VMup(shared_ptr<Object>);
    void overlay2D();
    shared_ptr<GLWidget> getActiveGL();
    glm::vec2 toNDC(glm::vec2, string);
    bool jumpSwitch();
    void dupeStenFix_check(shared_ptr<Object>);

    virtual void gizSideTgl_swap() { gizSideTgl = !gizSideTgl; }
    virtual void gizSpaceTgl_swap() { gizSpaceTgl = !gizSpaceTgl; }
    virtual void mpfTgl_swap() { mpfTgl = !mpfTgl; }
    virtual void rezGateTgl_swap() { rezGateTgl = !rezGateTgl; }
    virtual void statsTgl_swap() { statsTgl = !statsTgl; }
    virtual void wireOverTgl_swap() { wireOverTgl = !wireOverTgl; }

public slots:
    void clearCanvas();
    void paintAndCursorDrawHideTimer1();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void tabletEvent(QTabletEvent *);
    void wheelEvent(QWheelEvent *);
    void initializeGL();
    void paintGL();
    void dragEnterEvent(QDragEnterEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *);

    bool checkForHits();
    void matchFoundRaycast(shared_ptr<Object>);

    //timers
    void mpfTimerStart();
    void fpsCtrls();
    void getIndexIntoSplit();
    void getPtsBetweenRect();
    bool gridMatch(shared_ptr<Object>);
    void PMupOrtho();
    void radPop_GL(string);
    void switchGL_layout();
    void updateCamAttrs(string);

    bool paintCursorResize_request();
    vector <glm::vec2> bresenham(glm::vec2, glm::vec2, int);
    void bakeSomething();
    void paintSomething0();
    void paintSomething1(string);
    void clearCursor(bool);
    void brushOutlineUp();
    Map getCurrPaintLayer();
    void blendModeDebug(string);
    void paintAndCursorDrawHideTimer0();
    void GBuffer_BOIT();

signals:
    void changed(const QMimeData *mimeData = 0);
};

#endif // GLWIDGET_H
