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

#include "MainWin.h"

MainWin::MainWin(QWidget *parent) : QMainWindow(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(Qt::FramelessWindowHint);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    myFocus = new FocusCtrlDel;

    resize(1024, 600);
//    resize(1400, 1000);

    centerToScreen(this);

    loadSS();

    lastScene = "";
    menuSmallF = QFont("DejaVu Sans Mono", 10, 75);

    auto *pixMap60 = new QPixmap(60, 60);
    pixMap60->fill(toQC(glm::vec3(0.f, 0.f, 1.f)));
    myIconCam = QIcon(*pixMap60);

    pixMap60->fill(toQC(glm::vec3(1.f, 1.f, 0.f)));
    myIconLight = QIcon(*pixMap60);

    myAnim = new Anim(*this);
    myGLWidgetSh = new GLWidgetSh(*this);
    addAttrTables();
    myPaintWin = new PaintWin(*this);
    myPrefWin = new PrefWin(*this);
    myRadPop = new RadPop(*this);
    myGizmo = new Gizmo(*this);
    myPP = new PP(*this);

    myProgress = new QProgressBar(); //
    myProgress->setFont(QFont("DejaVu Sans Mono", 20, 75));

    auto *topWidget = new QWidget;
    setCentralWidget(topWidget);
    myMenuBar_top = new MenuBarTop(*this);
    setMenuBar(myMenuBar_top);
    addIoMenu(); addActMenu(); addEdMenu(); addEtcMenu(); addRenderMenu(); addRigMenu();
    addOutliner();

    stackedMain = new QStackedLayout(topWidget);
    stackedMain->insertWidget(0, myGLWidgetSh);
    show(); // show() now to initializeGL() in myGLWidgetSh

    //add splitters
    gridLay = new QSplitter(Qt::Vertical);
    hLayU = new QSplitter(Qt::Horizontal);
    hLayD = new QSplitter(Qt::Horizontal);
    vLay = new QSplitter(Qt::Vertical);
    gridLay->insertWidget(0, hLayU);
    gridLay->insertWidget(1, hLayD);

    blankW_main = new QWidget; //
    stackedMain->insertWidget(1, blankW_main);
//    stackedMain->insertWidget(2, gridLay); layoutType = "gridLay";
    stackedMain->insertWidget(2, vLay); layoutType = "vLay";
    stackedMain->insertWidget(3, myProgress);
    stackedMain->setCurrentIndex(3); // PROGRESSBAR

    progressAdd_init();

//    rgb2HSV_d();
}

void MainWin::loadSS()
{
    QFile loadStyleSheet;
    loadStyleSheet.setFileName("style/styleMain.stylesheet");
    loadStyleSheet.open(QFile::ReadOnly);

    myStyle = QString(QLatin1String(loadStyleSheet.readAll())).toStdString();
    loadStyleSheet.close();
    setStyleSheet(QString::fromStdString(myStyle)); //
}

void MainWin::progressAdd_init()
{
    myProgress->setValue(0);
    QApplication::processEvents();

    myGLWidgetSh->mapInit();
    myGLWidgetSh->proInit();

    myProgress->setValue(40);
    QApplication::processEvents();

    camInit();
    gizInit();
    objInit();
    glWidgetInit();

    for (auto &i : allObj)
        myGLWidgetSh->VAOup(i);

    myProgress->setValue(85);
    QApplication::processEvents();

    cutTable->cutTable_init(); cutTable->refreshTable();
    etcTable->refreshTable();
    glslTable->refreshTable();
    pathTable->refreshTable(); //
    attrTable->refreshTable();

    //PAINT TABLES
    myProgress->setValue(100);
    QApplication::processEvents();

    myProgress->reset();
    QApplication::processEvents();

    stackedMain->setCurrentIndex(2); //

    //INITIALIZE RM FOR DIR LIGHTS
    for (auto &i : allObj)
    {
        if (i->type == "CAMLI" && i->camLiType->val_s == "DIR")
            i->RM = i->rotOrderUse(i->rotOrder->val_s);
    }

    glClearColor(0.f, 0.f, 0.f, 0.f);

    myGLWidgetSh->addDeleteShadows("add");
    myGLWidgetSh->UBO_init();
    lightCt = countLights();

    //SINGLE VIEW AT START
    for (auto &i : allGL)
    {
        if (i->ID_GLWidget == 4)
        {
            blankW_main->setParent(0);
            stackedMain->insertWidget(1, &i->mySplitV);
            stackedMain->setCurrentIndex(1);
            savedIdx = 1;
        }
    }
}

void MainWin::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_I)
    {
        if (myPaintWin->stackedMain->currentIndex() == 0)
            PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_K)
    {
        if (myPaintWin->stackedMain->currentIndex() == 1)
            PaintWinTgl(0, 999);
    }
}

void MainWin::gridInit()
{
    GridSetup myGridSetup[] =
    {
        { "grid_front", { 0.f, 0.f, 0.f } },
        { "grid_frontB", { 0.f, 0.f, 90.f } }, //

        { "grid_left", { 0.f, 90.f, 0.f } },
        { "grid_leftB", { 90.f, 90.f, 0.f } },

        { "grid_top", { 90.f, 0.f, 0.f } },
        { "grid_topB", { 0.f, 90.f, 90.f } }, //ok
    };

    for (auto &i : myGridSetup)
    {
        newObj = myGLWidgetSh->VBOup("999", "GRID", i.name, 0); //

        for (auto &j : newObj)
        {
            j->deletable = false;
            j->selectable = false;
            j->ignoreOutliner = true;

            j->r->val_3 = i.r;
            j->s->val_3 = glm::vec3(attrTable->gridSize->val_f);
            allObj.push_back(j);
        }
    }
}

void MainWin::camInit()
{
    gridInit();

    CamSetup myCamSetup[] =
    {
        { "ORTHO", "back", { 0.f, 0.f, -5.f } },
        { "ORTHO", "bottom", { 0.f, -5.f, .002f } }, //
        { "ORTHO", "front", { 0.f, 0.f, 5.f } },
        { "ORTHO", "left", { 5.f, 0.f, 0.f } },
        { "ORTHO", "right", { -5.f, 0.f, 0.f } },
        { "ORTHO", "top", { 0.f, 5.f, .002f } }, //

        { "PERSP", "persp", { 10.f, 10.f, 10.f } },
        { "PERSP", "persp1", { -5.f, 2.f, 5.f } },
    };

    for (auto &i : myCamSetup)
    {
        newObj = myGLWidgetSh->VBOup("999", i.type, i.name, 0);

        for (auto &j : newObj)
        {
            j->Cgiz = glm::vec3(1.f, 0.f, 0.f);
            j->t->val_3 = i.t;
            j->piv->val_3 = j->t->val_3;

            if (i.type == "ORTHO") //sets in VBOup()
            {
                j->nearClip->val_f = -10000.f;
                j->farClip->val_f = 10000.f;
                j->orthoType->val_s = stringToUpper(i.name);
                j->ignoreOutliner = true;
                j->deletable = false;
                j->ref = true;
            }

            else if (i.name == "persp")
            {
                j->deletable = false;
                j->ref = true;
            }

            j->setTarg(j->targO, 0.f);
            j->v->val_b = (j->camLiType->val_s != "ORTHO") ? true : false;

            allObj.push_back(j);
        }
    }
}

