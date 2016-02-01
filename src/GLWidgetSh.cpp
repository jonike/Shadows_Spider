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

#include "GLWidgetSh.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

//////http://stackoverflow.com/questions/18176274/cubemap-shadow-mapping-not-working
CubeShadowTable myCubeShadowTable[6] =
{
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f) }
};

GLWidgetSh::GLWidgetSh(MainWin &myWinTemp, QWidget *parent) : QGLWidget(parent), myWin(myWinTemp)
{
    QGLFormat format;
    format.setVersion(4, 5);
    format.setProfile(QGLFormat::CompatibilityProfile);
    format.setSwapInterval(0);
    setFormat(format);

    resize(200, 200);

    typeList = { "ALBEDO", "ALPHA", "ANISO", "LENS", "METALLIC", "RUFF", "SSS" };

//    cubeFBON = cubeDynNode_create("cube", 1024, 1024);
}

void GLWidgetSh::UBO_init()
{
    auto MAX_LIGHTS = 50;
    glCreateBuffers(1, &UBO_lights);
    glNamedBufferData(UBO_lights, MAX_LIGHTS * sizeof(lightUBO), 0, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO_lights);
}

void GLWidgetSh::UBO_update()
{
    auto lightIter = 0;

    for (auto &i : myWin.allObj)
    {
        if (i->v->val_b && i->camLiTypeGet("light"))
        {
            lightUBO newUBO;

            float type;
            if (i->camLiType->val_s == "AREA") type = 0.f;
            else if (i->camLiType->val_s == "DIR") type = 1.f;
            else if (i->camLiType->val_s == "POINT") type = 2.f;
            else if (i->camLiType->val_s == "SPOT") type = 3.f;

            newUBO.Cl_type = glm::vec4(i->Cl->val_3, type);
            newUBO.falloff = glm::vec4(i->lInten->val_f, cos(glm::radians(i->lSpotI->val_f)), cos(glm::radians(i->lSpotO->val_f)), 0.f);
            newUBO.lDirRot = i->MM * glm::vec4(0.f, 0.f, -1.f, 0.f);
            newUBO.lP = glm::vec4(i->t->val_3, 0.f);

            glNamedBufferSubData(UBO_lights, lightIter * sizeof(newUBO), sizeof(newUBO), &newUBO);

            ++lightIter;
        }
    }

    UBO_light_needsUp = false;
}

void GLWidgetSh::initializeGL()
{
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
        cout << "error with glewInit : " << glewGetErrorString(glewInit()) << endl;
}

void GLWidgetSh::brushInit()
{
    auto newBrush = make_shared<Brush>();
    newBrush->name = "round";
    newBrush->scale = glm::vec3(.5f);
//    newBrush->opac = .1f;
    newBrush->opac = .025f;

    allBrushes.push_back(newBrush);

    auto newEraser = make_shared<Brush>();
    newEraser->name = "square";
    newEraser->scale = glm::vec3(.5f);
    newEraser->opac = 1.f;

    allBrushes.push_back(newEraser);

    for (auto &i : allBrushes)
    {
        if (i->name == "round")
            selBrush = i;

        if (i->name == "square")
            selEraser = i;
    }
}

void GLWidgetSh::mapInit()
{
    brushInit();

    //DEBUG
//    allMaps.push_back( { "debugBG", "DEBUG", "debug/nvidia-3d-vision.jpg" } );
//    allMaps.push_back( { "debugBG", "DEBUG", "debug/oldPaper.jpg" } );
    allMaps.push_back( { "debugBG", "DEBUG", "debug/oldPaper.png" } );
    allMaps.push_back( { "freiChen", "DEBUG", "debug/freiChen.tga" } );

    //ALBEDO
    allMaps.push_back( { "BLANK", "ALBEDO", "single/BLANK_white_256.tga" } );
    allMaps.push_back( { "checker", "ALBEDO", "albedo/checker.png" } );
    allMaps.push_back( { "uv", "ALBEDO", "albedo/uv.jpg" } );
    allMaps.push_back( { "gold", "ALBEDO", "single/gold_256.tga" } );
    allMaps.push_back( { "abj", "ALBEDO", "albedo/ABJ.png" } );

    //ALPHA
    allMaps.push_back( { "BLANK", "ALPHA", "single/BLANK_white_1024.png" } );
//    allMaps.push_back( { "BLANK", "ALPHA", "single/BLANK_white_1024.tga" } );
//    allMaps.push_back( { "BLANK", "ALPHA", "albedo/checker.png" } );

    //ANISO
    allMaps.push_back( { "BLANK", "ANISO", "single/BLANK_white_256.tga" } );
    allMaps.push_back( { "VIEW", "ANISO", "single/BLANK_white_256.tga" } );
    allMaps.push_back( { "rot1", "ANISO", "aniso/rot1.jpg" } );
    allMaps.push_back( { "rot2", "ANISO", "aniso/rot2.jpg" } );
    allMaps.push_back( { "rot3", "ANISO", "aniso/rot3.jpg" } );

    //BRUSH
//    allMaps.push_back( { "round", "BRUSH", "brush/superSoft.tga" } );
    allMaps.push_back( { "round", "BRUSH", "brush/round_soft.tga" } );
//    allMaps.push_back( { "round", "BRUSH", "brush/round_hard.tga" } );
//    allMaps.push_back( { "round", "BRUSH", "brush/wavy.tga" } );
    allMaps.push_back( { "square", "BRUSH", "brush/square.tga" } );

    //CUBE_IRRAD
    allMaps.push_back( { "ennis", "CUBE_IRRAD", "cube/ennis_cube_irradiance.dds" } );
    allMaps.push_back( { "glacier", "CUBE_IRRAD", "cube/glacier_cube_irradiance.dds" } );
    allMaps.push_back( { "grace", "CUBE_IRRAD", "cube/grace_cube_irradiance.dds" } );
    allMaps.push_back( { "pisa", "CUBE_IRRAD", "cube/pisa_cube_irradiance.dds" } );
    allMaps.push_back( { "uffizi", "CUBE_IRRAD", "cube/uffizi_cube_irradiance.dds" } );

    //CUBE_SPEC
    allMaps.push_back( { "ennis", "CUBE_SPEC", "cube/ennis_cube_specular.dds" } );
    allMaps.push_back( { "glacier", "CUBE_SPEC", "cube/glacier_cube_specular.dds" } );
    allMaps.push_back( { "grace", "CUBE_SPEC", "cube/grace_cube_specular.dds" } );
    allMaps.push_back( { "pisa", "CUBE_SPEC", "cube/pisa_cube_specular.dds" } );
    allMaps.push_back( { "uffizi", "CUBE_SPEC", "cube/uffizi_cube_specular.dds" } );

    //LENS
    allMaps.push_back( { "BLANK", "LENS", "lens/BLANK_black_256.png" } );
    allMaps.push_back( { "abjLens1", "LENS", "lens/abjLens1.png" } );

    //METALLIC
    allMaps.push_back( { "BLANK", "METALLIC", "single/BLANK_black_256.tga" } );
    allMaps.push_back( { "WHITE", "METALLIC", "single/BLANK_white_256.tga" } );

    //NORMAL
    allMaps.push_back( { "BLANK", "NORMAL", "normal/BLANK.png" } );
    allMaps.push_back( { "squares", "NORMAL", "normal/squares.jpg" } );
    allMaps.push_back( { "voronoi", "NORMAL", "normal/voronoi.jpg" } );
    allMaps.push_back( { "brushed_H", "NORMAL", "normal/brushed_H.png" } );
    allMaps.push_back( { "brushed_V", "NORMAL", "normal/brushed_V.png" } );
    allMaps.push_back( { "rand", "NORMAL", "normal/rand.png" } ); // for ssao only

    //RUFF
    allMaps.push_back( { "BLANK", "RUFF", "single/BLANK_black_256.tga" } );
//    allMaps.push_back( { "BLANK", "RUFF", "single/BLANK_gray_256.tga" } );
    allMaps.push_back( { "gray", "RUFF", "single/BLANK_gray_256.tga" } );
//    allMaps.push_back( { "BLANK", "RUFF", "single/BLANK_white_256.tga" } );
    allMaps.push_back( { "white", "RUFF", "single/BLANK_white_256.tga" } );
    allMaps.push_back( { "squiggle0", "RUFF", "ruff/squiggle0.tga" } );
    allMaps.push_back( { "squiggle1", "RUFF", "ruff/squiggle1.tga" } );

    //SSS
    allMaps.push_back( { "BLANK", "SSS", "single/BLANK_black_256.tga" } );
    allMaps.push_back( { "WHITE", "SSS", "single/BLANK_white_256.tga" } );
    allMaps.push_back( { "sssLookup", "SSS_ref", "sss/lookUp.png" } );

    //TXT
    allMaps.push_back( { "atlas", "TXT", "txt/verasansmono.png" } );

    unsigned int IDct = 0;

    //push LAYERS back into maps
    for (auto &i : allMaps)
    {
        i.layer.push_back( { texN_create("layer0", i.type, i.flat, 0) } );

//        //drag u/d and change idx debug
        if (i.name == "BLANK" && i.type == "ALBEDO")
        {
            i.layer.push_back( { texN_create("layer1", i.type, i.flat, 1) } );
            i.layer.push_back( { texN_create("layer2", i.type, i.flat, 2) } );
        }

        i.ID = IDct;
        ++IDct;
    }

    //upload 64 bit bindless handles for all LAYERS
    for (auto &i : allMaps)
    {
        for (auto &j : i.layer)
        {
            up64N(j, 1);
            j.ID = IDct;
            ++IDct;
        }
    }

    //GEN BRUSH OUTLINES FOR ALL BRUSHES AND PUSH BACK OUTLINE INTO MAP LAYER[1]
    brushOutlineUpB = true;

    //for brushes render the sobel outline into FBO2 / tex2
    //use 128x128 size

//    ssaoUp_dynamic();
}

