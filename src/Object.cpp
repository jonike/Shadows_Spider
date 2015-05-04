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

#include "Object.h"

Object::Object(MainWin &myWinTemp) : myWin(myWinTemp)
{
    name = make_shared<MultiAttr>();
    name->name = "name";
    name->type = "QString";
    name->val_s = "renameMe";

    //TRANSFORM
    transformSep = make_shared<MultiAttr>();
    transformSep->name = "transform";
    transformSep->type = "sep";
    multiObj.push_back(transformSep);

    t = make_shared<MultiAttr>();
    t->name = "t";
    t->type = "vec3";
    t->val_3 = glm::vec3(0.f);
    multiObj.push_back(t);

    r = make_shared<MultiAttr>();
    r->name = "r";
    r->type = "vec3";
    r->val_3 = glm::vec3(0.f);
    multiObj.push_back(r);

    s = make_shared<MultiAttr>();
    s->name = "s";
    s->type = "vec3";
    s->val_3 = glm::vec3(1.f);
    multiObj.push_back(s);

    v = make_shared<MultiAttr>();
    v->name = "v";
    v->type = "bool";
    v->val_b = 1;
    multiObj.push_back(v);

    bb = make_shared<MultiAttr>();
    bb->name = "bb";
    bb->type = "bool";
    bb->val_b = 0;
    multiObj.push_back(bb);

    //GLSL
    shaderSep = make_shared<MultiAttr>();
    shaderSep->name = "shader";
    shaderSep->type = "sep";
    multiObj.push_back(shaderSep);

    albedoM = make_shared<MultiAttr>();
    albedoM->name = "albedoM";
    albedoM->type = "enum";
    albedoM->typeX = "OBJ";
    albedoM->val_s = "BLANK";
    multiObj.push_back(albedoM);

    alphaM = make_shared<MultiAttr>();
    alphaM->name = "alphaM";
    alphaM->type = "enum";
    alphaM->typeX = "OBJ";
    alphaM->val_s = "BLANK";
    multiObj.push_back(alphaM);

    anisoM = make_shared<MultiAttr>();
    anisoM->name = "anisoM";
    anisoM->type = "enum";
    anisoM->typeX = "OBJ";
    anisoM->val_s = "BLANK";
    multiObj.push_back(anisoM);

    cubeM = make_shared<MultiAttr>();
    cubeM->name = "cubeM";
    cubeM->type = "enum";
    cubeM->typeX = "FBO";
//    cubeM->val_s = "uffizi";
    cubeM->val_s = "ennis";
    multiObj.push_back(cubeM);

    lensM = make_shared<MultiAttr>();
    lensM->name = "lensM";
    lensM->type = "enum";
    lensM->typeX = "FBO";
    lensM->val_s = "abjLens1";
    multiObj.push_back(lensM);

    metallicM = make_shared<MultiAttr>();
    metallicM->name = "metallicM";
    metallicM->type = "enum";
    metallicM->typeX = "OBJ";
    metallicM->val_s = "BLANK";
    multiObj.push_back(metallicM);

    normalM = make_shared<MultiAttr>();
    normalM->name = "normalM";
    normalM->type = "enum";
    normalM->typeX = "OBJ";
//    normalM->val_s = "BLANK";
    normalM->val_s = "voronoi";
    multiObj.push_back(normalM);

    ruffM = make_shared<MultiAttr>();
    ruffM->name = "ruffM";
    ruffM->type = "enum";
    ruffM->typeX = "OBJ";
    ruffM->val_s = "BLANK";
    multiObj.push_back(ruffM);

    sssM = make_shared<MultiAttr>();
    sssM->name = "sssM";
    sssM->type = "enum";
    sssM->typeX = "OBJ";
    sssM->val_s = "BLANK";
    multiObj.push_back(sssM);

        //TILE
        tileSep = make_shared<MultiAttr>();
        tileSep->name = "tile";
        tileSep->type = "sep";
        tileSep->tab = 1;
        multiObj.push_back(tileSep);

        tileMaps(0, "set");

    //BASE
    Kr = make_shared<MultiAttr>();
    Kr->name = "Kr";
    Kr->type = "float";
    Kr->typeX = "OBJ";
    Kr->val_f = .3f;
    Kr->min = 0.008f;
    Kr->max = 1.f;
    multiObj.push_back(Kr);

    Ksss = make_shared<MultiAttr>();
    Ksss->name = "Ksss";
    Ksss->type = "float";
    Ksss->typeX = "OBJ";
    Ksss->val_f = 10.f;
    Ksss->min = 0.f;
    multiObj.push_back(Ksss);

    Ko = make_shared<MultiAttr>();
    Ko->name = "Ko";
    Ko->type = "float";
    Ko->typeX = "OBJ";
    Ko->val_f = 1.f;
    Ko->min = 0.f;
    Ko->max = 1.f;
    multiObj.push_back(Ko);

    ior = make_shared<MultiAttr>();
    ior->name = "ior";
    ior->type = "float";
    ior->typeX = "OBJ";
    ior->val_f = 1.5f;
    ior->min = 1.f;
    ior->max = 30.f;
    multiObj.push_back(ior);

    ruffA = make_shared<MultiAttr>();
    ruffA->name = "ruffA";
    ruffA->type = "vec2";
    ruffA->typeX = "OBJ";
    ruffA->val_2 = glm::vec2(.3f, .07f);
    ruffA->min = .001f;
    ruffA->max = 1.f;
    multiObj.push_back(ruffA);

    ruffD = make_shared<MultiAttr>();
    ruffD->name = "ruffD";
    ruffD->type = "float";
    ruffD->typeX = "OBJ";
    ruffD->val_f = .5f;
    ruffD->min = 0.f;
    ruffD->max = 1.f;
    multiObj.push_back(ruffD);

    sssSpread = make_shared<MultiAttr>();
    sssSpread->name = "sssSpread";
    sssSpread->type = "float";
    sssSpread->typeX = "OBJ";
    sssSpread->val_f = .15f;
    sssSpread->min = .001f;
    sssSpread->max = 1.f;
    multiObj.push_back(sssSpread);

        shadowCast = make_shared<MultiAttr>();
        shadowCast->name = "shadowCast";
        shadowCast->type = "bool";
        shadowCast->tab = 1;
        shadowCast->val_b = 1;
        multiObj.push_back(shadowCast);

        backface = make_shared<MultiAttr>();
        backface->name = "backface";
        backface->type = "bool";
        backface->tab = 1;
        backface->val_b = 0;
        multiObj.push_back(backface);

        Cwire = make_shared<MultiAttr>();
        Cwire->name = "Cwire";
        Cwire->type = "color";
        Cwire->typeX = "OBJ";
        Cwire->val_3 = glm::vec3(0.f);
        Cwire->tab = 1;
        multiObj.push_back(Cwire);

        normVLen = make_shared<MultiAttr>();
        normVLen->name = "normVLen";
        normVLen->type = "float";
        normVLen->typeX = "OBJ";
        normVLen->val_f = .3f;
        normVLen->min = 0.f;
        normVLen->max = 1.f;
        normVLen->tab = 1;
        multiObj.push_back(normVLen);

        normWeight = make_shared<MultiAttr>();
        normWeight->name = "normWeight";
        normWeight->type = "float";
        normWeight->typeX = "OBJ";
        normWeight->val_f = 1.f;
        normWeight->min = 0.f;
        normWeight->max = 1.f;
        normWeight->tab = 1;
        multiObj.push_back(normWeight);

        rotOrder = make_shared<MultiAttr>();
        rotOrder->name = "rotOrder";
        rotOrder->type = "enum";
        rotOrder->val_s = "XZY";
        rotOrder->comboList << "XYZ" << "YZX" << "ZXY" << "XZY" << "YXZ" << "ZYX";
        rotOrder->repop = 0;
        rotOrder->tab = 1;
        multiObj.push_back(rotOrder);

        piv = make_shared<MultiAttr>();
        piv->name = "piv";
        piv->type = "vec3";
        piv->val_3 = glm::vec3(0.f);
        piv->tab = 1;
        multiObj.push_back(piv);

    //CAMLI
    camLiType = make_shared<MultiAttr>();
    camLiType->name = "camLiType";
    camLiType->type = "enum";
    camLiType->typeX = "CAMLI";
    camLiType->val_s = "PERSP";
    camLiType->comboList << "DIR" << "FPS" << "ORTHO" << "PERSP" << "POINT" << "SPOT";
    camLiType->repop = 0;
    multiObj.push_back(camLiType);

    camSep = make_shared<MultiAttr>();
    camSep->name = "cam";
    camSep->type = "sep";
    camSep->typeX = "CAMLI";
    multiObj.push_back(camSep);

    fov = make_shared<MultiAttr>();
    fov->name = "fov";
    fov->type = "float";
    fov->typeX = "CAMLI";
    fov->val_f = 50.f;
    fov->min = 1.f;
    multiObj.push_back(fov);

    nearClip = make_shared<MultiAttr>();
    nearClip->name = "nearClip";
    nearClip->type = "float";
    nearClip->typeX = "CAMLI";
    nearClip->val_f = .01f;
    nearClip->min = .01f;
    nearClip->tab = 1;
    multiObj.push_back(nearClip);

    farClip = make_shared<MultiAttr>();
    farClip->name = "farClip";
    farClip->type = "float";
    farClip->typeX = "CAMLI";
    farClip->val_f = 100.f;
    farClip->min = .01f;
    farClip->tab = 1;
    multiObj.push_back(farClip);

    nearShadow = make_shared<MultiAttr>();
    nearShadow->name = "nearShadow";
    nearShadow->type = "float";
    nearShadow->typeX = "CAMLI";
    nearShadow->tab = 1;
    nearShadow->val_f = 2.f;
    nearShadow->min = 0.f;
    multiObj.push_back(nearShadow);

    farShadow = make_shared<MultiAttr>();
    farShadow->name = "farShadow";
    farShadow->type = "float";
    farShadow->typeX = "CAMLI";
    farShadow->tab = 1;
    farShadow->val_f = 100.f;
    multiObj.push_back(farShadow);

    orthoFree = make_shared<MultiAttr>();
    orthoFree->name = "orthoFree";
    orthoFree->type = "bool";
    orthoFree->typeX = "CAMLI";
    orthoFree->val_b = 0;
    multiObj.push_back(orthoFree);

    orthoType = make_shared<MultiAttr>();
    orthoType->name = "orthoType";
    orthoType->type = "enum";
    orthoType->typeX = "CAMLI";
    orthoType->val_s = "TOP";
    orthoType->comboList << "BACK" << "BOTTOM" << "FRONT" << "LEFT" << "RIGHT" << "TOP";
    orthoType->repop = 0;
    multiObj.push_back(orthoType);

    orthoZoom = make_shared<MultiAttr>();
    orthoZoom->name = "orthoZoom";
    orthoZoom->type = "float";
    orthoZoom->typeX = "CAMLI";
    orthoZoom->val_f = 2.f;
    orthoZoom->min = .001f;
    multiObj.push_back(orthoZoom);

    // LIGHT
    lightSep = make_shared<MultiAttr>();
    lightSep->name = "light";
    lightSep->type = "sep";
    lightSep->typeX = "CAMLI";
    multiObj.push_back(lightSep);

    Cl = make_shared<MultiAttr>();
    Cl->name = "Cl";
    Cl->type = "color";
    Cl->typeX = "CAMLI";
    Cl->val_3 = glm::vec3(1.f);
    multiObj.push_back(Cl);

    lInten = make_shared<MultiAttr>();
    lInten->name = "lInten";
    lInten->type = "float";
    lInten->typeX = "CAMLI";
    lInten->val_f = 3.f; //
    lInten->min = 0.f;
    multiObj.push_back(lInten);

    lSpotI = make_shared<MultiAttr>();
    lSpotI->name = "lSpotI";
    lSpotI->type = "float";
    lSpotI->typeX = "CAMLI";
    lSpotI->val_f = 10.f;
    lSpotI->min = 0.f;
    multiObj.push_back(lSpotI);

    lSpotO = make_shared<MultiAttr>();
    lSpotO->name = "lSpotO";
    lSpotO->type = "float";
    lSpotO->typeX = "CAMLI";
    lSpotO->val_f = 30.f;
    multiObj.push_back(lSpotO);

    //VOLUME LIGHT
    volSep = make_shared<MultiAttr>();
    volSep->name = "vol";
    volSep->type = "sep";
    volSep->typeX = "CAMLI";
    multiObj.push_back(volSep);

    volCone = make_shared<MultiAttr>();
    volCone->name = "volCone";
    volCone->type = "bool";
    volCone->typeX = "CAMLI";
    volCone->val_b = 0;
    multiObj.push_back(volCone);

    volDist = make_shared<MultiAttr>();
    volDist->name = "volDist";
    volDist->type = "float";
    volDist->typeX = "CAMLI";
    volDist->val_f = 5.5f;
    volDist->min = 0.f;
    multiObj.push_back(volDist);

    volS = make_shared<MultiAttr>();
    volS->name = "volS";
    volS->type = "vec3";
    volS->typeX = "CAMLI";
    volS->val_3 = glm::vec3(1.f);
    multiObj.push_back(volS);

    /* FSQ */
    //BLOOM
    bloomSep = make_shared<MultiAttr>();
    bloomSep->name = "bloom";
    bloomSep->type = "sep";
    bloomSep->typeX = "FBO";
    multiObj.push_back(bloomSep);

    bloomI = make_shared<MultiAttr>();
    bloomI->name = "bloomI";
    bloomI->type = "float";
    bloomI->typeX = "FBO";
    bloomI->val_f = .1f;
    bloomI->min = 0.f;
    bloomI->max = 1.f;
    multiObj.push_back(bloomI);

    bloomLensI = make_shared<MultiAttr>();
    bloomLensI->name = "bloomLensI";
    bloomLensI->type = "float";
    bloomLensI->typeX = "FBO";
    bloomLensI->val_f = .2f;
    bloomLensI->min = 0.f;
    bloomLensI->max = 1.f;
    multiObj.push_back(bloomLensI);

    // HDR
    hdrSep = make_shared<MultiAttr>();
    hdrSep->name = "hdr";
    hdrSep->type = "sep";
    hdrSep->typeX = "FBO";
    multiObj.push_back(hdrSep);

    adaptTime = make_shared<MultiAttr>();
    adaptTime->name = "adaptTime";
    adaptTime->type = "float";
    adaptTime->typeX = "FBO";
    adaptTime->val_f = .5f;
    adaptTime->min = 0.f;
    multiObj.push_back(adaptTime);

    adaptAuto = make_shared<MultiAttr>();
    adaptAuto->name = "adaptAuto";
    adaptAuto->type = "bool";
    adaptAuto->typeX = "FBO";
    adaptAuto->val_b = 0;
    multiObj.push_back(adaptAuto);

    expo = make_shared<MultiAttr>();
    expo->name = "expo";
    expo->type = "float";
    expo->typeX = "FBO";
    expo->val_f = 1.1f;
    expo->min = 1.001f;
    multiObj.push_back(expo);

    vignette = make_shared<MultiAttr>();
    vignette->name = "vignette";
    vignette->type = "bool";
    vignette->typeX = "FBO";
    vignette->val_b = 0;
    multiObj.push_back(vignette);

    vignetteD = make_shared<MultiAttr>();
    vignetteD->name = "vignetteD";
    vignetteD->type = "float";
    vignetteD->typeX = "FBO";
    vignetteD->val_f = 1.5f;
    vignetteD->min = 1.3f;
    multiObj.push_back(vignetteD);

        // FXAA
        fxaaSep = make_shared<MultiAttr>();
        fxaaSep->name = "fxaa";
        fxaaSep->type = "sep";
        fxaaSep->typeX = "FBO";
        fxaaSep->tab = 1;
        multiObj.push_back(fxaaSep);

        fxaaBlur = make_shared<MultiAttr>();
        fxaaBlur->name = "fxaaBlur";
        fxaaBlur->type = "bool";
        fxaaBlur->typeX = "FBO";
        fxaaBlur->val_b = 1;
        fxaaBlur->tab = 1;
        multiObj.push_back(fxaaBlur);

        fxaaSubPix = make_shared<MultiAttr>();
        fxaaSubPix->name = "fxaaSubPix";
        fxaaSubPix->type = "float";
        fxaaSubPix->typeX = "FBO";
        fxaaSubPix->val_f = .75f;
        fxaaSubPix->min = .001f;
        fxaaSubPix->tab = 1;
        multiObj.push_back(fxaaSubPix);

        fxaaEdgeThr = make_shared<MultiAttr>();
        fxaaEdgeThr->name = "fxaaEdgeThr";
        fxaaEdgeThr->type = "float";
        fxaaEdgeThr->typeX = "FBO";
        fxaaEdgeThr->val_f = .125f;
        fxaaEdgeThr->min = .001f;
        fxaaEdgeThr->tab = 1;
        multiObj.push_back(fxaaEdgeThr);

        fxaaEdgeThrMin = make_shared<MultiAttr>();
        fxaaEdgeThrMin->name = "fxaaEdgeThrMin";
        fxaaEdgeThrMin->type = "float";
        fxaaEdgeThrMin->typeX = "FBO";
        fxaaEdgeThrMin->val_f = .0833f;
        fxaaEdgeThrMin->min = .001f;
        fxaaEdgeThrMin->tab = 1;
        multiObj.push_back(fxaaEdgeThrMin);

    // SSAO
    ssaoSep = make_shared<MultiAttr>();
    ssaoSep->name = "ssao";
    ssaoSep->type = "sep";
    ssaoSep->typeX = "FBO";
    multiObj.push_back(ssaoSep);

    ssaoBias = make_shared<MultiAttr>();
    ssaoBias->name = "ssaoBias";
    ssaoBias->type = "float";
    ssaoBias->typeX = "FBO";
    ssaoBias->val_f = .1f;
    ssaoBias->min = 0.f;
    ssaoBias->max = 1.f;
    multiObj.push_back(ssaoBias);

    ssaoBlur = make_shared<MultiAttr>();
    ssaoBlur->name = "ssaoBlur";
    ssaoBlur->type = "bool";
    ssaoBlur->typeX = "FBO";
    ssaoBlur->val_b = 1;
    multiObj.push_back(ssaoBlur);

    ssaoInten = make_shared<MultiAttr>();
    ssaoInten->name = "ssaoInten";
    ssaoInten->type = "float";
    ssaoInten->typeX = "FBO";
    ssaoInten->val_f = 5.f;
    ssaoInten->min = 0.f;
    multiObj.push_back(ssaoInten);

    ssaoRad = make_shared<MultiAttr>();
    ssaoRad->name = "ssaoRad";
    ssaoRad->type = "float";
    ssaoRad->typeX = "FBO";
    ssaoRad->val_f = .25f;
    ssaoRad->min = .001f;
    multiObj.push_back(ssaoRad);

    ssaoRand = make_shared<MultiAttr>();
    ssaoRand->name = "ssaoRand";
    ssaoRand->type = "int";
    ssaoRand->typeX = "FBO";
    ssaoRand->val_i = 4;
    ssaoRand->min = 2;
    ssaoRand->max = 64;
    multiObj.push_back(ssaoRand);

    populateTexCombos();

    //UPDATE
    dirtyShadow = dirtyVM = 1;

    //ETC
    expand = gridV = ignoreOutliner = ref = selected = 0;
    deletable = selectable = 1;
    type = "EMPTY";

    //GLSL
    biasM = glm::mat4(
        .5f, 0.f, 0.f, 0.f,
        0.f, .5f, 0.f, 0.f,
        0.f, 0.f, .5f, 0.f,
        .5f, .5f, .5f, 1.f);

    Crand = genColorID();
    showN = 0;
    nType = 0;

    //GIZ
    Cgiz = glm::vec3(0.f);
    Cgiz_stored = glm::vec3(0.f);

    pivRt = glm::vec3(0.f);
    vertsOnObj = 0;

    distO = 0.f;
    lookO = glm::vec3(0.f, 0.f, -1.f);
    rightO = glm::vec3(1.f, 0.f, 0.f);
    targO = glm::vec3(0.f);
    upO = glm::vec3(0.f, 1.f, 0.f);
    lookFPS = glm::normalize(t->val_3);

    //TXT
    txtOrigin = glm::vec2(-.95f, .8f);
}