void MainWin::glWidgetInit()
{
    GLSetup myGL_setup[] =
    {
        { "top", 0, 0, *hLayU },
        { "persp", 1, 1, *hLayU },

        { "front", 0, 2, *hLayD },
        { "left", 1, 3, *hLayD },

        { "persp", 0, 4, *vLay },
        { "persp1", 1, 5, *vLay },
    };

    //create 6 GLWidgets and add them to allGL
    for (auto &i : myGL_setup)
    {
        auto *splitV = new QSplitter(Qt::Vertical);
        splitV->setHandleWidth(2);

        auto myGL = make_shared<GLWidget>(*this, *splitV, myGLWidgetSh);
        myGL->ID_GLWidget = i.ID;
        allGL.push_back(myGL);

        auto myCamCombo = make_shared<CamCombo>(*this, myGL);
        allCamCombo.push_back(myCamCombo);

        auto camIdx = myCamCombo->findText(QString::fromStdString(i.cam)); //find the default cam
        myCamCombo->setCurrentIndex(camIdx);
        myCamCombo->activated(camIdx);

        splitV->insertWidget(0, myCamCombo.get());
        splitV->insertWidget(1, myGL.get()); //

        splitV->setStretchFactor(0, 0);
        splitV->setStretchFactor(1, 1);

        i.split.insertWidget(i.splitIdx, splitV);
    }
}

void MainWin::objInit()
{
    newObj = myGLWidgetSh->VBOup("999", "BB", "bb", 0);

    for (auto &i : newObj)
    {
        i->deletable = false;
        i->ignoreOutliner = true;
        i->selectable = false;
        i->v->val_b = false;
        allObj.push_back(i);
        myBB = i;
    }

    newObj = myGLWidgetSh->VBOup("999", "FBO", "fsq", 0);
    for (auto &i : newObj)
    {
        i->deletable = false;
        i->ignoreOutliner = true;
        i->selectable = false;

        for (auto &j : myGLWidgetSh->allMaps)
        {
            if (j.name == "ennis")
            {
                if (j.type == "CUBE_SPEC")
                    cubeM_specular_32 = j.layer[0].tex1_32;

                else if (j.type == "CUBE_IRRAD")
                    cubeM_irradiance_32 = j.layer[0].tex1_32;

                i->cubeM->val_s = j.name;
            }
        }

        allObj.push_back(i);
        myFSQ = i;
    }

    newObj = myGLWidgetSh->VBOup("999", "FBO", "paintStroke", 0);
    for (auto &i : newObj)
    {
        i->deletable = false;
        i->ignoreOutliner = true;
        i->selectable = false;

        allObj.push_back(i);
        paintStroke = i;
    }

    newObj = myGLWidgetSh->VBOup("999", "SELRECT", "selRect", 0);
    for (auto &i : newObj)
    {
        i->deletable = false;
        i->ignoreOutliner = true;
        i->selectable = false;
        allObj.push_back(i);
    }

    newObj = myGLWidgetSh->VBOup("999", "TXT", "txt", 0);
    for (auto &i : newObj)
    {
        i->deletable = false;
        i->ignoreOutliner = true;
        i->selectable = false;
        allObj.push_back(i);
        myTxt = i;
    }

    newObj = myGLWidgetSh->VBOup("999", "GIZ_CUBE", "pivot", 0);
    for (auto &i : newObj)
    {
        i->v->val_b = false;
        i->s->val_3 = glm::vec3(.3f);
        i->Cgiz = glm::vec3(1.f, 1.f, 0.f);
        i->deletable = false;
        i->ignoreOutliner = true;
        i->selectable = false;
        allObj.push_back(i);
        myPivot = i;
    }

    startupScene("teapotPlane");
//    startupScene("pointShadows");
//    startupScene("lotsOfSpheres");

    LightSetup myLightSetup[] =
    {
        { "AREA", "areaLight" },
        { "DIR", "dirLight" },
        { "POINT", "pointLight" },
        { "SPOT", "spotLight" },
    };

    for (auto &i : myLightSetup)
    {
        newObj = myGLWidgetSh->VBOup("999", i.type, i.name, 0);
        for (auto &j : newObj)
        {
            j->ignoreOutliner = true;
            j->selectable = 0;
            j->ref = true;
            j->v->val_b = false;
            allObj.push_back(j);

            if (i.name == "areaLight") myAreaLight = j;
            else if (i.name == "dirLight") myDirLight = j;
            else if (i.name == "pointLight") myPointLight = j;
            else if (i.name == "spotLight") mySpotLight = j;
        }
    }

    string conc = pathTable->pathObj->val_s;
    conc.append("cone.obj");

    newObj = myGLWidgetSh->VBOup(conc, "VOL", "volCone", 0);
    for (auto &i : newObj)
    {
        i->ignoreOutliner = true;
        i->selectable = 0;
        allObj.push_back(i);
        myVolCone = i;
    }

    myOutliner->refreshOutliner(1);
}

