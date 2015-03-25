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

MainWin::MainWin(QWidget *parent) : QMainWindow(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    setWindowFlags(Qt::FramelessWindowHint);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    myFocus = new FocusCtrlDel;
//    resize(1024, 600);
    resize(1400, 1000);

    centerToScreen(this);

    //load ss
    QFile loadStyleSheet;
    loadStyleSheet.setFileName("style/styleMain.stylesheet");
    loadStyleSheet.open(QFile::ReadOnly);
    myStyle = QLatin1String(loadStyleSheet.readAll());
    loadStyleSheet.close();

    setStyleSheet(myStyle); //

    CPopWinTgl = PrefWinTgl = fogTgl = 0;
    lastScene = "";
    menuSmallF = QFont("DejaVu Sans Mono", 10, 75);

    QPixmap *pixMap60 = new QPixmap(60, 60);
    pixMap60->fill(Qt::blue);
    myIconCam = QIcon(*pixMap60);

    pixMap60->fill(Qt::yellow);
    myIconLight = QIcon(*pixMap60);

    addAttrTables();

    myAnim = new Anim(*this);
    myGLWidgetSh = new GLWidgetSh(*this);
    myCPopWin = new CPopWin(*this);
    myPrefWin = new PrefWin(*this);
    myRadPop = new RadPop(*this);
    myGizmo = new Gizmo(*this);
    myPP = new PP(*this);

    myProgress = new QProgressBar(); //
    myProgress->setFont(QFont("DejaVu Sans Mono", 20, 75));

    QWidget *topWidget = new QWidget;
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
}

void MainWin::progressAdd_init()
{
    myProgress->setValue(0);
    QApplication::processEvents();

    myGLWidgetSh->texInit();
    myGLWidgetSh->proUp();

    myProgress->setValue(40);
    QApplication::processEvents();

    camInit();
    gizInit();
    objInit();
    glWidgetInit();

    for (unsigned int i = 0; i < allObj.size(); ++i)
        myGLWidgetSh->VAOup(allObj[i]);

    myProgress->setValue(85);
    QApplication::processEvents();

    cutTable->cutTable_init(); //
    cutTable->refreshTable();
    etcTable->refreshTable();
    glslTable->refreshTable();
    pathTable->refreshTable(); //
    attrTable->refreshTable();

    myProgress->setValue(100);
    QApplication::processEvents();

    myProgress->reset();
    QApplication::processEvents();

    stackedMain->setCurrentIndex(2); //

    //INITIALIZE RM FOR DIR LIGHTS
    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (allObj[i]->type == "CAMLI" && allObj[i]->camLiType->val_s == "DIR")
            allObj[i]->RM = allObj[i]->rotOrderUse(allObj[i]->rotOrder->val_s);
    }

    myGLWidgetSh->addDeleteShadows("add");
//    myGLWidgetSh->cubeFBO_node = myGLWidgetSh->cubeDynNode_create("cube", 1024, 1024);

    lightCt = countLights();

    //SINGLE VIEW AT START
    for (unsigned int i = 0; i < allGL.size(); ++i)
    {
        if (allGL[i]->debugID == 4)
        {
            blankW_main->setParent(0);
            stackedMain->insertWidget(1, &allGL[i]->mySplitV);
            stackedMain->setCurrentIndex(1);
            savedIdx = 1;
        }
    }
}

void MainWin::gridInit()
{
    GridSetup myGridSetup[] =
    {
        { "grid_front", {0.f, 0.f, 0.f} },
        { "grid_frontB", {0.f, 0.f, 90.f} }, //

        { "grid_left", {0.f, 90.f, 0.f} },
        { "grid_leftB", {90.f, 90.f, 0.f} },

        { "grid_top", {90.f, 0.f, 0.f} },
        { "grid_topB", {0.f, 90.f, 90.f} }, //ok
    };

    for (unsigned int i = 0; i < arraySize(myGridSetup); ++i)
    {
        loadO = myGLWidgetSh->VBOup(0, "GRID", myGridSetup[i].name, 0); //
        for (unsigned int j = 0; j < loadO.size(); ++j)
        {
            loadO[j]->deletable = 0;
            loadO[j]->selectable = 0;
            loadO[j]->ignoreOutliner = 1;
            loadO[j]->ssaoTgl->val_b = 0;

            loadO[j]->r->val_3 = myGridSetup[i].r;
            loadO[j]->s->val_3 = glm::vec3(attrTable->gridSize->val_f);
            allObj.push_back(loadO[j]);
        }
    }
}

void MainWin::camInit()
{
    gridInit();

    CamSetup myCamSetup[] =
    {
        { "ORTHO", "back", {0.f, 0.f, -5.f} },
        { "ORTHO", "bottom", {0.f, -5.f, .002f} }, //
        { "ORTHO", "front", {0.f, 0.f, 5.f} },
        { "ORTHO", "left", {5.f, 0.f, 0.f} },
        { "ORTHO", "right", {-5.f, 0.f, 0.f} },
        { "ORTHO", "top", {0.f, 5.f, .002f} }, //

        { "PERSP", "persp", {10.f, 10.f, 10.f} },
        { "PERSP", "persp2", {-5.f, 2.f, 5.f} },
//        { "SPOT", "persp2", {-5.f, 2.f, 5.f} },
    };

    for (unsigned int i = 0; i < arraySize(myCamSetup); ++i)
    {
        loadO = myGLWidgetSh->VBOup(0, myCamSetup[i].type, myCamSetup[i].name, 0);
        for (unsigned int j = 0; j < loadO.size(); ++j)
        {
            loadO[j]->Cgiz = glm::vec3(1.f, 0.f, 0.f);
            loadO[j]->t->val_3 = myCamSetup[i].t;
            loadO[j]->piv->val_3 = loadO[j]->t->val_3;

            if (myCamSetup[i].type == "ORTHO") //sets in VBOup()
            {
                loadO[j]->nearClip->val_f = -10000.f;
                loadO[j]->farClip->val_f = 10000.f;
                loadO[j]->orthoType->val_s = myCamSetup[i].name.toUpper();
                loadO[j]->ignoreOutliner = 1;
                loadO[j]->deletable = 0;
                loadO[j]->ref = 1;
            }

            else if (myCamSetup[i].name == "persp")
            {
                loadO[j]->deletable = 0;
                loadO[j]->ref = 1;
            }

            loadO[j]->setTarg(loadO[j]->targO, 0.f);
            loadO[j]->v->val_b = (loadO[j]->camLiType->val_s != "ORTHO") ? 1 : 0;

            allObj.push_back(loadO[j]);
        }
    }
}

