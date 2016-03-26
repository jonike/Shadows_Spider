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

#include "PP.h"

PP::PP(MainWin &myWinTemp) : myWin(myWinTemp) {}

bool PP::fboPrep()
{
    myWin.myGL->gBufN = gBufN_create();

    myWin.myGL->depthRevN = depthRevN_create();
    myWin.myGLWidgetSh->up64N(myWin.myGL->depthRevN, 1);

    myWin.myGL->bgN = bgN_create(); //giz / grid / sky
    myWin.myGLWidgetSh->up64N(myWin.myGL->bgN, 1);

    myWin.myGL->tonemapN = tonemapN_create();
    myWin.myGLWidgetSh->up64N(myWin.myGL->tonemapN, 1);

    myWin.myGL->alphaGaussN = dualN_create("alphaGauss", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->alphaGaussN, 1);

    myWin.myGL->bloomCN = singleN_create("bloomC", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->bloomCN, 1);

    myWin.myGL->deferredN = singleN_create("def", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->deferredN, 1);

    myWin.myGL->fxaaN = singleN_create("fxaa", GL_RGB16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->fxaaN, 1);

    myWin.myGL->brushN = dualN_create("brush", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->brushN, 1);

    myWin.myGL->brushBGN = dualN_create("brushBG", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->brushBGN, 1);

    myWin.myGL->brushTempN = dualN_create("brushTemp", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->brushTempN, 1);

    myWin.myGL->eraserN = dualN_create("eraser", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->eraserN, 1);

    myWin.myGL->cursorN = singleN_create("cursor", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->cursorN, 1);

    myWin.myGL->sobelN = singleN_create("sobel", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->sobelN, 1);

    myWin.myGL->ssaoN = singleN_create("ssao", GL_R16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->ssaoN, 1);

    myWin.myGL->ssaoGaussN = dualN_create("ssaoGauss", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->ssaoGaussN, 1);

    myWin.myGL->ssrN = singleN_create("ssr", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->ssrN, 1);

    myWin.myGL->tonemapExpN = singleN_create("tonemapExp", GL_RGB16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->tonemapExpN, 1);

    myWin.myGL->lumaInitN = singleN_create("lumaInit", GL_R16F, 1024, 1024);

    for (int i = 0; i < 2; ++i)
    {
        string conc = "lumaAdapt";
        conc.append(to_string(i));

        myWin.myGL->lumaAdaptN[i] = singleN_create(conc, GL_R16F, 1024, 1024);
    }

    bloomBufferCreate();

    return true;
}

void PP::bloomBufferCreate()
{
    myWin.myGL->bloomN = singleN_create("bloom", GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
    myWin.myGLWidgetSh->up64N(myWin.myGL->bloomN, 1);

    auto bWidth = myWin.myGL->width() / 2;
    auto bHeight = myWin.myGL->height() / 2;

    for (int i = 0; i < 6; ++i)
    {
        string conc = "down";
        conc.append(to_string(i));

        myWin.myGL->downN[i] = singleN_create(conc, GL_RGBA16F, bWidth, bHeight);
        myWin.myGLWidgetSh->up64N(myWin.myGL->downN[i], 1);

        conc = "bloomGauss";
        conc.append(to_string(i));

        myWin.myGL->bloomGaussN[i] = dualN_create(conc, GL_RGBA16F, bWidth, bHeight);
        myWin.myGLWidgetSh->up64N(myWin.myGL->bloomGaussN[i], 1);

        bWidth /= 2;
        bHeight /= 2;
    }
}

AbjNode PP::bgN_create()
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

    for (int i = 0; i < cAttachNum; ++i)
    {
        glTextureStorage2D(setupRTT[i], 1, GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glNamedFramebufferTexture(fboNew, DrawBuffers[i], setupRTT[i], 0);
    }

    //DEPTH STEN
    GLuint myDS;
    glCreateTextures(GL_TEXTURE_2D, 1, &myDS);
    glTextureStorage2D(myDS, 1, GL_DEPTH32F_STENCIL8, myWin.myGL->width(), myWin.myGL->height());
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(myDS, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myDS, 0);
    glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with bgN_create" << endl;

    return { "bg", myWin.myGL->width(), myWin.myGL->height(), fboNew, setupRTT[0], 0, 0, setupRTT[1], 0, myDS };
}

AbjNode PP::depthRevN_create()
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    auto usableW = myWin.myGL->width() / 1.f;
    auto usableH = myWin.myGL->height() / 1.f;

    //DEPTH STEN
    GLuint myDS;
    glCreateTextures(GL_TEXTURE_2D, 1, &myDS);
    glTextureStorage2D(myDS, 1, GL_DEPTH32F_STENCIL8, usableW, usableH);
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(myDS, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_ATTACHMENT, myDS, 0);
    glNamedFramebufferDrawBuffer(fboNew, GL_NONE);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with depthRevN_create" << endl;

    return { "depthRev", usableW, usableH, fboNew, 0, 0, 0, 0, 0, myDS };
}