void GLWidgetSh::ssaoUp_dynamic()
{
    //CREATE SSAO KERNEL / TEX
    // SAMPLE KERNEL
    uniform_real_distribution<float> randomFloats(0.f, 1.f); // generates random floats 0.f..1.f
    default_random_engine generator;

    for (unsigned int i = 0; i < 32; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.f - 1.f, randomFloats(generator) * 2.f - 1.f, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        // Scale samples s.t. they're more aligned to center of kernel
        float scale = float(i) / 32.f;
        scale = lerp(.1f, 1.f, scale * scale);
        sample *= scale;
        ssaoKernel32.push_back(sample);
    }

    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.f - 1.f, randomFloats(generator) * 2.f - 1.f, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        // Scale samples s.t. they're more aligned to center of kernel
        float scale = float(i) / 64.f;
        scale = lerp(.1f, 1.f, scale * scale);
        sample *= scale;
        ssaoKernel64.push_back(sample);
    }

//    for (auto &i : ssaoKernel32)
//        cout << "ssaoKernel32 = " << glm::to_string(i) << endl;

    //NOISE TEX
    for (unsigned int i = 0; i < 16; ++i)
    {
        glm::vec3 noise(randomFloats(generator) * 2.f - 1.f, randomFloats(generator) * 2.f - 1.f, 0.f); // rotate around z-axis (in tangent space)
        noise = glm::normalize(noise);
        ssaoNoise.push_back(noise);
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &ssaoNoiseTex_32);
    glTextureStorage2D(ssaoNoiseTex_32, 1, GL_RGB16F, 4, 4);
    glTextureParameteri(ssaoNoiseTex_32, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(ssaoNoiseTex_32, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(ssaoNoiseTex_32, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(ssaoNoiseTex_32, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    up64T(ssaoNoiseTex_32, ssaoNoiseTex_64, 1);
}

float GLWidgetSh::lerp(float a, float b, float c)
{
    return a + (b - a) * c;
}


AbjNode GLWidgetSh::topLayer(Map mapIn)
{
    for (auto &i : mapIn.layer)
    {
        if (i.idx == 0)
            return i;
    }

    AbjNode myLayer;

    return myLayer;
}

void GLWidgetSh::cubemapGen()
{
    //get selB and its (piv) pos
    //hide selB if viz
    //render
    //unhide

    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBON.fbo1);
    glViewport(0, 0, cubeFBON.width, cubeFBON.height);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    myWin.creatingDynCubeRGB = true;

    auto storedV = myWin.selB->v->val_b;
    myWin.selB->v->val_b = false;

    for (int i = 0; i < 6; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeFBON.tex1_32, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto targ = myCubeShadowTable[i].targ;
        auto up = myCubeShadowTable[i].up;
        dynVM = glm::lookAt(glm::vec3(0.f), targ, up);
        negCubeCenter = glm::translate(glm::mat4(), -myWin.selB->t->val_3);

        for (auto &j : myWin.allObj)
        {
            if (j->type == "OBJ" && myWin.searchUp(j))
            {
                myWin.myGLWidgetSh->glUseProgram2("pGBuffer");
                j->render(myWin.allGL[4]); // REPLACE WITH ACTIVEGL
            }
        }
    }

    myWin.selB->v->val_b = storedV;

    myWin.creatingDynCubeRGB = false;
}

AbjNode GLWidgetSh::cubeDynNode_create(string name, int widthIn, int heightIn)
{
    //cubeFBON = cubeDynNode_create("cube", 1024, 1024);
    //glMakeTextureHandleNonResidentARB(myGL->cubeFBON.tex1_64);
    //glDeleteTextures(1, &cubeFBON.tex1_32);
    //glDeleteFramebuffers(1, &cubeFBON.fbo1);

    GLuint texNew;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texNew);

    glTextureParameteri(texNew, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texNew, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texNew, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texNew, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texNew, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (int i = 0; i < 6; ++i)
        glTextureImage2DEXT(texNew, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, widthIn, heightIn, 0, GL_RGBA, GL_FLOAT, 0);

    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    //DEPTH STEN
    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
    };

    GLuint DStex;
    glCreateTextures(GL_TEXTURE_2D, 1, &DStex);

    glTextureStorage2D(DStex, 1, GL_DEPTH32F_STENCIL8, widthIn, heightIn);
    glTextureParameteri(DStex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(DStex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(DStex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(DStex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, DStex, 0);
//    glNamedFramebufferTexture(fboNew, GL_COLOR_ATTACHMENT0, texNew, 0); attach cube faces
    glNamedFramebufferDrawBuffers(fboNew, 1, DrawBuffers);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with FBO2_shadowCube_create" << endl;

    return { name, widthIn, heightIn, fboNew, texNew };
}

/* SHADOWS */
void GLWidgetSh::addDeleteShadows(string type)
{
    if (type == "add")
    {
        for (auto &i : myWin.allObj)
        {
            if (i->v->val_b && i->camLiTypeGet("light"))
            {
                int shadowSize = 1024;
                AbjNode shadow_no = shadowNode_create(i->name->val_s, shadowSize, shadowSize);

                glMakeTextureHandleNonResidentARB(shadow_no.tex1_64);
                shadow_no.tex1_64 = glGetTextureHandleARB(shadow_no.tex1_32);
                glMakeTextureHandleResidentARB(shadow_no.tex1_64);

                glMakeTextureHandleNonResidentARB(shadow_no.tex2_64);
                shadow_no.tex2_64 = glGetTextureHandleARB(shadow_no.tex2_32);
                glMakeTextureHandleResidentARB(shadow_no.tex2_64);

                allShadow.push_back(shadow_no);
            }
        }
    }

    else if (type == "delete")
    {
        //SHADOW FBO
        for (auto &i : allShadow)
        {
            glMakeTextureHandleNonResidentARB(i.tex1_64);
            glDeleteTextures(1, &i.tex1_32);
            glDeleteFramebuffers(1, &i.fbo1);

            glMakeTextureHandleNonResidentARB(i.tex2_64);
            glDeleteTextures(1, &i.tex2_32);
            glDeleteFramebuffers(1, &i.fbo2);
        }

        allShadow.clear();
    }

    else if (type == "refresh")
    {
        addDeleteShadows("delete");
        addDeleteShadows("add");
    }
}

void GLWidgetSh::writeShadow(shared_ptr<Object> obj)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    //for each light in allShadow, render each object into the light's FBO1
    for (auto &i : allShadow)
    {
        if (i.name == obj->name->val_s) // if it's a light
        {
            shadowObj = obj; //found the light
            shared_ptr<GLWidget> activeGL;

            for (auto &j : myWin.allGL)
            {
                if (j->isVisible())
                {
                    activeGL = j;
                    break;
                }
            }

            // dir / spot
            glBindFramebuffer(GL_FRAMEBUFFER, i.fbo1);
            glViewport(0, 0, i.width, i.height);
            glClear(GL_DEPTH_BUFFER_BIT);
            glClearColor(0.f, 0.f, 0.f, 0.f);

            for (auto &j : myWin.allObj)
            {
                if (j->type == "OBJ" && j->shadowCast->val_b && myWin.searchUp(j))
                    j->render(activeGL); //
            }

            obj->dirtyShadow = false;
        }
    }
}

AbjNode GLWidgetSh::shadowNode_create(string name, int widthIn, int heightIn)
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLfloat border[] = { 1.f, 1.f, 1.f, 1.f };

    GLuint texNew;
    glCreateTextures(GL_TEXTURE_2D, 1, &texNew);
    glTextureStorage2D(texNew, 1, GL_DEPTH_COMPONENT32F, widthIn, heightIn);
    glTextureParameteri(texNew, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(texNew, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(texNew, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texNew, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(texNew, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTextureParameteri(texNew, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTextureParameterfv(texNew, GL_TEXTURE_BORDER_COLOR, border);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_ATTACHMENT, texNew, 0);
    glNamedFramebufferDrawBuffer(fboNew, GL_NONE);
    glNamedFramebufferReadBuffer(fboNew, GL_NONE);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with shadowNode_create FBO1" << endl;

    GLuint fboNew2;
    glGenFramebuffers(1, &fboNew2);
    glBindFramebuffer(GL_FRAMEBUFFER, fboNew2);

    GLuint texNew2;
    glGenTextures(1, &texNew2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texNew2);
    glTextureParameteri(texNew2, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texNew2, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texNew2, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texNew2, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texNew2, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(texNew2, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTextureParameteri(texNew2, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    for (int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, widthIn, heightIn, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glNamedFramebufferTexture(fboNew2, GL_DEPTH_ATTACHMENT, texNew2, 0);

    if (glCheckNamedFramebufferStatus(fboNew2, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with shadowNode_create FBO2" << endl;

    return { name, widthIn, heightIn, fboNew, texNew, 0, fboNew2, texNew2 };
}

AbjNode GLWidgetSh::texN_create(string name, string type, string pathIn, int idx)
{
    //this is basically myWin.myGLWidgetSH->singleN_create() /w a texture uploaded into slot 1

    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
    };

    //upload tex into slot 0...consider additional slots (2 total) for brush's outline silhouette
    auto pathTex = myWin.pathTable->pathTex->val_s;
    auto pathTemp = pathTex + pathIn;
    const auto *path = pathTemp.c_str();

    GLuint fboNew2, tex, tex2;
    int imgW, imgH, chan;
    imgW = 999;
    imgH = 999;

    if (type == "CUBE_IRRAD" || type == "CUBE_SPEC")
    {
        auto *myFile = fopen(path, "rb");

        if (!myFile)
        {
            fprintf(stderr, "Error opening file '%s'\n", path);

            return { name, imgW, imgH, 0, 0, 0, 0, 0, 0, 0, 0, idx };
        }

        char filecode[4];
        fread(filecode, 1, 4, myFile);

        ddsHeader myHeader;
        fread(&myHeader, sizeof(ddsHeader), 1, myFile);

        vector<uint8_t*> _data;

        for (int i = 0; i < 6; ++i)
        {
            int w = myHeader.dwWidth;
            int h = myHeader.dwHeight;

            for (unsigned int j = 0; j < myHeader.dwMipMapCount; ++j)
            {
                int size = w * h * 8;
                uint8_t *pixels = new uint8_t[size];

                fread(pixels, size, 1, myFile);
                _data.push_back(pixels);

                w = (w > 1) ? w >> 1 : 1;
                h = (h > 1) ? h >> 1 : 1;
            }
        }

        fclose(myFile);

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &tex);

        for (int i = 0; i < 6; ++i)
        {
            auto w = myHeader.dwWidth;
            auto h = myHeader.dwHeight;

            for (unsigned int j = 0; j < myHeader.dwMipMapCount; ++j)
            {
                void* texData = _data[i * myHeader.dwMipMapCount + j];

                glTextureImage2DEXT(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGBA16F, w, h, 0, GL_RGBA, GL_HALF_FLOAT, texData);

                w >>= 1;
                h >>= 1;
                w = w ? w : 1;
                h = h ? h : 1;
            }
        }

        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }

    else
    {
        auto *img = stbi_load(path, &imgW, &imgH, &chan, 0);

        for (int h = 0; h < imgH / 2; ++h) //vFlip
        {
            auto top = h * imgW * chan;
            auto btm = (imgH - 1 - h) * imgW * chan;

            for (int w = 0; w < imgW * chan; ++w)
            {
                GLubyte temp = img[top];
                img[top] = img[btm];
                img[btm] = temp;
                ++top;
                ++btm;
            }
        }

        GLint formatI, formatP;
        if (chan == 3) { formatI = GL_RGB8; formatP = GL_RGB; }
        else if (chan == 4) { formatI = GL_RGBA16; formatP = GL_RGBA; }

        int numMip;
        if (pathIn == "txt/verasansmono.png") numMip = 1;
        else numMip = 1 + floor(log2(glm::max(imgW, imgH)));

        glCreateTextures(GL_TEXTURE_2D, 1, &tex);

        glTextureStorage2D(tex, numMip, formatI, imgW, imgH);
        glTextureSubImage2D(tex, 0, 0, 0, imgW, imgH, formatP, GL_UNSIGNED_BYTE, img);

        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        //ANISOTROPIC FILTERING
        GLfloat maxAniso = 0.f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        glGenerateTextureMipmap(tex);

        stbi_image_free(img);
    }

    glNamedFramebufferTexture(fboNew, DrawBuffers[0], tex, 0);
    glNamedFramebufferDrawBuffers(fboNew, 1, DrawBuffers);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with texN_create" << endl;

    if (type == "BRUSH")
    {
        glCreateFramebuffers(1, &fboNew2);

        GLenum DrawBuffers[] =
        {
            GL_COLOR_ATTACHMENT0,
        };

        glCreateTextures(GL_TEXTURE_2D, 1, &tex2);

        glTextureStorage2D(tex2, 1, GL_RGBA16F, imgW, imgH);
        glTextureParameteri(tex2, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(tex2, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(tex2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(tex2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glNamedFramebufferTexture(fboNew2, DrawBuffers[0], tex2, 0);
        glNamedFramebufferDrawBuffers(fboNew2, 1, DrawBuffers);

        if (glCheckNamedFramebufferStatus(fboNew2, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "error with BRUSH fbo2 create" << endl;
    }

    else if (type != "CUBE_IRRAD" && type != "CUBE_SPEC")
    {
        //create the "copyTex" FBO
        glCreateFramebuffers(1, &fboNew2);

        GLenum DrawBuffers[] =
        {
            GL_COLOR_ATTACHMENT0,
        };

        glCreateTextures(GL_TEXTURE_2D, 1, &tex2);

        glTextureStorage2D(tex2, 1, GL_RGBA16F, imgW, imgH);
        glTextureParameteri(tex2, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(tex2, GL_TEXTURE_WRAP_T, GL_REPEAT);

//        glTextureParameteri(tex2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTextureParameteri(tex2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(tex2, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(tex2, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glNamedFramebufferTexture(fboNew2, DrawBuffers[0], tex2, 0);
        glNamedFramebufferDrawBuffers(fboNew2, 1, DrawBuffers);

        if (glCheckNamedFramebufferStatus(fboNew2, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "error with TEX AKA COPYTEX fbo2 create" << endl;
    }

    return { name, imgW, imgH, fboNew, tex, 0, fboNew2, tex2, 0, 0, 0, idx };
}

void GLWidgetSh::up64N(AbjNode &node, bool up)
{
    glMakeTextureHandleNonResidentARB(node.DS_64);
    glMakeTextureHandleNonResidentARB(node.tex1_64);
    glMakeTextureHandleNonResidentARB(node.tex2_64);

    if (up)
    {
        node.tex1_64 = glGetTextureHandleARB(node.tex1_32);
        glMakeTextureHandleResidentARB(node.tex1_64);

        node.tex2_64 = glGetTextureHandleARB(node.tex2_32);
        glMakeTextureHandleResidentARB(node.tex2_64);

        node.DS_64 = glGetTextureHandleARB(node.DS_32);
        glMakeTextureHandleResidentARB(node.DS_64);
    }

    else
    {
        glDeleteTextures(1, &node.DS_32);

        glDeleteTextures(1, &node.tex1_32);
        glDeleteFramebuffers(1, &node.fbo1);

        glDeleteTextures(1, &node.tex2_32);
        glDeleteFramebuffers(1, &node.fbo2);
    }
}

void GLWidgetSh::up64T(GLuint &tex32, GLuint64 &tex64, bool up)
{
    glMakeTextureHandleNonResidentARB(tex64);

    if (up)
    {
        tex64 = glGetTextureHandleARB(tex32);
        glMakeTextureHandleResidentARB(tex64);
    }

    else
        glDeleteTextures(1, &tex32);
}

void GLWidgetSh::proInit()
{
    allPro.push_back( { "pAlphaAsRGBA", createProg("fboV.glsl", "", "alphaAsRGBAF.glsl") } );
    allPro.push_back( { "pGBuffer", createProg("gBufferV.glsl", "gBufferG.glsl", "gBufferF.glsl") } );
    allPro.push_back( { "pBB", createProg("bbV.glsl", "", "bbF.glsl") } );
    allPro.push_back( { "pBlendMode", createProg("wsQuadV.glsl", "", "blendModeF.glsl") } );
    allPro.push_back( { "pBloomC", createProg("fboV.glsl", "", "bloomCF.glsl") } );
    allPro.push_back( { "pBloom", createProg("fboV.glsl", "", "bloomF.glsl") } );
    allPro.push_back( { "pCopyTex", createProg("fboV.glsl", "", "copyTexF.glsl") } );
    allPro.push_back( { "pDef", createProg("fboV.glsl", "", "defF.glsl") } );
    allPro.push_back( { "pDepthRev", createProg("depthRevV.glsl", "", "depthRevF.glsl") } );
    allPro.push_back( { "pDown", createProg("downV.glsl", "", "downF.glsl") } );
    allPro.push_back( { "pFinal", createProg("fboV.glsl", "", "finalF.glsl") } );
    allPro.push_back( { "pEdgeDetect", createProg("fboV.glsl", "", "edgeDetectF.glsl") } );
    allPro.push_back( { "pFxaa", createProg("fboV.glsl", "", "fxaaF.glsl") } );
    allPro.push_back( { "pGauss", createProg("fboV.glsl", "", "gaussF.glsl") } );
    allPro.push_back( { "pGiz", createProg("gizV.glsl", "", "gizF.glsl") } );
    allPro.push_back( { "pGiz_circ", createProg("giz_circV.glsl", "", "gizF.glsl") } );
    allPro.push_back( { "pGrid", createProg("gizV.glsl", "", "gridF.glsl") } );
    allPro.push_back( { "pLumaInit", createProg("fboV.glsl", "", "lumaInitF.glsl") } );
    allPro.push_back( { "pLumaAdapt", createProg("fboV.glsl", "", "lumaAdaptF.glsl") } );
    allPro.push_back( { "pLumaAdapt_viz", createProg("fboV.glsl", "", "lumaAdapt_vizF.glsl") } );
    allPro.push_back( { "pEraseMix", createProg("fboV.glsl", "", "eraseMixF.glsl") } );
    allPro.push_back( { "pPaintProj", createProg("paintProjV.glsl", "", "paintProjF.glsl") } );
    allPro.push_back( { "pPaintStroke", createProg("wsQuadV.glsl", "", "paintStrokeF.glsl") } );
    allPro.push_back( { "pSelRect", createProg("selRectV.glsl", "", "selRectF.glsl") } );
    allPro.push_back( { "pShadow", createProg("shadowV.glsl", "", "shadowF.glsl") } );
    allPro.push_back( { "pSky", createProg("skyV.glsl", "", "skyF.glsl") } );
    allPro.push_back( { "pSSAO_32", createProg("fboV.glsl", "", "ssao_32F.glsl") } );
    allPro.push_back( { "pSSAO_64", createProg("fboV.glsl", "", "ssao_64F.glsl") } );
    allPro.push_back( { "pSSR", createProg("fboV.glsl", "", "ssrF.glsl") } );
    allPro.push_back( { "pStencilHi", createProg("stencilHiV.glsl", "", "stencilHiF.glsl") } );
    allPro.push_back( { "pStencilGeo", createProg("stencilGeoV.glsl", "", "stencilGeoF.glsl") } );
    allPro.push_back( { "pTonemap", createProg("fboV.glsl", "", "tonemapF.glsl") } );
    allPro.push_back( { "pTxt", createProg("txtV.glsl", "txtG.glsl", "txtF.glsl") } );
    allPro.push_back( { "pVolumeLight", createProg("volumeLightV.glsl", "", "volumeLightF.glsl") } );
    allPro.push_back( { "pWireframe", createProg("wireframeV.glsl", "", "wireframeF.glsl") } );
}

void GLWidgetSh::glUseProgram2(string name)
{
    for (auto &i : allPro)
    {
        if (i.name == name)
            pro = i.pro;
    }

    glUseProgram(pro);
    proN = name;
}

void GLWidgetSh::VAOup(shared_ptr<Object> obj)
{
    if (!obj->dynVAO_perGL.empty())
        obj->dynVAO_perGL.clear();

    for (auto &i : myWin.allGL)
    {
        GLuint VAO = 0;
        obj->dynVAO_perGL.push_back( { i, 0, VAO } );
    }

    obj->BBup();
}

vector<shared_ptr<Object>> GLWidgetSh::VBOup(string path, string type, string name, shared_ptr<Object> dupe)
{
    if (!myWin.newObj.empty())
        myWin.newObj.clear();

    if (!newO.empty())
        newO.clear();

    if (path == "999")
    {
        auto obj = (dupe == 0) ? make_shared<Object>(myWin) : dupe;

        if (!dontRename)
            obj->rename(name);

        if (type == "AREA" || type == "DIR" || type == "FPS" || type == "ORTHO" || type == "PERSP" || type == "POINT" || type == "SPOT")
        {
            obj->type = "CAMLI";
            obj->camLiType->val_s = type;
            obj->rotOrder->val_s = "YZX";
        }

        else
            obj->type = type;

        if (obj->type == "BB")
        {
            glm::vec3 bbV[] =
            {
                { -.5f, -.5f, -.5f }, {  .5f, -.5f, -.5f },
                {  .5f,  .5f, -.5f }, { -.5f,  .5f, -.5f },
                { -.5f, -.5f,  .5f }, {  .5f, -.5f,  .5f },
                {  .5f,  .5f,  .5f }, { -.5f,  .5f,  .5f },
            };

            for (auto &i : bbV)
                obj->pE.push_back(i);

            GLushort bbI[] =
            {
                0, 1, 2, 3,
                4, 5, 6, 7,
                0, 4, 1, 5,
                2, 6, 3, 7
            };

            for (auto &i : bbI)
                obj->idxE.push_back(i);
        }

        else if (obj->type == "CAMLI")
        {
            if (obj->camLiTypeGet("cam"))
            {
                auto sX = 1.f;
                auto sY = .5f;
                auto sZ = 1.f;

                //box
                obj->pE.push_back(glm::vec3(-1.f * sX, 1.f * sY, 0.f));
                obj->pE.push_back(glm::vec3(1.f * sX, 1.f * sY, 0.f));
                obj->pE.push_back(glm::vec3(1.f * sX, -1.f * sY, 0.f));
                obj->pE.push_back(glm::vec3(-1.f * sX, -1.f * sY, 0.f));
                obj->pE.push_back(glm::vec3(-1.f * sX, 1.f * sY, 0.f)); //

                //cone
                obj->pE.push_back(glm::vec3(0.f, 0.f, sZ));
                obj->pE.push_back(glm::vec3(-1.f * sX, 1.f * sY, 0.f));

                obj->pE.push_back(glm::vec3(1.f * sX, 1.f * sY, 0.f));
                obj->pE.push_back(glm::vec3(0.f, 0.f, sZ));

                obj->pE.push_back(glm::vec3(1.f * sX, -1.f * sY, 0.f));
                obj->pE.push_back(glm::vec3(0.f, 0.f, sZ));

                obj->pE.push_back(glm::vec3(-1.f * sX, -1.f * sY, 0.f));
                obj->pE.push_back(glm::vec3(0.f, 0.f, sZ));
            }

            else if (obj->camLiType->val_s == "AREA")
            {
                lightArrowAdd(obj, -1.f, 1.f, 0.f, "horiz");
                lightArrowAdd(obj, 1.f, 1.f, 0.f, "vert");
                lightArrowAdd(obj, 1.f, -1.f, 0.f, "horiz");
                lightArrowAdd(obj, -1.f, -1.f, 0.f, "vert");
            }

            else if (obj->camLiType->val_s == "DIR")
            {
                //LR
                lightArrowAdd(obj, -.25f, 0.f, 0.f, "vert");
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));

                lightArrowAdd(obj, .25f, 0.f, 0.f, "vert");
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));

                //UD
                lightArrowAdd(obj, 0.f, .25f, 0.f, "horiz");
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));

                lightArrowAdd(obj, 0.f, -.25f, 0.f, "horiz");
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
            }

            else if (obj->camLiType->val_s == "POINT")
            {
                float scale = .5f;
                //XY
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(-scale, scale, 0.f));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(scale, scale, 0.f));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(scale, -scale, 0.f));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(-scale, -scale, 0.f));

                //XZ
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(-scale, 0.f, -scale));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(scale, 0.f, -scale));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(scale, 0.f, scale));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(-scale, 0.f, scale));

                //YZ
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(0.f, scale, scale));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(0.f, scale, -scale));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(0.f, -scale, -scale));

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                obj->pE.push_back(glm::vec3(0.f, -scale, scale));
            }

            else if (obj->camLiType->val_s == "SPOT")
            {
                glm::vec3 storedBegin;
                auto radius = .3f;

                for (int i = 0; i < 32; ++i)
                {
                    auto theta = 2.f * PI * i / 32.f;

                    auto x = radius * cos(theta);
                    auto y = radius * sin(theta);
                    obj->pE.push_back(glm::vec3(x, y, -1.f));

                    if (i == 0 || i == 8 || i == 16 || i == 24)
                    {
                        obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                        obj->pE.push_back(glm::vec3(x, y, -1.f));

                        if (i == 0)
                            storedBegin = glm::vec3(x, y, -1.f);
                    }
                }

                obj->pE.push_back(storedBegin);

                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
                lightArrowAdd(obj, 0.f, 0.f, -.375f, "vert");
                obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));
            }
        }

        else if (obj->type == "GRID")
        {
            auto gridLines = myWin.glslTable->gridLines->val_i + 1;
            auto gridMinus = myWin.glslTable->gridLines->val_i / 2.f;

            vector<glm::vec3> gridVerts;
            gridVerts.resize(gridLines * gridLines);

            for (int i = 0; i < gridLines; ++i)
            {
                for (int j = 0; j < gridLines; ++j)
                {
                    gridVerts[i + j * gridLines].x =  (i - gridMinus) / gridMinus;
                    gridVerts[i + j * gridLines].y =  (j - gridMinus) / gridMinus;
                }
            }

            for (auto &i : gridVerts)
                obj->pE.push_back(i);

            //THICK MIDDLE LINE
//            for (auto &i : gridVerts)
//            {
//                if (i.z == 0.f)
//                    obj->vboVC.push_back(glm::vec3(0.f));

//                else
//                    obj->vboVC.push_back(myWin.attrTable->Cgrid->val_3);
//            }
        }

        else if (obj->type == "GIZ_CIRC" || obj->type == "GIZ_CIRC_HALF")
        {
            for (int i = 0; i < 30; ++i)
                obj->pE.push_back(glm::vec3(0.f));
        }

        else if (obj->type == "GIZ_CIRC_FILL" || obj->type == "GIZ_DUAL_HANDLE")
        {
            auto r = 1.f;
            auto seg = 30;

            obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));

            for (int j = 0; j <= seg; ++j)
            {
                auto x = r * cos(j * 2.f * PI / seg);
                auto y = r * sin(j * 2.f * PI / seg);

                obj->pE.push_back(glm::vec3(x, y, 0.f));
            }
        }

        else if (obj->type == "GIZ_CONE")
        {
            glm::vec3 axX(1.f, 0.f, 0.f);
            glm::vec3 axY(0.f, 1.f, 0.f);
            glm::vec3 axZ(0.f, 0.f, 1.f);
            glm::vec3 pt, ax, axMult1, axMult2;

            auto fct = .075f;
            auto fct2 = .83f;

            if (name == "gizConeX" || name == "gizConeXSide")
            {
                ax = axX; axMult1 = axY; axMult2 = axZ;
            }

            else if (name == "gizConeY" || name == "gizConeYSide")
            {
                ax = axY; axMult1 = axX; axMult2 = axZ;
            }

            else if (name == "gizConeZ" || name == "gizConeZSide")
            {
                ax = axZ; axMult1 = axX; axMult2 = axY;
            }

            for (int i = 0; i <= 30; ++i)
            {
                pt = axMult1 * cos(((2.f * PI) / 30.f) * i) * fct;
                pt += axMult2 * sin(((2.f * PI) / 30.f) * i) * fct;
                pt += ax * fct2;
                obj->pE.push_back(pt);

                pt = axMult1 * cos(((2.f * PI) / 30.f) * (i + 1.f)) * fct;
                pt += axMult2 * sin(((2.f * PI) / 30.f) * (i + 1.f)) * fct;
                pt += ax * fct2;
                obj->pE.push_back(pt);

                obj->pE.push_back(ax);
            }
        }

        else if (obj->type == "GIZ_CUBE")
        {
            const glm::vec3 verts[] =
            {
                { -.5f, -.5f,  .5f }, { -.5f,  .5f,  .5f },
                {  .5f,  .5f,  .5f }, {  .5f, -.5f,  .5f },
                { -.5f, -.5f, -.5f }, { -.5f,  .5f, -.5f },
                {  .5f,  .5f, -.5f }, {  .5f, -.5f, -.5f },
            };

            const GLuint indices[] =
            {
                0, 2, 1, 0, 3, 2,
                4, 3, 0, 4, 7, 3,
                4, 1, 5, 4, 0, 1,
                3, 6, 2, 3, 7, 6,
                1, 6, 5, 1, 2, 6,
                7, 5, 6, 7, 4, 5
            };

            for (auto &i : verts)
                obj->pE.push_back(i);

            for (auto &i : indices)
                obj->idxE.push_back(i);
        }

        else if (obj->type == "GIZ_LINE")
        {
            obj->pE.push_back(glm::vec3(0.f));

            if (name == "gizLineX" || name == "gizLineXSide")
                obj->pE.push_back(glm::vec3(1.f, 0.f, 0.f));

            else if (name == "gizLineY" || name == "gizLineYSide")
                obj->pE.push_back(glm::vec3(0.f, 1.f, 0.f));

            else if (name == "gizLineZ" || name == "gizLineZSide")
                obj->pE.push_back(glm::vec3(0.f, 0.f, 1.f));
        }

        newO.push_back(obj);
    }

    else //load obj with opengl-tutorial.org loader
    {
        const auto *pathC = path.c_str();

        auto obj = make_shared<Object>(myWin);
        obj->type = type;
        obj->rename(name);

        vector<glm::vec3> vertices;
        vector<glm::vec2> uvs;
        vector<glm::vec3> normals;
        vector<glm::vec3> bE;

        if (loadNewOBJ(pathC, vertices, uvs, normals))
        {
            vector<glm::vec3> tangents;
            vector<glm::vec3> bitangents;

            computeTangentBasis(vertices, uvs, normals,tangents, bitangents);
            indexVBO_TBN(vertices, uvs, normals, tangents, bitangents, obj->idxE, obj->pE, obj->uvE, obj->nE, obj->tE, bE);

            newO.push_back(obj);
        }
    }

    for (auto &i : newO)
    {
        GLuint VBO_P, VBO_UV, VBO_T, VBO_N, VBO_IDX;

        if (!i->pE.empty() || i->type == "TXT")
        {
            glCreateBuffers(1, &VBO_P);

            if (i->type == "TXT")
            {
                i->txt2D = "ABJ GL QT 2015";
                glNamedBufferData(VBO_P, (GLsizei)strlen(i->txt2D), i->txt2D, GL_STATIC_DRAW);
            }

            else
                glNamedBufferData(VBO_P, i->pE.size() * 12, &i->pE[0], GL_STATIC_DRAW); //0
        }

        if (!i->uvE.empty())
        {
            glCreateBuffers(1, &VBO_UV);
            glNamedBufferData(VBO_UV, i->uvE.size() * 8, &i->uvE[0], GL_STATIC_DRAW); // 1
        }

        if (!i->tE.empty())
        {
            glCreateBuffers(1, &VBO_T);
            glNamedBufferData(VBO_T, i->tE.size() * 12, &i->tE[0], GL_STATIC_DRAW); // 2
        }

        if (!i->nE.empty())
        {
            glCreateBuffers(1, &VBO_N);
            glNamedBufferData(VBO_N, i->nE.size() * 12, &i->nE[0], GL_STATIC_DRAW); // 4
        }

        if (!i->idxE.empty())
        {
            glCreateBuffers(1, &VBO_IDX);
            glNamedBufferData(VBO_IDX, i->idxE.size() * 2, &i->idxE[0], GL_STATIC_DRAW);
        }

        /* delete any matching / pre-existing data */
        for (auto it = GLDataSh.begin(); it != GLDataSh.end();)
        {
            if ((*it).obj == i)
                it = GLDataSh.erase(it);

            else
                ++it;
        }

        GLDataSh.push_back( { i, VBO_P, VBO_UV, VBO_T, VBO_N, VBO_IDX } );

        VAOup(i);
    }

    return newO;
}

