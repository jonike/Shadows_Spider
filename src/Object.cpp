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

#include "Object.h"

Object::Object(MainWin &myWinTemp) : myWin(myWinTemp)
{
    name = make_shared<MultiAttr>();
    name->name = "name";
    name->type = "string";
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
    v->val_b = true;
    multiObj.push_back(v);

    bb = make_shared<MultiAttr>();
    bb->name = "bb";
    bb->type = "bool";
    bb->val_b = false;
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
    cubeM->name = "cube_specM";
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
    normalM->val_s = "BLANK";
//    normalM->val_s = "voronoi";
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

        tileMaps(0, "create");

    //BASE
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

    ruffOren = make_shared<MultiAttr>();
    ruffOren->name = "ruffOren";
    ruffOren->type = "float";
    ruffOren->typeX = "OBJ";
    ruffOren->val_f = .5f;
    ruffOren->min = 0.f;
    ruffOren->max = 1.f;
    multiObj.push_back(ruffOren);

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
        shadowCast->val_b = true;
        multiObj.push_back(shadowCast);

        twoSided = make_shared<MultiAttr>();
        twoSided->name = "twoSided";
        twoSided->type = "enum";
        twoSided->val_s = "BLACK";
        twoSided->comboList = { "OFF", "BLACK", "FULL" };
        twoSided->repop = false;
        twoSided->tab = 1;
        multiObj.push_back(twoSided);

        Cwire = make_shared<MultiAttr>();
        Cwire->name = "Cwire";
        Cwire->type = "color";
        Cwire->typeX = "OBJ";
        Cwire->val_3 = glm::vec3(0.f);
        Cwire->tab = 1;
        multiObj.push_back(Cwire);

        normWt = make_shared<MultiAttr>();
        normWt->name = "normWt";
        normWt->type = "float";
        normWt->typeX = "OBJ";
        normWt->val_f = 1.f;
        normWt->min = 0.f;
        normWt->max = 1.f;
        normWt->tab = 1;
        multiObj.push_back(normWt);

        rotOrder = make_shared<MultiAttr>();
        rotOrder->name = "rotOrder";
        rotOrder->type = "enum";
        rotOrder->val_s = "XZY";
        rotOrder->comboList = { "XYZ", "YZX", "ZXY", "XZY", "YXZ", "ZYX" };
        rotOrder->repop = false;
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
    camLiType->comboList = { "DIR", "FPS", "ORTHO", "PERSP", "POINT", "SPOT" };
    camLiType->repop = false;
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
    nearShadow->val_f = 1.f;
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
    orthoFree->val_b = false;
    multiObj.push_back(orthoFree);

    orthoType = make_shared<MultiAttr>();
    orthoType->name = "orthoType";
    orthoType->type = "enum";
    orthoType->typeX = "CAMLI";
    orthoType->val_s = "TOP";
    orthoType->comboList = { "BACK", "BOTTOM", "FRONT", "LEFT", "RIGHT", "TOP" };
    orthoType->repop = false;
    multiObj.push_back(orthoType);

    orthoZoom = make_shared<MultiAttr>();
    orthoZoom->name = "orthoZoom";
    orthoZoom->type = "float";
    orthoZoom->typeX = "CAMLI";
    orthoZoom->val_f = 2.f;
    orthoZoom->min = .001f;
    multiObj.push_back(orthoZoom);

    //LIGHT
    lightSep = make_shared<MultiAttr>();
    lightSep->name = "light";
    lightSep->grp = "light";
    lightSep->type = "sep";
    lightSep->typeX = "CAMLI";
    multiObj.push_back(lightSep);

    Cl = make_shared<MultiAttr>();
    Cl->name = "Cl";
    Cl->grp = "light";
    Cl->type = "color";
    Cl->typeX = "CAMLI";
    Cl->val_3 = glm::vec3(1.f);
    multiObj.push_back(Cl);

    lInten = make_shared<MultiAttr>();
    lInten->name = "lInten";
    lInten->grp = "light";
    lInten->type = "float";
    lInten->typeX = "CAMLI";
    lInten->val_f = 3.f; //
    lInten->min = 0.f;
    multiObj.push_back(lInten);

    lSpotI = make_shared<MultiAttr>();
    lSpotI->name = "lSpotI";
    lSpotI->grp = "light";
    lSpotI->type = "float";
    lSpotI->typeX = "CAMLI";
    lSpotI->val_f = 10.f;
    lSpotI->min = 0.f;
    multiObj.push_back(lSpotI);

    lSpotO = make_shared<MultiAttr>();
    lSpotO->name = "lSpotO";
    lSpotO->grp = "light";
    lSpotO->type = "float";
    lSpotO->typeX = "CAMLI";
    lSpotO->val_f = 30.f;
    multiObj.push_back(lSpotO);

    //VOLUME LIGHT
    volSep = make_shared<MultiAttr>();
    volSep->name = "vol";
    volSep->grp = "vol";
    volSep->type = "sep";
    volSep->typeX = "CAMLI";
    multiObj.push_back(volSep);

    volCone = make_shared<MultiAttr>();
    volCone->name = "volCone";
    volCone->grp = "vol";
    volCone->type = "bool";
    volCone->typeX = "CAMLI";
    volCone->val_b = false;
    multiObj.push_back(volCone);

    volDist = make_shared<MultiAttr>();
    volDist->name = "volDist";
    volDist->grp = "vol";
    volDist->type = "float";
    volDist->typeX = "CAMLI";
    volDist->val_f = 5.5f;
    volDist->min = 0.f;
    multiObj.push_back(volDist);

    volS = make_shared<MultiAttr>();
    volS->name = "volS";
    volS->grp = "vol";
    volS->type = "vec3";
    volS->typeX = "CAMLI";
    volS->val_3 = glm::vec3(1.f);
    multiObj.push_back(volS);

    /* FSQ */
    //BLOOM
    bloomSep = make_shared<MultiAttr>();
    bloomSep->name = "bloom";
    bloomSep->grp = "bloom";
    bloomSep->type = "sep";
    bloomSep->typeX = "FBO";
    multiObj.push_back(bloomSep);

    bloomInten = make_shared<MultiAttr>();
    bloomInten->name = "inten";
    bloomInten->grp = "bloom";
    bloomInten->type = "float";
    bloomInten->typeX = "FBO";
    bloomInten->val_f = .1f;
    bloomInten->min = 0.f;
    bloomInten->max = 1.f;
    multiObj.push_back(bloomInten);

    bloomLensInten = make_shared<MultiAttr>();
    bloomLensInten->name = "intenL";
    bloomLensInten->grp = "bloom";
    bloomLensInten->type = "float";
    bloomLensInten->typeX = "FBO";
    bloomLensInten->val_f = .2f;
    bloomLensInten->min = 0.f;
    bloomLensInten->max = 1.f;
    multiObj.push_back(bloomLensInten);

    //HDR
    hdrSep = make_shared<MultiAttr>();
    hdrSep->name = "hdr";
    hdrSep->grp = "hdr";
    hdrSep->type = "sep";
    hdrSep->typeX = "FBO";
    multiObj.push_back(hdrSep);

    adaptTime = make_shared<MultiAttr>();
    adaptTime->name = "adaptTime";
    adaptTime->grp = "hdr";
    adaptTime->type = "float";
    adaptTime->typeX = "FBO";
    adaptTime->val_f = .5f;
    adaptTime->min = 0.f;
    multiObj.push_back(adaptTime);

    adaptAuto = make_shared<MultiAttr>();
    adaptAuto->name = "adaptAuto";
    adaptAuto->grp = "hdr";
    adaptAuto->type = "bool";
    adaptAuto->typeX = "FBO";
    adaptAuto->val_b = true;
    multiObj.push_back(adaptAuto);

    expo = make_shared<MultiAttr>();
    expo->name = "expo";
    expo->grp = "hdr";
    expo->type = "float";
    expo->typeX = "FBO";
    expo->val_f = 1.1f;
    expo->min = 1.001f;
    multiObj.push_back(expo);

    Kgi = make_shared<MultiAttr>();
    Kgi->name = "Kgi";
    Kgi->grp = "hdr";
    Kgi->type = "float";
    Kgi->typeX = "FBO";
//    Kgi->val_f = 1.f;
    Kgi->val_f = .75f;
    Kgi->min = 0.f;
    Kgi->max = 1.f;
    multiObj.push_back(Kgi);

    vign = make_shared<MultiAttr>();
    vign->name = "vign";
    vign->grp = "hdr";
    vign->type = "bool";
    vign->typeX = "FBO";
    vign->val_b = false;
    multiObj.push_back(vign);

    vignDist = make_shared<MultiAttr>();
    vignDist->name = "vignDist";
    vignDist->grp = "hdr";
    vignDist->type = "float";
    vignDist->typeX = "FBO";
    vignDist->val_f = 1.5f;
    vignDist->min = 1.3f;
    multiObj.push_back(vignDist);

        //FXAA
        fxaaSep = make_shared<MultiAttr>();
        fxaaSep->name = "fxaa";
        fxaaSep->grp = "fxaa";
        fxaaSep->type = "sep";
        fxaaSep->typeX = "FBO";
        fxaaSep->tab = 1;
        multiObj.push_back(fxaaSep);

        fxaaBlur = make_shared<MultiAttr>();
        fxaaBlur->name = "fxaaBlur";
        fxaaBlur->grp = "fxaa";
        fxaaBlur->type = "bool";
        fxaaBlur->typeX = "FBO";
        fxaaBlur->val_b = true;
        fxaaBlur->tab = 1;
        multiObj.push_back(fxaaBlur);

        fxaaSubPix = make_shared<MultiAttr>();
        fxaaSubPix->name = "subPix";
        fxaaSubPix->grp = "fxaa";
        fxaaSubPix->type = "float";
        fxaaSubPix->typeX = "FBO";
        fxaaSubPix->val_f = .75f;
        fxaaSubPix->min = .001f;
        fxaaSubPix->tab = 1;
        multiObj.push_back(fxaaSubPix);

        fxaaEdgeThr = make_shared<MultiAttr>();
        fxaaEdgeThr->name = "edgeThr";
        fxaaEdgeThr->grp = "fxaa";
        fxaaEdgeThr->type = "float";
        fxaaEdgeThr->typeX = "FBO";
        fxaaEdgeThr->val_f = .125f;
        fxaaEdgeThr->min = .001f;
        fxaaEdgeThr->tab = 1;
        multiObj.push_back(fxaaEdgeThr);

        fxaaEdgeThrMin = make_shared<MultiAttr>();
        fxaaEdgeThrMin->name = "edgeThrMin";
        fxaaEdgeThrMin->grp = "fxaa";
        fxaaEdgeThrMin->type = "float";
        fxaaEdgeThrMin->typeX = "FBO";
        fxaaEdgeThrMin->val_f = .0833f;
        fxaaEdgeThrMin->min = .001f;
        fxaaEdgeThrMin->tab = 1;
        multiObj.push_back(fxaaEdgeThrMin);

    //PAINT
    paintSep = make_shared<MultiAttr>();
    paintSep->name = "paint";
    paintSep->grp = "paint";
    paintSep->type = "sep";
    paintSep->typeX = "FBO";
    multiObj.push_back(paintSep);

    clearBehav = make_shared<MultiAttr>();
    clearBehav->name = "clearBehav";
    clearBehav->grp = "paint";
    clearBehav->type = "enum";
    clearBehav->typeX = "FBO";
    clearBehav->val_s = "OnBake";
    clearBehav->comboList = { "OnBake", "OnVMup", "Manual" };
    clearBehav->repop = false;
    multiObj.push_back(clearBehav);

    displMode = make_shared<MultiAttr>();
    displMode->name = "displMode";
    displMode->grp = "paint";
    displMode->type = "enum";
    displMode->typeX = "FBO";
    displMode->val_s = "FULL";
    displMode->comboList = { "FULL", "LAYER", "MAP" };
    displMode->repop = false;
    multiObj.push_back(displMode);

    edgeThr = make_shared<MultiAttr>();
    edgeThr->name = "edgeThr";
    edgeThr->grp = "paint";
    edgeThr->type = "float";
    edgeThr->typeX = "FBO";
    edgeThr->val_f = .6f;
    edgeThr->min = .001f;
    multiObj.push_back(edgeThr);

    //SSAO
    ssaoSep = make_shared<MultiAttr>();
    ssaoSep->name = "ssao";
    ssaoSep->grp = "ssao";
    ssaoSep->type = "sep";
    ssaoSep->typeX = "FBO";
    multiObj.push_back(ssaoSep);

    ssaoBias = make_shared<MultiAttr>();
    ssaoBias->name = "bias";
    ssaoBias->grp = "ssao";
    ssaoBias->type = "float";
    ssaoBias->typeX = "FBO";
    ssaoBias->val_f = .075f;
    ssaoBias->min = 0.f;
    ssaoBias->max = 1.f;
    multiObj.push_back(ssaoBias);

    ssaoInten = make_shared<MultiAttr>();
    ssaoInten->name = "inten";
    ssaoInten->grp = "ssao";
    ssaoInten->type = "float";
    ssaoInten->typeX = "FBO";
    ssaoInten->val_f = 4.f;
    ssaoInten->min = 0.f;
    multiObj.push_back(ssaoInten);

    ssaoKernel = make_shared<MultiAttr>();
    ssaoKernel->name = "kernel";
    ssaoKernel->grp = "ssao";
    ssaoKernel->type = "enum";
    ssaoKernel->typeX = "FBO";
    ssaoKernel->val_s = "32";
    ssaoKernel->comboList = { "32", "64" };
    ssaoKernel->repop = false;
    multiObj.push_back(ssaoKernel);

    ssaoRad = make_shared<MultiAttr>();
    ssaoRad->name = "rad";
    ssaoRad->grp = "ssao";
    ssaoRad->type = "float";
    ssaoRad->typeX = "FBO";
    ssaoRad->val_f = 1.f;
    ssaoRad->min = .001f;
    multiObj.push_back(ssaoRad);

    //TRANSP
    transpSep = make_shared<MultiAttr>();
    transpSep->name = "transp";
    transpSep->grp = "transp";
    transpSep->type = "sep";
    transpSep->typeX = "FBO";
    multiObj.push_back(transpSep);

    transpW = make_shared<MultiAttr>();
    transpW->name = "transpW";
    transpW->grp = "transp"; //TEMP
    transpW->type = "float";
    transpW->typeX = "FBO";
    transpW->val_f = .005f;
    transpW->min = .001f;
    transpW->max = 1.f;
    multiObj.push_back(transpW);

    //SSR
    ssrSep = make_shared<MultiAttr>();
    ssrSep->name = "ssr";
    ssrSep->grp = "ssr";
    ssrSep->type = "sep";
    ssrSep->typeX = "FBO";
    ssrSep->tab = 1;
    multiObj.push_back(ssrSep);

    ssrIter = make_shared<MultiAttr>();
    ssrIter->name = "iter";
    ssrIter->grp = "ssr";
    ssrIter->type = "int";
    ssrIter->typeX = "FBO";
    ssrIter->val_i = 75;
    ssrIter->min = 1;
    ssrIter->tab = 1;
    multiObj.push_back(ssrIter);

    ssrRefine = make_shared<MultiAttr>();
    ssrRefine->name = "refine";
    ssrRefine->grp = "ssr";
    ssrRefine->type = "int";
    ssrRefine->typeX = "FBO";
    ssrRefine->val_i = 4;
    ssrRefine->min = 0;
    ssrRefine->tab = 1;
    multiObj.push_back(ssrRefine);

    ssrPixStride = make_shared<MultiAttr>();
    ssrPixStride->name = "pixStride";
    ssrPixStride->grp = "ssr";
    ssrPixStride->type = "int";
    ssrPixStride->typeX = "FBO";
    ssrPixStride->val_i = 10;
    ssrPixStride->min = 0;
    ssrPixStride->tab = 1;
    multiObj.push_back(ssrPixStride);

    ssrPixStrideZ = make_shared<MultiAttr>();
    ssrPixStrideZ->name = "pixStrideZ";
    ssrPixStrideZ->grp = "ssr";
    ssrPixStrideZ->type = "float";
    ssrPixStrideZ->typeX = "FBO";
    ssrPixStrideZ->val_f = 13.f;
    ssrPixStrideZ->min = 0.f;
    ssrPixStrideZ->tab = 1;
    multiObj.push_back(ssrPixStrideZ);

    ssrPixZSize = make_shared<MultiAttr>();
    ssrPixZSize->name = "pixZSize";
    ssrPixZSize->grp = "ssr";
    ssrPixZSize->type = "float";
    ssrPixZSize->typeX = "FBO";
    ssrPixZSize->val_f = .1f;
    ssrPixZSize->min = 0.f;
    ssrPixZSize->tab = 1;
    multiObj.push_back(ssrPixZSize);

    ssrMaxRayDist = make_shared<MultiAttr>();
    ssrMaxRayDist->name = "maxRayDist";
    ssrMaxRayDist->grp = "ssr";
    ssrMaxRayDist->type = "float";
    ssrMaxRayDist->typeX = "FBO";
    ssrMaxRayDist->val_f = 10.f;
    ssrMaxRayDist->min = 0.f;
    ssrMaxRayDist->tab = 1;
    multiObj.push_back(ssrMaxRayDist);

    ssrEdgeFade = make_shared<MultiAttr>();
    ssrEdgeFade->name = "edgeFade";
    ssrEdgeFade->grp = "ssr";
    ssrEdgeFade->type = "float";
    ssrEdgeFade->typeX = "FBO";
    ssrEdgeFade->val_f = .75f;
    ssrEdgeFade->min = 0.f;
    ssrEdgeFade->max = .99f;
    ssrEdgeFade->tab = 1;
    multiObj.push_back(ssrEdgeFade);

    ssrEyeFade0 = make_shared<MultiAttr>();
    ssrEyeFade0->name = "eyeFade0";
    ssrEyeFade0->grp = "ssr";
    ssrEyeFade0->type = "float";
    ssrEyeFade0->typeX = "FBO";
    ssrEyeFade0->val_f = 0.f;
    ssrEyeFade0->min = 0.f;
    ssrEyeFade0->max = 1.f;
    ssrEyeFade0->tab = 1;
    multiObj.push_back(ssrEyeFade0);

    ssrEyeFade1 = make_shared<MultiAttr>();
    ssrEyeFade1->name = "eyeFade1";
    ssrEyeFade1->grp = "ssr";
    ssrEyeFade1->type = "float";
    ssrEyeFade1->typeX = "FBO";
    ssrEyeFade1->val_f = 1.f;
    ssrEyeFade1->min = 0.f;
    ssrEyeFade1->max = 1.f;
    ssrEyeFade1->tab = 1;
    multiObj.push_back(ssrEyeFade1);

    populateTexCombos();

    dirtyShadow = dirtyVM = true;
    expand = gridV = ignoreOutliner = ref = selected = false;
    deletable = selectable = true;
    type = "EMPTY";
    VAO_loaded = false;
    VAO = 0;

    biasM = glm::mat4(
        .5f, 0.f, 0.f, 0.f,\
        0.f, .5f, 0.f, 0.f,
        0.f, 0.f, .5f, 0.f,
        .5f, .5f, .5f, 1.f);

    Crand = genColorID();

    // TEXSEL
    texSel.type = "ALBEDO"; //
//    texSel.type = "ALPHA";
    texSel.map.name = "BLANK";
    texSel.map.ID = 9999;
    texSel.layer.name = "layer0";
    texSel.layer.ID = 9999;

    setTexSel_init(); // for non selected storage

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

void Object::setTexSel_init()
{
    for (auto &i : myWin.myGLWidgetSh->typeList)
    {
        //typeList << "ALBEDO" << "ALPHA" << "LENS" << "METALLIC" << "RUFF" << "SSS";

        for (auto &j : myWin.myGLWidgetSh->allMaps)
        {
            //GET STARTING TEXSEL.MAP.ID AND TEXSEL.LAYER.ID
            if (j.type == texSel.type && j.name == texSel.map.name)
            {
                texSel.map.ID = j.ID;

                for (auto &k : j.layer)
                {
                    if (k.name == texSel.layer.name)
                        texSel.layer.ID = k.ID;
                }
            }

            if (j.type == i)
            {
                if (j.name == texSel.map.name) //"BLANK"
                {
                    //texSel (on init) is: type, ID (0), name, ID
                    texSel.typeMap.push_back( { j.type, 0, j.name, j.ID } );
                }

                for (auto &k : j.layer)
                {
                    if (k.name == texSel.layer.name) //"layer0"
                        texSel.mapLayer.push_back( { j.name, j.ID, k.name, k.ID } );
                }
            }
        }
    }
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

                //tile
                else if (multiObj[i]->name == "tile") tileSep = multiObj[i];

            //BASE
            else if (multiObj[i]->name == "Ko") Ko = multiObj[i];
            else if (multiObj[i]->name == "ior") ior = multiObj[i];
            else if (multiObj[i]->name == "ruffA") ruffA = multiObj[i];
            else if (multiObj[i]->name == "ruffOren") ruffOren = multiObj[i];
            else if (multiObj[i]->name == "sssSpread") sssSpread = multiObj[i];

                else if (multiObj[i]->name == "shadowCast") shadowCast = multiObj[i];
                else if (multiObj[i]->name == "twoSided") twoSided = multiObj[i];
                else if (multiObj[i]->name == "Cwire") Cwire = multiObj[i];
                else if (multiObj[i]->name == "normWt") normWt = multiObj[i];
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
            else if (multiObj[i]->name == "bloomInten") bloomInten = multiObj[i];
            else if (multiObj[i]->name == "bloomLensInten") bloomLensInten = multiObj[i];
            else if (multiObj[i]->name == "lensM") lensM = multiObj[i];

            //HDR
            else if (multiObj[i]->name == "hdr") hdrSep = multiObj[i];
            else if (multiObj[i]->name == "expo") expo = multiObj[i];
            else if (multiObj[i]->name == "Kgi") Kgi = multiObj[i];
            else if (multiObj[i]->name == "adaptTime") adaptTime = multiObj[i];
            else if (multiObj[i]->name == "adaptAuto") adaptAuto = multiObj[i];
            else if (multiObj[i]->name == "vign") vign = multiObj[i];
            else if (multiObj[i]->name == "vignDist") vignDist = multiObj[i];

                //FXAA
                else if (multiObj[i]->name == "fxaa") fxaaSep = multiObj[i];
                else if (multiObj[i]->name == "fxaaBlur") fxaaBlur = multiObj[i];
                else if (multiObj[i]->name == "fxaaSubPix") fxaaSubPix = multiObj[i];
                else if (multiObj[i]->name == "fxaaEdgeThr") fxaaEdgeThr = multiObj[i];
                else if (multiObj[i]->name == "fxaaEdgeThrMin") fxaaEdgeThrMin = multiObj[i];

            //PAINT
            else if (multiObj[i]->name == "paint") paintSep = multiObj[i];
            else if (multiObj[i]->name == "clearBehav") clearBehav = multiObj[i];
            else if (multiObj[i]->name == "displMode") displMode = multiObj[i];
            else if (multiObj[i]->name == "edgeThr") edgeThr = multiObj[i];

            //SSAO
            else if (multiObj[i]->name == "ssao") ssaoSep = multiObj[i];
            else if (multiObj[i]->name == "ssaoBias") ssaoBias = multiObj[i];
            else if (multiObj[i]->name == "ssaoInten") ssaoInten = multiObj[i];
            else if (multiObj[i]->name == "ssaoKernel") ssaoKernel = multiObj[i];
            else if (multiObj[i]->name == "ssaoRad") ssaoRad = multiObj[i];

            //TRANSP
            else if (multiObj[i]->name == "transpSep") transpSep = multiObj[i];
            else if (multiObj[i]->name == "transpW") transpW = multiObj[i];

            //SSR
            else if (multiObj[i]->name == "ssrSep") ssrSep = multiObj[i];
            else if (multiObj[i]->name == "ssrIter") ssrIter = multiObj[i];
            else if (multiObj[i]->name == "ssrRefine") ssrRefine = multiObj[i];
            else if (multiObj[i]->name == "ssrPixStride") ssrPixStride = multiObj[i];
            else if (multiObj[i]->name == "ssrPixZSize") ssrPixZSize = multiObj[i];
            else if (multiObj[i]->name == "ssrPixStrideZ") ssrPixStrideZ = multiObj[i];
            else if (multiObj[i]->name == "ssrMaxRayDist") ssrMaxRayDist = multiObj[i];
            else if (multiObj[i]->name == "ssrEdgeFade") ssrEdgeFade = multiObj[i];
            else if (multiObj[i]->name == "ssrEyeFade0") ssrEyeFade0 = multiObj[i];
            else if (multiObj[i]->name == "ssrEyeFade1") ssrEyeFade1 = multiObj[i];
        }
    }

    //VBO vertex attrs
    VAO_loaded = 0;
    VAO = 0;

    pE = obj.pE;
    nE = obj.nE;
    tE = obj.tE;
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
    type = obj.type;

    //GLSL
    Crand = genColorID();
    showN = obj.showN;
    gaussStage = obj.gaussStage;

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
    //repop is for MAP enums which could change in name / size as opposed to a regular enum like clearBehav
    for (auto &i : multiObj)
    {
        if (i->type == "enum" && i->repop)
        {
            i->comboList.clear();

            auto multiUp = myWin.stringToUpper(i->name);
            multiUp.resize(multiUp.size() - 1);

            for (auto &j : myWin.myGLWidgetSh->allMaps)
            {
                if (multiUp == j.type)
                    i->comboList.push_back(j.name);
            }
        }
    }
}