Object::Object(const Object &obj) : myWin(obj.myWin) //COPY CONSTRUCTOR
{
    name = obj.name->Clone();
    multiObj.resize(obj.multiObj.size());

    for (unsigned int i = 0; i < obj.multiObj.size(); ++i)
    {
        multiObj[i] = obj.multiObj[i]->Clone();

        if (multiObj[i]->name == obj.multiObj[i]->name)
        {
            //TRANSFORM
            if (multiObj[i]->name == "transform") transformSep = multiObj[i];
            else if (multiObj[i]->name == "t") t = multiObj[i];
            else if (multiObj[i]->name == "r") r = multiObj[i];
            else if (multiObj[i]->name == "s") s = multiObj[i];
            else if (multiObj[i]->name == "v") v = multiObj[i];
            else if (multiObj[i]->name == "bb") bb = multiObj[i];

            //GLSL
            else if (multiObj[i]->name == "shader") shaderSep = multiObj[i];
            else if (multiObj[i]->name == "albedoM") albedoM = multiObj[i];
            else if (multiObj[i]->name == "alphaM") alphaM = multiObj[i];
            else if (multiObj[i]->name == "anisoM") anisoM = multiObj[i];
            else if (multiObj[i]->name == "metallicM") metallicM = multiObj[i];
            else if (multiObj[i]->name == "normalM") normalM = multiObj[i];
            else if (multiObj[i]->name == "ruffM") ruffM = multiObj[i];
            else if (multiObj[i]->name == "sssM") sssM = multiObj[i];

                //sep
                else if (multiObj[i]->name == "tile") tileSep = multiObj[i];
                else if (multiObj[i]->name == "albTile") albTile = multiObj[i];
                else if (multiObj[i]->name == "anisoTile") anisoTile = multiObj[i];
                else if (multiObj[i]->name == "normTile") normTile = multiObj[i];
                else if (multiObj[i]->name == "ruffTile") ruffTile = multiObj[i];
                else if (multiObj[i]->name == "sssTile") sssTile = multiObj[i];

            //BASE
            else if (multiObj[i]->name == "Kr") Kr = multiObj[i];
            else if (multiObj[i]->name == "Ksss") Ksss = multiObj[i];
            else if (multiObj[i]->name == "Ko") Ko = multiObj[i];
            else if (multiObj[i]->name == "ior") ior = multiObj[i];
            else if (multiObj[i]->name == "ruffA") ruffA = multiObj[i];
            else if (multiObj[i]->name == "ruffD") ruffD = multiObj[i];
            else if (multiObj[i]->name == "sssSpread") sssSpread = multiObj[i];

                else if (multiObj[i]->name == "shadowCast") shadowCast = multiObj[i];
                else if (multiObj[i]->name == "backface") backface = multiObj[i];
                else if (multiObj[i]->name == "Cwire") Cwire = multiObj[i];
                else if (multiObj[i]->name == "normVLen") normVLen = multiObj[i];
                else if (multiObj[i]->name == "normWeight") normWeight = multiObj[i];
                else if (multiObj[i]->name == "piv") piv = multiObj[i];
                else if (multiObj[i]->name == "rotOrder") rotOrder = multiObj[i];


            //CAMLI
            else if (multiObj[i]->name == "camLiType") camLiType = multiObj[i];

            //CAM
            else if (multiObj[i]->name == "cam") camSep = multiObj[i];
            else if (multiObj[i]->name == "fov") fov = multiObj[i];
            else if (multiObj[i]->name == "nearClip") nearClip = multiObj[i];
            else if (multiObj[i]->name == "farClip") farClip = multiObj[i];
            else if (multiObj[i]->name == "nearShadow") nearShadow = multiObj[i];
            else if (multiObj[i]->name == "farShadow") farShadow = multiObj[i];
            else if (multiObj[i]->name == "orthoFree") orthoFree = multiObj[i];
            else if (multiObj[i]->name == "orthoType") orthoType = multiObj[i];
            else if (multiObj[i]->name == "orthoZoom") orthoZoom = multiObj[i];

            //LIGHT
            else if (multiObj[i]->name == "light") lightSep = multiObj[i];
            else if (multiObj[i]->name == "Cl") Cl = multiObj[i];
            else if (multiObj[i]->name == "lInten") lInten = multiObj[i];
            else if (multiObj[i]->name == "lSpotI") lSpotI = multiObj[i];
            else if (multiObj[i]->name == "lSpotO") lSpotO = multiObj[i];

            //VOLUME LIGHT
            else if (multiObj[i]->name == "volSep") volSep = multiObj[i];
            else if (multiObj[i]->name == "volCone") volCone = multiObj[i];
            else if (multiObj[i]->name == "volDist") volDist = multiObj[i];
            else if (multiObj[i]->name == "volS") volS = multiObj[i];

            /*FSQ */
            else if (multiObj[i]->name == "cubeM") cubeM = multiObj[i];

            //BLOOM
            else if (multiObj[i]->name == "bloom") bloomSep = multiObj[i];
            else if (multiObj[i]->name == "bloomI") bloomI = multiObj[i];
            else if (multiObj[i]->name == "bloomLensI") bloomLensI = multiObj[i];
            else if (multiObj[i]->name == "lensM") lensM = multiObj[i];

            //HDR
            else if (multiObj[i]->name == "hdr") hdrSep = multiObj[i];
            else if (multiObj[i]->name == "expo") expo = multiObj[i];
            else if (multiObj[i]->name == "adaptTime") adaptTime = multiObj[i];
            else if (multiObj[i]->name == "adaptAuto") adaptAuto = multiObj[i];
            else if (multiObj[i]->name == "vignette") vignette = multiObj[i];
            else if (multiObj[i]->name == "vignetteD") vignetteD = multiObj[i];

                //FXAA
                else if (multiObj[i]->name == "fxaa") fxaaSep = multiObj[i];
                else if (multiObj[i]->name == "fxaaBlur") fxaaBlur = multiObj[i];
                else if (multiObj[i]->name == "fxaaSubPix") fxaaSubPix = multiObj[i];
                else if (multiObj[i]->name == "fxaaEdgeThr") fxaaEdgeThr = multiObj[i];
                else if (multiObj[i]->name == "fxaaEdgeThrMin") fxaaEdgeThrMin = multiObj[i];

            //SSAO
            else if (multiObj[i]->name == "ssao") ssaoSep = multiObj[i];
            else if (multiObj[i]->name == "ssaoBias") ssaoBias = multiObj[i];
            else if (multiObj[i]->name == "ssaoBlur") ssaoBlur = multiObj[i];
            else if (multiObj[i]->name == "ssaoInten") ssaoInten = multiObj[i];
            else if (multiObj[i]->name == "ssaoRad") ssaoRad = multiObj[i];
            else if (multiObj[i]->name == "ssaoRand") ssaoRand = multiObj[i];
        }
    }

    //VBO vertex attrs
    dynVAO_perGL = obj.dynVAO_perGL; //

    pE = obj.pE;
    nE = obj.nE;
    tE = obj.tE;
    bE = obj.bE;
    uvE = obj.uvE;
    idxE = obj.idxE;

    parentTo = obj.parentTo;

    //CAM
    distO = obj.distO;
    lookO = obj.lookO;
    rightO = obj.rightO;
    upO = obj.upO;
    targO = obj.targO;
    lookFPS = obj.lookFPS;
    rightFPS = obj.rightFPS;
    upFPS = obj.upFPS;

    //UPDATE
    dirtyShadow = obj.dirtyShadow;
    dirtyVM = obj.dirtyVM;
    savedShader = obj.savedShader;

    //ETC
    deletable = obj.deletable;
    expand = obj.expand;
    gridV = obj.gridV;
    ignoreOutliner = obj.ignoreOutliner;
    selectable = obj.selectable;
    selected = obj.selected;
    ref = obj.ref;
    vertsOnObj = obj.vertsOnObj;
    dupeStenFix = obj.dupeStenFix;
    manipTypeO = obj.manipTypeO;
    type = obj.type;

    //GLSL
    Crand = genColorID();
    showN = obj.showN;
    gaussStage = obj.gaussStage;
    nType = obj.nType;

    //GIZ
    Cgiz = obj.Cgiz;
    Cgiz_stored = obj.Cgiz_stored;
    gizType = obj.gizType;
    gizSideObj = obj.gizSideObj;

    //TXT
    txtOrigin = obj.txtOrigin;

    //TRANSFORM
    MVP = obj.MVP;
    MV = obj.MV;
    NM = obj.NM;
    MM = obj.MM;
    PM = obj.PM;
    VM = obj.VM;
    TM = obj.TM;
    RM = obj.RM;
    SM = obj.SM;

    bbScaleM = obj.bbScaleM;
    bbrotM = obj.bbrotM;
    bbtransM = obj.bbtransM;
    obbMVP = obj.obbMVP;

    bbMin = obj.bbMin;
    bbMax = obj.bbMax;
    bbCenter = obj.bbCenter;
    bbSize = obj.bbSize;
    bbSizeFull = obj.bbSizeFull;

    pivM = obj.pivM;
    pivRtM = obj.pivRtM;
    rpM = obj.rpM;
    rM = obj.rM;
    pivRt = obj.pivRt;
}