AbjNode PP::tonemapN_create()
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

    for (int i = 0; i < cAttachNum; ++i)
    {
        glTextureStorage2D(setupRTT[i], 1, GL_RGB16F, myWin.myGL->width(), myWin.myGL->height());
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (myWin.myFSQ->fxaaBlur->val_b)
        {
            glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        else
        {
            glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        glNamedFramebufferTexture(fboNew, DrawBuffers[i], setupRTT[i], 0);
    }

    glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with tonemapN_create" << endl;

    return { "tonemap", myWin.myGL->width(), myWin.myGL->height(), fboNew, setupRTT[0] , 0, 0, setupRTT[1] };
}

AbjNode PP::singleN_create(string name, GLenum format, int widthIn, int heightIn)
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint texNew[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, texNew);

    for (int i = 0; i < cAttachNum; ++i)
    {
        auto numMip = 1 + floor(log2(glm::max(widthIn, heightIn)));
        glTextureStorage2D(texNew[i], numMip, format, widthIn, heightIn);

        glTextureParameteri(texNew[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texNew[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (name == "paintProj")
        {
            glTextureParameteri(texNew[i], GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(texNew[i], GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        if (name == "lumaInitN" || name == "lumaAdapt0" || name == "lumaAdapt1")
        {
            glTextureParameteri(texNew[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(texNew[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        else
        {
            glTextureParameteri(texNew[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(texNew[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        glGenerateTextureMipmap(texNew[i]);
        glNamedFramebufferTexture(fboNew, DrawBuffers[i], texNew[i], 0);
        glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

        if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "error with singleN_create" << endl;
    }

    return { name, widthIn, heightIn, fboNew, texNew[0] };
}

AbjNode PP::dualN_create(string name, GLenum format, int widthIn, int heightIn)
{
    const auto numFBO = 2;

    GLuint nodeNew[numFBO];
    glCreateFramebuffers(numFBO, nodeNew);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[numFBO * cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, numFBO * cAttachNum, setupRTT);

    for (int i = 0; i < numFBO; ++i)
    {
        for (int j = 0; j < cAttachNum; ++j)
        {
            glTextureStorage2D(setupRTT[i], 1, format, widthIn, heightIn);
            glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glNamedFramebufferTexture(nodeNew[i], DrawBuffers[j], setupRTT[i], 0);
            glNamedFramebufferDrawBuffers(nodeNew[i], cAttachNum, DrawBuffers);

            if (glCheckNamedFramebufferStatus(nodeNew[i], GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                cout << "error with dualN_create" << endl;
        }
    }

    return { name, widthIn, heightIn, nodeNew[0], setupRTT[0], 0, nodeNew[1], setupRTT[1] };
}

AbjNode PP::gBufN_create()
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7,
    };

    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

    for (int i = 0; i < cAttachNum; ++i)
    {
        if (i == 0)
            glTextureStorage2D(setupRTT[i], 1, GL_RGBA32F, myWin.myGL->width(), myWin.myGL->height());

        else if (i == 6)
            glTextureStorage2D(setupRTT[i], 1, GL_RGBA16F, myWin.myGL->width(), myWin.myGL->height());

        else if (i == 7)
            glTextureStorage2D(setupRTT[i], 1, GL_R16F, myWin.myGL->width(), myWin.myGL->height());

        else
            glTextureStorage2D(setupRTT[i], 1, GL_RGBA32UI, myWin.myGL->width(), myWin.myGL->height());

        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glNamedFramebufferTexture(fboNew, DrawBuffers[i], setupRTT[i], 0);
    }

    myWin.myGL->gBuf0_32 = setupRTT[0];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf0_32, myWin.myGL->gBuf0_64, 1);

    myWin.myGL->gBuf1_32 = setupRTT[1];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf1_32, myWin.myGL->gBuf1_64, 1);

    myWin.myGL->gBuf2_32 = setupRTT[2];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf2_32, myWin.myGL->gBuf2_64, 1);

    myWin.myGL->gBuf3_32 = setupRTT[3];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf3_32, myWin.myGL->gBuf3_64, 1);

    myWin.myGL->gBuf4_32 = setupRTT[4];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf4_32, myWin.myGL->gBuf4_64, 1);

    myWin.myGL->gBuf5_32 = setupRTT[5];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf5_32, myWin.myGL->gBuf5_64, 1);

    myWin.myGL->gBuf6_32 = setupRTT[6];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf6_32, myWin.myGL->gBuf6_64, 1);

    myWin.myGL->gBuf7_32 = setupRTT[7];
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf7_32, myWin.myGL->gBuf7_64, 1);

    //DEPTH STEN
    glCreateTextures(GL_TEXTURE_2D, 1, &myWin.myGL->gBuf_DS_32);
    glTextureStorage2D(myWin.myGL->gBuf_DS_32, 1, GL_DEPTH32F_STENCIL8, myWin.myGL->width(), myWin.myGL->height());
    glTextureParameteri(myWin.myGL->gBuf_DS_32, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myWin.myGL->gBuf_DS_32, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myWin.myGL->gBuf_DS_32, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(myWin.myGL->gBuf_DS_32, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myWin.myGL->gBuf_DS_32, 0);
    glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with gBufN_create" << endl;

    return { "def", myWin.myGL->width(), myWin.myGL->height(), fboNew };
}

GLuint PP::gaussianLinear(GLuint src, AbjNode dest)
{
    for (int i = 0; i < 2; ++i)
    {
        if (i == 0)
            glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo1);

        else
            glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo2);

        glViewport(0, 0, dest.width, dest.height);
        glClear(GL_COLOR_BUFFER_BIT);

        myWin.myGL->rttGaussIn32 = (i == 0) ? src : dest.tex1_32;
        myWin.myGLWidgetSh->up64T(myWin.myGL->rttGaussIn32, myWin.myGL->rttGaussIn64, 1);

        myWin.myFSQ->gaussStage = i;
        myWin.myFSQ->render();
    }

    return dest.tex2_32;
}

void PP::gaussianRecursivePasses(GLuint src, AbjNode dest, int passes, int iter)
{
    myWin.myGL->tempGauss = gaussianLinear(src, dest);

    if (iter + 1 < passes)
        gaussianRecursivePasses(myWin.myGL->tempGauss, dest, passes, iter + 1);
}

GLuint PP::gaussianBlur(AbjNode src, AbjNode dest, int passes)
{
    if (passes == 0)
        return src.tex1_32;

    gaussianRecursivePasses(src.tex1_32, dest, passes, 0);

    return myWin.myGL->tempGauss;
}

GLuint PP::gaussianBlur2(GLuint src, AbjNode dest, int passes)
{
    if (passes == 0)
        return src;

    gaussianRecursivePasses(src, dest, passes, 0);

    return myWin.myGL->tempGauss;
}

void PP::downSampRender(AbjNode src, AbjNode dest, int downPixAmt)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo1);
    glViewport(0, 0, dest.width, dest.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myWin.myGLWidgetSh->up64T(src.tex1_32, myWin.myGL->downSamp_64, 1);

    myWin.myGL->texelSize = glm::vec2(downPixAmt / src.width, downPixAmt / src.height);
    myWin.myFSQ->render();
}

void PP::bloomRender()
{
    myWin.myGLWidgetSh->glUseProgram2("pDown");

    for (int i = 0; i < 6; ++i) //DOWNSAMP
    {
        if (i == 0)
            downSampRender(myWin.myGL->deferredN, myWin.myGL->downN[i], 2);

        else
            downSampRender(myWin.myGL->downN[i - 1], myWin.myGL->downN[i], 2);
    }

    myWin.myGLWidgetSh->glUseProgram2("pGauss");

    for (int i = 0; i < 6; ++i)
        gaussianBlur(myWin.myGL->downN[i], myWin.myGL->bloomGaussN[i], 4);

    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->bloomN.fbo1);
    glViewport(0, 0, myWin.myGL->bloomN.width, myWin.myGL->bloomN.height);
    glClear(GL_COLOR_BUFFER_BIT);

    myWin.myGLWidgetSh->glUseProgram2("pBloom");
    myWin.myFSQ->render();
}

void PP::postFX()
{
    //SSAO
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->ssaoN.fbo1);
    glViewport(0, 0, myWin.myGL->ssaoN.width, myWin.myGL->ssaoN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pSSAO_" + myWin.myFSQ->ssaoKernel->val_s);
    myWin.myFSQ->render();

    glMakeTextureHandleNonResidentARB(myWin.myGL->ssaoGaussN.tex2_64);
    myWin.myGLWidgetSh->glUseProgram2("pGauss");
    myWin.myGL->ssaoGaussN.tex2_64 = glGetTextureHandleARB(gaussianBlur(myWin.myGL->ssaoN, myWin.myGL->ssaoGaussN, true));
    glMakeTextureHandleResidentARB(myWin.myGL->ssaoGaussN.tex2_64);

    //DEFERRED LIGHTING
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->deferredN.fbo1);
    glViewport(0, 0, myWin.myGL->deferredN.width, myWin.myGL->deferredN.height);
    glClear(GL_COLOR_BUFFER_BIT);

    pDefDyn = "pDef";
    pDefDyn.append(to_string(myWin.lightCt));
    myWin.myGLWidgetSh->glUseProgram2(pDefDyn);
    myWin.myFSQ->render();

        /* TRANSP COMPOSITE */
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        myWin.myGLWidgetSh->glUseProgram2("pTranspComp");
        myWin.myFSQ->render();
        glDisable(GL_BLEND);

    //LUMA INIT
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->lumaInitN.fbo1);
    glViewport(0, 0, myWin.myGL->lumaInitN.width, myWin.myGL->lumaInitN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pLumaInit");
    myWin.myFSQ->render();
    myWin.myGLWidgetSh->up64T(myWin.myGL->lumaInitN.tex1_32, myWin.myGL->lumaInitN.tex1_64, 1);

    //ADAPT LUMA
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].fbo1);
    glViewport(0, 0, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].width, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt");
    myWin.myFSQ->render();
    glGenerateTextureMipmap(myWin.myGL->lumaAdaptN[myWin.myGL->currLum].tex1_32);
    myWin.myGLWidgetSh->up64T(myWin.myGL->lumaAdaptN[myWin.myGL->currLum].tex1_32, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].tex1_64, 1);

    //BLOOM + C
    bloomRender();

    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->bloomCN.fbo1);
    glViewport(0, 0, myWin.myGL->bloomCN.width, myWin.myGL->bloomCN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pBloomC");
    myWin.myFSQ->render();

    //TONEMAP EXPOSURE
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->tonemapExpN.fbo1);
    glViewport(0, 0, myWin.myGL->tonemapExpN.width, myWin.myGL->tonemapExpN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt_viz");
    myWin.myFSQ->render();

    //TONEMAP
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->tonemapN.fbo1);
    glViewport(0, 0, myWin.myGL->tonemapN.width, myWin.myGL->tonemapN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pTonemap");
    myWin.myFSQ->render();

    //SSR
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->ssrN.fbo1);
    glViewport(0, 0, myWin.myGL->ssrN.width, myWin.myGL->ssrN.height);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //
    myWin.myGLWidgetSh->glUseProgram2("pSSR");
    myWin.myFSQ->render();
    glDisable(GL_BLEND);

    //FXAA
    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->fxaaN.fbo1);
    glViewport(0, 0, myWin.myGL->fxaaN.width, myWin.myGL->fxaaN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGL->texelSize = glm::vec2(1.f / myWin.myGL->fxaaN.width, 1.f / myWin.myGL->fxaaN.height);
    myWin.myGLWidgetSh->glUseProgram2("pFxaa");
    myWin.myFSQ->render();

    //FINAL
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, myWin.myGL->width(), myWin.myGL->height());
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pFinal");
    myWin.myFSQ->render();

    myWin.myGL->currLum = !myWin.myGL->currLum;
}


