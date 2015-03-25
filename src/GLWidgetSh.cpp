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

#include "GLWidgetSh.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

//////http://stackoverflow.com/questions/18176274/cubemap-shadow-mapping-not-working
CubeShadowTable myCubeShadowTable[6] =
{
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
};

GLWidgetSh::GLWidgetSh(MainWin &myWinTemp, QWidget *parent) : QGLWidget(parent), myWin(myWinTemp)
{
    QGLFormat format;
    format.setVersion(4, 5);
    format.setProfile(QGLFormat::CompatibilityProfile);
    format.setSwapInterval(0);
    setFormat(format);

    resize(200, 200);

//    cubeFBO_node = cubeDynNode_create("cube", 1024, 1024);
}

void GLWidgetSh::initializeGL()
{
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
        qDebug("Error %s", glewGetErrorString(glewInit()));
}

void GLWidgetSh::texInit()
{
//    allTex.push_back( { "brush5", "ALBEDO", texUp("brush/green16.jpg") } );
//    allTex.push_back( { "brush5", "ALBEDO", texUp("brush/green64.jpg") } );

    //ALBEDO
    allTex.push_back( { "BLANK", "ALBEDO", texUp("albedo/BLANK.png") } );
    allTex.push_back( { "checker", "ALBEDO", texUp("albedo/checker.png") } );
    allTex.push_back( { "uv", "ALBEDO", texUp("albedo/uv.jpg") } );
    allTex.push_back( { "gold", "ALBEDO", texUp("single/gold_256.tga") } );

    //ALPHA
    allTex.push_back( { "BLANK", "ALPHA", texUp("single/BLANK_white_256.tga") } );

    //CUBE IRRADIANCE / SPECULAR
    allTex.push_back( { "ennis", "CUBE", dds16fUp("cube/ennis_cube_specular.dds") } );
    allTex.push_back( { "glacier", "CUBE", dds16fUp("cube/glacier_cube_specular.dds") } );
    allTex.push_back( { "grace", "CUBE", dds16fUp("cube/grace_cube_specular.dds") } );
    allTex.push_back( { "pisa", "CUBE", dds16fUp("cube/pisa_cube_specular.dds") } );
    allTex.push_back( { "uffizi", "CUBE", dds16fUp("cube/uffizi_cube_specular.dds") } );

    allTex.push_back( { "ennis", "CUBE_IRRAD", dds16fUp("cube/ennis_cube_irradiance.dds") } );
    allTex.push_back( { "glacier", "CUBE_IRRAD", dds16fUp("cube/glacier_cube_irradiance.dds") } );
    allTex.push_back( { "grace", "CUBE_IRRAD", dds16fUp("cube/grace_cube_irradiance.dds") } );
    allTex.push_back( { "pisa", "CUBE_IRRAD", dds16fUp("cube/pisa_cube_irradiance.dds") } );
    allTex.push_back( { "uffizi", "CUBE_IRRAD", dds16fUp("cube/uffizi_cube_irradiance.dds") } );

    //METALLIC
    allTex.push_back( { "BLANK", "METALLIC", texUp("single/BLANK_black_256.tga") } );
    allTex.push_back( { "WHITE", "METALLIC", texUp("single/BLANK_white_256.tga") } );

    //NORMAL
    allTex.push_back( { "BLANK", "NORMAL", texUp("normal/BLANK.png") } );
    allTex.push_back( { "squares", "NORMAL", texUp("normal/squares.jpg") } );
    allTex.push_back( { "voronoi", "NORMAL", texUp("normal/voronoi.jpg") } );
    allTex.push_back( { "brushed_H", "NORMAL", texUp("normal/brushed_H.png") } );
    allTex.push_back( { "brushed_V", "NORMAL", texUp("normal/brushed_V.png") } );
    allTex.push_back( { "rand", "NORMAL", texUp("normal/rand.png") } ); // for ssao only

    //RUFF
    allTex.push_back( { "BLANK", "RUFF", texUp("single/BLANK_gray_256.tga") } );

    //TXT
    allTex.push_back( { "atlas", "TXT", texUp("txt/verasansmono.png") } );

    //LENS
    allTex.push_back( { "BLANK", "LENS", texUp("lens/BLANK_black_256.png") } );
    allTex.push_back( { "abjLens1", "LENS", texUp("lens/abjLens1.png") } );

    for (unsigned int i = 0; i < allTex.size(); ++i) //up64
        up64T(allTex[i].tex_32, allTex[i].tex_64, 1);
}

