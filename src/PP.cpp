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

bool PP::fboPrep(shared_ptr<GLWidget> myGLin)
{
    myGL = myGLin;

    myGL->gBufN = gBufN_create();

    myGL->depthRevN = depthRevN_create();
    myWin.myGLWidgetSh->up64N(myGL->depthRevN, 1);

    myGL->bgN = bgN_create(); //giz / grid / sky
    myWin.myGLWidgetSh->up64N(myGL->bgN, 1);

    myGL->tonemapN = tonemapN_create();
    myWin.myGLWidgetSh->up64N(myGL->tonemapN, 1);

    myGL->alphaGaussN = dualN_create("alphaGauss", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->alphaGaussN, 1);

    myGL->bloomCN = singleN_create("bloomC", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->bloomCN, 1);

    myGL->deferredN = singleN_create("def", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->deferredN, 1);

    myGL->fxaaN = singleN_create("fxaa", GL_RGB16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->fxaaN, 1);

    myGL->brushN = dualN_create("brush", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->brushN, 1);

    myGL->brushBGN = dualN_create("brushBG", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->brushBGN, 1);

    myGL->brushTempN = dualN_create("brushTemp", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->brushTempN, 1);

    myGL->eraserN = dualN_create("eraser", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->eraserN, 1);

    myGL->cursorN = singleN_create("cursor", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->cursorN, 1);

    myGL->sobelN = singleN_create("sobel", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->sobelN, 1);

    myGL->ssaoN = singleN_create("ssao", GL_R16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->ssaoN, 1);

    myGL->ssaoGaussN = dualN_create("ssao", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->ssaoGaussN, 1);

    myGL->ssrN = singleN_create("ssr", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->ssrN, 1);

    myGL->tonemapExpN = singleN_create("tonemapExp", GL_RGB16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->tonemapExpN, 1);

    myGL->lumaInitN = singleN_create("lumaInit", GL_R16F, 1024, 1024);

    for (int i = 0; i < 2; ++i)
    {
        string conc = "lumaAdapt";
        conc.append(to_string(i));

        myGL->lumaAdaptN[i] = singleN_create(conc, GL_R16F, 1024, 1024);
    }

    bloomBufferCreate();

    return true;
}

void PP::bloomBufferCreate()
{
    myGL->bloomN = singleN_create("bloom", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->bloomN, 1);

    auto bWidth = myGL->width() / 2;
    auto bHeight = myGL->height() / 2;

    for (int i = 0; i < 6; ++i)
    {
        string conc = "down";
        conc.append(to_string(i));

        myGL->downN[i] = singleN_create(conc, GL_RGBA16F, bWidth, bHeight);
        myWin.myGLWidgetSh->up64N(myGL->downN[i], 1);

        conc = "bloomGauss";
        conc.append(to_string(i));

        myGL->bloomGaussN[i] = dualN_create(conc, GL_RGBA16F, bWidth, bHeight);
        myWin.myGLWidgetSh->up64N(myGL->bloomGaussN[i], 1);

        bWidth /= 2;
        bHeight /= 2;
    }
}

AbjNode PP::bgN_create()
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
    };

    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

    for (int i = 0; i < cAttachNum; ++i)
    {
        glTextureStorage2D(setupRTT[i], 1, GL_RGBA16F, myGL->width(), myGL->height());
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glNamedFramebufferTexture(fboNew, DrawBuffers[i], setupRTT[i], 0);
    }

    //DEPTH STEN
    GLuint myDS;
    glCreateTextures(GL_TEXTURE_2D, 1, &myDS);
    glTextureStorage2D(myDS, 1, GL_DEPTH32F_STENCIL8, myGL->width(), myGL->height());
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(myDS, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myDS, 0);
    glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with bgN_create" << endl;

    return { "bg", myGL->width(), myGL->height(), fboNew, setupRTT[0], 0, 0, setupRTT[1], 0, myDS };
}