//void PP::postFX()
//{
//    //SSAO
//    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->ssaoN.fbo1);
//    glViewport(0, 0, myWin.myGL->ssaoN.width, myWin.myGL->ssaoN.height);
//    glClear(GL_COLOR_BUFFER_BIT);
//    myWin.myGLWidgetSh->glUseProgram2("pSSAO_" + myWin.myFSQ->ssaoKernel->val_s);
//    myWin.myFSQ->render();

//    glMakeTextureHandleNonResidentARB(myWin.myGL->ssaoGaussN.tex2_64);
//    myWin.myGLWidgetSh->glUseProgram2("pGauss");
//    myWin.myGL->ssaoGaussN.tex2_64 = glGetTextureHandleARB(gaussianBlur(myWin.myGL->ssaoN, myWin.myGL->ssaoGaussN, true));
//    glMakeTextureHandleResidentARB(myWin.myGL->ssaoGaussN.tex2_64);

//    //DEFERRED LIGHTING
//    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->deferredN.fbo1);
//    glViewport(0, 0, myWin.myGL->deferredN.width, myWin.myGL->deferredN.height);
//    glClear(GL_COLOR_BUFFER_BIT);

//    pDefDyn = "pDef";
//    pDefDyn.append(to_string(myWin.lightCt));
//    myWin.myGLWidgetSh->glUseProgram2(pDefDyn);
//    myWin.myFSQ->render();