void GLWidgetSh::cubemapGen()
{
    //get selB and its (piv) pos
    //hide selB if viz
    //render
    //unhide

    glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO_node.fbo1);
    glViewport(0, 0, cubeFBO_node.width, cubeFBO_node.height);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    myWin.creatingDynCubeRGB = 1;

    bool storedV = myWin.selB->v->val_b;
    myWin.selB->v->val_b = 0;

    for (int i = 0; i < 6; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeFBO_node.tex1, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 targ = myCubeShadowTable[i].targ;
        glm::vec3 up = myCubeShadowTable[i].up;
        dynVM = glm::lookAt(glm::vec3(0.f), targ, up);
        negCubeCenter = glm::translate(glm::mat4(), -myWin.selB->t->val_3);

        for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
        {
            if (myWin.allObj[j]->type == "OBJ" && myWin.searchUp(myWin.allObj[j]))
            {
                myWin.myGLWidgetSh->glUseProgram2("pBase");
//                myWin.allObj[j]->render(myWin.allGL[GLidx]);
                myWin.allObj[j]->render(myWin.allGL[4]);
            }
        }
    }

    myWin.selB->v->val_b = storedV;

    myWin.creatingDynCubeRGB = 0;
}

AbjNode GLWidgetSh::cubeDynNode_create(QString name, int widthIn, int heightIn)
{
    //cubeFBO_node = cubeDynNode_create("cube", 1024, 1024);
    //glMakeTextureHandleNonResidentARB(myGL->cubeFBO_node.tex1_64);
    //glDeleteTextures(1, &cubeFBO_node.tex1);
    //glDeleteFramebuffers(1, &cubeFBO_node.fbo1);

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
        qDebug() << "error with FBO2_shadowCube_create";

    return { name, widthIn, heightIn, fboNew, texNew, 0, 0, 0, 0 };
}

/* SHADOWS */
void GLWidgetSh::addDeleteShadows(string type)
{
    if (type == "add")
    {
        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->v->val_b && myWin.allObj[i]->camLiTypeGet("light"))
            {
                int shadowSize = 1024;
                AbjNode shadow_no = shadowNode_create(myWin.allObj[i]->name->val_s, shadowSize, shadowSize);

                glMakeTextureHandleNonResidentARB(shadow_no.tex1_64);
                shadow_no.tex1_64 = glGetTextureHandleARB(shadow_no.tex1);
                glMakeTextureHandleResidentARB(shadow_no.tex1_64);

                glMakeTextureHandleNonResidentARB(shadow_no.tex2_64);
                shadow_no.tex2_64 = glGetTextureHandleARB(shadow_no.tex2);
                glMakeTextureHandleResidentARB(shadow_no.tex2_64);

                allShadow.push_back(shadow_no);
            }
        }
    }

    else if (type == "delete")
    {
        //SHADOW FBO
        for (unsigned int i = 0; i < allShadow.size(); ++i)
        {
            glMakeTextureHandleNonResidentARB(allShadow[i].tex1_64);
            glDeleteTextures(1, &allShadow[i].tex1);
            glDeleteFramebuffers(1, &allShadow[i].fbo1);

            glMakeTextureHandleNonResidentARB(allShadow[i].tex2_64);
            glDeleteTextures(1, &allShadow[i].tex2);
            glDeleteFramebuffers(1, &allShadow[i].fbo2);
        }

        allShadow.clear();
    }

    else if (type == "refresh")
    {
        addDeleteShadows("delete");
        addDeleteShadows("add");
    }
}