void Object::populateTexCombos()
{
    for (unsigned int i = 0; i < multiObj.size(); ++i)
    {
        if (multiObj[i]->type == "enum" && multiObj[i]->repop)
        {
            multiObj[i]->comboList.clear();
            QString multiUp = multiObj[i]->name.toUpper();
            multiUp.chop(1);

            for (unsigned int j = 0; j < myWin.myGLWidgetSh->allTex.size(); ++j)
            {
                if (multiUp == myWin.myGLWidgetSh->allTex[j].type)
                    multiObj[i]->comboList.push_back(myWin.myGLWidgetSh->allTex[j].name);
            }
        }
    }
}

void Object::loadVAO(shared_ptr<GLWidget> myGL)
{
    glGetError();

    for (unsigned int i = 0; i < dynVAO_perGL.size(); ++i)
    {
        if (dynVAO_perGL[i].GL == myGL && !dynVAO_perGL[i].loaded)
        {
            for (unsigned int j = 0; j < myWin.myGLWidgetSh->GLDataSh.size(); ++j)
            {
                if (myWin.myGLWidgetSh->GLDataSh[j].obj.get() == this)
                {
                    glCreateVertexArrays(1, &dynVAO_perGL[i].VAO);

                    if (type == "BB")
                    {
                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 0, myWin.myGLWidgetSh->GLDataSh[j].VBO_P, 0, 12);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

                        glVertexArrayElementBuffer(dynVAO_perGL[i].VAO, myWin.myGLWidgetSh->GLDataSh[j].VBO_IDX);
                    }

                    else if (type == "GRID")
                    {
                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 0, myWin.myGLWidgetSh->GLDataSh[j].VBO_P, 0, 12);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
                    }

                    else if (type == "TXT")
                    {
                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 0, myWin.myGLWidgetSh->GLDataSh[j].VBO_P, 0, 1);
                        glVertexArrayAttribIFormat(dynVAO_perGL[i].VAO, 0, 1, GL_UNSIGNED_BYTE, 0);
                    }

                    else if (type == "GIZ_CIRC" || type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_CIRC_HALF" || type == "GIZ_LINE" || type == "CAMLI")
                    {
                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 0, myWin.myGLWidgetSh->GLDataSh[j].VBO_P, 0, 12);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

                        if (type == "GIZ_CUBE")
                            glVertexArrayElementBuffer(dynVAO_perGL[i].VAO, myWin.myGLWidgetSh->GLDataSh[j].VBO_IDX);
                    }

                    else if (type == "OBJ" || type == "VOL")
                    {
                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 0, myWin.myGLWidgetSh->GLDataSh[j].VBO_P, 0, 12);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 1);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 1, myWin.myGLWidgetSh->GLDataSh[j].VBO_UV, 0, 8);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 1, 2, GL_FLOAT, GL_FALSE, 0);

                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 2);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 2, myWin.myGLWidgetSh->GLDataSh[j].VBO_T, 0, 12);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 2, 3, GL_FLOAT, GL_FALSE, 0);

                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 3);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 3, myWin.myGLWidgetSh->GLDataSh[j].VBO_B, 0, 12);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 3, 3, GL_FLOAT, GL_FALSE, 0);

                        glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 4);
                        glVertexArrayVertexBuffer(dynVAO_perGL[i].VAO, 4, myWin.myGLWidgetSh->GLDataSh[j].VBO_N, 0, 12);
                        glVertexArrayAttribFormat(dynVAO_perGL[i].VAO, 4, 3, GL_FLOAT, GL_FALSE, 0);

                        glVertexArrayElementBuffer(dynVAO_perGL[i].VAO, myWin.myGLWidgetSh->GLDataSh[j].VBO_IDX);
                    }

                    vertsOnObj = (int)pE.size();
                }
            }

            dynVAO_perGL[i].loaded = 1;
        }
    }

    glErrorPrint("Couldn't load VAO into dynVAO_perGL");
}