//    /* TRANSP COMPOSITE */
//    glEnable(GL_BLEND);
//    glBlendEquation(GL_FUNC_ADD);
//    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
//    myWin.myGLWidgetSh->glUseProgram2("pTranspComp");
//    myWin.myFSQ->render();
//    glDisable(GL_BLEND);

//    //    //LUMA INIT
//    //    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->lumaInitN.fbo1);
//    //    glViewport(0, 0, myWin.myGL->lumaInitN.width, myWin.myGL->lumaInitN.height);
//    //    glClear(GL_COLOR_BUFFER_BIT);
//    //    myWin.myGLWidgetSh->glUseProgram2("pLumaInit");
//    //    myWin.myFSQ->render();
//    //    myWin.myGLWidgetSh->up64T(myWin.myGL->lumaInitN.tex1_32, myWin.myGL->lumaInitN.tex1_64, 1);

//    //    //ADAPT LUMA
//    //    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].fbo1);
//    //    glViewport(0, 0, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].width, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].height);
//    //    glClear(GL_COLOR_BUFFER_BIT);
//    //    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt");
//    //    myWin.myFSQ->render();
//    //    glGenerateTextureMipmap(myWin.myGL->lumaAdaptN[myWin.myGL->currLum].tex1_32);
//    //    myWin.myGLWidgetSh->up64T(myWin.myGL->lumaAdaptN[myWin.myGL->currLum].tex1_32, myWin.myGL->lumaAdaptN[myWin.myGL->currLum].tex1_64, 1);