void Object::VAO_load()
{
    glGetError();

    if (!VAO_loaded)
    {
        for (auto &j : myWin.myGLWidgetSh->GLDataSh)
        {
            if (j.obj.get() == this)
            {
                glCreateVertexArrays(1, &VAO);

                if (type == "BB")
                {
                    glEnableVertexArrayAttrib(VAO, 0);
                    glVertexArrayVertexBuffer(VAO, 0, j.VBO_P, 0, 12);
                    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

                    glVertexArrayElementBuffer(VAO, j.VBO_IDX);
                }

                else if (type == "GRID")
                {
                    glEnableVertexArrayAttrib(VAO, 0);
                    glVertexArrayVertexBuffer(VAO, 0, j.VBO_P, 0, 12);
                    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
                }

                else if (type == "TXT")
                {
                    glEnableVertexArrayAttrib(VAO, 0);
                    glVertexArrayVertexBuffer(VAO, 0, j.VBO_P, 0, 1);
                    glVertexArrayAttribIFormat(VAO, 0, 1, GL_UNSIGNED_BYTE, 0);
                }

                else if (type == "GIZ_CIRC" || type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_CIRC_HALF" || type == "GIZ_LINE" || type == "CAMLI")
                {
                    glEnableVertexArrayAttrib(VAO, 0);
                    glVertexArrayVertexBuffer(VAO, 0, j.VBO_P, 0, 12);
                    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

                    if (type == "GIZ_CUBE")
                        glVertexArrayElementBuffer(VAO, j.VBO_IDX);
                }

                else if (type == "OBJ" || type == "VOL")
                {
                    glEnableVertexArrayAttrib(VAO, 0);
                    glVertexArrayVertexBuffer(VAO, 0, j.VBO_P, 0, 12);
                    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

                    glEnableVertexArrayAttrib(VAO, 1);
                    glVertexArrayVertexBuffer(VAO, 1, j.VBO_UV, 0, 8);
                    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 0);

                    glEnableVertexArrayAttrib(VAO, 2);
                    glVertexArrayVertexBuffer(VAO, 2, j.VBO_T, 0, 12);
                    glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, 0);

                    glEnableVertexArrayAttrib(VAO, 3);
                    glVertexArrayVertexBuffer(VAO, 3, j.VBO_N, 0, 12);
                    glVertexArrayAttribFormat(VAO, 3, 3, GL_FLOAT, GL_FALSE, 0);

                    glVertexArrayElementBuffer(VAO, j.VBO_IDX);
                }

                vertsOnObj = (int)pE.size();
            }
        }

        VAO_loaded = true;
    }

    glErrorPrint("Couldn't load VAO");
}