void Object::BBup()
{
    bbMin = (pE.empty()) ? glm::vec3(0.f) : pE[0];
    bbMax = (pE.empty()) ? glm::vec3(0.f) : pE[0];

    for (unsigned int i = 0; i < pE.size(); ++i)
    {
        bbMin = glm::min(pE[i], bbMin);
        bbMax = glm::max(pE[i], bbMax);
    }

    bbSize = bbMax - bbMin;
    bbCenter = .5f * (bbMin + bbMax);
    obbMVP = glm::translate(glm::mat4(), bbCenter) * glm::scale(glm::mat4(), bbSize);
}

void Object::setTarg(glm::vec3 foc, float minD)
{
    targO = foc;
    distO = glm::distance(t->val_3, targO);

    if (minD != 0.f)
        distO = min(minD, distO);

    glm::mat4 V = glm::lookAt(t->val_3, targO, upO);

    if (V[0][0] < 0)
        r->val_3.x = glm::degrees((float)(PI - asinf(-V[2][0])));

    else
        r->val_3.x = glm::degrees(asinf(-V[2][0]));

    r->val_3.y = glm::degrees(asinf(-V[1][2]));

    setDirty();
}

glm::mat4 Object::rotOrderUse(QString myRotOrder)
{
    QString rotOrder0 = myRotOrder.mid(0, 1);
    QString rotOrder1 = myRotOrder.mid(1, 1);
    QString rotOrder2 = myRotOrder.mid(2, 1);

    glm::vec3 axX(1.f, 0.f, 0.f);
    glm::vec3 axY(0.f, 1.f, 0.f);
    glm::vec3 axZ(0.f, 0.f, 1.f);

    glm::mat4 order0, order1, order2;

    //0
    if (rotOrder0 == "X")      order0 = glm::rotate(glm::mat4(), glm::radians(r->val_3.x), axX);
    else if (rotOrder0 == "Y") order0 = glm::rotate(glm::mat4(), glm::radians(r->val_3.y), axY);
    else if (rotOrder0 == "Z") order0 = glm::rotate(glm::mat4(), glm::radians(r->val_3.z), axZ);

    //1
    if (rotOrder1 == "X")      order1 = glm::rotate(glm::mat4(), glm::radians(r->val_3.x), axX);
    else if (rotOrder1 == "Y") order1 = glm::rotate(glm::mat4(), glm::radians(r->val_3.y), axY);
    else if (rotOrder1 == "Z") order1 = glm::rotate(glm::mat4(), glm::radians(r->val_3.z), axZ);

    //2
    if (rotOrder2 == "X")      order2 = glm::rotate(glm::mat4(), glm::radians(r->val_3.x), axX);
    else if (rotOrder2 == "Y") order2 = glm::rotate(glm::mat4(), glm::radians(r->val_3.y), axY);
    else if (rotOrder2 == "Z") order2 = glm::rotate(glm::mat4(), glm::radians(r->val_3.z), axZ);

    return order0 * order1 * order2;
}