//    //    //BLOOM + C
//    //    bloomRender();

//    //    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->bloomCN.fbo1);
//    //    glViewport(0, 0, myWin.myGL->bloomCN.width, myWin.myGL->bloomCN.height);
//    //    glClear(GL_COLOR_BUFFER_BIT);
//    //    myWin.myGLWidgetSh->glUseProgram2("pBloomC");
//    //    myWin.myFSQ->render();

//    //    //TONEMAP EXPOSURE
//    //    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->tonemapExpN.fbo1);
//    //    glViewport(0, 0, myWin.myGL->tonemapExpN.width, myWin.myGL->tonemapExpN.height);
//    //    glClear(GL_COLOR_BUFFER_BIT);
//    //    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt_viz");
//    //    myWin.myFSQ->render();

//    //    //TONEMAP
//    //    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->tonemapN.fbo1);
//    //    glViewport(0, 0, myWin.myGL->tonemapN.width, myWin.myGL->tonemapN.height);
//    //    glClear(GL_COLOR_BUFFER_BIT);
//    //    myWin.myGLWidgetSh->glUseProgram2("pTonemap");
//    //    myWin.myFSQ->render();

//    //    //SSR
//    //    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->ssrN.fbo1);
//    //    glViewport(0, 0, myWin.myGL->ssrN.width, myWin.myGL->ssrN.height);
//    //    glClear(GL_COLOR_BUFFER_BIT);