void MainWin::glWidgetInit()
{
    GLSetup myGL_setup[] =
    {
        { "top", 0, *hLayU },
        { "persp", 1, *hLayU },

        { "front", 0, *hLayD },
        { "left", 1, *hLayD },

        { "persp", 0, *vLay },
        { "persp2", 1, *vLay },
//        { "top", 1, *vLay },
    };

    for (unsigned int i = 0; i < arraySize(myGL_setup); ++i)
    {
        QSplitter *splitV = new QSplitter(Qt::Vertical);
        splitV->setHandleWidth(2);

        shared_ptr<GLWidget> myGL = make_shared<GLWidget>(*this, *splitV, myGLWidgetSh);
        myGL.get()->debugID = i;
        allGL.push_back(myGL);

        shared_ptr<CamCombo> myCamCombo = make_shared<CamCombo>(*this, myGL);
        allCamCombo.push_back(myCamCombo);

        int camIdx = myCamCombo->findText(myGL_setup[i].cam); //find the default cam
        myCamCombo->setCurrentIndex(camIdx);
        myCamCombo->activated(camIdx);

        splitV->insertWidget(0, myCamCombo.get());
        splitV->insertWidget(1, myGL.get()); //

        splitV->setStretchFactor(0, 0);
        splitV->setStretchFactor(1, 1);

        myGL_setup[i].split.insertWidget(myGL_setup[i].idx, splitV);
    }
}

void MainWin::objInit()
{
    loadO = myGLWidgetSh->VBOup(0, "BB", "bb", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->deletable = 0;
        loadO[i]->ignoreOutliner = 1;
        loadO[i]->selectable = 0;
        loadO[i]->v->val_b = 0;
        loadO[i]->ssaoTgl->val_b = 0;
        allObj.push_back(loadO[i]);
        myBB = loadO[i];
    }

    loadO = myGLWidgetSh->VBOup(0, "FBO", "fsq", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->deletable = 0;
        loadO[i]->ignoreOutliner = 1;
        loadO[i]->selectable = 0;
        loadO[i]->ssaoTgl->val_b = 0;

        for (unsigned int j = 0; j < myGLWidgetSh->allTex.size(); ++j)
        {
            if (myGLWidgetSh->allTex[j].name == "ennis")
            {
                if (myGLWidgetSh->allTex[j].type == "CUBE")
                    cubeM_specular_32 = myGLWidgetSh->allTex[j].tex_32;

                else if (myGLWidgetSh->allTex[j].type == "CUBE_IRRAD")
                    cubeM_irradiance_32 = myGLWidgetSh->allTex[j].tex_32;

                loadO[i]->cubeM->val_s = myGLWidgetSh->allTex[j].name;
            }
        }

        allObj.push_back(loadO[i]);
        myFSQ = loadO[i];
    }

    loadO = myGLWidgetSh->VBOup(0, "SELRECT", "selRect", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->deletable = 0;
        loadO[i]->ignoreOutliner = 1;
        loadO[i]->selectable = 0;
        allObj.push_back(loadO[i]);
    }

    loadO = myGLWidgetSh->VBOup(0, "TXT", "txt", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->deletable = 0;
        loadO[i]->ignoreOutliner = 1;
        loadO[i]->selectable = 0;
        allObj.push_back(loadO[i]);
        myTxt = loadO[i];
    }

    loadO = myGLWidgetSh->VBOup(0, "RTT", "rtt", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->s->val_3 = glm::vec3(.075f, 0.f, 0.f);
        loadO[i]->t->val_3 = glm::vec3(-2.f, .05f, 0.f);
        loadO[i]->v->val_b = 0;
        allObj.push_back(loadO[i]);
    }

    loadO = myGLWidgetSh->VBOup(0, "GIZ_CUBE", "pivot", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->v->val_b = 0;
        loadO[i]->s->val_3 = glm::vec3(.3f);
        loadO[i]->Cgiz = glm::vec3(1.f, 1.f, 0.f);
        loadO[i]->deletable = 0;
        loadO[i]->ignoreOutliner = 1;
        loadO[i]->selectable = 0;
        allObj.push_back(loadO[i]);
        myPivot = loadO[i];
    }

//    startupScene("cubemapDebug");
//    startupScene("ddsDebug_sphere");
//    startupScene("lotsOfSpheres");
    startupScene("teapotPlane");

    //DEFAULT LIGHTS
    loadO = myGLWidgetSh->VBOup(0, "SPOT", "light", 0);
//    loadO = myGLWidgetSh->VBOup(0, "POINT", "light", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->t->val_3 = glm::vec3(0.f, 10.f, .1f);
        loadO[i]->piv->val_3 = loadO[i]->t->val_3;
        loadO[i]->Cgiz = glm::vec3(1.f, 0.f, 0.f);
        loadO[i]->setTarg(loadO[i]->targO, 0.f);
        loadO[i]->lSpotO->val_f = 60.f;
        loadO[i]->lInten->val_f = 20.f;
//        loadO[i]->lInten->val_f = 1.f;

        allObj.push_back(loadO[i]);
    }