void MainWin::startupScene(string name)
{
    if (name == "teapotPlane")
    {
        //myFSQ->Kgi->val_f = 0.f;

        newObj = myGLWidgetSh->VBOup("999", "POINT", "light", 0);
        for (auto &i : newObj)
        {
            i->t->val_3 = glm::vec3(5.f);
            i->piv->val_3 = i->t->val_3;
            i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
            i->setTarg(i->targO, 0.f);
            i->lInten->val_f = 5.f;
            i->nearShadow->val_f = 1.f;

            allObj.push_back(i);
        }

//        newObj = myGLWidgetSh->VBOup("999", "SPOT", "light", 0);
//        for (auto &i : newObj)
//        {
//            i->t->val_3 = glm::vec3(0.f, 8.f, 8.f);
//            i->piv->val_3 = i->t->val_3;
//            i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
//            i->setTarg(i->targO, 0.f);
//            //i->lSpotO->val_f = 40.f;
//            i->lInten->val_f = 5.f;
//            i->nearShadow->val_f = 2.f;

//            allObj.push_back(i);
//        }

//        newObj = myGLWidgetSh->VBOup("999", "SPOT", "light", 0);
//        for (auto &i : newObj)
//        {
//            i->t->val_3 = glm::vec3(8.f);
//            i->piv->val_3 = i->t->val_3;
//            i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
//            i->setTarg(i->targO, 0.f);
//            //i->lSpotO->val_f = 40.f;
//            i->lInten->val_f = 5.f;
//            i->nearShadow->val_f = 2.f;

//            allObj.push_back(i);
//        }

//        newObj = myGLWidgetSh->VBOup("999", "SPOT", "light", 0);
//        for (auto &i : newObj)
//        {
//            i->t->val_3 = glm::vec3(-8.f, 8.f, 8.f);
//            i->piv->val_3 = i->t->val_3;
//            i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
//            i->setTarg(i->targO, 0.f);
//            i->lSpotO->val_f = 60.f;
//            i->nearShadow->val_f = 2.f;

//            allObj.push_back(i);
//        }

        auto conc = pathTable->pathObj->val_s;
        conc.append("teapot.obj");

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "teapot", 0);
        for (auto &i : newObj)
        {
            //i->metallicM->val_s = "WHITE";
            //i->ruffM->val_s = "white";
            //i->shadowCast->val_b = false;
            i->r->val_3 = glm::vec3(0.f, 90.f, 0.f);
            i->s->val_3 = glm::vec3(1.5f);
            allObj.push_back(i);
        }

        conc = pathTable->pathObj->val_s;
        conc.append("plane.obj");

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "planeD", 0);
        for (auto &i : newObj)
        {
            //i->ruffM->val_s = "white";
            //i->shadowCast->val_b = false;
            i->s->val_3 = glm::vec3(20.f);
            allObj.push_back(i);
        }
    }

    else if (name == "pointShadows")
    {
        float yShift = 10.f;

        newObj = myGLWidgetSh->VBOup("999", "POINT", "light", 0);
        for (auto &i : newObj)
        {
            i->nearShadow->val_f = .5f;
            i->t->val_3 = glm::vec3(0.f, yShift, .001f);
            i->piv->val_3 = i->t->val_3;
            i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
            i->setTarg(i->targO, 0.f);
            i->lInten->val_f = 5.f;

            allObj.push_back(i);
        }

        auto conc = pathTable->pathObj->val_s;
        conc.append("sphere.obj");

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "sphereF", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->t->val_3 = glm::vec3(0.f, yShift, 5.f);
            i->piv->val_3 = i->t->val_3;
            i->s->val_3 = glm::vec3(1.f);
            allObj.push_back(i);
        }

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "sphereB", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->t->val_3 = glm::vec3(0.f, yShift, -5.f);
            i->piv->val_3 = i->t->val_3;
            i->s->val_3 = glm::vec3(1.f);
            allObj.push_back(i);
        }

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "sphereL", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->t->val_3 = glm::vec3(-5.f, yShift, 0.f);
            i->piv->val_3 = i->t->val_3;
            i->s->val_3 = glm::vec3(1.f);
            allObj.push_back(i);
        }

        //        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "sphereR", 0);
        //        for (auto &i : newObj)
        //        {
        //            i->ruffM->val_s = "white";
        //            i->t->val_3 = glm::vec3(5.f, yShift, 0.f);
        //            i->piv->val_3 = i->t->val_3;
        //            i->s->val_3 = glm::vec3(1.f);
        //            allObj.push_back(i);
        //        }

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "sphereU", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->t->val_3 = glm::vec3(0.f, yShift + 5.f, 0.f);
            i->piv->val_3 = i->t->val_3;
            i->s->val_3 = glm::vec3(1.f);
            allObj.push_back(i);
        }

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "sphereD", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->t->val_3 = glm::vec3(0.f, yShift + -5.f, 0.f);
            i->piv->val_3 = i->t->val_3;
            i->s->val_3 = glm::vec3(1.f);
            allObj.push_back(i);
        }

        //BOX
        conc = pathTable->pathObj->val_s;
        conc.append("plane.obj");

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "planeU", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->r->val_3 = glm::vec3(0.f, 0.f, 180.f);
            i->t->val_3 = glm::vec3(0.f, yShift + 15.f, 0.f);
            i->s->val_3 = glm::vec3(20.f);
            allObj.push_back(i);
        }

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "planeD", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->t->val_3 = glm::vec3(0.f, yShift + -15.f, 0.f);
            i->s->val_3 = glm::vec3(20.f);
            allObj.push_back(i);
        }

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "planeL", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->r->val_3 = glm::vec3(0.f, 0.f, -90.f);
            i->t->val_3 = glm::vec3(-10.f, yShift + 0.f, 0.f);
            i->s->val_3 = glm::vec3(20.f);
            allObj.push_back(i);
        }

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", "planeB", 0);
        for (auto &i : newObj)
        {
            i->ruffM->val_s = "white";
            i->r->val_3 = glm::vec3(90.f, 0.f, 0.f);
//            i->t->val_3 = glm::vec3(0.f, yShift + 0.f, -10.f);
            i->t->val_3 = glm::vec3(0.f, yShift + 0.f, -15.f);
            i->s->val_3 = glm::vec3(20.f);
            allObj.push_back(i);
        }
    }

    else if (name == "lotsOfSpheres")
    {
        newObj = myGLWidgetSh->VBOup("999", "SPOT", "light", 0);
        for (auto &i : newObj)
        {
            //i->t->val_3 = glm::vec3(10.f);
            i->t->val_3 = glm::vec3(8.f);
            i->piv->val_3 = i->t->val_3;
            i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
            i->setTarg(i->targO, 0.f);
            //i->lSpotO->val_f = 40.f;
            i->lInten->val_f = 5.f;

            allObj.push_back(i);
        }

        newObj = myGLWidgetSh->VBOup("999", "SPOT", "light", 0);
        for (auto &i : newObj)
        {
            i->t->val_3 = glm::vec3(-8.f, 8.f, 8.f);
            i->piv->val_3 = i->t->val_3;
            i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
            i->setTarg(i->targO, 0.f);
            i->lSpotO->val_f = 60.f;

            allObj.push_back(i);
        }

        auto conc = pathTable->pathObj->val_s;
        conc.append("sphere.obj");

        for (int w = 0; w < 20; ++w)
        {
            for (int n = 0; n < 20; ++n)
            {
                newObj = myGLWidgetSh->VBOup(conc, "OBJ", "sphere", 0);

                for (auto &i : newObj)
                {
                    i->t->val_3 = glm::vec3(n + (n - .75f) - 15.f, 0.f, w + (w - .75f));
                    i->piv->val_3 = i->t->val_3;
                    i->s->val_3 = glm::vec3(1.f);
                    allObj.push_back(i);
                }
            }
        }
    }
}