bool Object::camLiTypeGet(QString toFind)
{
    bool found = 0;

    if (type == "CAMLI")
    {
        QString myType = camLiType->val_s;

        if (toFind == "cam")
        {
            if (myType == "FPS" || myType == "ORTHO" || myType == "PERSP")
                found = 1;
        }

        else if (toFind == "dirOrtho")
        {
            if (myType == "DIR" || myType == "ORTHO")
                found = 1;
        }

        else if (toFind == "light")
        {
            if (myType == "AREA" || myType == "DIR" || myType == "POINT" || myType == "SPOT")
                found = 1;
        }
    }

    return found;
}

void Object::mvpGet(shared_ptr<GLWidget> myGL)
{
    glm::vec3 usableScale = s->val_3;

    if (type == "RTT")
        usableScale = glm::vec3(myGL->width() * s->val_3.x, myGL->height() * s->val_3.x, 0.f) / 100.f;

    SM = glm::scale(glm::mat4(), usableScale);

    if (type == "CAMLI" && camLiType->val_s != "DIR")
        RM = glm::yawPitchRoll(glm::radians(r->val_3.x), glm::radians(r->val_3.y), 0.f);

    else
        RM = rotOrderUse(rotOrder->val_s);

    TM = glm::translate(glm::mat4(), t->val_3);

    if (name->val_s == "giz")
    {
        if (myWin.gizSpace == "local")
            RM = parentTo->RM;

        SM = glm::scale(glm::mat4(), glm::vec3(myWin.gizScale));
    }

    if (name->val_s == "pivot" || name->val_s == "giz")
    {
        //TM = glm::translate(glm::mat4(), parentTo->piv->val_3);
        TM = glm::translate(glm::mat4(), parentTo->t->val_3); //
    }

    //PIV
    pivM = glm::translate(glm::mat4(), t->val_3 - piv->val_3);
    pivRtM = glm::translate(glm::mat4(), pivRt);

    //MM
    //    if (piv->val_3 == glm::vec3(0.f)) MM = TM * RM * SM;
    //    else MM = TM * pivRtM * glm::inverse(pivM) * RM * SM * pivM;
    MM = TM * RM * SM;

    glm::mat4 PM_stored, VM_stored;

    if (type == "CAMLI")
    {
        PM_stored = PM;
        VM_stored = VM;
    }

    if (gizSideObj)
    {
        PM = myGL->PMgizSide;
        VM = myGL->VMgizSide;
    }

    else
    {
        PM = myGL->selCamLi->PM;
        VM = myGL->selCamLi->VM;
    }

    if (parentTo == 0 || name->val_s == "pivot")
        MV = VM * MM;

    else if (type == "GIZ_CIRC" || type == "GIZ_CIRC_HALF")
        MV = VM * parentTo->TM * parentTo->SM * MM;

    else
    {
        MV = VM * parentTo->MM * MM;
    }

    if (bb->val_b) MVP = PM * MV * obbMVP;
    else if (type == "RTT") MVP = myGL->PMrtt * MM;
    else MVP = PM * MV;

    if (myWin.creatingDynCubeRGB)
    {
        PM = glm::perspective(glm::radians(90.f), 1.f, 1.f, 100.f);
        VM = myWin.myGLWidgetSh->dynVM;

        MVP = PM * VM * myWin.myGLWidgetSh->negCubeCenter * MM;
    }

    NM = glm::mat3(glm::transpose(glm::inverse(MV)));

    if (type == "CAMLI")
    {
        PM = PM_stored;
        VM = VM_stored;
    }
}

void Object::proUse(shared_ptr<GLWidget> myGL)
{
    for (unsigned int i = 0; i < myWin.myGLWidgetSh->GLDataSh.size(); ++i)
    {
        if (myWin.myGLWidgetSh->GLDataSh[i].obj.get() == this)
        {
            GLuint &proH = myWin.myGLWidgetSh->pro;
            QString proN = myWin.myGLWidgetSh->proN;

//            if ((proN == "pDef") && myWin.lightCt > 0)
//                shadowPass(proH);

            //OBJ MATRIX
            glUniformMatrix4fv(glGetUniformLocation(proH, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix3fv(glGetUniformLocation(proH, "NM"), 1, GL_FALSE, &NM[0][0]);

            if (proN == "pBB" || proN == "pWireframe")
            {
                if (proN == "pBB")
                {
                    glUniform1i(glGetUniformLocation(proH, "CpickTgl"), myGL->colorPickTgl);
                    glUniform4fv(glGetUniformLocation(proH, "Crand"), 1, &Crand.r);
                }

                glm::vec3 temp = (selected) ? myWin.glslTable->Csel->val_3 : Cwire->val_3;
                glm::vec4 Cwire(temp, 1.f);
                glUniform4fv(glGetUniformLocation(proH, "Cwire"), 1, &Cwire.r);
            }

            if (proN == "pSky")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "PMinv_cube"), 1, GL_FALSE, &myGL->PMinv_cube[0][0]);

                glUniformMatrix4fv(glGetUniformLocation(proH, "VM"), 1, GL_FALSE, &VM[0][0]);
                glBindTextureUnit(0, myWin.cubeM_specular_32);
            }

            else if (proN == "pGrid")
                glUniform3fv(glGetUniformLocation(proH, "Cgrid"), 1, &myWin.glslTable->Cgrid->val_3.r);

            else if (proN == "pShadow") //WRITING TO A SINGLE LIGHT
            {
                glm::mat4 PM_shadow, VM_shadow, depthMVP;

                if (myWin.allObj[myWin.myGLWidgetSh->shadowIdx]->camLiType->val_s == "DIR") //
                {
                    PM_shadow = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 20.f);
                    VM_shadow = glm::inverse(myWin.allObj[myWin.myGLWidgetSh->shadowIdx]->RM);
                    depthMVP = PM_shadow * VM_shadow * MM;
                }

                else if (myWin.allObj[myWin.myGLWidgetSh->shadowIdx]->camLiType->val_s == "POINT")
                {
                    PM_shadow = glm::perspective(glm::radians(90.f), 1.f, 1.f, 100.f);
                    VM_shadow = myWin.myGLWidgetSh->VMcubeShadow;
                    depthMVP = PM_shadow * VM_shadow * myWin.myGLWidgetSh->negCubeCenter2 * MM;
                }

                else if (myWin.allObj[myWin.myGLWidgetSh->shadowIdx]->camLiType->val_s == "SPOT")
                {
                    PM_shadow = glm::perspective(glm::radians(myWin.allObj[myWin.myGLWidgetSh->shadowIdx]->fov->val_f), 1.f, 2.f, 100.f);
                    VM_shadow = myWin.allObj[myWin.myGLWidgetSh->shadowIdx]->VM;
                    depthMVP = PM_shadow * VM_shadow * MM;
                }

                glUniformMatrix4fv(glGetUniformLocation(proH, "depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);
            }

            else if (proN == "pBaseDef")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "MV"), 1, GL_FALSE, &MV[0][0]);

                glm::vec4 matPrefs0(Kr->val_f, ior->val_f, ruffD->val_f, Ko->val_f);
                glUniform4fv(glGetUniformLocation(proH, "matPrefs0"), 1, &matPrefs0.r);

                float shadowCastF = (shadowCast->val_b) ? 1.f : 0.f;
                glm::vec4 matPrefs1(ruffA->val_2, shadowCastF, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "matPrefs1"), 1, &matPrefs1.r);

                glm::vec4 sssSend(Ksss->val_f, sssSpread->val_f, 0.f, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "matPrefs2"), 1, &sssSend.r);

                tileMaps(proH, "get");

                for (unsigned int j = 0; j < myWin.myGLWidgetSh->allTex.size(); ++j)
                {
                    if (myWin.myGLWidgetSh->allTex[j].name == albedoM->val_s && myWin.myGLWidgetSh->allTex[j].type == "ALBEDO")
                        glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->allTex[j].tex_64);

                    else if (myWin.myGLWidgetSh->allTex[j].name == alphaM->val_s && myWin.myGLWidgetSh->allTex[j].type == "ALPHA")
                        glProgramUniformHandleui64ARB(proH, 1, myWin.myGLWidgetSh->allTex[j].tex_64);

                    else if (myWin.myGLWidgetSh->allTex[j].name == anisoM->val_s && myWin.myGLWidgetSh->allTex[j].type == "ANISO")
                    {
                        int anisoType;

                        if (anisoM->val_s == "BLANK") anisoType = -2; //0
                        else if (anisoM->val_s == "VIEW") anisoType = -1;
                        else anisoType = 1;

                        glUniform1i(glGetUniformLocation(proH, "anisoType"), anisoType);

                        glProgramUniformHandleui64ARB(proH, 2, myWin.myGLWidgetSh->allTex[j].tex_64);
                    }

                    else if (myWin.myGLWidgetSh->allTex[j].name == metallicM->val_s && myWin.myGLWidgetSh->allTex[j].type == "METALLIC")
                        glProgramUniformHandleui64ARB(proH, 3, myWin.myGLWidgetSh->allTex[j].tex_64);

                    else if (myWin.myGLWidgetSh->allTex[j].name == normalM->val_s && myWin.myGLWidgetSh->allTex[j].type == "NORMAL")
                        glProgramUniformHandleui64ARB(proH, 4, myWin.myGLWidgetSh->allTex[j].tex_64);

                    else if (myWin.myGLWidgetSh->allTex[j].name == ruffM->val_s && myWin.myGLWidgetSh->allTex[j].type == "RUFF")
                        glProgramUniformHandleui64ARB(proH, 5, myWin.myGLWidgetSh->allTex[j].tex_64);

                    else if (myWin.myGLWidgetSh->allTex[j].name == sssM->val_s && myWin.myGLWidgetSh->allTex[j].type == "SSS")
                        glProgramUniformHandleui64ARB(proH, 6, myWin.myGLWidgetSh->allTex[j].tex_64);
                }