AbjNode PP::depthRevN_create()
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    auto usableW = myGL->width() / 1.f;
    auto usableH = myGL->height() / 1.f;

    //DEPTH STEN
    GLuint myDS;
    glCreateTextures(GL_TEXTURE_2D, 1, &myDS);
    glTextureStorage2D(myDS, 1, GL_DEPTH32F_STENCIL8, usableW, usableH);
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myDS, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(myDS, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myDS, 0);
    glNamedFramebufferDrawBuffers(fboNew, 0, 0);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with depthRevN_create" << endl;

    return { "depthRev", usableW, usableH, fboNew, 0, 0, 0, 0, 0, myDS };
}

AbjNode PP::tonemapN_create()
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
    };

    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

    for (int i = 0; i < cAttachNum; ++i)
    {
        glTextureStorage2D(setupRTT[i], 1, GL_RGB16F, myGL->width(), myGL->height());
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

    return { "tonemap", myGL->width(), myGL->height(), fboNew, setupRTT[0] , 0, 0, setupRTT[1] };
}

AbjNode PP::singleN_create(string name, GLenum format, int widthIn, int heightIn)
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
    };

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

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
    };

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
    };

    const auto cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

    for (int i = 0; i < cAttachNum; ++i)
    {
        if (i == 0 || i == 1)
            glTextureStorage2D(setupRTT[i], 1, GL_RGBA32F, myGL->width(), myGL->height());

        else
            glTextureStorage2D(setupRTT[i], 1, GL_RGBA32UI, myGL->width(), myGL->height());

        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glNamedFramebufferTexture(fboNew, DrawBuffers[i], setupRTT[i], 0);
    }

    myGL->gBuf0_32 = setupRTT[0];
    myWin.myGLWidgetSh->up64T(myGL->gBuf0_32, myGL->gBuf0_64, 1);

    myGL->gBuf1_32 = setupRTT[1];
    myWin.myGLWidgetSh->up64T(myGL->gBuf1_32, myGL->gBuf1_64, 1);

    myGL->gBuf2_32 = setupRTT[2];
    myWin.myGLWidgetSh->up64T(myGL->gBuf2_32, myGL->gBuf2_64, 1);

    myGL->gBuf3_32 = setupRTT[3];
    myWin.myGLWidgetSh->up64T(myGL->gBuf3_32, myGL->gBuf3_64, 1);

    myGL->gBuf4_32 = setupRTT[4];
    myWin.myGLWidgetSh->up64T(myGL->gBuf4_32, myGL->gBuf4_64, 1);

    myGL->gBuf5_32 = setupRTT[5];
    myWin.myGLWidgetSh->up64T(myGL->gBuf5_32, myGL->gBuf5_64, 1);

    //DEPTH STEN
    glCreateTextures(GL_TEXTURE_2D, 1, &myGL->gBuf_DS_32);
    glTextureStorage2D(myGL->gBuf_DS_32, 1, GL_DEPTH32F_STENCIL8, myGL->width(), myGL->height());
    glTextureParameteri(myGL->gBuf_DS_32, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myGL->gBuf_DS_32, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myGL->gBuf_DS_32, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(myGL->gBuf_DS_32, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myGL->gBuf_DS_32, 0);
    glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "error with gBufN_create" << endl;

    return { "def", myGL->width(), myGL->height(), fboNew };
}

void PP::printFilterTypeD(int filterIn)
{
    if (filterIn == GL_LINEAR)
        cout << "match for GL_LINEAR" << endl;

    else if (filterIn == GL_NEAREST)
        cout << "match for GL_NEAREST" << endl;

    else
        cout << "match for NEITHER" << endl;
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

        myGL->rttGaussIn32 = (i == 0) ? src : dest.tex1_32;
        myWin.myGLWidgetSh->up64T(myGL->rttGaussIn32, myGL->rttGaussIn64, 1);

        myWin.myFSQ->gaussStage = i;
        myWin.myFSQ->render(myGL);
    }

    return dest.tex2_32;
}