void Object::BBup()
{
    bbMin = (pE.empty()) ? glm::vec3(0.f) : pE[0];
    bbMax = (pE.empty()) ? glm::vec3(0.f) : pE[0];

    for (auto &i : pE)
    {
        bbMin = glm::min(i, bbMin);
        bbMax = glm::max(i, bbMax);
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

    auto V = glm::lookAt(t->val_3, targO, upO);

    if (V[0][0] < 0)
        r->val_3.x = glm::degrees((float)(PI - asinf(-V[2][0])));

    else
        r->val_3.x = glm::degrees(asinf(-V[2][0]));

    r->val_3.y = glm::degrees(asinf(-V[1][2]));

    setDirty();
}

glm::mat4 Object::rotOrderUse(string myRotOrder)
{
    string rotOrder0(1, myRotOrder.at(0));
    string rotOrder1(1, myRotOrder.at(1));
    string rotOrder2(1, myRotOrder.at(2));

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

bool Object::camLiTypeGet(string toFind)
{
    auto found = false;

    if (type == "CAMLI")
    {
        auto myType = camLiType->val_s;

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

void Object::mvpGet()
{
    SM = glm::scale(glm::mat4(), s->val_3);

    if (type == "CAMLI" && camLiType->val_s != "DIR")
        RM = glm::yawPitchRoll(glm::radians(r->val_3.x), glm::radians(r->val_3.y), 0.f);

    else
        RM = rotOrderUse(rotOrder->val_s);

    TM = glm::translate(glm::mat4(), t->val_3);

    if (this == myWin.paintStroke.get())
    {
        PM2D = glm::ortho(-1.f * myWin.myGL->aspect, 1.f * myWin.myGL->aspect, -1.f, 1.f, -1.f, 1.f);
        TM = glm::translate(glm::mat4(), t->val_3 * myWin.myGL->aspectXYZ);
    }

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
        PM = myWin.myGL->PMgizSide;
        VM = myWin.myGL->VMgizSide;
    }

    else
    {
        PM = myWin.myGL->selCamLi->PM;
        VM = myWin.myGL->selCamLi->VM;
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
    else if (this == myWin.paintStroke.get()) MVP = PM2D * MM;
    else MVP = PM * MV;

    NM = glm::mat3(glm::transpose(glm::inverse(MV)));

    if (type == "CAMLI")
    {
        PM = PM_stored;
        VM = VM_stored;
    }
}

void Object::proUse()
{
    for (auto &i : myWin.myGLWidgetSh->GLDataSh)
    {
        if (i.obj.get() == this)
        {
            auto &proH = myWin.myGLWidgetSh->pro;
            auto proN = myWin.myGLWidgetSh->proN;

            if ((proN == myWin.myGL->pGBufferDyn) && myWin.lightCt > 0) //
                shadowPass();

            //OBJ MATRIX
            glUniformMatrix4fv(glGetUniformLocation(proH, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix3fv(glGetUniformLocation(proH, "NM"), 1, GL_FALSE, &NM[0][0]);

            glm::vec4 comboU;

            if (proN == "pBB")
            {
                if (myWin.myGL->colorPickTgl)
                    comboU = glm::vec4(Crand, 1.f);

                else
                {
                    auto temp = (selected) ? myWin.glslTable->Csel->val_3 : Cwire->val_3;
                    comboU = glm::vec4(temp, 1.f);
                }

                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.r);
            }

            if (proN == "pWireframe")
            {
                auto temp = (selected) ? myWin.glslTable->Csel->val_3 : Cwire->val_3;
                comboU = glm::vec4(temp, 1.f);
                glUniform4fv(glGetUniformLocation(proH, "Cwire"), 1, &comboU.x);
            }

            if (proN == "pSky")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "PMinv_cube"), 1, GL_FALSE, &myWin.myGL->PMinv_cube[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(proH, "VM"), 1, GL_FALSE, &VM[0][0]);
                glBindTextureUnit(0, myWin.cubeM_specular_32);
            }

            else if (proN == "pGrid")
            {
                comboU = glm::vec4(myWin.glslTable->Cgrid->val_3, 1.f);
                glUniform4fv(glGetUniformLocation(proH, "Cgrid"), 1, &comboU.x);
            }

            else if (proN == "pShadow") //WRITE TO A SINGLE LIGHT
            {
                glm::mat4 PM_shadow, VM_shadow, depthMVP;

                if (myWin.myGLWidgetSh->shadowObj->camLiType->val_s == "DIR") //
                {
                    PM_shadow = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 20.f);
                    VM_shadow = glm::inverse(myWin.myGLWidgetSh->shadowObj->RM);
                    depthMVP = PM_shadow * VM_shadow * MM;
                }

                else if (myWin.myGLWidgetSh->shadowObj->camLiType->val_s == "POINT")
                {
                    depthMVP = myWin.myGLWidgetSh->PM_cube * myWin.myGLWidgetSh->VM_cube * myWin.myGLWidgetSh->MM_cube * MM;
                }

                else if (myWin.myGLWidgetSh->shadowObj->camLiType->val_s == "SPOT")
                {
                    PM_shadow = glm::perspective(glm::radians(myWin.myGLWidgetSh->shadowObj->fov->val_f), 1.f, 2.f, 100.f);
                    VM_shadow = myWin.myGLWidgetSh->shadowObj->VM;
                    depthMVP = PM_shadow * VM_shadow * MM;
                }

                glUniformMatrix4fv(glGetUniformLocation(proH, "depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);
            }

            else if (proN == "pTransp")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "MV"), 1, GL_FALSE, &MV[0][0]);

                tileMaps(proH, "get");

                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.name == albedoM->val_s && j.type == "ALBEDO")
                        glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                    else if (j.name == alphaM->val_s && j.type == "ALPHA")
                        glProgramUniformHandleui64ARB(proH, 1, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                }

                comboU = glm::vec4(myWin.myFSQ->transpW->val_f, myWin.myGL->debug0, Ko->val_f, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }

            else if (proN == "pTranspComp")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->gBuf6_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->gBuf7_64);
            }

            else if (proN == myWin.myGL->pGBufferDyn)
            {
//                /* LOOK AT (BILLBOARDING, MOVE TO SEP SHADER) */
//                auto PV = glm::mat4(PM * VM);
//                glUniformMatrix4fv(glGetUniformLocation(proH, "PV"), 1, GL_FALSE, &PV[0][0]);

//                glm::vec3 camRight_WS(VM[0][0], VM[1][0], VM[2][0]);
//                glm::vec3 camUP_WS(VM[0][1], VM[1][1], VM[2][1]);

//                comboU = glm::vec4(camRight_WS, 0.f);
//                glUniform4fv(glGetUniformLocation(proH, "camRight_WS"), 1, &comboU.x);

//                comboU = glm::vec4(camUP_WS, 0.f);
//                glUniform4fv(glGetUniformLocation(proH, "camUp_WS"), 1, &comboU.x);


                glUniformMatrix4fv(glGetUniformLocation(proH, "MM"), 1, GL_FALSE, &MM[0][0]);

                comboU = glm::vec4(ior->val_f, ruffOren->val_f, Ko->val_f, myWin.myGL->debug0);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);

                auto shadowCastF = (shadowCast->val_b) ? 1.f : 0.f;
                comboU = glm::vec4(ruffA->val_2, shadowCastF, sssSpread->val_f);
                glUniform4fv(glGetUniformLocation(proH, "comboU1"), 1, &comboU.x);

                tileMaps(proH, "get");

                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.name == albedoM->val_s && j.type == "ALBEDO")
                        glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                    else if (j.name == alphaM->val_s && j.type == "ALPHA")
                        glProgramUniformHandleui64ARB(proH, 1, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                    else if (j.name == anisoM->val_s && j.type == "ANISO")
                    {
                        float anisoType;

                        if (anisoM->val_s == "BLANK")
                            anisoType = 0.f;

                        else if (anisoM->val_s == "VIEW")
                            anisoType = 1.f;

                        else //CUSTOM MAP
                            anisoType = 2.f;

                        glUniform1f(glGetUniformLocation(proH, "anisoType"), anisoType);
                        glProgramUniformHandleui64ARB(proH, 2, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                    }

                    else if (j.name == metallicM->val_s && j.type == "METALLIC")
                        glProgramUniformHandleui64ARB(proH, 3, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                    else if (j.name == normalM->val_s && j.type == "NORMAL")
                        glProgramUniformHandleui64ARB(proH, 4, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                    else if (j.name == ruffM->val_s && j.type == "RUFF")
                        glProgramUniformHandleui64ARB(proH, 5, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                    else if (j.name == sssM->val_s && j.type == "SSS")
                        glProgramUniformHandleui64ARB(proH, 6, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                }

                glUniform1i(glGetUniformLocation(proH, "NUM_LIGHTS"), myWin.lightCt);

                //pt shadows at 7/8
                //reg shadows at 9/10/11

                //for future release : shadows only need to be generated up at a certain distance (25, 50)
                    //in pShadow only render objs at dist < 50
                    //in gBuffer only calc lights for objs at dist < 50

                int start = 7;
                int realIdx = 0;

                //PT SHADOWS SHOULD BE 7/8
                for (unsigned int j = 0; j < myWin.myGLWidgetSh->allShadow.size(); ++j)
                {
                    for (unsigned int k = 0; k < myWin.allCamLi.size(); ++k)
                    {
                        if (myWin.myGLWidgetSh->allShadow[j].name == myWin.allCamLi[k]->name->val_s)
                        {
                            if (myWin.allCamLi[k]->camLiType->val_s == "POINT")
                            {
                                //cout << "POINT SHADOWS for : " << myWin.myGLWidgetSh->allShadow[j].name << " " << start + realIdx << " " << start << " " << realIdx << endl;
                                glBindTextureUnit(start + realIdx, myWin.myGLWidgetSh->allShadow[j].tex2_32);
                                ++realIdx;
                            }
                        }
                    }
                }

                start = 9;
                realIdx = 0;

                //SPOT SHADOWS SHOULD BE 9/10
                for (unsigned int j = 0; j < myWin.myGLWidgetSh->allShadow.size(); ++j)
                {
                    for (unsigned int k = 0; k < myWin.allCamLi.size(); ++k)
                    {
                        if (myWin.myGLWidgetSh->allShadow[j].name == myWin.allCamLi[k]->name->val_s)
                        {
                            if (myWin.allCamLi[k]->camLiType->val_s == "DIR" || myWin.allCamLi[k]->camLiType->val_s == "SPOT")
                            {
                                //cout << "SPOT SHADOWS for : " << myWin.myGLWidgetSh->allShadow[j].name << " " << start + realIdx << " " << start << " " << realIdx << endl;
                                glProgramUniformHandleui64ARB(proH, start + realIdx, myWin.myGLWidgetSh->allShadow[j].tex1_64);
                                ++realIdx;

                            }
                        }
                    }
                }
            }

            else if (proN == myWin.myPP->pDefDyn)
            {
//                cout << "in pDefDyn for : " << myWin.myPP->pDefDyn << endl;

                auto PMinv = glm::inverse(myWin.myGL->selCamLi->PM);
                glUniformMatrix4fv(glGetUniformLocation(proH, "PMinv"), 1, GL_FALSE, &PMinv[0][0]);

                glUniformMatrix4fv(glGetUniformLocation(proH, "VM"), 1, GL_FALSE, &VM[0][0]);

                auto VMinv = glm::inverse(VM);
                glUniformMatrix4fv(glGetUniformLocation(proH, "VMinv"), 1, GL_FALSE, &VMinv[0][0]);

                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->gBuf0_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->gBuf1_64);
                glProgramUniformHandleui64ARB(proH, 2, myWin.myGL->gBuf2_64);
                glProgramUniformHandleui64ARB(proH, 3, myWin.myGL->gBuf3_64);
                glProgramUniformHandleui64ARB(proH, 4, myWin.myGL->gBuf4_64);
                glProgramUniformHandleui64ARB(proH, 5, myWin.myGL->gBuf5_64);
//                glProgramUniformHandleui64ARB(proH, 6, myWin.myGL->gBuf6_64);
//                glProgramUniformHandleui64ARB(proH, 7, myWin.myGL->gBuf7_64);
                glProgramUniformHandleui64ARB(proH, 8, myWin.myGL->ssaoGaussN.tex2_64);
                glProgramUniformHandleui64ARB(proH, 9, myWin.myGL->bgN.tex1_64); // sky
                glBindTextureUnit(10, myWin.cubeM_specular_32);
                glBindTextureUnit(11, myWin.cubeM_irradiance_32);
                glBindTextureUnit(12, myWin.myGL->gBuf_DS_32);

                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.name == "sssLookup")
                        glProgramUniformHandleui64ARB(proH, 12, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                }

                comboU = glm::vec4(myWin.myFSQ->Kgi->val_f, myWin.lightCt, myWin.myGL->debug0, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }

            else if (proN == "pLumaInit")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->deferredN.tex1_64);
            }

            else if (proN == "pLumaAdapt")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->lumaAdaptN[!myWin.myGL->currLum].tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->lumaInitN.tex1_64);

                glUniform1f(glGetUniformLocation(proH, "dTime"), myWin.myGL->dTime);
            }

            else if (proN == "pLumaAdapt_viz")
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->lumaAdaptN[!myWin.myGL->currLum].tex1_64);

            else if (proN == "pDown")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->downSamp_64);
                glUniform2fv(glGetUniformLocation(proH, "texelSize"), 1, &myWin.myGL->texelSize.r);
            }

            else if (proN == "pGauss")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->rttGaussIn64);
                glUniform1i(glGetUniformLocation(proH, "gaussStage"), gaussStage);
            }

            else if (proN == "pBloom")
            {
                for (int j = 0; j < 6; ++j)
                    glProgramUniformHandleui64ARB(proH, j, myWin.myGL->bloomGaussN[j].tex2_64);

                glProgramUniformHandleui64ARB(proH, 6, myWin.myGL->deferredN.tex1_64);

                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.name == lensM->val_s && j.type == "LENS")
                        glProgramUniformHandleui64ARB(proH, 7, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                }

                comboU = glm::vec4(myWin.myFSQ->bloomInten->val_f, myWin.myFSQ->bloomLensInten->val_f, 0.f, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }

            else if (proN == "pBloomC")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->deferredN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->bloomN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 2, myWin.myGL->ssaoGaussN.tex2_64);
            }

            else if (proN == "pSSAO_32" || proN == "pSSAO_64")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "PM"), 1, GL_FALSE, &PM[0][0]);

                auto PMinv = glm::inverse(myWin.myGL->selCamLi->PM);
                glUniformMatrix4fv(glGetUniformLocation(proH, "PMinv"), 1, GL_FALSE, &PMinv[0][0]);

                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->gBuf1_64);
                glBindTextureUnit(1, myWin.myGL->gBuf_DS_32);

                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.name == "rand")
                        glProgramUniformHandleui64ARB(proH, 2, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                }

                comboU = glm::vec4(ssaoBias->val_f, ssaoInten->val_f, ssaoRad->val_f, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }

            else if (proN == "pSSR")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "PM"), 1, GL_FALSE, &PM[0][0]);

                auto PMinv = glm::inverse(PM);
                glUniformMatrix4fv(glGetUniformLocation(proH, "PMinv"), 1, GL_FALSE, &PMinv[0][0]);

                // projection matrix that maps to screen pixels (not NDC)
                auto screenScaleM = glm::scale(glm::mat4(), glm::vec3(myWin.myGL->width(), myWin.myGL->height(), 1.f));

                auto trs = glm::mat4(1.f);
                trs[0][0] = .5f;
                trs[0][3] = .5f;
                trs[1][1] = .5f;
                trs[1][3] = .5f;

                auto PM_d3d = PM;
                float temp23 = PM_d3d[2][3];
                PM_d3d[2][3] = PM_d3d[3][2];
                PM_d3d[3][2] = temp23;

                auto PM_SS_d3d = PM_d3d * trs * screenScaleM;
                glUniformMatrix4fv(glGetUniformLocation(proH, "PM_SS_d3d"), 1, GL_FALSE, &PM_SS_d3d[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(proH, "VM"), 1, GL_FALSE, &VM[0][0]);

                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->gBuf1_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->gBuf2_64);
                glBindTextureUnit(2, myWin.myGL->gBuf_DS_32);
                glProgramUniformHandleui64ARB(proH, 3, myWin.myGL->depthRevN.DS_64);
                glProgramUniformHandleui64ARB(proH, 4, myWin.myGL->tonemapN.tex1_64); // w/ giz
                //glProgramUniformHandleui64ARB(proH, 4, myWin.myGL->tonemapN.tex2_64); // no giz

                comboU = glm::vec4(myWin.myGL->selCamLi->farClip->val_f, myWin.myGL->selCamLi->nearClip->val_f, myWin.myFSQ->ssrIter->val_i, myWin.myFSQ->ssrRefine->val_i);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);

                comboU = glm::vec4(myWin.myFSQ->ssrPixStride->val_i, myWin.myFSQ->ssrPixStrideZ->val_f, myWin.myFSQ->ssrPixZSize->val_f, myWin.myFSQ->ssrMaxRayDist->val_f);
                glUniform4fv(glGetUniformLocation(proH, "comboU1"), 1, &comboU.x);

                comboU = glm::vec4(myWin.myFSQ->ssrEdgeFade->val_f, myWin.myFSQ->ssrEyeFade0->val_f, myWin.myFSQ->ssrEyeFade1->val_f, myWin.myGL->debug0);
                glUniform4fv(glGetUniformLocation(proH, "comboU2"), 1, &comboU.x);
            }

            else if (proN == "pTonemap")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->bloomCN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].tex1_64);
                glProgramUniformHandleui64ARB(proH, 2, myWin.myGL->bgN.tex2_64);

                comboU = glm::vec4(log(myWin.myFSQ->expo->val_f), myWin.myFSQ->adaptAuto->val_b, myWin.myFSQ->vign->val_b, myWin.myFSQ->vignDist->val_f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }

            else if (proN == "pFxaa")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->tonemapN.tex1_64);

                comboU = glm::vec4(myWin.myGL->edgeDetect_mode, myWin.myFSQ->fxaaSubPix->val_f, myWin.myFSQ->fxaaEdgeThr->val_f, myWin.myFSQ->fxaaEdgeThrMin->val_f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);

                glUniform1i(glGetUniformLocation(proH, "debug0"), myWin.myGL->debug0);
            }

            else if (proN == "pFinal")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->fxaaN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->ssrN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 2, myWin.myGL->cursorN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 3, myWin.myGL->deferredN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 4, myWin.myGL->brushN.tex2_64);
                glProgramUniformHandleui64ARB(proH, 5, myWin.myGL->gBuf6_64); //BOIT accum
                glProgramUniformHandleui64ARB(proH, 6, myWin.myGL->gBuf7_64); //BOIT revealage

                comboU = glm::vec4(myWin.myGL->rezGate_LD, myWin.myGL->rezGate_RU);
                glUniform4fv(glGetUniformLocation(proH, "LDRU"), 1, &comboU.x);

                auto rezGateTgl = (myWin.myGL->rezGateTgl || myWin.myGL->rezGateTgl_sel) ? true : false;
                comboU = glm::vec4(rezGateTgl, myWin.myGL->dragDrop, myWin.myGL->debug0, myWin.myGL->debug1);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }

            else if (proN == "pEdgeDetect")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->alphaGaussN.tex2_64);