//                glUniform1i(glGetUniformLocation(proH, "vignette"), myWin.myFSQ->vignette->val_b);
            }

            else if (proN == "pLumaInit")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->deferred_node.tex1_64);
            }

            else if (proN == "pLumaAdapt")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->lumaAdapt[!myGL->currLum].tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myGL->lumaInit.tex1_64);

                glUniform1f(glGetUniformLocation(proH, "dTime"), myGL->dTime);
            }

            else if (proN == "pLumaAdapt_viz")
                glProgramUniformHandleui64ARB(proH, 0, myGL->lumaAdapt[!myGL->currLum].tex1_64);

            else if (proN == "pRtt")
                glProgramUniformHandleui64ARB(proH, 0, myGL->tonemap_exposure_node.tex1_64);

            else if (proN == "pDown")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->downSamp_64);
                glUniform2fv(glGetUniformLocation(proH, "texelSize"), 1, &myGL->texelSize.r);
            }

            else if (proN == "pGauss")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->rttGaussIn64);
                glUniform1i(glGetUniformLocation(proH, "gaussStage"), gaussStage);
            }

            else if (proN == "pBloom")
            {
                for (int j = 0; j < 6; ++j)
                    glProgramUniformHandleui64ARB(proH, j, myGL->bloom_gauss_node[j].tex2_64);

                glProgramUniformHandleui64ARB(proH, 6, myGL->deferred_node.tex1_64);

                for (unsigned int j = 0; j < myWin.myGLWidgetSh->allTex.size(); ++j)
                {
                    if (myWin.myGLWidgetSh->allTex[j].name == lensM->val_s && myWin.myGLWidgetSh->allTex[j].type == "LENS")
                        glProgramUniformHandleui64ARB(proH, 7, myWin.myGLWidgetSh->allTex[j].tex_64);
                }

                glUniform1f(glGetUniformLocation(proH, "bloomI"), myWin.myFSQ->bloomI->val_f);
                glUniform1f(glGetUniformLocation(proH, "bloomLensI"), myWin.myFSQ->bloomLensI->val_f);
            }

            else if (proN == "pSSAO")
            {
                glm::mat4 PMinv = glm::inverse(myGL->selCamLi->PM);
                glUniformMatrix4fv(glGetUniformLocation(proH, "PMinv"), 1, GL_FALSE, &PMinv[0][0]);

                glProgramUniformHandleui64ARB(proH, 0, myGL->gbuf2_64);
                glProgramUniformHandleui64ARB(proH, 1, myGL->gbuf_DS_64); //

                for (unsigned int j = 0; j < myWin.myGLWidgetSh->allTex.size(); ++j)
                {
                    if (myWin.myGLWidgetSh->allTex[j].name == "rand")
                        glProgramUniformHandleui64ARB(proH, 2, myWin.myGLWidgetSh->allTex[j].tex_64);
                }

                glUniform1f(glGetUniformLocation(proH, "ssaoBias"), ssaoBias->val_f);
                glUniform1f(glGetUniformLocation(proH, "ssaoInten"), ssaoInten->val_f);
                glUniform1f(glGetUniformLocation(proH, "ssaoRad"), ssaoRad->val_f);
                glUniform1i(glGetUniformLocation(proH, "ssaoRand"), ssaoRand->val_i);
            }

            else if (proN == "pBloomC")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->deferred_node.tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myGL->bloom_node.tex1_64);
                glProgramUniformHandleui64ARB(proH, 2, myGL->ssao_gauss_node.tex2_64);
            }

            else if (proN == "pDef")
            {
                glm::mat4 PMinv = glm::inverse(myGL->selCamLi->PM);
                glUniformMatrix4fv(glGetUniformLocation(proH, "PMinv"), 1, GL_FALSE, &PMinv[0][0]);

                glUniformMatrix4fv(glGetUniformLocation(proH, "VM"), 1, GL_FALSE, &VM[0][0]);

                glm::mat3 VMinv = glm::inverse(glm::mat3(myGL->selCamLi->VM));
                glUniformMatrix3fv(glGetUniformLocation(proH, "VMinv"), 1, GL_FALSE, &VMinv[0][0]);

//                glProgramUniformHandleui64ARB(proH, 0, myGL->gbuf0_64);
                glProgramUniformHandleui64ARB(proH, 1, myGL->gbuf1_64);
                glProgramUniformHandleui64ARB(proH, 2, myGL->gbuf2_64);
                glProgramUniformHandleui64ARB(proH, 3, myGL->gbuf3_64);
                glProgramUniformHandleui64ARB(proH, 4, myGL->gbuf4_64);
                glProgramUniformHandleui64ARB(proH, 5, myGL->gbuf5_64);
                glProgramUniformHandleui64ARB(proH, 7, myGL->ssao_gauss_node.tex2_64);
                glProgramUniformHandleui64ARB(proH, 8, myGL->simp_sky_64);
                glBindTextureUnit(9, myWin.cubeM_specular_32);
                glBindTextureUnit(10, myWin.cubeM_irradiance_32);
                glProgramUniformHandleui64ARB(proH, 12, myGL->gbuf_DS_64); //

                for (unsigned int j = 0; j < myWin.myGLWidgetSh->allTex.size(); ++j)
                {
                    if (myWin.myGLWidgetSh->allTex[j].name == "sssLookup")
                        glProgramUniformHandleui64ARB(proH, 11, myWin.myGLWidgetSh->allTex[j].tex_64);
                }

                glUniform1i(glGetUniformLocation(proH, "vignette"), myWin.myFSQ->vignette->val_b);
                glUniform1i(glGetUniformLocation(proH, "NUM_LIGHTS"), myWin.lightCt);
            }

            else if (proN == "pTonemap")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->bloomC_node.tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myGL->lumaAdapt[myGL->currLum].tex1_64);
                glProgramUniformHandleui64ARB(proH, 2, myGL->simp_Giz_64);

                glUniform1f(glGetUniformLocation(proH, "expo"), log(myWin.myFSQ->expo->val_f));
                glUniform1i(glGetUniformLocation(proH, "adaptAuto"), myWin.myFSQ->adaptAuto->val_b);