//    //    glEnable(GL_BLEND);
//    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //
//    //    myWin.myGLWidgetSh->glUseProgram2("pSSR");
//    //    myWin.myFSQ->render();
//    //    glDisable(GL_BLEND);

//    //    //FXAA
//    //    glBindFramebuffer(GL_FRAMEBUFFER, myWin.myGL->fxaaN.fbo1);
//    //    glViewport(0, 0, myWin.myGL->fxaaN.width, myWin.myGL->fxaaN.height);
//    //    glClear(GL_COLOR_BUFFER_BIT);
//    //    myWin.myGL->texelSize = glm::vec2(1.f / myWin.myGL->fxaaN.width, 1.f / myWin.myGL->fxaaN.height);
//    //    myWin.myGLWidgetSh->glUseProgram2("pFxaa");
//    //    myWin.myFSQ->render();

//    //FINAL
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0, 0, myWin.myGL->width(), myWin.myGL->height());
//    glClear(GL_COLOR_BUFFER_BIT);
//    myWin.myGLWidgetSh->glUseProgram2("pFinal");
//    myWin.myFSQ->render();

//    myWin.myGL->currLum = !myWin.myGL->currLum;
//}

void PP::resizeTexClearMem()
{
    //GBUF FBO
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf0_32, myWin.myGL->gBuf0_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf1_32, myWin.myGL->gBuf1_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf2_32, myWin.myGL->gBuf2_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf3_32, myWin.myGL->gBuf3_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf4_32, myWin.myGL->gBuf4_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf5_32, myWin.myGL->gBuf5_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf6_32, myWin.myGL->gBuf6_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf7_32, myWin.myGL->gBuf7_64, 0);
    myWin.myGLWidgetSh->up64T(myWin.myGL->gBuf_DS_32, myWin.myGL->gBuf_DS_64, 0);
    glDeleteFramebuffers(1, &myWin.myGL->gBufN.fbo1);

    myWin.myGLWidgetSh->up64N(myWin.myGL->alphaGaussN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->bgN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->bloomN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->bloomCN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->deferredN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->fxaaN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->brushN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->brushBGN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->brushTempN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->sobelN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->eraserN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->cursorN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->ssaoN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->ssrN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->ssaoGaussN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->tonemapN, 0);
    myWin.myGLWidgetSh->up64N(myWin.myGL->tonemapExpN, 0);

    myWin.myGLWidgetSh->up64N(myWin.myGL->lumaInitN, 0);

    for (int i = 0; i < 2; ++i)
        myWin.myGLWidgetSh->up64N(myWin.myGL->lumaAdaptN[i], 0);

    myWin.myGL->fboReady = false;
}