//                comboU = glm::vec4(myWin.myGL->edgeDetect_mode, myWin.myFSQ->edgeThr->val_f, 0.f, 0.f);
                comboU = glm::vec4(myWin.myGL->edgeDetect_mode, .6f, 0.f, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }

            else if (proN == "pPaintStroke")
            {
                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.type == "BRUSH")
                    {
                        if (j.name == myWin.myGLWidgetSh->selBrush->name)
                        {
                            if (myWin.myGL->paintType == "BRUSH")
                                glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                            else if (myWin.myGL->paintType == "BRUSH_CURSOR")
                                glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex2_64);
                        }

                        else if (j.name == myWin.myGLWidgetSh->selEraser->name)
                        {
                            if (myWin.myGL->paintType == "ERASER")
                                glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex1_64);

                            else if (myWin.myGL->paintType == "ERASER_CURSOR")
                                glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex2_64);
                        }
                    }
                }

                glUniform4fv(glGetUniformLocation(proH, "brushRGBA"), 1, &myWin.myGL->brushRGBA.x);
            }

            else if (proN == "pEraseMix")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushBGN.tex2_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->eraserN.tex1_64);
            }

            else if (proN == "pCopyTex")
            {
                if (myWin.myGL->copyTgt == 0)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushTempN.tex1_64);

                else if (myWin.myGL->copyTgt == 1)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushTempN.tex2_64);

                else if (myWin.myGL->copyTgt == 2)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushBGN.tex1_64);

                else if (myWin.myGL->copyTgt == 3)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushBGN.tex2_64);

                else if (myWin.myGL->copyTgt == 4)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushN.tex1_64);

                else if (myWin.myGL->copyTgt == 5)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushN.tex2_64);

                else if (myWin.myGL->copyTgt == 10)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->myLayerIdx.layer[0].tex1_64);

                else if (myWin.myGL->copyTgt == 11)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->eraserN.tex1_64);

                else if (myWin.myGL->copyTgt == 12)
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->eraserN.tex2_64);

                else if (myWin.myGL->copyTgt == 99)
                {
                    glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->eraserN.tex2_64);

                    for (auto &j : myWin.myGLWidgetSh->allMaps)
                    {
                        if (j.name == "debugBG")
                            glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                    }
                }
            }

            else if (proN == "pPaintProj")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGL->brushN.tex1_64);
                glProgramUniformHandleui64ARB(proH, 1, myWin.myGL->myLayerIdx.layer[0].tex2_64);

                glUniformMatrix4fv(glGetUniformLocation(proH, "VM"), 1, GL_FALSE, &myWin.myGL->selCamLi->VM[0][0]);

                auto ProjectorM = biasM * myWin.myGL->selCamLi->PM * myWin.myGL->selCamLi->VM * MM;
                glUniformMatrix4fv(glGetUniformLocation(proH, "ProjectorM"), 1, GL_FALSE, &ProjectorM[0][0]);
            }

            else if (proN == "pAlphaAsRGBA")
            {
                glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(myWin.myGL->sobelMap).tex1_64);
            }

            else if (proN == "pBlendMode")
            {
                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.name == "REPLACEME")
                        glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                }
            }

            //GIZ
            else if (proN == "pGiz")
            {
                comboU = glm::vec4(Cgiz, 1.f);
                glUniform4fv(glGetUniformLocation(proH, "Cgiz"), 1, &comboU.x);
            }

            else if (proN == "pGiz_circ")
            {
                comboU = glm::vec4(Cgiz, 1.f);
                glUniform4fv(glGetUniformLocation(proH, "Cgiz"), 1, &comboU.x);

                if (type == "GIZ_CIRC")
                {
                    comboU = glm::vec4(myWin.myGL->selCamLi->rightO, 0.f);
                    glUniform4fv(glGetUniformLocation(proH, "circRight"), 1, &comboU.x);

                    comboU = glm::vec4(myWin.myGL->selCamLi->upO, 1);
                    glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
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
                        circHalfRight = glm::normalize(glm::cross(myWin.myGL->selCamLi->lookO, axX));
                        circHalfFront = glm::normalize(glm::cross(circHalfRight, axX));
                        myWin.myGL->gizHoverCheckX = circHalfFront;
                    }

                    else if (name->val_s == "gizCircHalfY")
                    {
                        circHalfRight = glm::normalize(glm::cross(myWin.myGL->selCamLi->lookO, axY));
                        circHalfFront = glm::normalize(glm::cross(circHalfRight, axY));
                        myWin.myGL->gizHoverCheckY = circHalfFront;
                    }

                    else if (name->val_s == "gizCircHalfZ")
                    {
                        circHalfRight = glm::normalize(glm::cross(myWin.myGL->selCamLi->lookO, axZ));
                        circHalfFront = glm::normalize(glm::cross(circHalfRight, axZ));
                        myWin.myGL->gizHoverCheckZ = circHalfFront;
                    }

                    comboU = glm::vec4(circHalfRight, 0.f);
                    glUniform4fv(glGetUniformLocation(proH, "circRight"), 1, &comboU.x);

                    comboU = glm::vec4(circHalfFront, 0);
                    glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
                }
            }

            else if (type == "SELRECT")
            {
                glm::vec4 LDRU(myWin.myGL->selRect_LD, myWin.myGL->selRect_RU);
                glUniform4fv(glGetUniformLocation(proH, "LDRU"), 1, &LDRU.x);
            }

            else if (proN == "pStencilHi")
            {
                comboU = glm::vec4(myWin.glslTable->Csel->val_3, 1.f);
                glUniform4fv(glGetUniformLocation(proH, "CstencilHi"), 1, &comboU.x);
            }

            else if (proN == "pTxt")
            {
                for (auto &j : myWin.myGLWidgetSh->allMaps)
                {
                    if (j.name == "atlas")
                        glProgramUniformHandleui64ARB(proH, 0, myWin.myGLWidgetSh->topLayer(j).tex1_64);
                }

                comboU = glm::vec4(myWin.glslTable->Ctxt->val_3, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "Ctxt"), 1, &comboU.x);

                glm::vec2 txtCellSize(1.f / 16, (300.f / 384) / 6);
                glm::vec2 txtCellOffset(.5f / 256);

                auto txtSize = .8f;
                glm::vec2 txtSizeUse(.75f * 16 / (myWin.myGL->width() / txtSize), .75f * 33.33 / (myWin.myGL->height() / txtSize));

                comboU = glm::vec4(txtCellSize, txtCellOffset);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);

                comboU = glm::vec4(txtSizeUse, txtOrigin);
                glUniform4fv(glGetUniformLocation(proH, "comboU1"), 1, &comboU.x);
            }

            else if (proN == "pVolumeLight")
            {
                glUniformMatrix4fv(glGetUniformLocation(proH, "MV"), 1, GL_FALSE, &MV[0][0]);

                comboU = glm::vec4(parentTo->Cl->val_3, 0.f);
                glUniform4fv(glGetUniformLocation(proH, "parentCl"), 1, &comboU.x);

                auto volTipView = glm::vec3(VM * glm::vec4(parentTo->t->val_3, 1.f));

                comboU = glm::vec4(volTipView, parentTo->volDist->val_f);
                glUniform4fv(glGetUniformLocation(proH, "comboU0"), 1, &comboU.x);
            }
        }
    }
}