//                glUniform1i(glGetUniformLocation(proH, "vignette"), myWin.myFSQ->vignette->val_b);
                glUniform1f(glGetUniformLocation(proH, "vignetteD"), myWin.myFSQ->vignetteD->val_f);
            }

            else if (proN == "pFxaa")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->tonemap_node.tex1_64);

                glUniform1f(glGetUniformLocation(proH, "fxaaSubPix"), myWin.myFSQ->fxaaSubPix->val_f);
                glUniform1f(glGetUniformLocation(proH, "fxaaEdgeThr"), myWin.myFSQ->fxaaEdgeThr->val_f);
                glUniform1f(glGetUniformLocation(proH, "fxaaEdgeThrMin"), myWin.myFSQ->fxaaEdgeThrMin->val_f);

                glUniform2fv(glGetUniformLocation(proH, "texelSize"), 1, &myGL->texelSize.r);
            }

            else if (proN == "pFinal")
            {
                glProgramUniformHandleui64ARB(proH, 0, myGL->fxaa_node.tex1_64);

                //REZGATE
                bool rezGateTgl = (myGL->rezGateTgl || myGL->rezGateTgl_sel) ? 1 : 0;
                glUniform1i(glGetUniformLocation(proH, "rezGateTgl"), rezGateTgl);
                glUniform2fv(glGetUniformLocation(proH, "rezGate_LD"), 1, &myGL->rezGate_LD.x);
                glUniform2fv(glGetUniformLocation(proH, "rezGate_RU"), 1, &myGL->rezGate_RU.x);
                glUniform1i(glGetUniformLocation(proH, "dragDrop"), myGL->dragDrop);
            }

            //GIZ
            else if (proN == "pGiz")
                glUniform3fv(glGetUniformLocation(proH, "Cgiz"), 1, &Cgiz.r);

            else if (proN == "pGiz_circ")
            {
                glUniform3fv(glGetUniformLocation(proH, "Cgiz"), 1, &Cgiz.r);

                if (type == "GIZ_CIRC")
                {
                    glUniform3fv(glGetUniformLocation(proH, "circRight"), 1, &myGL->selCamLi->rightO.x);
                    glUniform3fv(glGetUniformLocation(proH, "circUpOrFront"), 1, &myGL->selCamLi->upO.x);
                    glUniform1i(glGetUniformLocation(proH, "fullCirc"), 1);
                }

                else if (type == "GIZ_CIRC_HALF")
                {
                    glm::vec3 axX(1.f, 0.f, 0.f);
                    glm::vec3 axY(0.f, 1.f, 0.f);
                    glm::vec3 axZ(0.f, 0.f, 1.f);

                    if (myWin.gizSpace == "local" && myWin.selB)
                    {
                        glm::vec4 xLocal(myWin.selB->MM * glm::vec4(axX, 0.f));
                        axX = glm::normalize(glm::vec3(xLocal));

                        glm::vec4 yLocal(myWin.selB->MM * glm::vec4(axY, 0.f));
                        axY = glm::normalize(glm::vec3(yLocal));

                        glm::vec4 zLocal(myWin.selB->MM * glm::vec4(axZ, 0.f));
                        axZ = glm::normalize(glm::vec3(zLocal));
                    }

                    glm::vec3 circHalfRight, circHalfFront;

                    if (name->val_s == "gizCircHalfX")
                    {
                        circHalfRight = glm::normalize(glm::cross(myGL->selCamLi->lookO, axX));
                        circHalfFront = glm::normalize(glm::cross(circHalfRight, axX));
                        myGL->gizHoverCheckX = circHalfFront;
                    }

                    else if (name->val_s == "gizCircHalfY")
                    {
                        circHalfRight = glm::normalize(glm::cross(myGL->selCamLi->lookO, axY));
                        circHalfFront = glm::normalize(glm::cross(circHalfRight, axY));
                        myGL->gizHoverCheckY = circHalfFront;
                    }

                    else if (name->val_s == "gizCircHalfZ")
                    {
                        circHalfRight = glm::normalize(glm::cross(myGL->selCamLi->lookO, axZ));
                        circHalfFront = glm::normalize(glm::cross(circHalfRight, axZ));
                        myGL->gizHoverCheckZ = circHalfFront;
                    }

                    glUniform3fv(glGetUniformLocation(proH, "circRight"), 1, &circHalfRight.x);
                    glUniform3fv(glGetUniformLocation(proH, "circUpOrFront"), 1, &circHalfFront.x);
                    glUniform1i(glGetUniformLocation(proH, "fullCirc"), 0);
                }
            }

            else if (proN == "pNViz")
            {
                glUniform1i(glGetUniformLocation(proH, "nType"), nType);
                glUniform1f(glGetUniformLocation(proH, "normVLen"), normVLen->val_f);
            }

            else if (type == "SELRECT")
            {
                glUniform2fv(glGetUniformLocation(proH, "selRect_LD"), 1, &myGL->selRect_LD.x);
                glUniform2fv(glGetUniformLocation(proH, "selRect_RU"), 1, &myGL->selRect_RU.x);
            }

            else if (proN == "pStencilHi")
            {
                glUniform3fv(glGetUniformLocation(proH, "CstencilHi"), 1, &myWin.glslTable->Csel->val_3.r);
            }

            else if (proN == "pTxt")
            {
                for (unsigned int j = 0; j < myWin.myGLWidgetSh->allTex.size(); ++j)
                {
                    if (myWin.myGLWidgetSh->allTex[j].name == "atlas")
                        glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->allTex[j].tex_64);
                }

                glUniform3fv(glGetUniformLocation(proH, "Ctxt"), 1, &myWin.glslTable->Ctxt->val_3.r);

                glm::vec2 txtCellSize(1.f / 16, (300.f / 384) / 6);
                glUniform2fv(glGetUniformLocation(proH, "txtCellSize"), 1, &txtCellSize.x);

                glm::vec2 txtCellOffset(.5f / 256);
                glUniform2fv(glGetUniformLocation(proH, "txtCellOffset"), 1, &txtCellOffset.x);

                glUniform2fv(glGetUniformLocation(proH, "txtOrigin"), 1, &txtOrigin.x);

                float txtSize = .8f;
                glm::vec2 txtSizeUse(.75f * 16 / (myGL->width() / txtSize), .75f * 33.33 / (myGL->height() / txtSize));
                glUniform2fv(glGetUniformLocation(proH, "txtSize"), 1, &txtSizeUse.x);
            }

            else if (proN == "pVolumeLight")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "MV"), 1, GL_FALSE, &MV[0][0]);

                glm::vec3 volTip = parentTo->t->val_3;
                glm::vec3 volTipView = glm::vec3(VM * glm::vec4(parentTo->t->val_3, 1.f));

                glUniform3fv(glGetUniformLocation(proH, "parentCl"), 1, &parentTo->Cl->val_3.r);

                glUniform3fv(glGetUniformLocation(proH, "volTip"), 1, &volTip.x);
                glUniform3fv(glGetUniformLocation(proH, "volTipView"), 1, &volTipView.x);
                glUniform1f(glGetUniformLocation(proH, "volDist"), parentTo->volDist->val_f);
            }
        }
    }
}

void Object::shadowPass(GLuint /*proH*/)
{
//    //READING FROM MULTIPLE LIGHTS
//    int lightIter = 1;

//    stringstream ss;
//    ss << "myLightType[" << 0 << "]";
//    glUniform1i(glGetUniformLocation(proH, ss.str().c_str()), 999);
//    ss.str(""); ss.clear();

//    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
//    {
//        if (myWin.allObj[i]->v->val_b && myWin.allObj[i]->camLiTypeGet("light"))
//        {
//            //SHADOW
//            glm::mat4 PM_shadow, VM_shadow;

//            if (myWin.allObj[i]->camLiType->val_s == "DIR")
//            {
//                PM_shadow = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 20.f);
//                VM_shadow = glm::inverse(myWin.allObj[i]->RM);
//            }

//            else if (myWin.allObj[i]->camLiType->val_s == "POINT")
//            {
//                PM_shadow = glm::perspective(glm::radians(90.f), 1.f, myWin.allObj[i]->nearShadow->val_f, myWin.allObj[i]->farShadow->val_f);
//                VM_shadow = myWin.allObj[i]->VM;
//            }

//            else if (myWin.allObj[i]->camLiType->val_s == "SPOT")
//            {
//                PM_shadow = glm::perspective(glm::radians(50.f), 1.f, myWin.allObj[i]->nearShadow->val_f, myWin.allObj[i]->farShadow->val_f);
//                VM_shadow = myWin.allObj[i]->VM;
//            }

//            ss << "depthBiasMVP" << lightIter;
//            glm::mat4 depthBiasMVP = biasM * PM_shadow * VM_shadow * MM;
//            glUniformMatrix4fv(glGetUniformLocation(proH, ss.str().c_str()), 1, GL_FALSE, &depthBiasMVP[0][0]);
//            ss.str(""); ss.clear();

//            lightIter++;
//        }
//    }
}

void Object::render(shared_ptr<GLWidget> myGL)
{
    proUse(myGL);

    for (unsigned int i = 0; i < dynVAO_perGL.size(); ++i)
    {
        if (dynVAO_perGL[i].GL == myGL)
        {
            if (bb->val_b)
            {
                glLineWidth(2.f);

                glBindVertexArray(myWin.myBB->dynVAO_perGL[i].VAO);
                glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
                glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * 2));
                glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * 2));

                glLineWidth(1.f);
            }

            else
            {
                if (type == "FBO" || type == "RTT" || type == "SELRECT")
                {
                    glBindVertexArray(dynVAO_perGL[i].VAO);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                }

                else if (type == "GRID")
                {
                    glLineWidth(2.f);

                    glBindVertexArray(dynVAO_perGL[i].VAO);

                    int gridLines = myWin.glslTable->gridLines->val_i + 1;
                    for (int j = 0; j < gridLines; ++j)
                        glDrawArrays(GL_LINE_STRIP, gridLines * j, gridLines);

                    glLineWidth(1.f);
                }

                else if (type == "OBJ")
                {
                    glBindVertexArray(dynVAO_perGL[i].VAO);
                    glDrawElements(GL_TRIANGLES, (GLsizei)idxE.size(), GL_UNSIGNED_SHORT, 0);
                }

                else if (type == "VOL")
                {
                    glBindVertexArray(myWin.myVolCone->dynVAO_perGL[i].VAO);
                    glDrawElements(GL_TRIANGLES, (GLsizei)myWin.myVolCone->idxE.size(), GL_UNSIGNED_SHORT, 0);
                }

                else if (type == "CAMLI")
                {
                    glLineWidth(2.f);
                    glDisable(GL_DEPTH_TEST);

                    if (camLiTypeGet("cam"))
                    {
                        glBindVertexArray(dynVAO_perGL[i].VAO);
                        glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)pE.size());
                    }

                    else
                    {
                        if (camLiType->val_s == "AREA")
                        {
                            glBindVertexArray(myWin.myAreaLight->dynVAO_perGL[i].VAO);
                            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.myAreaLight->pE.size());
                        }

                        else if (camLiType->val_s == "DIR")
                        {
                            glBindVertexArray(myWin.myDirLight->dynVAO_perGL[i].VAO);
                            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.myDirLight->pE.size());
                        }

                        else if (camLiType->val_s == "POINT")
                        {
                            glBindVertexArray(myWin.myPointLight->dynVAO_perGL[i].VAO);
                            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.myPointLight->pE.size());
                        }

                        else if (camLiType->val_s == "SPOT")
                        {
                            glBindVertexArray(myWin.mySpotLight->dynVAO_perGL[i].VAO);
                            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.mySpotLight->pE.size());
                        }
                    }

                    glLineWidth(1.f);
                    glEnable(GL_DEPTH_TEST);
                }

                else if (type == "TXT")
                {
                    glBindVertexArray(dynVAO_perGL[i].VAO);
                    glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                    glDrawArrays(GL_POINTS, 0, (GLsizei)strlen(txt2D));
                }

                else if (type == "GIZ_CONE" || type == "GIZ_DUAL_HANDLE")
                {
                    if (type == "GIZ_DUAL_HANDLE")
                        glDisable(GL_CULL_FACE);

                    glBindVertexArray(dynVAO_perGL[i].VAO);
                    glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)pE.size());

                    if (type == "GIZ_DUAL_HANDLE")
                        glEnable(GL_CULL_FACE);
                }

                else if (type == "GIZ_CIRC" || type == "GIZ_CIRC_HALF" || type == "GIZ_LINE")
                {
                    glLineWidth(3.f);

                    glBindVertexArray(dynVAO_perGL[i].VAO);
                    glEnableVertexArrayAttrib(dynVAO_perGL[i].VAO, 0);
                    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)pE.size());

                    glLineWidth(1.f);
                }

                else if (type == "GIZ_CUBE")
                {
                    glBindVertexArray(dynVAO_perGL[i].VAO);
                    glDrawElements(GL_TRIANGLES, (GLsizei)idxE.size(), GL_UNSIGNED_SHORT, 0);
                }
            }
        }
    }
}