/* OPENGL-TUTORIAL.ORG */
bool GLWidgetSh::loadNewOBJ(const char *path, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals)
{
    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;

    auto *file = fopen(path, "r");

    if (!file)
        return 0;

    while(1) // read the first word of the line
    {
        char lineHeader[128];
        auto res = fscanf(file, "%s", lineHeader);

        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;

            if (fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z) != 3)
            {
                cout << "fscanf v is incorrect" << endl;

                return 0;
            }

            temp_vertices.push_back(vertex);
        }

        else if (strcmp(lineHeader, "vt") == 0)
        {
            glm::vec2 uv;

            if (fscanf(file, "%f %f\n", &uv.x, &uv.y) != 2)
            {
                cout << "fscanf vt is incorrect" << endl;

                return 0;
            }

            temp_uvs.push_back(uv);
        }

        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;

            if (fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z) != 3)
            {
                cout << "fscanf vn is incorrect" << endl;

                return 0;
            }

            temp_normals.push_back(normal);
        }

        else if (strcmp(lineHeader, "f") == 0)
        {
            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            auto matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            if (matches != 9)
            {
                printf("File can't be read by our simple parser :-(Try exporting with other options\n");

                return 0;
            }

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }

        else
        {
            char unusedBuffer[1000];

            if (fgets(unusedBuffer, 1000, file) == 0)
            {
                // Probably a comment, eat up the rest of the line
            }
        }
    }

    fclose(file);

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); ++i)
    {
        // Get the attributes thanks to the index
        auto vertex = temp_vertices[vertexIndices[i] - 1];
        auto uv = temp_uvs[uvIndices[i] - 1];
        auto normal = temp_normals[normalIndices[i] - 1];

        // Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }

    return 1;
}