void GLWidgetSh::writeShadow(int idx)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    //render multiple times, once for each light, into shadow FBO, for each GL
    for (unsigned int i = 0; i < allShadow.size(); ++i)
    {
        if (myWin.allObj[idx]->name->val_s == allShadow[i].name) // if it's a light
        {
            shadowIdx = idx; //found the light
            int GLidx;

            for (unsigned int j = 0; j < myWin.allGL.size(); ++j)
            {
                if (myWin.allGL[j].get()->isVisible())
                {
                    GLidx = j;

                    break;
                }
            }

            // dir / spot
            glBindFramebuffer(GL_FRAMEBUFFER, allShadow[i].fbo1);
            glViewport(0, 0, allShadow[i].width, allShadow[i].height);
            glClear(GL_DEPTH_BUFFER_BIT);
            glClearColor(0.f, 0.f, 0.f, 0.f);

            for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
            {
                if (myWin.allObj[j]->type == "OBJ" && myWin.allObj[j]->shadowCast->val_b && myWin.searchUp(myWin.allObj[j]))
                {
                    myWin.allObj[j]->mvpGet(myWin.allGL[GLidx]); //
                    myWin.allObj[j]->render(myWin.allGL[GLidx]); //
                }
            }

            myWin.allObj[idx]->dirtyShadow = 0;
        }
    }
}

AbjNode GLWidgetSh::shadowNode_create(QString name, int widthIn, int heightIn)
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
        qDebug() << "error with shadowNode_create FBO1";

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
        qDebug() << "error with shadowNode_create FBO2";

    return { name, widthIn, heightIn, fboNew, texNew, 0, fboNew2, texNew2, 0 };
}

