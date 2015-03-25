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

#include "PP.h"

PP::PP(MainWin &myWinTemp) : myWin(myWinTemp) {}

bool PP::fboPrep(shared_ptr<GLWidget> myGLin)
{
    myGL = myGLin;

    myGL->main_node = mainNode_create();

    myGL->aoBloomC_node = singleNode_create("bloomC", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->aoBloomC_node, 1);

    myGL->fxaa_node = singleNode_create("fxaa", GL_RGB16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->fxaa_node, 1);

    myGL->ssao_node = singleNode_create("ssao", GL_R16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->ssao_node, 1);

    myGL->ssao_gauss_node = gaussNode_create("ssao", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->ssao_gauss_node, 1);

    myGL->tonemap_node = singleNode_create("tonemap", GL_RGB16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->tonemap_node, 1);

    myGL->tonemap_exposure_node = singleNode_create("tonemap_exposure", GL_RGB16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->tonemap_exposure_node, 1);

    myGL->lumaInit = singleNode_create("lumaInit", GL_R16F, 1024, 1024);

    for (int i = 0; i < 2; ++i)
        myGL->lumaAdapt[i] = singleNode_create(QString::fromStdString("lumaAdapt" + to_string(i)), GL_R16F, 1024, 1024);

    bloomBufferCreate();

    return 1;
}

void PP::bloomBufferCreate()
{
    myGL->bloom_node = singleNode_create("bloom", GL_RGBA16F, myGL->width(), myGL->height());
    myWin.myGLWidgetSh->up64N(myGL->bloom_node, 1);

    int bWidth = myGL->width() / 2;
    int bHeight = myGL->height() / 2;

    for (int i = 0; i < 6; ++i)
    {
        myGL->down_node[i] = singleNode_create(QString::fromStdString("down" + to_string(i)), GL_RGBA16F, bWidth, bHeight);
        myWin.myGLWidgetSh->up64N(myGL->down_node[i], 1);

        myGL->bloom_gauss_node[i] = gaussNode_create(QString::fromStdString("bloom_gauss" + to_string(i)), GL_RGBA16F, bWidth, bHeight);
        myWin.myGLWidgetSh->up64N(myGL->bloom_gauss_node[i], 1);

        bWidth /= 2;
        bHeight /= 2;
    }
}

AbjNode PP::gaussNode_create(QString name, GLenum format, int widthIn, int heightIn)
{
    const int numFBO = 2;

    GLuint gaussNew[numFBO];
    glCreateFramebuffers(numFBO, gaussNew);

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
    };

    const int cAttachNum = arraySize(DrawBuffers);

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

            glNamedFramebufferTexture(gaussNew[i], DrawBuffers[j], setupRTT[i], 0);
            glNamedFramebufferDrawBuffers(gaussNew[i], cAttachNum, DrawBuffers);

            if (glCheckNamedFramebufferStatus(gaussNew[i], GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                qDebug() << "error with gaussNode_create";
        }
    }

    return { name, widthIn, heightIn, gaussNew[0], setupRTT[0], 0, gaussNew[1], setupRTT[1], 0 };
}