//    loadO = myGLWidgetSh->VBOup(0, "SPOT", "light", 0);
//    //loadO = myGLWidgetSh->VBOup(0, "POINT", "light", 0);
//    for (unsigned int i = 0; i < loadO.size(); ++i)
//    {
////        loadO[i]->t->val_3 = glm::vec3(0.f, 8.f, .1f);
//        loadO[i]->t->val_3 = glm::vec3(6.f, 8.f, .1f);
//        loadO[i]->piv->val_3 = loadO[i]->t->val_3;
//        loadO[i]->Cgiz = glm::vec3(1.f, 0.f, 0.f);
//        loadO[i]->setTarg(loadO[i]->targO, 0.f);
//        loadO[i]->lSpotO->val_f = 60.f;

//        allObj.push_back(loadO[i]);
//    }

//    loadO = myGLWidgetSh->VBOup(0, "SPOT", "light", 0); // #2
//    for (unsigned int i = 0; i < loadO.size(); ++i)
//    {
//        loadO[i]->t->val_3 = glm::vec3(0.f, 8.f, 0.f);
//        loadO[i]->piv->val_3 = loadO[i]->t->val_3;
//        loadO[i]->Cgiz = glm::vec3(1.f, 0.f, 0.f);
//        loadO[i]->setTarg(loadO[i]->targO, 0.f);

//        allObj.push_back(loadO[i]);
//    }

    LightSetup myLightSetup[] =
    {
        { "AREA", "areaLight"},
        { "DIR", "dirLight"},
        { "POINT", "pointLight"},
        { "SPOT", "spotLight"},
    };

    for (unsigned int i = 0; i < arraySize(myLightSetup); ++i)
    {
        loadO = myGLWidgetSh->VBOup(0, myLightSetup[i].type, myLightSetup[i].name, 0);
        for (unsigned int j = 0; j < loadO.size(); ++j)
        {
            loadO[j]->ignoreOutliner = 1;
            loadO[j]->selectable = 0;
            loadO[j]->ref = 1;
            loadO[j]->v->val_b = 0;
            allObj.push_back(loadO[j]);

            if (myLightSetup[i].name == "areaLight") myAreaLight = loadO[j];
            else if (myLightSetup[i].name == "dirLight") myDirLight = loadO[j];
            else if (myLightSetup[i].name == "pointLight") myPointLight = loadO[j];
            else if (myLightSetup[i].name == "spotLight") mySpotLight = loadO[j];
        }
    }

    loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "cone.obj", "VOL", "volCone", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->ssaoTgl->val_b = 0;
        loadO[i]->ignoreOutliner = 1;
        loadO[i]->selectable = 0;
        allObj.push_back(loadO[i]);
        myVolCone = loadO[i];
    }

    myOutliner->refreshOutliner(1);
}