void GLWidgetSh::computeTangentBasis(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals, vector<glm::vec3> &tangents, vector<glm::vec3> &bitangents)
{
    for (unsigned int i = 0; i < vertices.size(); i += 3)
    {
        // Shortcuts for vertices
        auto &v0 = vertices[i + 0];
        auto &v1 = vertices[i + 1];
        auto &v2 = vertices[i + 2];

        // Shortcuts for UVs
        auto &uv0 = uvs[i + 0];
        auto &uv1 = uvs[i + 1];
        auto &uv2 = uvs[i + 2];

        // Edges of the triangle : postion delta
        auto deltaPos1 = v1 - v0;
        auto deltaPos2 = v2 - v0;

        // UV delta
        auto deltaUV1 = uv1 - uv0;
        auto deltaUV2 = uv2 - uv0;

        auto r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        auto tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        auto bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        // Set the same tangent for all three vertices of the triangle.
        // They will be merged later, in vboindexer.cpp
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        // Same thing for bitangents
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

    // See "Going Further"
    for (unsigned int i = 0; i < vertices.size(); ++i)
    {
        auto &n = normals[i];
        auto &t = tangents[i];
        auto &b = bitangents[i];

        // Gram-Schmidt orthogonalize
        t = glm::normalize(t - n * glm::dot(n, t));

        // Calculate handedness
        if (glm::dot(glm::cross(n, t), b) < 0.f)
            t = t * -1.f;
    }
}

// Returns true if v1 can be considered equal to v2
bool GLWidgetSh::is_near(float v1, float v2)
{
    return fabs(v1 - v2) < .01f;
}

// Searches through all already-exported vertices for a similar one. Similar = same position + same UVs + same normal
bool GLWidgetSh::getSimilarVertexIndex(glm::vec3 &in_vertex, glm::vec2 &in_uv, glm::vec3 &in_normal, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals, unsigned short &result
)
{
    // Lame linear search
    for (unsigned int i = 0; i < out_vertices.size(); ++i)
    {
        if( is_near(in_vertex.x, out_vertices[i].x) && is_near(in_vertex.y, out_vertices[i].y) && is_near(in_vertex.z, out_vertices[i].z) &&
            is_near(in_uv.x, out_uvs[i].x) && is_near(in_uv.y, out_uvs[i].y) &&
            is_near(in_normal.x, out_normals[i].x) && is_near(in_normal.y, out_normals[i].y) && is_near(in_normal.z, out_normals[i].z))
        {
            result = i;

            return 1;
        }
    }

    // No other vertex could be used instead. Looks like we'll have to add it to the VBO.
    return 0;
}

void GLWidgetSh::indexVBO_TBN(vector<glm::vec3> &in_vertices, vector<glm::vec2> &in_uvs, vector<glm::vec3> &in_normals, vector<glm::vec3> &in_tangents, vector<glm::vec3> &in_bitangents, vector<unsigned short> &out_indices, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals, vector<glm::vec3> &out_tangents, vector<glm::vec3> &out_bitangents
)
{
    // For each input vertex
    for (unsigned int i = 0; i < in_vertices.size(); ++i)
    {
        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        auto found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i], out_vertices, out_uvs, out_normals, index);

        if (found)
        {
            // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back(index);

            // Average the tangents and the bitangents
            out_tangents[index] += in_tangents[i];
            out_bitangents[index] += in_bitangents[i];
        }

        else
        {
            // If not, it needs to be added in the output data.
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            out_tangents.push_back(in_tangents[i]);
            out_bitangents.push_back(in_bitangents[i]);
            out_indices.push_back((unsigned short)out_vertices.size() - 1);
        }
    }
}