void PP::gaussianRecursivePasses(GLuint src, AbjNode dest, int passes, int iter)
{
    myGL->tempGauss = gaussianLinear(src, dest);

    if (iter + 1 < passes)
        gaussianRecursivePasses(myGL->tempGauss, dest, passes, iter + 1);
}

GLuint PP::gaussianBlur(AbjNode src, AbjNode dest, int passes)
{
    if (passes == 0)
        return src.tex1_32;

    gaussianRecursivePasses(src.tex1_32, dest, passes, 0);

    return myGL->tempGauss;
}

GLuint PP::gaussianBlur2(GLuint src, AbjNode dest, int passes)
{
    if (passes == 0)
        return src;

    gaussianRecursivePasses(src, dest, passes, 0);

    return myGL->tempGauss;
}

void PP::downSampRender(AbjNode src, AbjNode dest, int downPixAmt)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo1);
    glViewport(0, 0, dest.width, dest.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myWin.myGLWidgetSh->up64T(src.tex1_32, myGL->downSamp_64, 1);

    myGL->texelSize = glm::vec2(downPixAmt / src.width, downPixAmt / src.height);
    myWin.myFSQ->render(myGL);
}

void PP::bloomRender()
{
    myWin.myGLWidgetSh->glUseProgram2("pDown");

    for (int i = 0; i < 6; ++i) //DOWNSAMP
    {
        if (i == 0)
            downSampRender(myGL->deferredN, myGL->downN[i], 2);

        else
            downSampRender(myGL->downN[i - 1], myGL->downN[i], 2);
    }

    myWin.myGLWidgetSh->glUseProgram2("pGauss");

    for (int i = 0; i < 6; ++i)
        gaussianBlur(myGL->downN[i], myGL->bloomGaussN[i], 4);

    glBindFramebuffer(GL_FRAMEBUFFER, myGL->bloomN.fbo1);
    glViewport(0, 0, myGL->bloomN.width, myGL->bloomN.height);
    glClear(GL_COLOR_BUFFER_BIT);

    myWin.myGLWidgetSh->glUseProgram2("pBloom");
    myWin.myFSQ->render(myGL);
}

void PP::postFX(shared_ptr<GLWidget> myGLin)
{
    myGL = myGLin;

    //SSAO
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->ssaoN.fbo1);
    glViewport(0, 0, myGL->ssaoN.width, myGL->ssaoN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    myWin.myGLWidgetSh->glUseProgram2("pSSAO_" + myWin.myFSQ->ssaoKernel->val_s);
    myWin.myFSQ->render(myGL);

    glMakeTextureHandleNonResidentARB(myGL->ssaoGaussN.tex2_64);
    myWin.myGLWidgetSh->glUseProgram2("pGauss");
    myGL->ssaoGaussN.tex2_64 = glGetTextureHandleARB(gaussianBlur(myGL->ssaoN, myGL->ssaoGaussN, true));
    glMakeTextureHandleResidentARB(myGL->ssaoGaussN.tex2_64);

    //DEFERRED LIGHTING
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->deferredN.fbo1);
    glViewport(0, 0, myGL->deferredN.width, myGL->deferredN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    myWin.myGLWidgetSh->glUseProgram2("pDef");
    myWin.myFSQ->render(myGL);

    //LUMA INIT
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->lumaInitN.fbo1);
    glViewport(0, 0, myGL->lumaInitN.width, myGL->lumaInitN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    myWin.myGLWidgetSh->glUseProgram2("pLumaInit");
    myWin.myFSQ->render(myGL);
    myWin.myGLWidgetSh->up64T(myGL->lumaInitN.tex1_32, myGL->lumaInitN.tex1_64, 1);

    //ADAPT LUMA
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->lumaAdaptN[myGL->currLum].fbo1);
    glViewport(0, 0, myGL->lumaAdaptN[myGL->currLum].width, myGL->lumaAdaptN[myGL->currLum].height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt");
    myWin.myFSQ->render(myGL);
    glGenerateTextureMipmap(myGL->lumaAdaptN[myGL->currLum].tex1_32);
    myWin.myGLWidgetSh->up64T(myGL->lumaAdaptN[myGL->currLum].tex1_32, myGL->lumaAdaptN[myGL->currLum].tex1_64, 1);

    //BLOOM + C
    bloomRender();

    glBindFramebuffer(GL_FRAMEBUFFER, myGL->bloomCN.fbo1);
    glViewport(0, 0, myGL->bloomCN.width, myGL->bloomCN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pBloomC");
    myWin.myFSQ->render(myGL);

    //TONEMAP EXPOSURE
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->tonemapExpN.fbo1);
    glViewport(0, 0, myGL->tonemapExpN.width, myGL->tonemapExpN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt_viz");
    myWin.myFSQ->render(myGL);

    //TONEMAP
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->tonemapN.fbo1);
    glViewport(0, 0, myGL->tonemapN.width, myGL->tonemapN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pTonemap");
    myWin.myFSQ->render(myGL);

    //SSR
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //
    glEnable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->ssrN.fbo1);
    glViewport(0, 0, myGL->ssrN.width, myGL->ssrN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pSSR");
    myWin.myFSQ->render(myGL);
    glDisable(GL_BLEND);

    //FXAA
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->fxaaN.fbo1);
    glViewport(0, 0, myGL->fxaaN.width, myGL->fxaaN.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myGL->texelSize = glm::vec2(1.f / myGL->fxaaN.width, 1.f / myGL->fxaaN.height);
    myWin.myGLWidgetSh->glUseProgram2("pFxaa");
    myWin.myFSQ->render(myGL);

    //FINAL
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, myGL->width(), myGL->height());
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pFinal");
    myWin.myFSQ->render(myGL);

    myGL->currLum = !myGL->currLum;
}