void MainWin::camLiAdd()
{
    clearSel();

    newObj = myGLWidgetSh->VBOup("999", "PERSP", "camli", 0);
    for (auto &i : newObj)
    {
        i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
        i->selected = true;

        selB = i;
        allObj.push_back(i);
    }

    for (auto &i : allGL)
    {
        if (i->ID_GLWidget == ID_lastFocused)
        {
            selB->t->val_3 = i->selCamLi->t->val_3;
            selB->piv->val_3 = i->selCamLi->t->val_3;
            selB->setTarg(selB->targO, 0.f);
            selB->r->val_3 = i->selCamLi->r->val_3;

            i->selCamLi = selB;
        }
    }

    myOutliner->refreshOutliner(1);
    attrTable->refreshTable();

    for (auto &i : allCamCombo)
        i->refresh();

    //dont need to refresh shadows b/c creating a cam
}

void MainWin::objAdd(string type, string method)
{
    clearSel();

    if (method == "DRAGDROP")
        newObj = myGLWidgetSh->VBOup(type, "OBJ", "renameMe", 0);

    else if (method == "POP")
    {
        auto conc = pathTable->pathObj->val_s;
        conc.append(type);
        conc.append(".obj");

        newObj = myGLWidgetSh->VBOup(conc, "OBJ", type, 0);
    }

    for (auto &i : newObj)
    {
        i->selected = true;
        selB = i;
        allObj.push_back(i);
    }

    setLightsDirty();

    myOutliner->refreshOutliner(1);
    attrTable->refreshTable();
}