void Object::setMM()
{
    if (this != myWin.paintStroke.get())
    {
        SM = glm::scale(glm::mat4(), s->val_3);

        if (type == "CAMLI" && camLiType->val_s != "DIR")
            RM = glm::yawPitchRoll(glm::radians(r->val_3.x), glm::radians(r->val_3.y), 0.f);

        else
            RM = rotOrderUse(rotOrder->val_s);

        TM = glm::translate(glm::mat4(), t->val_3);

    //    if (this == myWin.paintStroke.get())
    //    {
    //        PM2D = glm::ortho(-1.f * myGL->aspect, 1.f * myGL->aspect, -1.f, 1.f, -1.f, 1.f);
    //        TM = glm::translate(glm::mat4(), t->val_3 * myGL->aspectXYZ);
    //    }

        if (parentTo != 0)
        {
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
        }

        //PIV
        pivM = glm::translate(glm::mat4(), t->val_3 - piv->val_3);
        pivRtM = glm::translate(glm::mat4(), pivRt);

        //MM
        //    if (piv->val_3 == glm::vec3(0.f)) MM = TM * RM * SM;
        //    else MM = TM * pivRtM * glm::inverse(pivM) * RM * SM * pivM;
        MM = TM * RM * SM;
    }
}

void Object::AABB_toWS()
{
    setMM();

    AABB_WS =
    {
        //near (maxZ)
        { bbMin.x, bbMin.y, bbMax.z },
        { bbMax.x, bbMin.y, bbMax.z },
        { bbMax.x, bbMax.y, bbMax.z },
        { bbMin.x, bbMax.y, bbMax.z },

        //far (minZ)
        { bbMin.x, bbMin.y, bbMin.z },
        { bbMax.x, bbMin.y, bbMin.z },
        { bbMax.x, bbMax.y, bbMin.z },
        { bbMin.x, bbMax.y, bbMin.z },
    };

    //    for (int i = 0; i < 8; ++i)
    //        cout << i <<  " OS = " << glm::to_string(AABB_WS[i]) << endl;

    // transform into WS
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 3; ++j)
            AABB_WS[i][j] = (AABB_WS[i].x * MM[0][j]) + (AABB_WS[i].y * MM[1][j]) + (AABB_WS[i].z * MM[2][j]) + MM[3][j];
    }

    //    for (int i = 0; i < 8; ++i)
    //        cout << i <<  " WS = " << glm::to_string(AABB_WS[i]) << endl;
}