AbjNode PP::mainNode_create()
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
    };

    const int cAttachNum = arraySize(DrawBuffers);

    GLuint setupRTT[cAttachNum];

    glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

    for (int i = 0; i < cAttachNum; ++i)
    {
        if (i == 3) //rttP needs to be 32F for ssao
            glTextureStorage2D(setupRTT[i], 1, GL_RGBA32F, myGL->width(), myGL->height());

        else
            glTextureStorage2D(setupRTT[i], 1, GL_RGBA16F, myGL->width(), myGL->height());

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

    myGL->rttC_32 = setupRTT[0];
    myWin.myGLWidgetSh->up64T(myGL->rttC_32, myGL->rttC_64, 1);

    myGL->rttN_32 = setupRTT[1];
    myWin.myGLWidgetSh->up64T(myGL->rttN_32, myGL->rttN_64, 1);

    myGL->rttUV_32 = setupRTT[2];
    myWin.myGLWidgetSh->up64T(myGL->rttUV_32, myGL->rttUV_64, 1);

    myGL->rttP_32 = setupRTT[3];
    myWin.myGLWidgetSh->up64T(myGL->rttP_32, myGL->rttP_64, 1);

    myGL->ssaoMask_32 = setupRTT[4];
    myWin.myGLWidgetSh->up64T(myGL->ssaoMask_32, myGL->ssaoMask_64, 1);

    myGL->rttGiz_32 = setupRTT[5];
    myWin.myGLWidgetSh->up64T(myGL->rttGiz_32, myGL->rttGiz_64, 1);

    myGL->rttGizSide_32 = setupRTT[6];
    myWin.myGLWidgetSh->up64T(myGL->rttGizSide_32, myGL->rttGizSide_64, 1);

    //DEPTH STEN
    glCreateTextures(GL_TEXTURE_2D, 1, &myGL->rttDS_32);

    glTextureStorage2D(myGL->rttDS_32, 1, GL_DEPTH32F_STENCIL8, myGL->width(), myGL->height());
    glTextureParameteri(myGL->rttDS_32, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myGL->rttDS_32, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(myGL->rttDS_32, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(myGL->rttDS_32, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myGL->rttDS_32, 0);
    glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

    myWin.myGLWidgetSh->up64T(myGL->rttDS_32, myGL->rttDS_64, 1);

    if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "error with mainNode_create";

    return { "main", myGL->width(), myGL->height(), fboNew, myGL->rttC_32 };
}

AbjNode PP::singleNode_create(QString name, GLenum format, int widthIn, int heightIn)
{
    GLuint fboNew;
    glCreateFramebuffers(1, &fboNew);

    GLenum DrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0,
    };

    const int cAttachNum = arraySize(DrawBuffers);

    GLuint texNew[cAttachNum];
    glCreateTextures(GL_TEXTURE_2D, cAttachNum, texNew);

    for (int i = 0; i < cAttachNum; ++i)
    {
        int numMip = 1 + floor(log2(glm::max(widthIn, heightIn)));
        glTextureStorage2D(texNew[i], numMip, format, widthIn, heightIn);

        glTextureParameteri(texNew[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texNew[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (name == "lumaInit" || name == "lumaAdapt0" || name == "lumaAdapt1")
        {
            glTextureParameteri(texNew[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(texNew[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        else
        {
            glTextureParameteri(texNew[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(texNew[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//            if (myWin.myFSQ->fxaaBlur->val_b)
//            {
//                glTextureParameteri(texNew[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//                glTextureParameteri(texNew[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            }

//            else
//            {
//                glTextureParameteri(texNew[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//                glTextureParameteri(texNew[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//            }
        }

        glGenerateTextureMipmap(texNew[i]);
        glNamedFramebufferTexture(fboNew, DrawBuffers[i], texNew[i], 0);
        glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

        if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            qDebug() << "error with singleNode_create";
    }

    return { name, widthIn, heightIn, fboNew, texNew[0], 0, 0, 0, 0 };
}

GLuint PP::gaussianLinear(GLuint src, AbjNode dest)
{
    for (int i = 0; i < 2; ++i)
    {
        if (i == 0) glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo1);
        else glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo2);
        glViewport(0, 0, dest.width, dest.height);
        glClear(GL_COLOR_BUFFER_BIT);

        myGL->rttGaussIn32 = (i == 0) ? src : dest.tex1;
        myWin.myGLWidgetSh->up64T(myGL->rttGaussIn32, myGL->rttGaussIn64, 1);

        myWin.myFSQ->gaussStage = i;
        myWin.myFSQ->render(myGL);
    }

    return dest.tex2;
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
        return src.tex1;

    gaussianRecursivePasses(src.tex1, dest, passes, 0);

    return myGL->tempGauss;
}

void PP::downSampRender(AbjNode src, AbjNode dest, int downPixAmt)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dest.fbo1);
    glViewport(0, 0, dest.width, dest.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myWin.myGLWidgetSh->up64T(src.tex1, myGL->downSamp_64, 1);

    myGL->texelSize = glm::vec2(downPixAmt / src.width, downPixAmt / src.height);
    myWin.myFSQ->render(myGL);
}

void PP::bloomRender()
{
    myWin.myGLWidgetSh->glUseProgram2("pDown");

    for (int i = 0; i < 6; ++i) //DOWNSAMP
    {
        if (i == 0) downSampRender(myGL->main_node, myGL->down_node[i], 2);
        else downSampRender(myGL->down_node[i - 1], myGL->down_node[i], 2);
    }

    myWin.myGLWidgetSh->glUseProgram2("pGauss");

    for (int i = 0; i < 6; ++i)
        gaussianBlur(myGL->down_node[i], myGL->bloom_gauss_node[i], 4);

    glBindFramebuffer(GL_FRAMEBUFFER, myGL->bloom_node.fbo1);
    glViewport(0, 0, myGL->bloom_node.width, myGL->bloom_node.height);
    glClear(GL_COLOR_BUFFER_BIT);

    myWin.myGLWidgetSh->glUseProgram2("pBloom");
    myWin.myFSQ->render(myGL);
}

void PP::postFX(shared_ptr<GLWidget> myGLin)
{
    myGL = myGLin;

    //luma init
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->lumaInit.fbo1);
    glViewport(0, 0, myGL->lumaInit.width, myGL->lumaInit.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    myWin.myGLWidgetSh->glUseProgram2("pLumaInit");
    myWin.myFSQ->render(myGL);
    myWin.myGLWidgetSh->up64T(myGL->lumaInit.tex1, myGL->lumaInit.tex1_64, 1);

    //adapt luma
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->lumaAdapt[myGL->currLum].fbo1);
    glViewport(0, 0, myGL->lumaAdapt[myGL->currLum].width, myGL->lumaAdapt[myGL->currLum].height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt");
    myWin.myFSQ->render(myGL);
    glGenerateTextureMipmap(myGL->lumaAdapt[myGL->currLum].tex1);
    myWin.myGLWidgetSh->up64T(myGL->lumaAdapt[myGL->currLum].tex1, myGL->lumaAdapt[myGL->currLum].tex1_64, 1);

    //SSAO
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->ssao_node.fbo1);
    glViewport(0, 0, myGL->ssao_node.width, myGL->ssao_node.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    myWin.myGLWidgetSh->glUseProgram2("pSSAO");
    myWin.myFSQ->render(myGL);

    glMakeTextureHandleNonResidentARB(myGL->ssao_gauss_node.tex2_64);
    myWin.myGLWidgetSh->glUseProgram2("pGauss");
    myGL->ssao_gauss_node.tex2_64 = glGetTextureHandleARB(gaussianBlur(myGL->ssao_node, myGL->ssao_gauss_node, myWin.myFSQ->ssaoBlur->val_b));
    glMakeTextureHandleResidentARB(myGL->ssao_gauss_node.tex2_64);

    //BLOOM + C COMBINE
    bloomRender();

    glBindFramebuffer(GL_FRAMEBUFFER, myGL->aoBloomC_node.fbo1);
    glViewport(0, 0, myGL->aoBloomC_node.width, myGL->aoBloomC_node.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pAoBloomC");
    myWin.myFSQ->render(myGL);

    //TONEMAP EXPOSURE
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->tonemap_exposure_node.fbo1);
    glViewport(0, 0, myGL->tonemap_exposure_node.width, myGL->tonemap_exposure_node.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pLumaAdapt_viz");
    myWin.myFSQ->render(myGL);

    //TONEMAP
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->tonemap_node.fbo1);
    glViewport(0, 0, myGL->tonemap_node.width, myGL->tonemap_node.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myWin.myGLWidgetSh->glUseProgram2("pTonemap");
    myWin.myFSQ->render(myGL);

    //FXAA
    glBindFramebuffer(GL_FRAMEBUFFER, myGL->fxaa_node.fbo1);
    glViewport(0, 0, myGL->fxaa_node.width, myGL->fxaa_node.height);
    glClear(GL_COLOR_BUFFER_BIT);
    myGL->texelSize = glm::vec2(1.f / myGL->fxaa_node.width, 1.f / myGL->fxaa_node.height);
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

    //MAIN FBO
    myWin.myGLWidgetSh->up64T(myGL->rttC_32, myGL->rttC_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->rttN_32, myGL->rttN_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->rttUV_32, myGL->rttUV_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->rttP_32, myGL->rttP_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->ssaoMask_32, myGL->ssaoMask_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->rttGiz_32, myGL->rttGiz_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->rttGizSide_32, myGL->rttGizSide_64, 0);
    myWin.myGLWidgetSh->up64T(myGL->rttDS_32, myGL->rttDS_64, 0);
    glDeleteFramebuffers(1, &myGL->main_node.fbo1);

    myWin.myGLWidgetSh->up64N(myGL->bloom_node, 0);
    myWin.myGLWidgetSh->up64N(myGL->aoBloomC_node, 0);
    myWin.myGLWidgetSh->up64N(myGL->fxaa_node, 0);
    myWin.myGLWidgetSh->up64N(myGL->ssao_node, 0);
    myWin.myGLWidgetSh->up64N(myGL->ssao_gauss_node, 0);
    myWin.myGLWidgetSh->up64N(myGL->tonemap_node, 0);
    myWin.myGLWidgetSh->up64N(myGL->tonemap_exposure_node, 0);

    //JMP
    myWin.myGLWidgetSh->up64N(myGL->lumaInit, 0);

    for (int i = 0; i < 2; ++i)
        myWin.myGLWidgetSh->up64N(myGL->lumaAdapt[i], 0);

    myGL->fboReady = 0;
}