void MainWin::addAttrTables()
{
    dock_attrTable = new QDockWidget("a", this);
    dock_attrTable->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    dock_attrTable_holder = new QWidget;
    auto *dock_matrix_vbox = new QVBoxLayout(dock_attrTable_holder);

    attrTable_holder = new QWidget;
    stackedAttrTable = new QStackedLayout();
    auto *blankW_matrix = new QWidget;
    stackedAttrTable->insertWidget(0, blankW_matrix);

    attrTable_holder->setLayout(stackedAttrTable);
    dock_matrix_vbox->insertWidget(0, attrTable_holder);

    attrTable = new Attrs(*this);

    stackedAttrTable->insertWidget(1, attrTable);

    dock_attrTable->setWidget(dock_attrTable_holder);
    dock_attrTable_holder->setMaximumWidth(200);

    addDockWidget(Qt::RightDockWidgetArea, dock_attrTable);
    dock_attrTable->setVisible(1); //

    //PREF TABLES
    cutTable = new Attrs(*this);
    etcTable = new Attrs(*this);
    glslTable = new Attrs(*this);
    pathTable = new Attrs(*this);

    //LAYER PICK TABLES
    objectLay = new LayerSel(*this);
    typeLay = new LayerSel(*this);
    mapLay = new LayerSel(*this);
    layerLay = new LayerSel(*this);
    layerLay->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void MainWin::addOutliner()
{
    dock_outliner = new QDockWidget("o", this);
    dock_outliner->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    dock_outliner_holder = new QWidget();
    auto *outlinerGridLayout = new QGridLayout(dock_outliner_holder);

    myOutliner = new Outliner(*this);
    outlinerGridLayout->addWidget(myOutliner);

    dock_outliner->setWidget(dock_outliner_holder);
    addDockWidget(Qt::LeftDockWidgetArea, dock_outliner);

    auto startFloating = false;
    if (startFloating)
    {
        dock_outliner->setFloating(1); //
        dock_outliner->setVisible(1); //

        centerToScreen(dock_outliner);
        glm::vec2 moveRelative(glm::vec2(-650, 200) + toVec2(dock_outliner->pos()));
        dock_outliner->move(moveRelative.x, moveRelative.y);
        dock_outliner->resize(dock_outliner->width() - 100, dock_outliner->height());
    }

    //SHOW / HIDE OUTLINER
    auto *showHideOutliner = dock_outliner->toggleViewAction();
    showHideOutliner->setShortcut(tr("8"));
    edMenu->addAction(showHideOutliner);

    dock_outliner->resize(dock_outliner->width(), dock_outliner->height() + 100);
}

void MainWin::addIoMenu()
{
    ioMenu = menuBar()->addMenu("~");
    ioMenu->setTearOffEnabled(1);
    ioMenu->setFont(menuSmallF);

    ioMenu->addAction("new", this, SLOT(newScene()));
    auto *new_s = new QShortcut(tr("CTRL+N"), this);
    connect(new_s, SIGNAL(activated()), this, SLOT(newScene()));

    ioMenu->addAction("open", this, SLOT(openScene()));
    auto *open_s = new QShortcut(tr("CTRL+O"), this);
    connect(open_s, SIGNAL(activated()), this, SLOT(openScene()));

    ioMenu->addAction("save", this, SLOT(saveScene()));
    auto *save_s = new QShortcut(tr("CTRL+S"), this);
    connect(save_s, SIGNAL(activated()), this, SLOT(saveScene()));

    ioMenu->addAction("saveAs", this, SLOT(saveSceneAs()));
    auto *saveAs_s = new QShortcut(tr("CTRL+SHIFT+S"), this);
    connect(saveAs_s, SIGNAL(activated()), this, SLOT(saveSceneAs()));

    ioMenu->addSeparator();

    ioMenu->addAction("quit", this, SLOT(quitConfirm()));
    auto *quit_s = new QShortcut(tr("CTRL+Q"), this);
    connect(quit_s, SIGNAL(activated()), this, SLOT(quitConfirm()));
}

void MainWin::addActMenu()
{
    actMenu = menuBar()->addMenu("actions");
    actMenu->setTearOffEnabled(1);
    actMenu->setFont(menuSmallF);

    actMenu->addAction("getChild", this, SLOT(getChild()));
    auto *getChild_s = new QShortcut(Qt::Key_Down, this);
    connect(getChild_s, SIGNAL(activated()), this, SLOT(getChild()));

    actMenu->addAction("getParent", this, SLOT(getParent()));
    auto *getParent_s = new QShortcut(Qt::Key_Up, this);
    connect(getParent_s, SIGNAL(activated()), this, SLOT(getParent()));

    //myWin.cutTable->multiCut
    actMenu->addAction("delete", this, SLOT(delete_()));
    actMenu->addAction("dupe", this, SLOT(dupe()));
    actMenu->addAction("hide", this, SLOT(hide()));
    actMenu->addAction("unhide", this, SLOT(unHide()));
    actMenu->addAction("manip space", this, SLOT(gizSpaceSwitch()));
    actMenu->addAction("parent", this, SLOT(parent()));
    actMenu->addAction("parent world", this, SLOT(parentWorld()));
    actMenu->addAction("piv center", this, SLOT(pivCenter()));
    actMenu->addAction("piv tgl", this, SLOT(pivTgl()));
    actMenu->addAction("invert sel", this, SLOT(invertSel()));
}

void MainWin::addEdMenu()
{
    edMenu = menuBar()->addMenu("editors");
    edMenu->setTearOffEnabled(1);
    edMenu->setFont(menuSmallF);

    edMenu->addAction("attrTableTgl", this, SLOT(attrTableVizToggle()));
    auto *attrTableTgl_s = new QShortcut(tr("CTRL+A"), this);
    connect(attrTableTgl_s, SIGNAL(activated()), this, SLOT(attrTableVizToggle()));
}

void MainWin::attrTableVizToggle()
{
    if (!dock_attrTable->isVisible())
        dock_attrTable->setVisible(1);

    else
    {
        if (selB)
        {
            attrTable->tabIdx = (attrTable->tabIdx == 0) ? 1 : 0;
            attrTable->refreshTable();
        }
    }
}

void MainWin::addEtcMenu()
{
    etcMenu = menuBar()->addMenu("etc");
    etcMenu->setTearOffEnabled(1);
    etcMenu->setFont(menuSmallF);
}

void MainWin::addRenderMenu()
{
    renderMenu = menuBar()->addMenu("render");
    renderMenu->setTearOffEnabled(1);
    renderMenu->setFont(menuSmallF);

    renderMenu->addAction("deferred prefs", this, SLOT(deferredPrefs()));
    auto *deferredPrefs_s = new QShortcut(Qt::Key_F4, this);
    connect(deferredPrefs_s, SIGNAL(activated()), this, SLOT(deferredPrefs()));
}

void MainWin::quitConfirm()
{
    //on first CTRL+Q : switch to different gl background / overlay to confirm
    //on second CTRL+Q : actually quit

    //DELETE ALL OBJECTS AND THEIR VBOS
    for (auto &i : allObj)
        i->selected = true;

    delete_();

    //DELETE ALL FBOS AND THEIR ATTACHED TEXTURES
    for (auto &i : allGL)
        myPP->resizeTexClearMem(i);

    //DELETE ALL SHADOWMAPS
    myGLWidgetSh->addDeleteShadows("delete");

    //DELETE ALL  "REG" TEXTURES
    for (auto &i : myGLWidgetSh->allMaps)
    {
        for (auto &j : i.layer)
        {
            glMakeTextureHandleNonResidentARB(j.tex1_64);
            glMakeTextureHandleNonResidentARB(j.tex2_64);
            glDeleteTextures(1, &j.tex1_32);
            glDeleteTextures(1, &j.tex2_32);
        }
    }

    //DELETE ALL PROGRAMS
    for (auto &i : myGLWidgetSh->allPro)
        glDeleteProgram(i.pro);

    //DELETE REMAINING ABJ NODES THAT HAVE FBOS INCLUDING ONES IN GLWIDGETSH
    //delete ubo
    glDeleteBuffers(1, &myGLWidgetSh->pro);
    glDeleteBuffers(1, &myGLWidgetSh->UBO_lights);

    qApp->quit();
}

void MainWin::deferredPrefs()
{
    if (myFSQ->selected)
    {
        myFSQ->selected = false;
        auto foundOldSelB = false;

        for (auto &i : allObj)
        {
            if (i->name->val_s == oldSelB && i->selected)
            {
                selB = i;
                foundOldSelB = 1;
            }
        }

        if (!foundOldSelB)
            selB = 0;

        oldSelB = "";
    }

    else
    {
        myFSQ->selected = true;

        if (selB)
            oldSelB = selB->name->val_s;

        selB = myFSQ;
    }

    myOutliner->refreshOutliner(1);
    attrTable->refreshTable();
}

void MainWin::addRigMenu()
{
    rigMenu = menuBar()->addMenu("rig");
    rigMenu->setTearOffEnabled(1);
    rigMenu->setFont(menuSmallF);
}

void MainWin::gridLayTgl()
{
    restoreZoomedLayout();

    gridLay->setParent(0);
    hLayU->setParent(0);
    vLay->setParent(0);
    gridLay->insertWidget(0, hLayU);

    hLayU->show();
    stackedMain->insertWidget(2, gridLay);
    stackedMain->setCurrentIndex(2);
    layoutType = "gridLay";
}

void MainWin::hLayTgl()
{
    restoreZoomedLayout();

    gridLay->setParent(0);
    hLayU->setParent(0);
    vLay->setParent(0);

    hLayU->show();
    stackedMain->insertWidget(2, hLayU);
    stackedMain->setCurrentIndex(2);
    layoutType = "hLay";
}

void MainWin::vLayTgl()
{
    restoreZoomedLayout();

    gridLay->setParent(0);
    hLayU->setParent(0);
    vLay->setParent(0);

    vLay->show();
    stackedMain->insertWidget(2, vLay);
    stackedMain->setCurrentIndex(2);
    layoutType = "vLay";
}

glm::vec3 MainWin::gammaCsel()
{
    glm::vec3 gc(pow(glslTable->Csel->val_3.r, gamma), pow(glslTable->Csel->val_3.g, gamma), pow(glslTable->Csel->val_3.b, gamma));

    return gc;
}

void MainWin::refreshAllPaintTables()
{
    objectLay->refreshTable();
    typeLay->refreshTable();
    mapLay->refreshTable();
    layerLay->refreshTable();
}

void MainWin::PaintWinTgl(bool open, int idx)
{
    if (selB && !myPaintWin->isVisible() && open)
    {        
        auto prefP = toVec2(QCursor::pos());
        glm::vec2 showPrefP(prefP.x - (myPaintWin->width() / 2), prefP.y - (myPaintWin->height() / 2));
        myPaintWin->move(toQP(showPrefP));

        refreshAllPaintTables();

        myPaintWin->stackedMain->setCurrentIndex(idx);
        myPaintWin->show();
    }

    else if (!open)
        myPaintWin->hide();
}

void MainWin::restoreZoomedLayout()
{
    for (auto &i : allGL)
    {
        for (int j = 0; j < stackedMain->count(); ++j)
        {
            if (stackedMain->widget(j) == &i->mySplitV)
            {
                if (layoutType == "gridLay")
                    i->splitIdx.split->insertWidget(i->splitIdx.idx, &i->mySplitV);

                else if (layoutType == "hLay")
                    hLayU->insertWidget(i->splitIdx.idx, &i->mySplitV);

                else if (layoutType == "vLay")
                    vLay->insertWidget(i->splitIdx.idx, &i->mySplitV);

                i->mySplitV.show();

                for (auto &k : allCamCombo)
                {
                    if (k->myGL.get() == i.get())
                         k->myGL->resize(350, 350);
                }

                stackedMain->insertWidget(1, blankW_main);
                stackedMain->setCurrentIndex(2);

                break;
            }
        }
    }
}

void MainWin::clearSel()
{
    selB = 0;

    for (auto &i : allObj)
        i->selected = false;
}

unsigned int MainWin::countSel()
{
    auto count = 0;

    for (auto &i : allObj)
    {
        if (i->selected)
            count += 1;
    }

    return count;
}

int MainWin::countLights()
{
    auto count = 0;

    for (auto &i : allObj)
    {
        if (!i->ref && i->v->val_b && i->camLiTypeGet("light"))
            count += 1;
    }

    return count;
}

bool MainWin::tryAddSwitchLight(string type)
{
    auto lessThanMax = false;

    if (type == "AREA" || "DIR" || "POINT" || "SPOT")
    {
        if (lightCt < 3)
            lessThanMax = true;
    }

    else
        lessThanMax = true;

    return lessThanMax;
}

void MainWin::setLightsDirty()
{
    for (auto &i : allObj)
    {
        if (i->v->val_b && i->type == "CAMLI" && i->camLiTypeGet("light"))
            i->setDirty();
    }
}

void MainWin::gizInit()
{
    newObj = myGLWidgetSh->VBOup("999", "GIZ", "giz", 0);
    for (auto &i : newObj)
    {
        i->v->val_b = false;
        i->ignoreOutliner = true;
        i->deletable = false;
        i->selectable = false;
        allObj.push_back(i);
        myGizNull = i; // parent to this
    }

    float dualP = .5f; // dual axis handle P

    GizSetup myGizSetup[] =
    {
        { "GIZ_CIRC", "gizCirc", { .2f, .2f, .2f } },
        { "GIZ_CIRC_HALF", "gizCircHalfX", { 1.f, 0.f, 0.f } },
        { "GIZ_CIRC_HALF", "gizCircHalfY", { 0.f, 1.f, 0.f } },
        { "GIZ_CIRC_HALF", "gizCircHalfZ", { 0.f, 0.f, 1.f } },

        { "GIZ_CONE", "gizConeX", { 1.f, 0.f, 0.f } },
        { "GIZ_CONE", "gizConeY", { 0.f, 1.f, 0.f } },
        { "GIZ_CONE", "gizConeZ", { 0.f, 0.f, 1.f } },

        { "GIZ_CUBE", "gizCubeX", { 1.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f } },
        { "GIZ_CUBE", "gizCubeY", { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f } },
        { "GIZ_CUBE", "gizCubeZ", { 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 1.f } },
        { "GIZ_CUBE", "gizCubeXYZ", { 0.f, 0.f, 0.f } },

        { "GIZ_DUAL_HANDLE", "gizDualXZ", { 0.f, 1.f, 0.f }, { 90.f, 0.f, 0.f }, { dualP, 0.f, dualP } },
        { "GIZ_DUAL_HANDLE", "gizDualXY", { 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f }, { dualP, dualP, 0.f } },
        { "GIZ_DUAL_HANDLE", "gizDualYZ", { 1.f, 0.f, 0.f }, { 0.f, 90.f, 0.f }, { 0.f, dualP, dualP } },

        { "GIZ_LINE", "gizLineX", { 1.f, 0.f, 0.f } },
        { "GIZ_LINE", "gizLineY", { 0.f, 1.f, 0.f } },
        { "GIZ_LINE", "gizLineZ", { 0.f, 0.f, 1.f } },
    };

    for (auto &i : myGizSetup)
    {
        newObj = myGLWidgetSh->VBOup("999", i.type, i.name, 0);

        for (auto &j : newObj)
        {
            j->Cgiz = i.Cgiz;
            j->Cgiz_stored = i.Cgiz;

            if (i.type == "GIZ_CUBE")
                j->s->val_3 = glm::vec3(.15f);

            else if (i.type == "GIZ_DUAL_HANDLE")
                j->s->val_3 = glm::vec3(.1f);

            j->r->val_3 = i.r;
            j->t->val_3 = i.t;
            j->piv->val_3 = i.t;
            j->deletable = false;
            j->selectable = false;
            j->ignoreOutliner = true;
            j->parentObj("giz");
            allObj.push_back(j);
            allGiz.push_back(j);
        }
    }

    GizSetup myGizSetupSide[] =
    {
        { "GIZ_LINE", "gizLineXSide", { 1.f, 0.f, 0.f } },
        { "GIZ_LINE", "gizLineYSide", { 0.f, 1.f, 0.f } },
        { "GIZ_LINE", "gizLineZSide", { 0.f, 0.f, 1.f } },
    };

    for (auto &i : myGizSetupSide)
    {
        newObj = myGLWidgetSh->VBOup("999", i.type, i.name, 0);

        for (auto &j : newObj)
        {
            j->Cgiz = i.Cgiz;
            j->Cgiz_stored = i.Cgiz;
            j->deletable = false;
            j->selectable = false;
            j->ignoreOutliner = true;
            j->gizSideObj = true;
            allObj.push_back(j);
            allGizSide.push_back(j);
        }
    }
}

void MainWin::gizShow(string gizTypeShow)
{
    if (selB)
    {
        for (auto &i : allObj)
        {
            auto type = i->type;

            if (gizTypeShow == "T")
            {
                if (type == "GIZ_CIRC") i->v->val_b = false;
                else if (type == "GIZ_CIRC_HALF") i->v->val_b = false;
                else if (type == "GIZ_CONE") i->v->val_b = true;
                else if (type == "GIZ_CUBE") i->v->val_b = false;
                else if (type == "GIZ_DUAL_HANDLE") i->v->val_b = true;
                else if (type == "GIZ_LINE") i->v->val_b = true;
            }

            else if (gizTypeShow == "R")
            {
                if (type == "GIZ_CIRC") i->v->val_b = true;
                else if (type == "GIZ_CIRC_HALF") i->v->val_b = true;
                else if (type == "GIZ_CONE") i->v->val_b = false;
                else if (type == "GIZ_CUBE") i->v->val_b = false;
                else if (type == "GIZ_DUAL_HANDLE") i->v->val_b = false;
                else if (type == "GIZ_LINE") i->v->val_b = false;
            }

            else if (gizTypeShow == "S")
            {
                if (type == "GIZ_CIRC") i->v->val_b = false;
                else if (type == "GIZ_CIRC_HALF") i->v->val_b = false;
                else if (type == "GIZ_CONE") i->v->val_b = false;
                else if (type == "GIZ_CUBE" && i != myPivot) i->v->val_b = true;
                else if (type == "GIZ_DUAL_HANDLE") i->v->val_b = true;
                else if (type == "GIZ_LINE") i->v->val_b = true;
            }
        }

        myGizNull->parentTo = selB;
        myGizNull->v->val_b = true;
        myGizNull->gizType = gizTypeShow;

        myOutliner->refreshOutliner(1);
    }
}

void MainWin::gizPivAutoShow()
{
    if (selB && selB->name->val_s != "pivot")
    {
        if (myGizNull->v->val_b)
            myGizNull->parentTo = selB;

        if (myPivot->v->val_b)
            myPivot->parentTo = selB;
    }

    else
    {
        myGizNull->v->val_b = false;
        myPivot->v->val_b = false;
    }
}

void MainWin::gizSpaceSwitch()
{
    gizSpace = (gizSpace == "local") ? "world" : "local";
}

void MainWin::newScene()
{
    cout << "newScene" << endl;
}

void MainWin::openScene()
{
    cout << "openScene" << endl;
}

void MainWin::saveScene()
{
//    cout << "saveScene" << endl;

    string savedSceneName = "ABJ - ";
    savedSceneName.append("savedSceneNameGoesHere");

    setWindowTitle(QString::fromStdString(savedSceneName));
}

void MainWin::saveSceneAs()
{
    cout << "saveSceneAs" << endl;
}

void MainWin::pivMatchTest()
{
    for (auto &i : allObj)
    {
        if (i->selected)
        {
            i->piv->val_3 = glm::vec3(2.f, 2.f, 0.f);
            //i->piv->val_3 = glm::vec3(2.f, 1.414f, -1.414f);

            glm::vec3 t(i->t->val_3);
            glm::vec3 r(i->r->val_3);
            glm::vec3 piv(i->piv->val_3);
            glm::vec3 pivRt(i->pivRt);

            cout << "t = " << glm::to_string(t) << endl;
    //            cout << "rp_os = " << glm::to_string(t - piv) << endl;
            cout << "rp_os = " << glm::to_string(piv - t) << endl;
            cout << "rp_ws = " << glm::to_string(piv) << endl;
            cout << "rt_os = " << glm::to_string(t - pivRt) << endl;
            cout << "rt_ws = " << glm::to_string(pivRt) << endl;
    //            cout << "ra = " << glm::to_string(i->t->val_3) << endl; //axis
            cout << "ro = " << glm::to_string(r) << endl;
    //            cout << "roo = " << glm::to_string(i->r->val_3) << endl; //xyz

            cout << "MM[0] = " << glm::to_string(i->MM[0]) << endl;
            cout << "MM[1] = " << glm::to_string(i->MM[1]) << endl;
            cout << "MM[2] = " << glm::to_string(i->MM[2]) << endl;
            cout << "MM[3] = " << glm::to_string(i->MM[3]) << endl << endl;
        }
    }
}

void MainWin::pivCenter()
{
//    pivMatchTest();

//    for (auto &i : allObj)
//    {
//        if (i->selected)
//        {
//            glm::vec3 cent(5.f, 0.f, -2.f);
//            i->piv->val_3 = glm::vec3(cent);

//            glm::mat4 oM = i->TM * i->RM * i->SM;

//            glm::mat4 invM = glm::inverse(i->MM);
////            glm::mat4 invM = glm::inverse(oM);

//            glm::vec3 off = glm::mat3(invM) * cent;
////            off = -off;
//            cout << "off = " << glm::to_string(off) << endl;

//            glm::vec3 pivMovePlusOff(cent - off);
//            cout << "pivMovePlusOff = " << glm::to_string(pivMovePlusOff) << endl;

//            //i->pivRtM = glm::translate(glm::mat4(), cent);
//            i->pivRtM = glm::translate(glm::mat4(), pivMovePlusOff);
//        }
//    }
}

//void MainWin::pivCenter() // blender
//{
//    for (auto &i : allObj)
//    {
//        if (i->selected)
//        {
//            glm::mat4 centerM = glm::translate(i->bbCenter) * i->MM;
//            glm::vec3 center = glm::vec3(centerM[3].x, centerM[3].y, centerM[3].z);

//            //Blender
//            i->t->val_3 = center;
//            i->piv->val_3 = glm::vec3(0.f);
//        }
//    }

//    attrTable->refreshTable();
//}

void MainWin::pivTgl()
{
    if (selB && selB != myPivot)
    {
        myPivot->parentTo = selB;
        myPivot->v->val_b = !myPivot->v->val_b;
    }

    myOutliner->refreshOutliner(1);
}

void MainWin::pivHide()
{
    myPivot->v->val_b = false;
}

void MainWin::getChild()
{
    if (selB)
    {
        for (auto &i : allObj)
        {
            if (i->parentTo == selB)
            {
                clearSel();
                i->selected = true;
                selB = i;
            }
        }

        myOutliner->refreshOutliner(1);
        attrTable->refreshTable();
    }
}

void MainWin::getParent()
{
    if (selB)
    {
        for (auto &i : allObj)
        {
            if (selB->parentTo == i)
            {
                clearSel();
                i->selected = true;
                selB = i;
            }
        }

        myOutliner->refreshOutliner(1);
        attrTable->refreshTable();
    }
}

void MainWin::searchUp_intermediate(shared_ptr<Object> obj)
{
    if (obj->v->val_b)
    {
        if (obj->parentTo)
        {
            if (obj->parentTo->v->val_b)
            {
                auto found = false;
                shared_ptr<Object> tempObj;

                for (auto &i : allObj)
                {
                    if (obj->parentTo->parentTo == i)
                    {
                        found = true;
                        tempObj = i;

                        continue;
                    }
                }

                if (!found)
                    searchB = true;

                else
                    searchUp_intermediate(tempObj);
            }

            else
                searchB = false;
        }

        else
            searchB = true;
    }

    else
        searchB = false;
}

bool MainWin::searchUp(shared_ptr<Object> obj)
{
    searchUp_intermediate(obj);

    if (searchB)
        return true;

    else
        return false;
}

void MainWin::boundingBoxTgl()
{
    for (auto &i : allObj)
    {
        if (i->selected)
        {
            i->bb->val_b = !i->bb->val_b;
            setLightsDirty();
        }
    }

    attrTable->refreshTable();
}

void MainWin::delete_()
{
    myGizNull->v->val_b = false;
    myGizNull->parentTo = 0;
    myPivot->parentTo = 0;
    auto needsShadowRefresh = false;

    pivHide();

    for (auto &i : allObj) //select downstream, too
    {
        if (i->selected)
        {
            for (auto &j : allObj)
            {
                if (j->parentTo == i)
                    j->selected = true;
            }
        }
    }

    for (auto it = allObj.begin(); it != allObj.end();)
    {
        if ((*it)->deletable && (*it)->selected)
        {
            for (auto &i : allObj)
            {
                if ((*it)->dupeStenFix == i->name->val_s)
                {
                    //reset
                    i->dupeStenFix = "999";
                    i->v->val_b = true;
                }
            }

            if ((*it)->camLiTypeGet("light"))
                needsShadowRefresh = 1;

            (*it)->deleteVAO_VBO();
            it = allObj.erase(it);
        }

        else
            ++it;
    }

    if (needsShadowRefresh)
        myGLWidgetSh->addDeleteShadows("refresh");

    lightCt = countLights();
    setLightsDirty();
    myOutliner->refreshOutliner(0);
    attrTable->refreshTable();

    for (auto &i : allCamCombo)
        i->refresh();
}

void MainWin::dupe()
{
    if (selB)
    {
        shared_ptr<GLWidget> activeGL;

        for (auto &i : allGL)
        {
            if (i->isVisible())
            {
                activeGL = i;

                break;
            }
        }

        auto gizWasOn = false;

        if (myGizNull->v->val_b)
        {
            myGizNull->v->val_b = false;
            myGizNull->parentTo = 0;

            gizWasOn = 1;
        }

        vector<shared_ptr<Object>> dupeTemp;

        for (auto &i : allObj)
        {
            if (i->selected)
                dupeTemp.push_back(i);
        }

        for (auto &i : dupeTemp)
        {
            newObj = myGLWidgetSh->VBOup("999", i->type, i->name->val_s, make_shared<Object>(*i));

            auto changedLight = false;

            if (newObj[0]->camLiTypeGet("light"))
            {
                if (!tryAddSwitchLight(newObj[0]->camLiType->val_s))
                    newObj[0]->camLiType->val_s = "PERSP";

                changedLight = 1;
            }

            newObj[0]->dupeStenFix = i->name->val_s;

            allObj.push_back(newObj[0]);

            newObj[0]->loadVAO(activeGL);

            selB = newObj[0];
            i->selected = false;

            if (changedLight)
                lightCt = countLights();
        }

        if (gizWasOn)
        {
            myGizNull->v->val_b = true;
            myGizNull->parentTo = selB;
        }

        myOutliner->refreshOutliner(1);
        attrTable->refreshTable();

        for (auto &i : allCamCombo)
            i->refresh();

        setLightsDirty();
    }
}

void MainWin::hide()
{
    auto changedLight = false;

    for (auto &i : allObj)
    {
        if (i->selected)
        {
            i->v->val_b = false; //turn off viz

            if (i->camLiTypeGet("light"))
            {
                i->setDirty();
                changedLight = 1;
            }
        }
    }

    if (changedLight)
    {
        myGLWidgetSh->addDeleteShadows("refresh");
        lightCt = countLights();
    }

    setLightsDirty();
    myOutliner->refreshOutliner(0);
    attrTable->refreshTable();
}

void MainWin::unHide()
{
    auto changedLight = false;

    for (auto &i : allObj)
    {
        if (i->selected)
        {
            if (i->camLiTypeGet("light"))
            {
                if (!tryAddSwitchLight(i->camLiType->val_s))
                    i->camLiType->val_s = "PERSP";

                i->setDirty();
                changedLight = 1;
            }

            i->v->val_b = true;
        }
    }

    if (changedLight)
    {
        myGLWidgetSh->addDeleteShadows("refresh");
        lightCt = countLights();
    }

    setLightsDirty();
    myOutliner->refreshOutliner(1);
    attrTable->refreshTable();
}

void MainWin::invertSel()
{
    if (selB && countSel() != (unsigned int)allObj.size())
    {
        selB = 0;

        for (auto &i : allObj)
        {
            if (!i->ignoreOutliner)
            {
                if (i->selected)
                    i->selected = false;

                else
                {
                    i->selected = true;
                    selB = i;
                }
            }
        }

        myOutliner->refreshOutliner(1);
        attrTable->refreshTable();
    }
}

void MainWin::parent()
{
    if (countSel() > 1)
    {
        for (auto &i : allObj)
        {
            if (i->selected)
            {
                if (i->parentTo && i->parentTo != selB)
                {
                    if (i != selB)
                    {
                        i->parentTo = selB;
                        i->s->val_3 /= i->parentTo->s->val_3;
                    }
                }

                else
                {
                    if (i != selB)
                    {
                        i->parentTo = selB;
                        i->s->val_3 /= i->parentTo->s->val_3;
                    }
                }
            }
        }

        myOutliner->refreshOutliner(0);
        attrTable->refreshTable();
    }
}

void MainWin::parentWorld()
{
    for (auto &i : allObj)
    {
        if (i->selected && i->parentTo)
        {
            i->s->val_3 *= i->parentTo->s->val_3;
            i->parentTo = 0;
        }
    }

    myOutliner->refreshOutliner(0);
    attrTable->refreshTable();
}

void MainWin::centerToScreen(QWidget *widget)
{
    auto *dw = QApplication::desktop();
    auto desk_rect = dw->screenGeometry(dw->screenNumber(QCursor::pos()));

    auto desk_x = desk_rect.width();
    auto desk_y = desk_rect.height();
    auto desk_L = desk_rect.left();
    auto desk_U = desk_rect.top();

    auto widget_x = widget->width();
    auto widget_y = widget->height();

    //auto chosenMonitor = "default";
    auto chosenMonitor = "abj1080_120";

    if (chosenMonitor == "abj1080_120")
    {
        desk_x = 1920;
        desk_y = 1080;
        desk_L = 0;
        desk_U = 0;
    }

//    cout << "desk_x / y = " << desk_x << desk_y << endl;
//    cout << "desk_rect L / U = " << desk_L << desk_U << endl;
//    cout << "widget_x / y = " << widget_x << widget_y << endl << endl;

    widget->move(desk_x / 2 - widget_x / 2 + desk_L, desk_y / 2 - widget_y / 2 + desk_U);
}

string MainWin::stringToUpper(string myString)
{
    auto out = myString;

    for (unsigned int i = 0; i < out.length(); ++i)
        out[i] = toupper(out[i]);

    return out;
}

vector<string> MainWin::stringSplit(string data, string token)
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

void MainWin::rgb2HSV_d()
{
    glm::vec3 rgb2HSV0(0.25f, 0.45f, 0.65f);
    cout << "starting RGB color : " << glm::to_string(rgb2HSV0) << endl << endl;

    auto rgb2HSV1 = QColor::fromRgbF(rgb2HSV0.r, rgb2HSV0.g, rgb2HSV0.b).toHsv();
    glm::vec3 rgb2HSV1_toHSV_qc(rgb2HSV1.hue(), rgb2HSV1.saturationF(), rgb2HSV1.valueF());
    cout << "rgb2HSV1_toHSV_qc = " << glm::to_string(rgb2HSV1_toHSV_qc) << endl;
    cout << "hueF = " << rgb2HSV1.hueF() << " " << rgb2HSV1.hue() << " " << rgb2HSV1.hue() / 360.f << endl;


    auto rgb2HSV0_glmConv = glm::hsvColor(rgb2HSV0);
    cout << "rgb2HSV0_glmConv = " << glm::to_string(rgb2HSV0_glmConv) << endl;
    cout << "0/1/2 = " << rgb2HSV0_glmConv[0] << " " << rgb2HSV0_glmConv[1] << " " << rgb2HSV0_glmConv[2] << endl;
    cout << "R/G/B = " << rgb2HSV0_glmConv.r << " " << rgb2HSV0_glmConv.g << " " << rgb2HSV0_glmConv.b << endl;


    auto myRGB = glm::rgbColor(rgb2HSV0_glmConv);
    cout << "myRGB = " << glm::to_string(myRGB) << endl;
}