void Object::buildFrustumPlanes()
{
    //NEAR
    nc = t->val_3 + lookO * nearClip->val_f;
    Hnear = 2.f * tan(glm::radians(fov->val_f / 2.f)) * nearClip->val_f;
    Wnear = Hnear * myWin.myGL->aspect;
    ntl = nc + (upO * Hnear / 2.f) - (rightO * Wnear / 2.f);
    ntr = nc + (upO * Hnear / 2.f) + (rightO * Wnear / 2.f);
    nbl = nc - (upO * Hnear / 2.f) - (rightO * Wnear / 2.f);
    nbr = nc - (upO * Hnear / 2.f) + (rightO * Wnear / 2.f);

    //FAR
    fc = t->val_3 + lookO * farClip->val_f;
    Hfar = 2.f * tan(glm::radians(fov->val_f / 2.f)) * farClip->val_f;
    Wfar = Hfar * myWin.myGL->aspect;
    ftl = fc + (upO * Hfar / 2.f) - (rightO * Wfar / 2.f);
    ftr = fc + (upO * Hfar / 2.f) + (rightO * Wfar / 2.f);
    fbl = fc - (upO * Hfar / 2.f) - (rightO * Wfar / 2.f);
    fbr = fc - (upO * Hfar / 2.f) + (rightO * Wfar / 2.f);

    fPlanes_temp =
    {
        { ntr, ntl, ftl }, //U
        { nbl, nbr, fbr }, //D
        { ntl, nbl, fbl }, //L
        { nbr, ntr, fbr }, //R
        { ntl, ntr, nbr }, //N
        { ftr, ftl, fbl }, //F
    };

    for (int i = 0; i < 6; ++i)
    {
        glm::vec3 e1 = fPlanes_temp[i].v2 - fPlanes_temp[i].v1;
        glm::vec3 e2 = fPlanes_temp[i].v3 - fPlanes_temp[i].v1;

        glm::vec3 N = glm::normalize(glm::cross(e1, e2));
        float d = -glm::dot(N, fPlanes_temp[i].v2);

        fPlanes[i] = glm::vec4(N, d);
    }
}