void GLWidgetSh::lightArrowAdd(shared_ptr<Object> obj, float arrX, float arrY, float arrZ, string orient)
{
    float orientX_l, orientX_r, orientY_l, orientY_r;

    if (orient == "horiz")
    {
        orientX_r = arrX + .1f; orientY_r = arrY;
        orientX_l = arrX - .1f; orientY_l = arrY;
    }

    else
    {
        orientX_r = arrX; orientY_r = arrY + .1f;
        orientX_l = arrX; orientY_l = arrY - .1f;
    }

    obj->pE.push_back(glm::vec3(arrX, arrY, arrZ));
    obj->pE.push_back(glm::vec3(arrX, arrY, arrZ - 1.f));
    obj->pE.push_back(glm::vec3(orientX_r, orientY_r, arrZ - 1.f)); //
    obj->pE.push_back(glm::vec3(arrX, arrY, arrZ - 1.25f));
    obj->pE.push_back(glm::vec3(orientX_l, orientY_l, arrZ - 1.f)); //
    obj->pE.push_back(glm::vec3(arrX, arrY, arrZ - 1.f));
    obj->pE.push_back(glm::vec3(arrX, arrY, arrZ));
}

GLuint GLWidgetSh::createProg(string vIn, string gIn, string fIn)
{
    auto pathGLSL = myWin.pathTable->pathGLSL->val_s;

    auto vTemp = pathGLSL + vIn;
    const auto *vFile = vTemp.c_str();

    auto gTemp = pathGLSL + gIn;
    const auto *gFile = gTemp.c_str();

    auto fTemp = pathGLSL + fIn;
    const auto *fFile = fTemp.c_str();

    //
    auto pro = glCreateProgram();
    GLuint shaderV, shaderF, shaderG;

    if (vFile)
    {
        shaderV = createIndy(vFile, GL_VERTEX_SHADER);

        if (!shaderV)
            return 0;

        glAttachShader(pro, shaderV);
    }

    if (gFile != pathGLSL)
    {
        shaderG = createIndy(gFile, GL_GEOMETRY_SHADER);

        if (!shaderG)
            return 0;

        glAttachShader(pro, shaderG);
    }

    if (fFile)
    {
        shaderF = createIndy(fFile, GL_FRAGMENT_SHADER);

        if (!shaderF)
            return 0;

        glAttachShader(pro, shaderF);
    }

    glLinkProgram(pro);
    auto link_ok = GL_FALSE;
    glGetProgramiv(pro, GL_LINK_STATUS, &link_ok);

    if (!link_ok)
    {
        fprintf(stderr, "glLinkProgram:");
        shader_error(pro);
        glDeleteProgram(pro);

        return 0;
    }

    glDetachShader(pro, shaderV);
    glDetachShader(pro, shaderF);
    glDetachShader(pro, shaderG);
    glDeleteShader(shaderV);
    glDeleteShader(shaderF);
    glDeleteShader(shaderG);

    return pro;
}