glm::vec4 Object::genColorID()
{
    bool preExist = 0;

    int myRand = rand();
    int r = (myRand & 0x000000FF) >>  0;
    int g = (myRand & 0x0000FF00) >>  8;
    int b = (myRand & 0x00FF0000) >> 16;

    glm::vec4 myColorID(r / 255.f, g / 255.f, b / 255.f, 1.f);

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->Crand == myColorID)
            preExist = 1;
    }

    if (preExist)
    {
        genColorID();

        return glm::vec4(1.f);
    }

    else
        return myColorID;
}

void Object::glErrorPrint(const char *msg)
{
    if (glGetError() != GL_NO_ERROR)
    {
        const char* APPEND_DETAIL_STRING = ": %s\n";
        const size_t APPEND_LENGTH = strlen(APPEND_DETAIL_STRING) + 1;
        const size_t message_length = strlen(msg);
        char* display_message = (char*)malloc(message_length + APPEND_LENGTH);

        memcpy(display_message, msg, message_length);
        memcpy(&display_message[message_length], APPEND_DETAIL_STRING, APPEND_LENGTH);

        fprintf(stderr, display_message, glGetError());
        free(display_message);
        exit(EXIT_FAILURE);
    }
}

void Object::parentObj(QString parentName)
{
    bool found = 0;

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->name->val_s == parentName)
        {
            found = 1;
            parentTo = myWin.allObj[i];

            break;
        }
    }

    if (!found)
        qDebug() << "parentObj() could not find " << parentName;
}

void Object::rename(QString newName) //recursive
{
    if (!ref)
    {
        bool preExist = 0;

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->name->val_s == newName)
                preExist = 1;
        }

        if (preExist)
        {
            newName = QString::fromStdString(getIncName(newName.toStdString()));
            rename(newName);
        }

        else
        {
            for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
            {
                if (myWin.allObj[i]->parentTo && myWin.allObj[i]->name->val_s != "pivot")
                {
                    if (myWin.allObj[i]->parentTo->name->val_s == name->val_s)
                        myWin.allObj[i]->parentTo->name->val_s = newName;
                }
            }

            name->val_s = newName;
        }
    }
}

string Object::getIncName(string dupeName)
{
    size_t last_index = dupeName.find_last_not_of("0123456789");
    string trailingNumbers = dupeName.substr(last_index + 1);
    dupeName.erase(last_index + 1);

    int incrementedTrailingNum = atoi(trailingNumbers.c_str()) + 1; // string2int

    stringstream incrementedName;
    incrementedName << dupeName << incrementedTrailingNum;

    return incrementedName.str();
}

void Object::tileMaps(GLuint proH, QString mode)
{
    if (mode == "get")
    {
        glm::vec4 albedoAlphaTile, metallicRuffTile, normalTile, anisoSssTile;

        for (unsigned int i = 0; i < multiObj.size(); ++i)
        {
            if (multiObj[i].get()->type == "vec2" && multiObj[i].get()->name.endsWith("T"))
            {
                //albedoAlpha
                if (multiObj[i].get()->name == "albedoT")
                {
                    albedoAlphaTile.x = multiObj[i].get()->val_2.x;
                    albedoAlphaTile.y = multiObj[i].get()->val_2.y;
                }

                else if (multiObj[i].get()->name == "alphaT")
                {
                    albedoAlphaTile.z = multiObj[i].get()->val_2.x;
                    albedoAlphaTile.w = multiObj[i].get()->val_2.y;
                }

                //anisoSss
                else if (multiObj[i].get()->name == "anisoT")
                {
                    anisoSssTile.x = multiObj[i].get()->val_2.x;
                    anisoSssTile.y = multiObj[i].get()->val_2.y;
                }

                else if (multiObj[i].get()->name == "sssT")
                {
                    anisoSssTile.z = multiObj[i].get()->val_2.x;
                    anisoSssTile.w = multiObj[i].get()->val_2.y;
                }

                //metallicRuff
                else if (multiObj[i].get()->name == "metallicT")
                {
                    metallicRuffTile.x = multiObj[i].get()->val_2.x;
                    metallicRuffTile.y = multiObj[i].get()->val_2.y;
                }

                else if (multiObj[i].get()->name == "ruffT")
                {
                    metallicRuffTile.z = multiObj[i].get()->val_2.x;
                    metallicRuffTile.w = multiObj[i].get()->val_2.y;
                }

                //normal
                else if (multiObj[i].get()->name == "normalT")
                {
                    normalTile.x = multiObj[i].get()->val_2.x;
                    normalTile.y = multiObj[i].get()->val_2.y;
                    normalTile.z = normWeight->val_f;
                }
            }
        }

        glUniform4fv(glGetUniformLocation(proH, "albedoAlphaTile"), 1, &albedoAlphaTile.r);
        glUniform4fv(glGetUniformLocation(proH, "metallicRuffTile"), 1, &metallicRuffTile.r);
        glUniform4fv(glGetUniformLocation(proH, "normalTile"), 1, &normalTile.r);
        glUniform4fv(glGetUniformLocation(proH, "anisoSssTile"), 1, &anisoSssTile.r);
    }

    else if (mode == "set")
    {
        for (unsigned int i = 0; i < multiObj.size(); ++i)
        {
            if (multiObj[i].get()->type == "enum" && multiObj[i].get()->name.endsWith("M"))
            {
                QString tileName = multiObj[i].get()->name;
                tileName.chop(1);
                QString myTile = tileName + "T";

                shared_ptr<MultiAttr> newTileAttr = make_shared<MultiAttr>();
                newTileAttr->name = myTile;
                newTileAttr->type = "vec2";
                newTileAttr->typeX = "OBJ";
                newTileAttr->val_2 = glm::vec2(1.f, 1.f);
                newTileAttr->min = 1;
                newTileAttr->tab = 1;
                multiObj.push_back(newTileAttr);
            }
        }
    }
}

void Object::delete_()
{
    glGetError();

    for (vector<GLSharedData>::iterator it = myWin.myGLWidgetSh->GLDataSh.begin(); it != myWin.myGLWidgetSh->GLDataSh.end(); )
    {
        if ((*it).obj.get() == this)
        {
            glDeleteBuffers(1, &(*it).VBO_P);
            glDeleteBuffers(1, &(*it).VBO_UV);
            glDeleteBuffers(1, &(*it).VBO_T);
            glDeleteBuffers(1, &(*it).VBO_B);
            glDeleteBuffers(1, &(*it).VBO_N);
            glDeleteBuffers(1, &(*it).VBO_IDX);
            glErrorPrint("Couldnt delete VBOs");

            it = myWin.myGLWidgetSh->GLDataSh.erase(it);
        }

        else
            ++it;
    }

    glBindVertexArray(0);
    for (unsigned int i = 0; i < dynVAO_perGL.size(); ++i)
    {
        glDeleteVertexArrays(1, &dynVAO_perGL[i].VAO);
        dynVAO_perGL[i].loaded = 0;
    }

    glErrorPrint("Couldnt delete VAOs");
}

void Object::setDirty()
{
    dirtyVM = 1;

    if (camLiTypeGet("light"))
        dirtyShadow = 1;
}