void MainWin::startupScene(QString name)
{
    if (name == "lotsOfSpheres")
    {
        for (int n = 0; n < 20; ++n)
        {
            loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphere", 0);
            for (unsigned int i = 0; i < loadO.size(); ++i)
            {
                loadO[i]->t->val_3 = glm::vec3(n + (n - .75f) - 15.f, 0.f, 0.f);
//                loadO[i]->t->val_3 = glm::vec3(n + .5f, 0.f, 0.f);
                loadO[i]->piv->val_3 = loadO[i]->t->val_3;
                loadO[i]->s->val_3 = glm::vec3(1.f);
                allObj.push_back(loadO[i]);
            }
        }
    }

    else if (name == "cubemapDebug")
    {
        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphereF", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->t->val_3 = glm::vec3(0.f, 10.f, 5.f);
            loadO[i]->piv->val_3 = loadO[i]->t->val_3;
            loadO[i]->s->val_3 = glm::vec3(1.f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphereB", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->t->val_3 = glm::vec3(0.f, 10.f, -5.f);
            loadO[i]->piv->val_3 = loadO[i]->t->val_3;
            loadO[i]->s->val_3 = glm::vec3(1.f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphereL", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->t->val_3 = glm::vec3(-5.f, 10.f, 0.f);
            loadO[i]->piv->val_3 = loadO[i]->t->val_3;
            loadO[i]->s->val_3 = glm::vec3(1.f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphereR", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->t->val_3 = glm::vec3(5.f, 10.f, 0.f);
            loadO[i]->piv->val_3 = loadO[i]->t->val_3;
            loadO[i]->s->val_3 = glm::vec3(1.f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphereU", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->t->val_3 = glm::vec3(0.f, 15.f, 0.f);
            loadO[i]->piv->val_3 = loadO[i]->t->val_3;
            loadO[i]->s->val_3 = glm::vec3(1.f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphereD", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->t->val_3 = glm::vec3(0.f, 5.f, 0.f);
            loadO[i]->piv->val_3 = loadO[i]->t->val_3;
            loadO[i]->s->val_3 = glm::vec3(1.f);
            allObj.push_back(loadO[i]);
        }

//        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphereCenter", 0);
//        for (int i = 0; i < loadO.size(); ++i)
//        {
//            loadO[i]->t->val_3 = glm::vec3(0.f, 10.f, 0.f);
//            loadO[i]->piv->val_3 = loadO[i]->t->val_3;
//            loadO[i]->s->val_3 = glm::vec3(1.f);
//            allObj.push_back(loadO[i]);
//        }

        //CORNELL
        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "plane.obj", "OBJ", "planeU", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->r->val_3 = glm::vec3(0.f, 0.f, 180.f);
            loadO[i]->t->val_3 = glm::vec3(0.f, 20.f, 0.f);
            loadO[i]->s->val_3 = glm::vec3(20.f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "plane.obj", "OBJ", "planeD", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->s->val_3 = glm::vec3(20.f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "plane.obj", "OBJ", "planeB", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->r->val_3 = glm::vec3(90.f, 0.f, 0.f);
            loadO[i]->t->val_3 = glm::vec3(0.f, 10.f, -10.f);
            loadO[i]->s->val_3 = glm::vec3(20.f);
            allObj.push_back(loadO[i]);
        }
    }

    else if (name == "teapotPlane")
    {
        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "teapot.obj", "OBJ", "teapot", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
//            loadO[i]->t->val_3 = glm::vec3(0.f, 3.f, 0.f);
            loadO[i]->r->val_3 = glm::vec3(0.f, 90.f, 0.f);
            loadO[i]->s->val_3 = glm::vec3(1.5f);
            allObj.push_back(loadO[i]);
        }

        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "plane.obj", "OBJ", "planeD", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->s->val_3 = glm::vec3(20.f);
            allObj.push_back(loadO[i]);
        }
    }

    else if (name == "ddsDebug_sphere")
    {
        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + "sphere.obj", "OBJ", "sphere", 0);
        for (unsigned int i = 0; i < loadO.size(); ++i)
        {
            loadO[i]->r->val_3 = glm::vec3(0.f, 90.f, 0.f);
            loadO[i]->s->val_3 = glm::vec3(4.f);
//            loadO[i]->Kr->val_f = 1.f;
            allObj.push_back(loadO[i]);
        }
    }
}

void MainWin::camLiAdd()
{
    clearSel();

    loadO = myGLWidgetSh->VBOup(0, "PERSP", "camli", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->Cgiz = glm::vec3(1.f, 0.f, 0.f);
        loadO[i]->selected = 1;

        selB = loadO[i];
        allObj.push_back(loadO[i]);
    }

    for (unsigned int i = 0; i < allGL.size(); ++i)
    {
        if (allGL[i]->debugID == lastFocusGL)
        {
            selB->t->val_3 = allGL[i]->selCamLi->t->val_3;
            selB->piv->val_3 = allGL[i]->selCamLi->t->val_3;
            selB->setTarg(selB->targO, 0.f);
            selB->r->val_3 = allGL[i]->selCamLi->r->val_3;

            allGL[i]->selCamLi = selB;
        }
    }

    myOutliner->refreshOutliner(1);
    attrTable->refreshTable();

    for (unsigned int i = 0; i < allCamCombo.size(); ++i)
        allCamCombo[i]->refresh();

    //dont need to refresh shadows b/c creating a cam
}

void MainWin::objAdd(QString type, QString method)
{
    clearSel();

    if (method == "DRAGDROP")
        loadO = myGLWidgetSh->VBOup(type, "OBJ", "renameMe", 0);

    else if (method == "POP")
        loadO = myGLWidgetSh->VBOup(pathTable->pathObj->val_s + type + ".obj", "OBJ", type, 0);

    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->selected = 1;
        selB = loadO[i];
        allObj.push_back(loadO[i]);
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
    QVBoxLayout *dock_matrix_vbox = new QVBoxLayout(dock_attrTable_holder);

    attrTable_holder = new QWidget;
    stackedAttrTable = new QStackedLayout();
    QWidget *blankW_matrix = new QWidget;
    stackedAttrTable->insertWidget(0, blankW_matrix);

    attrTable_holder->setLayout(stackedAttrTable);
    dock_matrix_vbox->insertWidget(0, attrTable_holder);

    attrTable = new Attrs(*this);
    attrTable->tableType = "attr";

    stackedAttrTable->insertWidget(1, attrTable);

    dock_attrTable->setWidget(dock_attrTable_holder);
    dock_attrTable_holder->setMaximumWidth(200); //width limit

    addDockWidget(Qt::RightDockWidgetArea, dock_attrTable);
    dock_attrTable->setVisible(1); //

    //PREF TABLES
    cutTable = new Attrs(*this); cutTable->tableType = "cut";
    etcTable = new Attrs(*this); etcTable->tableType = "etc";
    glslTable = new Attrs(*this); glslTable->tableType = "glsl";
    pathTable = new Attrs(*this); pathTable->tableType = "path";
}

void MainWin::addOutliner()
{
    dock_outliner = new QDockWidget("o",this);
    dock_outliner->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    dock_outliner_holder = new QWidget();
    QGridLayout *outlinerGridLayout = new QGridLayout(dock_outliner_holder);

    myOutliner = new Outliner(*this);
    outlinerGridLayout->addWidget(myOutliner);

    dock_outliner->setWidget(dock_outliner_holder);
    addDockWidget(Qt::LeftDockWidgetArea, dock_outliner);

    dock_outliner->setFloating(1); //
    dock_outliner->setVisible(1); //

    centerToScreen(dock_outliner);
    glm::vec2 moveRelative(glm::vec2(-650, 200) + toVec2(dock_outliner->pos()));
    dock_outliner->move(moveRelative.x, moveRelative.y);
    dock_outliner->resize(dock_outliner->width() - 100, dock_outliner->height());

    //SHOW / HIDE OUTLINER
    QAction *showHideOutliner = dock_outliner->toggleViewAction();
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
    QShortcut *new_s = new QShortcut(tr("CTRL+N"), this);
    connect(new_s, SIGNAL(activated()), this, SLOT(newScene()));

    ioMenu->addAction("open", this, SLOT(openScene()));
    QShortcut *open_s = new QShortcut(tr("CTRL+O"), this);
    connect(open_s, SIGNAL(activated()), this, SLOT(openScene()));

    ioMenu->addAction("save", this, SLOT(saveScene()));
    QShortcut *save_s = new QShortcut(tr("CTRL+S"), this);
    connect(save_s, SIGNAL(activated()), this, SLOT(saveScene()));

    ioMenu->addAction("saveAs", this, SLOT(saveSceneAs()));
    QShortcut *saveAs_s = new QShortcut(tr("CTRL+SHIFT+S"), this);
    connect(saveAs_s, SIGNAL(activated()), this, SLOT(saveSceneAs()));

    ioMenu->addSeparator();

    ioMenu->addAction("quit", this, SLOT(quitConfirm()));
    QShortcut *quit_s = new QShortcut(tr("CTRL+Q"), this);
    connect(quit_s, SIGNAL(activated()), this, SLOT(quitConfirm()));
}

void MainWin::addActMenu()
{
    actMenu = menuBar()->addMenu("actions");
    actMenu->setTearOffEnabled(1);
    actMenu->setFont(menuSmallF);

    actMenu->addAction("getChild", this, SLOT(getChild()));
    QShortcut *getChild_s = new QShortcut(Qt::Key_Down, this);
    connect(getChild_s, SIGNAL(activated()), this, SLOT(getChild()));

    actMenu->addAction("getParent", this, SLOT(getParent()));
    QShortcut *getParent_s = new QShortcut(Qt::Key_Up, this);
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

    edMenu->addAction("prefs", this, SLOT(PrefWinOpen()));
    QShortcut *PrefWinOpen_s = new QShortcut(Qt::Key_QuoteLeft, this);
    connect(PrefWinOpen_s, SIGNAL(activated()), this, SLOT(PrefWinOpen()));

    edMenu->addAction("color pop", this, SLOT(TglCPopWin()));

    edMenu->addAction("attrTableTgl", this, SLOT(attrTableVizToggle()));
    QShortcut *attrTableTgl_s = new QShortcut(tr("CTRL+A"), this);
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
    QShortcut *deferredPrefs_s = new QShortcut(Qt::Key_F4, this);
    connect(deferredPrefs_s, SIGNAL(activated()), this, SLOT(deferredPrefs()));
}

void MainWin::quitConfirm()
{
    for (unsigned int i = 0; i < allObj.size(); ++i)
        allObj[i]->selected = 1;

    delete_();

    for (unsigned int i = 0; i < allGL.size(); ++i)
        myPP->resizeTexClearMem(allGL[i]);

    myGLWidgetSh->addDeleteShadows("delete");

    qApp->quit();
}

void MainWin::deferredPrefs()
{
    if (myFSQ->selected)
    {
        myFSQ->selected = 0;

        bool foundOldSelB = 0;

        for (unsigned int i = 0; i < allObj.size(); ++i)
        {
            if (allObj[i]->name->val_s == oldSelB && allObj[i]->selected)
            {
                selB = allObj[i];
                foundOldSelB = 1;
            }
        }

        if (!foundOldSelB)
            selB = 0;

        oldSelB = "";
    }

    else
    {
        myFSQ->selected = 1;

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

void MainWin::TglCPopWin()
{
    if (!CPopWinTgl)
    {
        glm::vec2 popP = toVec2(QCursor::pos());
        glm::vec2 showCPopP(popP.x - (myCPopWin->width() / 2), popP.y - (myCPopWin->height() / 2));
        myCPopWin->move(toQP(showCPopP));
        myCPopWin->show();

        CPopWinTgl = 1;
    }

    else
    {
        myCPopWin->hide();
        CPopWinTgl = 0;
        myCPopWin->myCPop->targetAttr = 0;
    }
}

void MainWin::PrefWinOpen()
{
    if (PrefWinTgl)
    {
        myPrefWin->hide();
        PrefWinTgl = 0;
    }

    else
    {
        glm::vec2 prefP = toVec2(QCursor::pos());

        glm::vec2 showPrefP(prefP.x - (myPrefWin->width() / 2), prefP.y - (myPrefWin->height() / 2));
        myPrefWin->move(toQP(showPrefP));
        myPrefWin->show();

        PrefWinTgl = 1;
    }
}

void MainWin::restoreZoomedLayout()
{
    for (unsigned int i = 0; i < allGL.size(); ++i)
    {
        for (int j = 0; j < stackedMain->count(); ++j)
        {
            if (stackedMain->widget(j) == &allGL[i].get()->mySplitV)
            {
                if (layoutType == "gridLay")
                    allGL[i].get()->splitIdx.split->insertWidget(allGL[i].get()->splitIdx.idx, &allGL[i].get()->mySplitV);

                else if (layoutType == "hLay")
                    hLayU->insertWidget(allGL[i].get()->splitIdx.idx, &allGL[i].get()->mySplitV);

                else if (layoutType == "vLay")
                    vLay->insertWidget(allGL[i].get()->splitIdx.idx, &allGL[i].get()->mySplitV);

                allGL[i].get()->mySplitV.show();

                for (unsigned int k = 0; k < allCamCombo.size(); ++k)
                {
                    if (allCamCombo[k]->myGL.get() == allGL[i].get())
                         allCamCombo[k]->myGL.get()->resize(350, 350);
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

    for (unsigned int i = 0; i < allObj.size(); ++i)
        allObj[i]->selected = 0;
}

unsigned int MainWin::countSel()
{
    int count = 0;

    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (allObj[i]->selected)
            count += 1;
    }

    return count;
}

int MainWin::countLights()
{
    int count = 0;

    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (!allObj[i]->ref && allObj[i]->v->val_b && allObj[i]->camLiTypeGet("light"))
            count += 1;
    }

    return count;
}

bool MainWin::tryAddSwitchLight(QString type)
{
    bool lessThanMax = 0;

    if (type == "AREA" || "DIR" || "POINT" || "SPOT")
    {
        if (lightCt < 3)
            lessThanMax = 1;
    }

    else
        lessThanMax = 1;

    return lessThanMax;
}

void MainWin::setLightsDirty()
{
    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (allObj[i]->v->val_b && allObj[i]->type == "CAMLI" && allObj[i]->camLiTypeGet("light"))
            allObj[i]->setDirty();
    }
}

void MainWin::gizInit()
{
    loadO = myGLWidgetSh->VBOup(0, "GIZ", "giz", 0);
    for (unsigned int i = 0; i < loadO.size(); ++i)
    {
        loadO[i]->v->val_b = 0;
        loadO[i]->ignoreOutliner = 1;
        loadO[i]->deletable = 0;
        loadO[i]->selectable = 0;
        allObj.push_back(loadO[i]);
        myGizNull = loadO[i]; // parent to this
    }

    float dualP = .5f; // dual axis handle P

    GizSetup myGizSetup[] =
    {
        { "GIZ_CIRC", "gizCirc", {.2f, .2f, .2f} },
        { "GIZ_CIRC_HALF", "gizCircHalfX", {1.f, 0.f, 0.f} },
        { "GIZ_CIRC_HALF", "gizCircHalfY", {0.f, 1.f, 0.f} },
        { "GIZ_CIRC_HALF", "gizCircHalfZ", {0.f, 0.f, 1.f} },

        { "GIZ_CONE", "gizConeX", {1.f, 0.f, 0.f} },
        { "GIZ_CONE", "gizConeY", {0.f, 1.f, 0.f} },
        { "GIZ_CONE", "gizConeZ", {0.f, 0.f, 1.f} },

        { "GIZ_CUBE", "gizCubeX", {1.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {1.f, 0.f, 0.f} },
        { "GIZ_CUBE", "gizCubeY", {0.f, 1.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} },
        { "GIZ_CUBE", "gizCubeZ", {0.f, 0.f, 1.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f} },
        { "GIZ_CUBE", "gizCubeXYZ", {0.f, 0.f, 0.f} },

        { "GIZ_DUAL_HANDLE", "gizDualXZ", {0.f, 1.f, 0.f}, {90.f, 0.f, 0.f}, {dualP, 0.f, dualP} },
        { "GIZ_DUAL_HANDLE", "gizDualXY", {0.f, 0.f, 1.f}, {0.f, 0.f, 0.f}, {dualP, dualP, 0.f} },
        { "GIZ_DUAL_HANDLE", "gizDualYZ", {1.f, 0.f, 0.f}, {0.f, 90.f, 0.f}, {0.f, dualP, dualP} },

        { "GIZ_LINE", "gizLineX", {1.f, 0.f, 0.f} },
        { "GIZ_LINE", "gizLineY", {0.f, 1.f, 0.f} },
        { "GIZ_LINE", "gizLineZ", {0.f, 0.f, 1.f} },
    };

    for (unsigned int i = 0; i < arraySize(myGizSetup); ++i)
    {
        loadO = myGLWidgetSh->VBOup(0, myGizSetup[i].type, myGizSetup[i].name, 0);

        for (unsigned int j = 0; j < loadO.size(); ++j)
        {
            loadO[j]->Cgiz = myGizSetup[i].Cgiz;
            loadO[j]->Cgiz_stored = myGizSetup[i].Cgiz;

            if (myGizSetup[i].type == "GIZ_CUBE") loadO[j]->s->val_3 = glm::vec3(.15f);
            else if (myGizSetup[i].type == "GIZ_DUAL_HANDLE") loadO[j]->s->val_3 = glm::vec3(.1f);

            loadO[j]->r->val_3 = myGizSetup[i].r;
            loadO[j]->t->val_3 = myGizSetup[i].t;
            loadO[j]->piv->val_3 = myGizSetup[i].t;
            loadO[j]->ssaoTgl->val_b = 0;
            loadO[j]->deletable = 0;
            loadO[j]->selectable = 0;
            loadO[j]->ignoreOutliner = 1;
            loadO[j]->parentObj("giz");
            allObj.push_back(loadO[j]);
            allGiz.push_back(loadO[j]);
        }
    }

    GizSetup myGizSetupSide[] =
    {
        { "GIZ_LINE", "gizLineXSide", {1.f, 0.f, 0.f} },
        { "GIZ_LINE", "gizLineYSide", {0.f, 1.f, 0.f} },
        { "GIZ_LINE", "gizLineZSide", {0.f, 0.f, 1.f} },
    };

    for (unsigned int i = 0; i < arraySize(myGizSetupSide); ++i)
    {
        loadO = myGLWidgetSh->VBOup(0, myGizSetupSide[i].type, myGizSetupSide[i].name, 0);

        for (unsigned int j = 0; j < loadO.size(); ++j)
        {
            loadO[j]->Cgiz = myGizSetupSide[i].Cgiz;
            loadO[j]->Cgiz_stored = myGizSetupSide[i].Cgiz;
            loadO[j]->ssaoTgl->val_b = 0;
            loadO[j]->deletable = 0;
            loadO[j]->selectable = 0;
            loadO[j]->ignoreOutliner = 1;
            loadO[j]->gizSideObj = 1;
            allObj.push_back(loadO[j]);
            allGizSide.push_back(loadO[j]);
        }
    }
}

void MainWin::gizShow(string gizTypeShow)
{
    if (selB)
    {
        for (unsigned int i = 0; i < allObj.size(); ++i)
        {
            QString type = allObj[i]->type;

            if (gizTypeShow == "T")
            {
                if (type == "GIZ_CIRC") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_CIRC_HALF") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_CONE") allObj[i]->v->val_b = 1;
                else if (type == "GIZ_CUBE") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_DUAL_HANDLE") allObj[i]->v->val_b = 1;
                else if (type == "GIZ_LINE") allObj[i]->v->val_b = 1;
            }

            else if (gizTypeShow == "R")
            {
                if (type == "GIZ_CIRC") allObj[i]->v->val_b = 1;
                else if (type == "GIZ_CIRC_HALF") allObj[i]->v->val_b = 1;
                else if (type == "GIZ_CONE") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_CUBE") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_DUAL_HANDLE") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_LINE") allObj[i]->v->val_b = 0;
            }

            else if (gizTypeShow == "S")
            {
                if (type == "GIZ_CIRC") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_CIRC_HALF") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_CONE") allObj[i]->v->val_b = 0;
                else if (type == "GIZ_CUBE" && allObj[i] != myPivot) allObj[i]->v->val_b = 1;
                else if (type == "GIZ_DUAL_HANDLE") allObj[i]->v->val_b = 1;
                else if (type == "GIZ_LINE") allObj[i]->v->val_b = 1;
            }
        }

        myGizNull->parentTo = selB;
        myGizNull->v->val_b = 1;
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
        myGizNull->v->val_b = 0;
        myPivot->v->val_b = 0;
    }
}

void MainWin::gizSpaceSwitch()
{
    gizSpace = (gizSpace == "local") ? "world" : "local";
}

void MainWin::newScene()
{
    qDebug() << "newScene";
}

void MainWin::openScene()
{
    qDebug() << "openScene";
}

void MainWin::saveScene()
{
    qDebug() << "saveScene";

    QString savedSceneName = "abj - ";
    savedSceneName += "savedSceneName";

    setWindowTitle(savedSceneName);
}

void MainWin::saveSceneAs()
{
    qDebug() << "saveSceneAs";
}

void MainWin::pivMatchTest()
{
    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (allObj[i]->selected)
        {
            allObj[i]->piv->val_3 = glm::vec3(2.f, 2.f, 0.f);
            //allObj[i]->piv->val_3 = glm::vec3(2.f, 1.414f, -1.414f);

            glm::vec3 t(allObj[i]->t->val_3);
            glm::vec3 r(allObj[i]->r->val_3);
            glm::vec3 piv(allObj[i]->piv->val_3);
            glm::vec3 pivRt(allObj[i]->pivRt);

            cout << "t = " << glm::to_string(t) << endl;
    //            cout << "rp_os = " << glm::to_string(t - piv) << endl;
            cout << "rp_os = " << glm::to_string(piv - t) << endl;
            cout << "rp_ws = " << glm::to_string(piv) << endl;
            cout << "rt_os = " << glm::to_string(t - pivRt) << endl;
            cout << "rt_ws = " << glm::to_string(pivRt) << endl;
    //            cout << "ra = " << glm::to_string(allObj[i]->t->val_3) << endl; //axis
            cout << "ro = " << glm::to_string(r) << endl;
    //            cout << "roo = " << glm::to_string(allObj[i]->r->val_3) << endl; //xyz

            cout << "MM[0] = " << glm::to_string(allObj[i]->MM[0]) << endl;
            cout << "MM[1] = " << glm::to_string(allObj[i]->MM[1]) << endl;
            cout << "MM[2] = " << glm::to_string(allObj[i]->MM[2]) << endl;
            cout << "MM[3] = " << glm::to_string(allObj[i]->MM[3]) << endl << endl;
        }
    }
}

void MainWin::pivCenter()
{
//    pivMatchTest();

//    for (int i = 0; i < allObj.size(); ++i)
//    {
//        if (allObj[i]->selected)
//        {
//            glm::vec3 cent = glm::vec3(5.f, 0.f, -2.f);
//            allObj[i]->piv->val_3 = glm::vec3(cent);

//            glm::mat4 oM = allObj[i]->TM * allObj[i]->RM * allObj[i]->SM;

//            glm::mat4 invM = glm::inverse(allObj[i]->MM);
////            glm::mat4 invM = glm::inverse(oM);

//            glm::vec3 off = glm::mat3(invM) * cent;
////            off = -off;
//            cout << "off = " << glm::to_string(off) << endl;

//            glm::vec3 pivMovePlusOff(cent - off);
//            cout << "pivMovePlusOff = " << glm::to_string(pivMovePlusOff) << endl;

////            allObj[i]->pivRtM = glm::translate(glm::mat4(), cent);
//            allObj[i]->pivRtM = glm::translate(glm::mat4(), pivMovePlusOff);
//        }
//    }
}

//void MainWin::pivCenter() //good
//{
//    for (int i = 0; i < allObj.size(); ++i)
//    {
//        if (allObj[i]->selected)
//        {
//            glm::mat4 centerM = glm::translate(allObj[i]->bbCenter) * allObj[i]->MM;
//            glm::vec3 center = glm::vec3(centerM[3].x, centerM[3].y, centerM[3].z);

//            //Blender
//            allObj[i]->t->val_3 = center;
//            allObj[i]->piv->val_3 = glm::vec3(0.f);
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
    myPivot->v->val_b = 0;
}

void MainWin::getChild()
{
    if (selB)
    {
        for (unsigned int i = 0; i < allObj.size(); ++i)
        {
            if (allObj[i]->parentTo == selB)
            {
                clearSel();
                allObj[i]->selected = 1;
                selB = allObj[i];
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
        for (unsigned int i = 0; i < allObj.size(); ++i)
        {
            if (selB->parentTo == allObj[i])
            {
                clearSel();
                allObj[i]->selected = 1;
                selB = allObj[i];
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
                bool found = 0;
                int newIdx;

                for (unsigned int i = 0; i < allObj.size(); ++i)
                {
                    if (obj->parentTo->parentTo == allObj[i])
                    {
                        found = 1;
                        newIdx = i;

                        continue;
                    }
                }

                if (!found)
                    searchB = 1;

                else
                    searchUp_intermediate(allObj[newIdx]);
            }

            else
                searchB = 0;
        }

        else
            searchB = 1;
    }

    else
        searchB = 0;
}

bool MainWin::searchUp(shared_ptr<Object> obj)
{
    searchUp_intermediate(obj);

    if (searchB)
        return 1;

    else
        return 0;
}

void MainWin::delete_()
{
    myGizNull->v->val_b = 0;
    myGizNull->parentTo = 0;
    myPivot->parentTo = 0;
    bool needsShadowRefresh = 0;

    pivHide();

    for (unsigned int i = 0; i < allObj.size(); ++i) //select downstream as well
    {
        if (allObj[i]->selected)
        {
            for (unsigned int j = 0; j < allObj.size(); ++j)
            {
                if (allObj[j]->parentTo == allObj[i])
                    allObj[j]->selected = 1;
            }
        }
    }

    for (vector<shared_ptr<Object>>::iterator it = allObj.begin(); it != allObj.end();)
    {
        if ((*it)->deletable && (*it)->selected)
        {
            for (unsigned int i = 0; i < allObj.size(); ++i) //reset dupeStenFix
            {
                if ((*it)->dupeStenFix == allObj[i]->name->val_s)
                {
                    allObj[i]->dupeStenFix = "";
                    allObj[i]->v->val_b = 1;
                }
            }

            if ((*it)->camLiTypeGet("light"))
                needsShadowRefresh = 1;

            (*it)->delete_();
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

    for (unsigned int i = 0; i < allCamCombo.size(); ++i)
        allCamCombo[i]->refresh();
}

void MainWin::dupe()
{
    if (selB)
    {
        int GLidx;

        for (unsigned int i = 0; i < allGL.size(); ++i)
        {
            if (allGL[i].get()->isVisible())
            {
                GLidx = i;
                break;
            }
        }

        bool gizWasOn = 0;

        if (myGizNull->v->val_b)
        {
            myGizNull->v->val_b = 0;
            myGizNull->parentTo = 0;

            gizWasOn = 1;
        }

        vector<shared_ptr<Object>> dupeTemp;

        for (unsigned int i = 0; i < allObj.size(); ++i)
        {
            if (allObj[i]->selected)
                dupeTemp.push_back(allObj[i]);
        }

        for (unsigned int i = 0; i < dupeTemp.size(); ++i)
        {
            loadO = myGLWidgetSh->VBOup(0, dupeTemp[i]->type, dupeTemp[i]->name->val_s, make_shared<Object>(*dupeTemp[i]));

            bool changedLight = 0;

            if (loadO[0]->camLiTypeGet("light"))
            {
                if (!tryAddSwitchLight(loadO[0]->camLiType->val_s))
                    loadO[0]->camLiType->val_s = "PERSP";

                changedLight = 1;
            }

            loadO[0]->dupeStenFix = dupeTemp[i]->name->val_s;

            allObj.push_back(loadO[0]);
            loadO[0]->loadVAO(allGL[GLidx]);
            selB = loadO[0];
            dupeTemp[i]->selected = 0;

            if (changedLight)
                lightCt = countLights();
        }

        if (gizWasOn)
        {
            myGizNull->v->val_b = 1;
            myGizNull->parentTo = selB;
        }

        myOutliner->refreshOutliner(1);
        attrTable->refreshTable();

        for (unsigned int i = 0; i < allCamCombo.size(); ++i)
            allCamCombo[i]->refresh();

        setLightsDirty();
    }
}

void MainWin::hide()
{
    bool changedLight = 0;

    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (allObj[i]->selected)
        {
            allObj[i]->v->val_b = 0; //turn off viz

            if (allObj[i]->camLiTypeGet("light"))
            {
                allObj[i]->setDirty();
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
    bool changedLight = 0;

    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (allObj[i]->selected)
        {
            if (allObj[i]->camLiTypeGet("light"))
            {
                if (!tryAddSwitchLight(allObj[i]->camLiType->val_s))
                    allObj[i]->camLiType->val_s = "PERSP";

                allObj[i]->setDirty();
                changedLight = 1;
            }

            allObj[i]->v->val_b = 1;
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

        for (unsigned int i = 0; i < allObj.size(); ++i)
        {
            if (!allObj[i]->ignoreOutliner)
            {
                if (allObj[i]->selected)
                    allObj[i]->selected = 0;

                else
                {
                    allObj[i]->selected = 1;
                    selB = allObj[i];
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
        for (unsigned int i = 0; i < allObj.size(); ++i)
        {
            if (allObj[i]->selected)
            {
                if (allObj[i]->parentTo && allObj[i]->parentTo != selB)
                {
                    if (allObj[i] != selB)
                        allObj[i]->parentTo = selB;
                }

                else
                {
                    if (allObj[i] != selB)
                        allObj[i]->parentTo = selB;
                }
            }
        }

        myOutliner->refreshOutliner(0);
        attrTable->refreshTable();
    }
}

void MainWin::parentWorld()
{
    for (unsigned int i = 0; i < allObj.size(); ++i)
    {
        if (allObj[i]->selected)
            allObj[i]->parentTo = 0;
    }

    myOutliner->refreshOutliner(0);
    attrTable->refreshTable();
}

void MainWin::centerToScreen(QWidget *widget)
{
    QDesktopWidget *dw = QApplication::desktop();
    QRect desk_rect = dw->screenGeometry(dw->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}

double MainWin::GetTickCount2()
{
#if __unix__
    struct timespec now;

    if (clock_gettime(CLOCK_MONOTONIC, &now))
        return 0;

    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
#else
    return GetTickCount();
#endif
}