GLuint GLWidgetSh::createIndy(const char *file, GLenum type)
{
    const auto *source = shader_read(file);

    if (source == 0)
    {
        fprintf(stderr, "Error opening %s: ", file);
        perror("");

        return 0;
    }

    auto shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);

    free((void*)source);

    glCompileShader(shader);
    auto compile_ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);

    if (compile_ok == GL_FALSE) // error here
    {
        fprintf(stderr, "%s:", file);
        shader_error(shader);
        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

char* GLWidgetSh::shader_read(const char *file)
{
    auto *input = fopen(file, "rb");

    if (!input)
        return 0;

    if (fseek(input, 0, SEEK_END) == -1)
        return 0;

    auto size = ftell(input);

    if (size == -1)
        return 0;

    if (fseek(input, 0, SEEK_SET) == -1)
        return 0;

    auto *content = (char*)malloc((size_t) size + 1);

    if (content == 0)
        return 0;

    if (!(fread(content, 1, (size_t)size, input)) || ferror(input))
    {
        cout << "error reading shader" << endl;
        free(content);

        return 0;
    }

    fclose(input);
    content[size] = '\0';

    return content;
}

void GLWidgetSh::shader_error(GLuint object)
{
    auto log_length = 0;

    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);

    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);

    else
    {
        fprintf(stderr, "printlog: Not a shader or a program\n");

        return;
    }

    auto *log = (char*)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, 0, log);

    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, 0, log);

    fprintf(stderr, "%s", log);
    free(log);
}