bool Object::isAABBinFrustum()
{
    for (int i = 0; i < 6; ++i)
    {
        bool inside = false;

        for (int j = 0; j < 8; ++j)
        {
            if (glm::dot(AABB_WS[j], glm::vec3(myWin.myGL->selCamLi->fPlanes[i])) + myWin.myGL->selCamLi->fPlanes[i].w > 0.f)
            {
                inside = true;
                break;
            }
        }

        if (inside == false)
            return false;
    }

    return true;
}

void Object::shadowPass()
{
    //READ FROM MULTIPLE LIGHTS
    auto lightIter = 1;

    for (auto &i : myWin.allCamLi)
    {
        if (i->v->val_b && i->camLiTypeGet("light"))
        {
            //SHADOW
            glm::mat4 PM_shadow, VM_shadow;

            if (i->camLiType->val_s == "DIR")
            {
                PM_shadow = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 20.f);
                VM_shadow = glm::inverse(i->RM);

                glm::mat4 depthBiasMVP = biasM * PM_shadow * VM_shadow * MM;
                glNamedBufferSubData(myWin.myGLWidgetSh->UBO_lights, lightIter * sizeof(lightUBO) - sizeof(glm::mat4), sizeof(depthBiasMVP), &depthBiasMVP);
            }

            else if (i->camLiType->val_s == "POINT")
            {
            }

            else if (i->camLiType->val_s == "SPOT")
            {
                PM_shadow = glm::perspective(glm::radians(50.f), 1.f, i->nearShadow->val_f, i->farShadow->val_f);
                VM_shadow = i->VM;

                glm::mat4 depthBiasMVP = biasM * PM_shadow * VM_shadow * MM;
                glNamedBufferSubData(myWin.myGLWidgetSh->UBO_lights, lightIter * sizeof(lightUBO) - sizeof(glm::mat4), sizeof(depthBiasMVP), &depthBiasMVP);
            }

            ++lightIter;
        }
    }
}