void GLWidgetSh::paintSlow(shared_ptr<GLWidget> myGL)
{
    string pathTex = myWin.pathTable->pathTex->val_s.toStdString();
    string pathIn = "brush/green16.jpg";
    string pathTemp = pathTex + pathIn;
    const char *path = pathTemp.c_str();

    int imgW, imgH, chan;
    GLubyte *img = stbi_load(path, &imgW, &imgH, &chan, 0);

    float data[4];
    glNamedFramebufferReadBuffer(myGL->main_node.fbo1, GL_COLOR_ATTACHMENT2);
    glReadPixels(myGL->pNew.x, myGL->height() - myGL->pNew.y, 1, 1, GL_RGBA, GL_FLOAT, data); //

    float uu = data[0];
    float vv = data[1];

//    qDebug() << "imgW = " << imgW << " imgH = " << imgH;

    for (unsigned int i = 0; i < allTex.size(); ++i) //paint
    {
        if (allTex[i].name == "BLANK" && allTex[i].type == "albedo")
        {
            glTextureSubImage2D(allTex[i].tex_32, 0, uu * 256, vv * 256, imgW, imgH, GL_RGB, GL_UNSIGNED_BYTE, img);
//            glTextureSubImage2D(allTex[i].tex_32, 0, uu * 256, vv * 256, 8, 8, GL_RGB, GL_UNSIGNED_BYTE, img);

            glTextureParameteri(allTex[i].tex_32, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(allTex[i].tex_32, GL_TEXTURE_WRAP_T, GL_REPEAT);

//            //ANISOTROPIC FILTERING
            GLfloat maxAniso = 0.f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
            glTextureParameteri(allTex[i].tex_32, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(allTex[i].tex_32, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(allTex[i].tex_32, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
            glGenerateTextureMipmap(allTex[i].tex_32);

            stbi_image_free(img);
        }
    }
}


GLuint GLWidgetSh::dds16fUp(string pathIn)
{    
    string pathTex;
#if __unix__
    pathTex = "/home/aleks/Desktop/tex/";
#else
    pathTex = "C:/users/aleks/desktop/tex/";
#endif

    string pathTemp = pathTex + pathIn;
    const char *path = pathTemp.c_str();

    FILE *myFile = fopen(path, "rb");

    if (!myFile)
    {
        fprintf(stderr, "Error opening file '%s'\n", path);
        return 0;
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

    GLuint texNew;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texNew);

    for (int i = 0; i < 6; ++i)
    {
        int w = myHeader.dwWidth;
        int h = myHeader.dwHeight;

        for (unsigned int j = 0; j < myHeader.dwMipMapCount; ++j)
        {
            void* texData = _data[i * myHeader.dwMipMapCount + j];

            glTextureImage2DEXT(texNew, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGBA16F, w, h, 0, GL_RGBA, GL_HALF_FLOAT, texData);

            w >>= 1;
            h >>= 1;
            w = w ? w : 1;
            h = h ? h : 1;
        }
    }

    glTextureParameteri(texNew, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(texNew, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texNew, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(texNew, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    return texNew;
}


//GLuint GLWidgetSh::dds16fUp(string pathIn)
//{
//    string pathTex;
//    pathTex = "C:/users/aleks/desktop/tex/";
//    string pathTemp = pathTex + pathIn;
//    const char *path = pathTemp.c_str();

//    FILE *myFile = fopen(path, "rb");
//    char filecode[4];
//    fread(filecode, 1, 4, myFile);

//    ddsHeader myHeader;
//    fread(&myHeader, sizeof(ddsHeader), 1, myFile);

//    vector<uint8_t*> _data;

//    for (int i = 0; i < 6; ++i)
//    {
//        int w = myHeader.dwWidth;
//        int h = myHeader.dwHeight;

//        for (unsigned int j = 0; j < myHeader.dwMipMapCount; ++j)
//        {
//            int size = w * h * 8;
//            uint8_t *pixels = new uint8_t[size];

//            fread(pixels, size, 1, myFile);
//            _data.push_back(pixels);

//            w = (w > 1) ? w >> 1 : 1;
//            h = (h > 1) ? h >> 1 : 1;
//        }
//    }

//    fclose(myFile);

//    GLuint texNew;
//    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texNew);

//    for (int i = 0; i < 6; ++i)
//    {
//        int w = myHeader.dwWidth;
//        int h = myHeader.dwHeight;

//        for (unsigned int j = 0; j < myHeader.dwMipMapCount; ++j)
//        {
//            void* texData = _data[i * myHeader.dwMipMapCount + j];

//            glTextureImage2DEXT(texNew, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGBA16F, w, h, 0, GL_RGBA, GL_HALF_FLOAT, texData);

//            w >>= 1;
//            h >>= 1;
//            w = w ? w : 1;
//            h = h ? h : 1;
//        }
//    }

//    glTextureParameteri(texNew, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTextureParameteri(texNew, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTextureParameteri(texNew, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//    glTextureParameteri(texNew, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

//    return texNew;
//}

GLuint GLWidgetSh::texUp(string pathIn)
{
    string pathTex = myWin.pathTable->pathTex->val_s.toStdString();
    string pathTemp = pathTex + pathIn;
    const char *path = pathTemp.c_str();

    int imgW, imgH, chan;
    GLubyte *img = stbi_load(path, &imgW, &imgH, &chan, 0);

    for (int h = 0; h < imgH / 2; ++h) //vFlip
    {
        int top = h * imgW * chan;
        int btm = (imgH - 1 - h) * imgW * chan;

        for (int w = 0; w < imgW * chan; ++w)
        {
            GLubyte temp = img[top];
            img[top] = img[btm];
            img[btm] = temp;
            ++top;
            ++btm;
        }
    }

    GLint formatI = 0;
    GLint formatP = 0;
    if (chan == 3) { formatI = GL_RGB8; formatP = GL_RGB; }
    else if (chan == 4) { formatI = GL_RGBA16; formatP = GL_RGBA; }

    GLuint tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);

    int numMip;
    if (pathIn == "txt/verasansmono.png") numMip = 1;
    else numMip = 1 + floor(log2(glm::max(imgW, imgH)));

    glTextureStorage2D(tex, numMip, formatI, imgW, imgH);
    glTextureSubImage2D(tex, 0, 0, 0, imgW, imgH, formatP, GL_UNSIGNED_BYTE, img);

    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //ANISOTROPIC FILTERING
    GLfloat maxAniso = 0.f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(tex, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    glGenerateTextureMipmap(tex);

    stbi_image_free(img);

    return tex;
}

void GLWidgetSh::up64N(AbjNode &node, bool up)
{
    if (up)
    {
        glMakeTextureHandleNonResidentARB(node.tex1_64);
        node.tex1_64 = glGetTextureHandleARB(node.tex1);
        glMakeTextureHandleResidentARB(node.tex1_64);

        glMakeTextureHandleNonResidentARB(node.tex2_64);
        node.tex2_64 = glGetTextureHandleARB(node.tex2);
        glMakeTextureHandleResidentARB(node.tex2_64);
    }

    else
    {
        glMakeTextureHandleNonResidentARB(node.tex1_64);
        glDeleteTextures(1, &node.tex1);
        glDeleteFramebuffers(1, &node.fbo1);

        glMakeTextureHandleNonResidentARB(node.tex2_64);
        glDeleteTextures(1, &node.tex2);
        glDeleteFramebuffers(1, &node.fbo2);
    }
}

void GLWidgetSh::up64T(GLuint &tex32, GLuint64 &tex64, bool up)
{
    if (up)
    {
        glMakeTextureHandleNonResidentARB(tex64);
        tex64 = glGetTextureHandleARB(tex32);
        glMakeTextureHandleResidentARB(tex64);
    }

    else
    {
        glMakeTextureHandleNonResidentARB(tex64);
        glDeleteTextures(1, &tex32);
    }
}

void GLWidgetSh::proUp()
{
    allPro.push_back( { "pBase1", createProg("base1V.glsl", "base1G.glsl", "base1F.glsl") } );
    allPro.push_back( { "pBase2", createProg("base2V.glsl", "base2G.glsl", "base2F.glsl") } );
    allPro.push_back( { "pBase3", createProg("base3V.glsl", "base3G.glsl", "base3F.glsl") } );
    allPro.push_back( { "pBB", createProg("bbV.glsl", "", "bbF.glsl") } );
    allPro.push_back( { "pGBuffer", createProg("gBufferV.glsl", "", "gBufferF.glsl") } );
    allPro.push_back( { "pGiz", createProg("gizV.glsl", "", "gizF.glsl") } );
    allPro.push_back( { "pGiz_circ", createProg("giz_circV.glsl", "", "gizF.glsl") } );
    allPro.push_back( { "pGiz_side", createProg("gizV.glsl", "", "giz_sideF.glsl") } );
    allPro.push_back( { "pGiz_side_over", createProg("fboV.glsl", "", "giz_side_overF.glsl") } );
    allPro.push_back( { "pGrid", createProg("gizV.glsl", "", "gridF.glsl") } );
    allPro.push_back( { "pNViz", createProg("nVizV.glsl", "nVizG.glsl", "nVizF.glsl") } );

    allPro.push_back( { "pTxt", createProg("txtV.glsl", "txtG.glsl", "txtF.glsl") } );
    allPro.push_back( { "pVolumeLight", createProg("volumeLightV.glsl", "", "volumeLightF.glsl") } );

    allPro.push_back( { "pAtmos", createProg("fboV.glsl", "", "atmosF.glsl") } );
    allPro.push_back( { "pSky", createProg("skyV.glsl", "", "skyF.glsl") } );

    allPro.push_back( { "pShadow", createProg("shadowV.glsl", "", "shadowF.glsl") } );
    allPro.push_back( { "pRtt", createProg("rttV.glsl", "", "rttF.glsl") } );
    allPro.push_back( { "pSelRect", createProg("selRectV.glsl", "", "selRectF.glsl") } );

    allPro.push_back( { "pAoBloomC", createProg("fboV.glsl", "", "aoBloomCF.glsl") } );
    allPro.push_back( { "pBloom", createProg("fboV.glsl", "", "bloomF.glsl") } );
    allPro.push_back( { "pDown", createProg("downV.glsl", "", "downF.glsl") } );
    allPro.push_back( { "pFinal", createProg("fboV.glsl", "", "finalF.glsl") } );
    allPro.push_back( { "pFxaa", createProg("fboV.glsl", "", "fxaaF.glsl") } );
    allPro.push_back( { "pGauss", createProg("fboV.glsl", "", "gaussF.glsl") } );
    allPro.push_back( { "pLumaInit", createProg("fboV.glsl", "", "lumaInitF.glsl") } );
    allPro.push_back( { "pLumaAdapt", createProg("fboV.glsl", "", "lumaAdaptF.glsl") } );
    allPro.push_back( { "pLumaAdapt_viz", createProg("fboV.glsl", "", "lumaAdapt_vizF.glsl") } );
    allPro.push_back( { "pStencilHi", createProg("stencilHiV.glsl", "", "stencilHiF.glsl") } );
    allPro.push_back( { "pSSAO", createProg("fboV.glsl", "", "ssaoF.glsl") } );
    allPro.push_back( { "pTonemap", createProg("fboV.glsl", "", "tonemapF.glsl") } );
}

void GLWidgetSh::glUseProgram2(QString name)
{
    for (unsigned int i = 0; i < allPro.size(); ++i)
    {
        if (name == "pBase") //get num visible lights to choose shader
        {
            stringstream ss;
            ss << "pBase" << myWin.lightCt;
            QString pWithCount = QString::fromStdString(ss.str());

            if (allPro[i].name == pWithCount)
                pro = allPro[i].pro;
        }

        else if (allPro[i].name == name)
            pro = allPro[i].pro;
    }

    glUseProgram(pro);
    proN = name;
}

void GLWidgetSh::VAOup(shared_ptr<Object> obj)
{
    if (!obj->dynVAO_perGL.empty())
        obj->dynVAO_perGL.clear();

    for (unsigned int i = 0; i < myWin.allGL.size(); ++i)
    {
        GLuint VAO = 0;
        obj->dynVAO_perGL.push_back( {myWin.allGL[i], 0, VAO} );
    }

    obj->BBup();
}

vector<shared_ptr<Object>> GLWidgetSh::VBOup(QString path, QString type, QString name, shared_ptr<Object> dupe)
{
    if (!myWin.loadO.empty()) myWin.loadO.clear();
    if (!vboNames.empty()) vboNames.clear();
    if (!newO.empty()) newO.clear();

    if (path.isEmpty())
    {
        shared_ptr<Object> obj = (dupe == 0) ? make_shared<Object>(myWin) : dupe;

        if (!dontRename)
            obj->rename(name);

        if (type == "AREA" || type == "DIR" || type == "FPS" || type == "ORTHO" || type == "PERSP" || type == "POINT" || type == "SPOT")
        {
            obj->type = "CAMLI";
            obj->camLiType->val_s = type;
            obj->rotOrder->val_s = "YZX";
            obj->ssaoTgl->val_b = 0;
        }

        else
            obj->type = type;

        QString name = obj->name->val_s;

        if (obj->type == "BB")
        {
            glm::vec3 bbV[] =
            {
                {-.5f, -.5f, -.5f}, { .5f, -.5f, -.5f},
                { .5f,  .5f, -.5f}, {-.5f,  .5f, -.5f},
                {-.5f, -.5f,  .5f}, { .5f, -.5f,  .5f},
                { .5f,  .5f,  .5f}, {-.5f,  .5f,  .5f},
            };

            for (unsigned int i = 0; i < arraySize(bbV); ++i)
                obj->pE.push_back(bbV[i]);

            GLushort bbI[] =
            {
                0, 1, 2, 3,
                4, 5, 6, 7,
                0, 4, 1, 5,
                2, 6, 3, 7
            };

            for (unsigned int i = 0; i < arraySize(bbI); ++i)
                obj->idxE.push_back(bbI[i]);
        }

        else if (obj->type == "CAMLI")
        {
            if (obj->camLiTypeGet("cam"))
            {
                float sX = 1.f;
                float sY = .5f;
                float sZ = 1.f;

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
                float radius = .3f;

                for (int i = 0; i < 32; ++i)
                {
                    float theta = 2.f * PI * i / 32.f;

                    float x = radius * cos(theta);
                    float y = radius * sin(theta);
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
            int gridLines = myWin.glslTable->gridLines->val_i + 1;
            float gridMinus = myWin.glslTable->gridLines->val_i / 2;

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

            for (unsigned int i = 0; i < gridVerts.size(); ++i)
                obj->pE.push_back(gridVerts[i]);

            //THICK MIDDLE LINE
//            for (int i = 0; i < gridVerts.size(); ++i)
//            {
//                if (gridVerts[i].z == 0.f)
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
            float r = 1.f;
            int seg = 30;

            obj->pE.push_back(glm::vec3(0.f, 0.f, 0.f));

            for (int j = 0; j <= seg; ++j)
            {
                float x = r * cos(j * 2.f * PI / seg);
                float y = r * sin(j * 2.f * PI / seg);

                obj->pE.push_back(glm::vec3(x, y, 0.f));
            }
        }

        else if (obj->type == "GIZ_CONE")
        {
            glm::vec3 axX(1.f, 0.f, 0.f);
            glm::vec3 axY(0.f, 1.f, 0.f);
            glm::vec3 axZ(0.f, 0.f, 1.f);
            glm::vec3 pt, ax, axMult1, axMult2;

            float fct = .075f;
            float fct2 = .83f;

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
                {-.5f, -.5f,  .5f}, {-.5f,  .5f,  .5f},
                { .5f,  .5f,  .5f}, { .5f, -.5f,  .5f},
                {-.5f, -.5f, -.5f}, {-.5f,  .5f, -.5f},
                { .5f,  .5f, -.5f}, { .5f, -.5f, -.5f},
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

            for (unsigned int i = 0; i < arraySize(verts); ++i)
                obj->pE.push_back(verts[i]);

            for (unsigned int i = 0; i < arraySize(indices); ++i)
                obj->idxE.push_back(indices[i]);
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
        QByteArray byteArray = path.toUtf8();
        const char *pathC = byteArray.constData();

        shared_ptr<Object> obj = make_shared<Object>(myWin);
        obj->type = type;
        obj->rename(name);

        vector<glm::vec3> vertices;
        vector<glm::vec2> uvs;
        vector<glm::vec3> normals;

        if (loadOBJ(pathC, vertices, uvs, normals))
        {
            vector<glm::vec3> tangents;
            vector<glm::vec3> bitangents;
            computeTangentBasis(vertices, uvs, normals,tangents, bitangents);

            indexVBO_TBN(vertices, uvs, normals, tangents, bitangents, obj->idxE, obj->pE, obj->uvE, obj->nE, obj->tE, obj->bE);

            newO.push_back(obj);
            vboNames.push_back(obj->name->val_s);
        }
    }

    for (unsigned int i = 0; i < newO.size(); ++i)
    {
        GLuint VBO_P, VBO_UV, VBO_T, VBO_B, VBO_N, VBO_IDX;

        if (!newO[i]->pE.empty() || newO[i]->type == "TXT")
        {
            glCreateBuffers(1, &VBO_P);

            if (newO[i]->type == "TXT")
            {
                newO[i]->txt2D = "ABJ GL QT 2014";
                glNamedBufferData(VBO_P, (GLsizei)strlen(newO[i]->txt2D), newO[i]->txt2D, GL_STATIC_DRAW);
            }

            else
                glNamedBufferData(VBO_P, newO[i]->pE.size() * 12, &newO[i]->pE[0], GL_STATIC_DRAW);
        }

        if (!newO[i]->uvE.empty())
        {
            glCreateBuffers(1, &VBO_UV);
            glNamedBufferData(VBO_UV, newO[i]->uvE.size() * 8, &newO[i]->uvE[0], GL_STATIC_DRAW);
        }

        if (!newO[i]->tE.empty())
        {
            glCreateBuffers(1, &VBO_T);
            glNamedBufferData(VBO_T, newO[i]->tE.size() * 12, &newO[i]->tE[0], GL_STATIC_DRAW);
        }

        if (!newO[i]->bE.empty())
        {
            glCreateBuffers(1, &VBO_B);
            glNamedBufferData(VBO_B, newO[i]->bE.size() * 12, &newO[i]->bE[0], GL_STATIC_DRAW);
        }

        if (!newO[i]->nE.empty())
        {
            glCreateBuffers(1, &VBO_N);
            glNamedBufferData(VBO_N, newO[i]->nE.size() * 12, &newO[i]->nE[0], GL_STATIC_DRAW);
        }

        if (!newO[i]->idxE.empty())
        {
            glCreateBuffers(1, &VBO_IDX);
            glNamedBufferData(VBO_IDX, newO[i]->idxE.size() * 2, &newO[i]->idxE[0], GL_STATIC_DRAW);
        }

        /* delete any matching / pre-existing data */
        for (vector<GLSharedData>::iterator it = GLDataSh.begin(); it != GLDataSh.end();)
        {
            if ((*it).obj == newO[i])
                it = GLDataSh.erase(it);

            else
                ++it;
        }

        GLDataSh.push_back(
        { newO[i],
          VBO_P, VBO_UV, VBO_T, VBO_B, VBO_N, VBO_IDX } );

        VAOup(newO[i]);
    }

    if (!vboNames.empty()) vboNames.clear();

    return newO;
}

/* OPENGL-TUTORIAL.ORG */
bool GLWidgetSh::loadOBJ(const char *path, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals)
{
    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;

    FILE *file = fopen(path, "r");
    if (!file) return 0;

    while(1) // read the first word of the line
    {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);

        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;

            if (fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z) != 3)
            {
                qDebug() << "fscanf v is incorrect";
                return 0;
            }

            temp_vertices.push_back(vertex);
        }

        else if (strcmp(lineHeader, "vt") == 0)
        {
            glm::vec2 uv;

            if (fscanf(file, "%f %f\n", &uv.x, &uv.y) != 2)
            {
                qDebug() << "fscanf vt is incorrect";
                return 0;
            }

            temp_uvs.push_back(uv);
        }

        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;

            if (fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z) != 3)
            {
                qDebug() << "fscanf vn is incorrect";
                return 0;
            }

            temp_normals.push_back(normal);
        }

        else if (strcmp(lineHeader, "f") == 0)
        {
            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

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
        glm::vec3 vertex = temp_vertices[vertexIndices[i] - 1];
        glm::vec2 uv = temp_uvs[uvIndices[i] - 1];
        glm::vec3 normal = temp_normals[normalIndices[i] - 1];

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
        glm::vec3 & v0 = vertices[i + 0];
        glm::vec3 & v1 = vertices[i + 1];
        glm::vec3 & v2 = vertices[i + 2];

        // Shortcuts for UVs
        glm::vec2 & uv0 = uvs[i + 0];
        glm::vec2 & uv1 = uvs[i + 1];
        glm::vec2 & uv2 = uvs[i + 2];

        // Edges of the triangle : postion delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // UV delta
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        // Set the same tangent for all three vertices of the triangle.
        // They will be merged later, in vboindexer.cpp
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        // Same thing for binormals
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

    // See "Going Further"
    for (unsigned int i = 0; i < vertices.size(); i += 1)
    {
        glm::vec3 &n = normals[i];
        glm::vec3 &t = tangents[i];
        glm::vec3 &b = bitangents[i];

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

// Searches through all already-exported vertices for a similar one.
// Similar = same position + same UVs + same normal
bool GLWidgetSh::getSimilarVertexIndex(glm::vec3 &in_vertex, glm::vec2 &in_uv, glm::vec3 &in_normal, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals, unsigned short &result
)
{
    // Lame linear search
    for (unsigned int i = 0; i < out_vertices.size(); ++i)
    {
        if (
            is_near(in_vertex.x, out_vertices[i].x) &&
            is_near(in_vertex.y, out_vertices[i].y) &&
            is_near(in_vertex.z, out_vertices[i].z) &&
            is_near(in_uv.x, out_uvs[i].x) &&
            is_near(in_uv.y, out_uvs[i].y) &&
            is_near(in_normal.x, out_normals [i].x) &&
            is_near(in_normal.y, out_normals [i].y) &&
            is_near(in_normal.z, out_normals [i].z)
        )
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
        bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i], out_vertices, out_uvs, out_normals, index);

        if (found)
        { // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back(index);

            // Average the tangents and the bitangents
            out_tangents[index] += in_tangents[i];
            out_bitangents[index] += in_bitangents[i];
        }

        else
        { // If not, it needs to be added in the output data.
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
    string pathGLSL = myWin.pathTable->pathGLSL->val_s.toStdString();

    string vTemp = pathGLSL + vIn;
    const char *vFile = vTemp.c_str();

    string gTemp = pathGLSL + gIn;
    const char *gFile = gTemp.c_str();

    string fTemp = pathGLSL + fIn;
    const char *fFile = fTemp.c_str();

    //
    GLuint pro = glCreateProgram();
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
    GLint link_ok = GL_FALSE;
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
    const GLchar* source = shader_read(file);

    if (source == 0)
    {
        fprintf(stderr, "Error opening %s: ", file);
        perror("");

        return 0;
    }

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);

    free((void*)source);

    glCompileShader(shader);
    GLint compile_ok = GL_FALSE;
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
    FILE *input = fopen(file, "rb");

    if (!input) return 0;
    if (fseek(input, 0, SEEK_END) == -1) return 0;

    long size = ftell(input);

    if (size == -1) return 0;
    if (fseek(input, 0, SEEK_SET) == -1) return 0;

    char *content = (char*) malloc((size_t) size + 1);
    if (content == 0) return 0;

    if (!(fread(content, 1, (size_t)size, input)) || ferror(input))
    {
        qDebug() << "error reading shader";
        free(content);

        return 0;
    }

    fclose(input);
    content[size] = '\0';

    return content;
}

void GLWidgetSh::shader_error(GLuint object)
{
    GLint log_length = 0;

    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);

    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);

    else
    {
        fprintf(stderr, "printlog: Not a shader or a program\n");

        return;
    }

    char *log = (char*)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, 0, log);

    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, 0, log);

    fprintf(stderr, "%s", log);
    free(log);
}
