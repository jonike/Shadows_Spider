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

#ifndef OBJECT_H
#define OBJECT_H

#include "MainWin.h"

class Object
{
public:
    MainWin &myWin;
    Object(MainWin &);
    Object(const Object &);
    shared_ptr<Object> Clone() const { return(shared_ptr<Object>(CloneImpl())); }
    virtual Object* CloneImpl() const { return(new Object(*this)); }

    vector<DynVAO> dynVAO_perGL;
    vector<glm::vec3> pE, nE, tE, bE;
    vector<glm::vec2> uvE;
    vector<GLushort> idxE;
    shared_ptr<Object> parentTo;

    vector<shared_ptr<MultiAttr>> multiObj;

    shared_ptr<MultiAttr> name, transformSep, t, r, s, v, bb;
    shared_ptr<MultiAttr> shaderSep, albedoM, alphaM, cubeM, lensM, metallicM, normalM, ruffM, sssM;
    shared_ptr<MultiAttr> Kr, Ksss, Ko, anisoTgl, ior, ruffA, ruffD, sssSpread;
    shared_ptr<MultiAttr> shadowCast, backface, Cwire, normVLen, normWeight, piv, rotOrder;
    shared_ptr<MultiAttr> tileSep, albTile, normTile, ruffTile, sssTile;

    shared_ptr<MultiAttr> camLiType;
    shared_ptr<MultiAttr> camSep, fov, nearClip, farClip, nearShadow, farShadow, orthoFree, orthoType, orthoZoom;

    shared_ptr<MultiAttr> lightSep, Cl, lInten, lSpotI, lSpotO;
    shared_ptr<MultiAttr> volSep, volCone, volDist, volS;

    shared_ptr<MultiAttr> bloomSep, bloomI, bloomLensI;
    shared_ptr<MultiAttr> hdrSep, adaptTime, adaptAuto, expo, vignette, vignetteD;
        shared_ptr<MultiAttr> fxaaSep, fxaaBlur, fxaaSubPix, fxaaEdgeThr, fxaaEdgeThrMin;
    shared_ptr<MultiAttr> ssaoSep, ssaoBias, ssaoBlur, ssaoInten, ssaoRad, ssaoRand;

    //CAM
    float distO;
    glm::vec3 lookO, rightO, upO, targO, lookFPS, rightFPS, upFPS;

    //UPDATE
    bool dirtyShadow, dirtyVM;
    QString savedShader;

    //ETC
    bool expand, ignoreOutliner, ref, selected;
    bool deletable, gridV, selectable;

    int vertsOnObj;
    QString dupeStenFix = "";
    QString manipTypeO, type;

    //GLSL
    glm::mat4 biasM;
    glm::vec4 Crand;
    bool showN;
    int gaussStage, nType;

    //GIZ
    glm::vec3 Cgiz, Cgiz_stored;
    string gizType;
    bool gizSideObj = 0;

    //TXT
    const char *txt2D;
    glm::vec2 txtOrigin;

    //TRANSFORM
    glm::mat3 NM;
    glm::mat4 MVP, MV, MM, PM, VM, TM, RM, SM;
    glm::mat4 bbScaleM, bbrotM, bbtransM, obbMVP;
    glm::mat4 pivM, pivRtM, rpM, rM;
    glm::vec3 bbMin, bbMax, bbCenter, bbSize, bbSizeFull;
    glm::vec3 pivRt;


    void loadVAO(shared_ptr<GLWidget>);
    void render(shared_ptr<GLWidget>);
    void shadowPass(GLuint);

    void BBup();
    void delete_();
    void glErrorPrint(const char *);
    void rename(QString);
    void parentObj(QString);
    void setTarg(glm::vec3, float);
    bool camLiTypeGet(QString);
    void setDirty();
    glm::mat4 rotOrderUse(QString);
    void tileMaps(GLuint, QString);
    void mvpGet(shared_ptr<GLWidget>);
    void populateTexCombos();

protected:
    void proUse(shared_ptr<GLWidget>);

    glm::vec4 genColorID();
    string getIncName(string);

};

#endif // OBJECT_H