void PP::resizeTexClearMem(shared_ptr<GLWidget> myGLin)
{
    myGL = myGLin;

    //GBUF FBO
    myWin.myGLWidgetSh->up64T(myGL->gBuf0_32, myGL->gBuf0_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->gBuf1_32, myGL->gBuf1_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->gBuf2_32, myGL->gBuf2_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->gBuf3_32, myGL->gBuf3_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->gBuf4_32, myGL->gBuf4_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->gBuf5_32, myGL->gBuf5_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->gBuf_DS_32, myGL->gBuf_DS_64, 0);
    glDeleteFramebuffers(1, &myGL->gBufN.fbo1);

    myWin.myGLWidgetSh->up64N(myGL->alphaGaussN, 0);
    myWin.myGLWidgetSh->up64N(myGL->bloomN, 0);
    myWin.myGLWidgetSh->up64N(myGL->bloomCN, 0);
    myWin.myGLWidgetSh->up64N(myGL->deferredN, 0);
    myWin.myGLWidgetSh->up64N(myGL->fxaaN, 0);
    myWin.myGLWidgetSh->up64N(myGL->brushN, 0);
    myWin.myGLWidgetSh->up64N(myGL->brushBGN, 0);
    myWin.myGLWidgetSh->up64N(myGL->brushTempN, 0);
    myWin.myGLWidgetSh->up64N(myGL->sobelN, 0);
    myWin.myGLWidgetSh->up64N(myGL->eraserN, 0);
    myWin.myGLWidgetSh->up64N(myGL->cursorN, 0);
    myWin.myGLWidgetSh->up64N(myGL->ssaoN, 0);
    myWin.myGLWidgetSh->up64N(myGL->bgN, 0);
    myWin.myGLWidgetSh->up64N(myGL->ssrN, 0);
    myWin.myGLWidgetSh->up64N(myGL->ssaoGaussN, 0);
    myWin.myGLWidgetSh->up64N(myGL->tonemapN, 0);
    myWin.myGLWidgetSh->up64N(myGL->tonemapExpN, 0);
    myWin.myGLWidgetSh->up64N(myGL->lumaInitN, 0);

    for (int i = 0; i < 2; ++i)
        myWin.myGLWidgetSh->up64N(myGL->lumaAdaptN[i], 0);

    myGL->fboReady = false;
}