void Object::render()
{
    proUse();

    if (bb->val_b)
    {
        glLineWidth(2.f);

        glBindVertexArray(myWin.myBB->VAO);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * 2));
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * 2));

        glLineWidth(1.f);
    }

    else
    {
        if (type == "FBO" || type == "SELRECT")
        {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        else if (type == "GRID")
        {
            glLineWidth(2.f);

            glBindVertexArray(VAO);

            int gridLines = myWin.glslTable->gridLines->val_i + 1;
            for (int i = 0; i < gridLines; ++i)
                glDrawArrays(GL_LINE_STRIP, gridLines * i, gridLines);

            glLineWidth(1.f);
        }

        else if (type == "OBJ")
        {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)idxE.size(), GL_UNSIGNED_SHORT, 0);
        }

        else if (type == "VOL")
        {
            glBindVertexArray(myWin.myVolCone->VAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)myWin.myVolCone->idxE.size(), GL_UNSIGNED_SHORT, 0);
        }

        else if (type == "CAMLI")
        {
            glLineWidth(2.f);
            glDisable(GL_DEPTH_TEST);

            if (camLiTypeGet("cam"))
            {
                glBindVertexArray(VAO);
                glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)pE.size());
            }

            else
            {
                if (camLiType->val_s == "AREA")
                {
                    glBindVertexArray(myWin.myAreaLight->VAO);
                    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.myAreaLight->pE.size());
                }

                else if (camLiType->val_s == "DIR")
                {
                    glBindVertexArray(myWin.myDirLight->VAO);
                    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.myDirLight->pE.size());
                }

                else if (camLiType->val_s == "POINT")
                {
                    glBindVertexArray(myWin.myPointLight->VAO);
                    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.myPointLight->pE.size());
                }

                else if (camLiType->val_s == "SPOT")
                {
                    glBindVertexArray(myWin.mySpotLight->VAO);
                    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)myWin.mySpotLight->pE.size());
                }
            }

            glLineWidth(1.f);
            glEnable(GL_DEPTH_TEST);
        }

        else if (type == "TXT")
        {
            glBindVertexArray(VAO);
            glEnableVertexArrayAttrib(VAO, 0);
            glDrawArrays(GL_POINTS, 0, (GLsizei)strlen(txt2D));
        }

        else if (type == "GIZ_CONE" || type == "GIZ_DUAL_HANDLE")
        {
            if (type == "GIZ_DUAL_HANDLE")
                glDisable(GL_CULL_FACE);

            glBindVertexArray(VAO);
            glEnableVertexArrayAttrib(VAO, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)pE.size());

            if (type == "GIZ_DUAL_HANDLE")
                glEnable(GL_CULL_FACE);
        }

        else if (type == "GIZ_CIRC" || type == "GIZ_CIRC_HALF" || type == "GIZ_LINE")
        {
            glLineWidth(3.f);

            glBindVertexArray(VAO);
            glEnableVertexArrayAttrib(VAO, 0);
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)pE.size());

            glLineWidth(1.f);
        }

        else if (type == "GIZ_CUBE")
        {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)idxE.size(), GL_UNSIGNED_SHORT, 0);
        }
    }
}

glm::vec3 Object::genColorID()
{
    auto preExist = false;

    int myRand = rand();
    int r = (myRand & 0x000000FF) >>  0;
    int g = (myRand & 0x0000FF00) >>  8;
    int b = (myRand & 0x00FF0000) >> 16;

    glm::vec3 myColorID(r / 255.f, g / 255.f, b / 255.f);

    for (auto &i : myWin.allObj)
    {
        if (i->Crand == myColorID)
            preExist = 1;
    }

    if (preExist)
    {
        genColorID();

        return glm::vec3(1.f);
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

void Object::parentObj(string parentName)
{
    auto found = false;

    for (auto &i : myWin.allObj)
    {
        if (i->name->val_s == parentName)
        {
            found = 1;
            parentTo = i;

            break;
        }
    }

    if (!found)
        cout << "parentObj() could not find " << parentName;
}

void Object::rename(string newName) //recursive
{
    if (!ref)
    {
        auto preExist = false;

        for (auto &i : myWin.allObj)
        {
            if (i->name->val_s == newName)
                preExist = 1;
        }

        if (preExist)
        {
            newName = getIncName(newName);
            rename(newName);
        }

        else
        {
            for (auto &i : myWin.allObj)
            {
                if (i->parentTo && i->name->val_s != "pivot")
                {
                    if (i->parentTo->name->val_s == name->val_s)
                        i->parentTo->name->val_s = newName;
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

void Object::tileMaps(GLuint proH, string mode)
{
    if (mode == "get")
    {
        glm::vec4 albedoAlphaTile, metallicRuffTile, normalTile, anisoSssTile;

        for (auto &i : multiObj)
        {
            if (i->type == "vec2" && i->name.back() == 'T')
            {
                //albedoAlpha
                if (i->name == "albedoT")
                {
                    albedoAlphaTile.x = i->val_2.x;
                    albedoAlphaTile.y = i->val_2.y;
                }

                else if (i->name == "alphaT")
                {
                    albedoAlphaTile.z = i->val_2.x;
                    albedoAlphaTile.w = i->val_2.y;
                }

                //anisoSss
                else if (i->name == "anisoT")
                {
                    anisoSssTile.x = i->val_2.x;
                    anisoSssTile.y = i->val_2.y;
                }

                else if (i->name == "sssT")
                {
                    anisoSssTile.z = i->val_2.x;
                    anisoSssTile.w = i->val_2.y;
                }

                //metallicRuff
                else if (i->name == "metallicT")
                {
                    metallicRuffTile.x = i->val_2.x;
                    metallicRuffTile.y = i->val_2.y;
                }

                else if (i->name == "ruffT")
                {
                    metallicRuffTile.z = i->val_2.x;
                    metallicRuffTile.w = i->val_2.y;
                }

                //normal
                else if (i->name == "normalT")
                {
                    normalTile.x = i->val_2.x;
                    normalTile.y = i->val_2.y;
                    normalTile.z = normWt->val_f;
                }
            }
        }

        glUniform4fv(glGetUniformLocation(proH, "albedoAlphaTile"), 1, &albedoAlphaTile.r);
        glUniform4fv(glGetUniformLocation(proH, "metallicRuffTile"), 1, &metallicRuffTile.r);
        glUniform4fv(glGetUniformLocation(proH, "normalTile"), 1, &normalTile.r);
        glUniform4fv(glGetUniformLocation(proH, "anisoSssTile"), 1, &anisoSssTile.r);
    }

    else if (mode == "create")
    {
        vector<shared_ptr<MultiAttr>> multiObjTemp = multiObj;

        for (auto &i : multiObjTemp)
        {
            if (i->type == "enum" && i->name.back() == 'M')
            {
                string tileName = i->name;
                tileName.resize(tileName.size() - 1);
                tileName.append("T");

                auto newTileAttr = make_shared<MultiAttr>();
                newTileAttr->name = tileName;
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

void Object::deleteVAO_VBO()
{
    glGetError();

    for (auto it = myWin.myGLWidgetSh->GLDataSh.begin(); it != myWin.myGLWidgetSh->GLDataSh.end(); )
    {
        if ((*it).obj.get() == this)
        {
            glDeleteBuffers(1, &(*it).VBO_P);
            glDeleteBuffers(1, &(*it).VBO_UV);
            glDeleteBuffers(1, &(*it).VBO_T);
            glDeleteBuffers(1, &(*it).VBO_N);
            glDeleteBuffers(1, &(*it).VBO_IDX);
            glErrorPrint("Couldnt delete VBOs");

            it = myWin.myGLWidgetSh->GLDataSh.erase(it);
        }

        else
            ++it;
    }

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    VAO_loaded = false;

    glErrorPrint("Couldnt delete VAOs");
}

void Object::setDirty()
{
    dirtyVM = true;

    if (camLiTypeGet("light"))
        dirtyShadow = true;
}
