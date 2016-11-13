/*

Copyright 2016 Aleks Berg-Jones

This file is part of Shadows Spider.

Shadows Spider is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Shadows Spider is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Shadows Spider.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "GL_render.h"

void render_frame(Abj &myAbj)
{
	cout << "zTgl = " << myAbj.zTgl << endl;

	/////////////

	if (myAbj.mpfTgl)
		mpfTimerStart(myAbj);

	if (!myAbj.fboReady)
		myAbj.fboReady = fboPrep(myAbj);

	/* MVPGET / LOAD VAO */
	for (auto &i : myAbj.allObj)
	{
		//if (i->type != "SELRECT" && i->type != "TXT")
		{
			//if (!i->gizSideObj && i->name->val_s != "pivot")
			{
				if (searchUp(i, myAbj))
				{
					//cout << "setting MVP for " << i->name->val_s << endl;
					i->mvpGet();
				}
			}
		}

		if (i->dupeStenFix != "999")
			dupeStenFix_check(i, myAbj);

		i->VAO_load();
	}

	///* FRUSTUM CULL */
	//myAbj.frustumObjs.clear();

	//for (auto &i : myAbj.allObj)
	//{
	//	if (i->type != "SELRECT" && i->type != "TXT" && searchUp(i, myAbj))
	//	{
	//		//cout << "i->type / name frustumObjs 0 = " << i->type << " " << i->name->val_s << endl;

	//		if (i->isAABBinFrustum())
	//			myAbj.frustumObjs.push_back(i);
	//	}
	//}


	/* FRUSTUM CULL */
	bool useFrustum = 0;
	myAbj.frustumObjs.clear();

	for (auto &i : myAbj.allObj)
	{
		if (i->type != "SELRECT" && i->type != "TXT" && searchUp(i, myAbj))
		{
			//cout << "i->type / name frustumObjs 0 = " << i->type << " " << i->name->val_s << endl;

			if (useFrustum)
			{
				if (i->isAABBinFrustum())
					myAbj.frustumObjs.push_back(i);
			}

			else
				myAbj.frustumObjs.push_back(i);
		}
	}

	//cout << "# objs in frustum = " << myAbj.frustumObjs.size() << " //// " << endl;
	//for (auto &i : myAbj.frustumObjs)
	//{
	//	cout << "obj in frustum is : " << i->name->val_s << endl;
	//}

	/* SHADOWMAP */
	glUseProgram2("pShadow", myAbj);

	for (auto &i : myAbj.allCamLi)
	{
		if (i->v->val_b || i == myAbj.selCamLi)
		{
			if (i->dirtyVM)
				VMup(i, myAbj);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			if (i->dirtyShadow && i->camLiTypeGet("light"))
			{
				//cout << "dirtyShadow for " << i->name->val_s << endl;
				writeShadow(i, myAbj);
			}
		}
	}

	//if (myAbj.UBO_light_needsUp) //
		//uboUp(myAbj);

	uboUp(myAbj);

	//removed from GL core profile but necc for UBO?
	//glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
	//glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
	glEnableClientState(GL_UNIFORM_BUFFER_UNIFIED_NV);
	 
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.bgN.fbo0);
	glViewport(0, 0, myAbj.width, myAbj.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLenum DrawBuffersBG_0[] = { GL_COLOR_ATTACHMENT0 };
	GLenum DrawBuffersBG_1[] = { GL_COLOR_ATTACHMENT1 };

	glNamedFramebufferDrawBuffers(myAbj.bgN.fbo0, 1, DrawBuffersBG_0);

	glDisable(GL_DEPTH_TEST);
	glUseProgram2("pSky", myAbj);
	myAbj.myFSQ->render();
	glEnable(GL_DEPTH_TEST);

	if (myAbj.selCamLi->gridV)
	{
		glUseProgram2("pGrid", myAbj);

		for (auto &i : myAbj.allGrid)
		{
			if (i->type == "GRID" && gridMatch(i, myAbj))
				i->render();
		}
	}

	glNamedFramebufferDrawBuffers(myAbj.bgN.fbo0, 1, DrawBuffersBG_1);

	glUseProgram2("pGiz", myAbj);
	if (myAbj.gizSideTgl)
	{
		glViewport(0, 0, (int)myAbj.gizSideS.x, (int)myAbj.gizSideS.y);

		for (auto &i : myAbj.allGizSide)
		{
			i->mvpGet();
			i->render();
		}

		glViewport(0, 0, myAbj.width, myAbj.height);
	}

	if (myAbj.myPivot->v->val_b)
	{
		myAbj.myPivot->mvpGet();
		myAbj.myPivot->render();
	}

	for (auto &i : myAbj.allCamLi)
	{
		if (searchUp(i, myAbj))
		{
			if (i->selected)
			{
				if (!i->bb->val_b && myAbj.selMode == "OBJ")
				{
					//wireframe already, change color only
					auto Cgiz_temp = i->Cgiz;
					i->Cgiz = myAbj.Csel;
					i->render();
					i->Cgiz = Cgiz_temp;
				}
			}

			else
			{
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				i->render();
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			}
		}
	}

	for (auto &i : myAbj.allGiz)
	{
		if (!i->gizSideObj && searchUp(i, myAbj))
		{
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			i->render();
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
	}

	if (myAbj.myGizNull->v->val_b && myAbj.myGizNull->gizType == "R")
	{
		glUseProgram2("pGiz_circ", myAbj);

		for (auto &i : myAbj.allGiz)
		{
			if (i->type == "GIZ_CIRC" || i->type == "GIZ_CIRC_HALF")
			{
				glDisable(GL_DEPTH_TEST);
				i->render();
				glEnable(GL_DEPTH_TEST);
			}
		}
	}

	glUseProgram2("pBB", myAbj);

	//for (auto &i : myAbj.frustumObjs)
	//{
	//	if (i->bb->val_b)
	//		i->render();
	//}

	for (auto &i : myAbj.frustumObjs)
	{
		if (i->bb->val_b)
			i->render();
	}

	for (auto &i : myAbj.frustumObjs)
	{
		if (i->type == "OBJ" && !i->bb->val_b)
		{
			if (myAbj.wireOverTgl)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				glUseProgram2("pWireframe", myAbj);
				i->render();

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}

	for (auto &i : myAbj.frustumObjs)
	{
		if (i->selected && i->type == "OBJ" && !i->bb->val_b)
		{
			if (myAbj.selMode == "OBJ")
			{
				glClearStencil(0);
				glClear(GL_STENCIL_BUFFER_BIT);
				glEnable(GL_STENCIL_TEST);

				/* RENDER MESH */
				glStencilFunc(GL_ALWAYS, 1, 0xFF); // -1
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				glUseProgram2("pStencilGeo", myAbj);

				if (i->twoSided->val_s != "OFF")
					glDisable(GL_CULL_FACE);

				i->render();

				/* THICK WIRE VERSION */
				glStencilFunc(GL_NOTEQUAL, 1, -1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glLineWidth(7.f);

				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glUseProgram2("pStencilHi", myAbj);
				i->render();
				glDisable(GL_STENCIL_TEST);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glLineWidth(1.f);
			}
		}
	}

	//REVERSE DEPTH
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.depthRevN.fbo0);
	glViewport(0, 0, myAbj.depthRevN.width, myAbj.depthRevN.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDisable(GL_BLEND);

	glUseProgram2("pDepthRev", myAbj);

	for (auto &i : myAbj.frustumObjs)
	//for (auto &i : myAbj.allObj)
	{
		if (i->type == "OBJ" && !i->bb->val_b)
		{
			if (i->Ko->val_f == 1.f) // (PARTIALLY) OPAQUE OBJECTS
			{
				if (i->twoSided->val_s != "OFF")
					glDisable(GL_CULL_FACE);

				i->render();
				glEnable(GL_CULL_FACE);
			}
		}
	}

	GBuffer_BOIT(myAbj);

	if (myAbj.brushOutlineUpB)
		brushOutlineUp(myAbj);

	/* COMP */
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	postFX(myAbj);
	overlay2D(myAbj);
}

void GBuffer_BOIT(Abj &myAbj)
{
	/* OPAQUE PASS */
	/* ALPHA TESTING in opaque pass shader (for alpha == 1.f) */
	//if (col.a <= threshold)
	//    discard;

	/* G BUFFER */
	GLenum DrawBuffersOpaque[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
	};

	const auto cAttachNumO = arraySize(DrawBuffersOpaque);
	glNamedFramebufferDrawBuffers(myAbj.gBufN.fbo0, cAttachNumO, DrawBuffersOpaque);

	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.gBufN.fbo0);
	glViewport(0, 0, myAbj.gBufN.width, myAbj.gBufN.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);

	glUseProgram2("pGBuffer", myAbj);

	for (auto &i : myAbj.frustumObjs)
	//for (auto &i : myAbj.allObj)
	{
		if (i->type == "OBJ" && !i->bb->val_b)
		{
			if (i->Ko->val_f == 1.f) // (PARTIALLY) OPAQUE OBJECTS
			{
				if (i->twoSided->val_s != "OFF")
					glDisable(GL_CULL_FACE);

				i->render();
				glEnable(GL_CULL_FACE);
			}
		}
	}

	///* TRANSPARENT PASS */
	GLenum DrawBuffersTransp[] = { GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
	glNamedFramebufferDrawBuffers(myAbj.gBufN.fbo0, 2, DrawBuffersTransp);

	glDepthMask(GL_FALSE); //read depth from opaque pass but dont write to it

	float clearColor0[4] = { 0.f, 0.f, 0.f, 0.f };
	float clearColor1[4] = { 1.f, 1.f, 1.f, 1.f };
	glClearNamedFramebufferfv(myAbj.gBufN.fbo0, GL_COLOR, 0, clearColor0);
	glClearNamedFramebufferfv(myAbj.gBufN.fbo0, GL_COLOR, 1, clearColor1);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

	glUseProgram2("pTransp", myAbj);

	for (auto &i : myAbj.frustumObjs)
	{
		if (i->type == "OBJ" && !i->bb->val_b)
		{
			if (i->Ko->val_f < 1.f)
			{
				myAbj.BOIT_name_D = i->name->val_s;
				i->render();
			}
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void postFX(Abj &myAbj)
{
	//glBufferAddressRangeNV(GL_UNIFORM_BUFFER_ADDRESS_NV, 0, myAbj.uboDebug_64, sizeof(DebugData)); // !!!!!!!

	/* SSAO */
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.ssaoN.fbo0);
	glViewport(0, 0, myAbj.ssaoN.width, myAbj.ssaoN.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram2("pSSAO_" + myAbj.myFSQ->ssaoKernel->val_s, myAbj);
	myAbj.myFSQ->render();

	glMakeTextureHandleNonResidentARB(myAbj.ssaoGaussN.tex1_64);
	glUseProgram2("pGauss", myAbj);
	myAbj.ssaoGaussN.tex1_64 = glGetTextureHandleARB(gaussianBlur(myAbj.ssaoN, myAbj.ssaoGaussN, true, myAbj));
	glMakeTextureHandleResidentARB(myAbj.ssaoGaussN.tex1_64);

	/* DEFERRED LIGHTING */
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.deferredN.fbo0);
	glViewport(0, 0, myAbj.deferredN.width, myAbj.deferredN.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram2("pDef", myAbj);
	myAbj.myFSQ->render();

	/* TRANSP COMPOSITE */
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	glUseProgram2("pTranspComp", myAbj);
	myAbj.myFSQ->render();
	glDisable(GL_BLEND);

	/* LUMA INIT */
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.lumaInitN.fbo0);
	glViewport(0, 0, myAbj.lumaInitN.width, myAbj.lumaInitN.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram2("pLumaInit", myAbj);
	myAbj.myFSQ->render();
	up64T(myAbj.lumaInitN.tex0_32, myAbj.lumaInitN.tex0_64, 1);

	/* ADAPT LUMA */
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.lumaAdaptN[myAbj.currLum].fbo0);
	glViewport(0, 0, myAbj.lumaAdaptN[myAbj.currLum].width, myAbj.lumaAdaptN[myAbj.currLum].height);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram2("pLumaAdapt", myAbj);
	myAbj.myFSQ->render();
	glGenerateTextureMipmap(myAbj.lumaAdaptN[myAbj.currLum].tex0_32);
	up64T(myAbj.lumaAdaptN[myAbj.currLum].tex0_32, myAbj.lumaAdaptN[myAbj.currLum].tex0_64, 1);

	/* BLOOM + C */
	bloomRender(myAbj);

	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.bloomCN.fbo0);
	glViewport(0, 0, myAbj.bloomCN.width, myAbj.bloomCN.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram2("pBloomC", myAbj);
	myAbj.myFSQ->render();

	/* TONEMAP */
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.tonemapN.fbo0);
	glViewport(0, 0, myAbj.tonemapN.width, myAbj.tonemapN.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram2("pTonemap", myAbj);
	myAbj.myFSQ->render();

	/* FXAA */
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.fxaaN.fbo0);
	glViewport(0, 0, myAbj.fxaaN.width, myAbj.fxaaN.height);
	glClear(GL_COLOR_BUFFER_BIT);
	myAbj.texelSize = glm::vec2(1.f / myAbj.fxaaN.width, 1.f / myAbj.fxaaN.height);
	glUseProgram2("pFxaa", myAbj);
	myAbj.myFSQ->render();

	/* SSR */
	glBindFramebuffer(GL_FRAMEBUFFER, myAbj.ssrN.fbo0);
	glViewport(0, 0, myAbj.ssrN.width, myAbj.ssrN.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //
	glUseProgram2("pSSR", myAbj);
	myAbj.myFSQ->render();
	glDisable(GL_BLEND);

	/* FINAL */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, myAbj.width, myAbj.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram2("pFinal", myAbj);
	myAbj.myFSQ->render();

	myAbj.currLum = !myAbj.currLum;
}

void overlay2D(Abj &myAbj)
{
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	if (myAbj.rezGateTgl || myAbj.rezGateTgl_sel) //REZGATE
	{
		glm::vec2 center(myAbj.width / 2, myAbj.height / 2);
		glm::vec2 xyHalf(myAbj.width / 2, myAbj.height / 2);

		myAbj.rezGate_LD = toNDC(center - xyHalf, myAbj, "REG");
		myAbj.rezGate_RU = toNDC(center + xyHalf, myAbj, "REG");
	}

	if (!myAbj.mmbTgl & myAbj.lmbTgl && !myAbj.altTgl && !myAbj.disableSelRect && myAbj.selCamLi->camLiType->val_s != "FPS") //SELRECT
	{
		myAbj.selRect_LD = toNDC(myAbj.rayP, myAbj, "SELRECT");
		myAbj.selRect_RU = toNDC(myAbj.pMouseNew, myAbj, "SELRECT");

		glUseProgram2("pSelRect", myAbj);
		myAbj.mySelRect->render();
	}

	//TXT
	if (myAbj.mpfTgl || myAbj.gizSpaceTgl || myAbj.statsTgl)
	{
		glUseProgram2("pTxt", myAbj);

		if (myAbj.mpfTgl)
		{
			myAbj.myTxt->txt2D = myAbj.mpf.c_str();

			for (auto &i : myAbj.GLDataSh)
			{
				if (i.obj == myAbj.myTxt)
					glNamedBufferSubData(i.VBO_P, 0, (GLsizei)strlen(myAbj.myTxt->txt2D), myAbj.myTxt->txt2D);
			}

			myAbj.myTxt->txtOrigin = glm::vec2(.8f, -.9f);
			myAbj.myTxt->render();
		}

		if (myAbj.gizSpaceTgl) //gizSpace
		{
			if (myAbj.gizSpace == "local") myAbj.myTxt->txt2D = "l";
			else if (myAbj.gizSpace == "world") myAbj.myTxt->txt2D = "w";

			for (auto &i : myAbj.GLDataSh)
			{
				if (i.obj == myAbj.myTxt)
					glNamedBufferSubData(i.VBO_P, 0, (GLsizei)strlen(myAbj.myTxt->txt2D), myAbj.myTxt->txt2D);
			}

			myAbj.myTxt->txtOrigin = glm::vec2(.95f, -.9f);
			myAbj.myTxt->render();
		}

		if (myAbj.statsTgl)
		{
			auto vertsOnObj = 0; auto edgesOnObj = 0; auto facesOnObj = 0;

			for (auto &i : myAbj.allObj)
			{
				if (i->selected)
				{
					vertsOnObj += i->vertsOnObj;
					//edgesOnObj += i->edgesOnObj;
					//facesOnObj += i->facesOnObj;
				}
			}

			stringstream ss;
			ss << "v " << vertsOnObj << " e " << edgesOnObj << " f " << facesOnObj;

			auto selStats = ss.str();
			myAbj.myTxt->txt2D = selStats.c_str();

			for (auto &i : myAbj.GLDataSh)
			{
				if (i.obj == myAbj.myTxt)
					glNamedBufferSubData(i.VBO_P, 0, (GLsizei)strlen(myAbj.myTxt->txt2D), myAbj.myTxt->txt2D);
			}

			myAbj.myTxt->txtOrigin = glm::vec2(.5f, .85f);
			myAbj.myTxt->render();
		}
	}
}

bool gridMatch(shared_ptr<Object> obj, Abj &myAbj)
{
	auto found = false;
	auto gridName = obj->name->val_s;
	auto orthoType = myAbj.selCamLi->orthoType->val_s;

	if (myAbj.selCamLi->camLiType->val_s != "ORTHO" || myAbj.selCamLi->orthoFree->val_b)
	{
		if (gridName == "grid_top")
			found = 1;

		else if (gridName == "grid_topB")
			found = 1;
	}

	else
	{
		if ((gridName == "grid_front" && (orthoType == "BACK" || orthoType == "FRONT"))
			|| (gridName == "grid_left" && (orthoType == "LEFT" || orthoType == "RIGHT"))
			|| (gridName == "grid_top" && (orthoType == "BOTTOM" || orthoType == "TOP")))

			found = 1;

		else if ((gridName == "grid_frontB" && (orthoType == "BACK" || orthoType == "FRONT"))
			|| (gridName == "grid_leftB" && (orthoType == "LEFT" || orthoType == "RIGHT"))
			|| (gridName == "grid_topB" && (orthoType == "BOTTOM" || orthoType == "TOP")))

			found = 1;
	}

	return found;
}

void dupeStenFix_check(shared_ptr<Object> obj, Abj &myAbj)
{
	for (auto &i : myAbj.allObj)
	{
		if (obj->v->val_b && obj->dupeStenFix == i->name->val_s)
		{
			if (obj->MM == i->MM)
				i->v->val_b = false;

			else
			{
				obj->dupeStenFix = "999";
				i->v->val_b = true;
			}
		}
	}
}

void resize(Abj &myAbj)
{
	glViewport(0, 0, myAbj.width, myAbj.height);
	myAbj.aspect = (0.f) ? 1.f : (float)myAbj.width / myAbj.height;
	myAbj.aspectSide = (0.f) ? 1.f : myAbj.gizSideS.x / myAbj.gizSideS.y;

	if (myAbj.selCamLi->camLiTypeGet("dirOrtho"))
		myAbj.selCamLi->PM = glm::ortho(-myAbj.selCamLi->orthoZoom->val_f * myAbj.aspect, myAbj.selCamLi->orthoZoom->val_f * myAbj.aspect, -myAbj.selCamLi->orthoZoom->val_f, myAbj.selCamLi->orthoZoom->val_f, myAbj.selCamLi->nearClip->val_f, myAbj.selCamLi->farClip->val_f);

	else
		myAbj.selCamLi->PM = glm::perspective(glm::radians(myAbj.selCamLi->fov->val_f), myAbj.aspect, myAbj.selCamLi->nearClip->val_f, myAbj.selCamLi->farClip->val_f);

	myAbj.PMinv_cube = glm::inverse(glm::perspective(glm::radians(myAbj.selCamLi->fov->val_f), myAbj.aspect, myAbj.selCamLi->nearClip->val_f, myAbj.selCamLi->farClip->val_f));
	myAbj.PMgizSide = glm::perspective(glm::radians(50.f), myAbj.aspectSide, .001f, 1000.f);

	myAbj.aspectXYZ.x = (myAbj.aspect > 1.f) ? myAbj.aspect : 1.f;
	myAbj.aspectXYZ.y = (myAbj.aspect > 1.f) ? 1.f : myAbj.aspect;
	myAbj.aspectXYZ.z = 1.f;

	if (myAbj.fboReady)
		resizeTexClearMem(myAbj);

	myAbj.selCamLi->setDirty();
}

void PMupOrtho(Abj &myAbj)
{
	//refresh PM because of selCamLi orthoZoom change
	myAbj.aspect = (float)myAbj.width / myAbj.height;
	myAbj.aspectSide = myAbj.gizSideS.x / myAbj.gizSideS.y;

	if (myAbj.aspect == 0.f)
		myAbj.aspect = 1.f;

	if (myAbj.aspectSide == 0.f)
		myAbj.aspectSide = 1.f;

	myAbj.selCamLi->PM = glm::ortho(-myAbj.selCamLi->orthoZoom->val_f * myAbj.aspect, myAbj.selCamLi->orthoZoom->val_f * myAbj.aspect, -myAbj.selCamLi->orthoZoom->val_f, myAbj.selCamLi->orthoZoom->val_f, myAbj.selCamLi->nearClip->val_f, myAbj.selCamLi->farClip->val_f);

	myAbj.selCamLi->buildFrustumPlanes();
}

void VMup(shared_ptr<Object> obj, Abj &myAbj)
{
	glm::mat4 R = glm::yawPitchRoll(glm::radians(obj->r->val_3.x), glm::radians(obj->r->val_3.y), 0.f);

	if (obj->camLiType->val_s == "FPS")
	{
		obj->piv->val_3 = obj->t->val_3;

		obj->lookFPS = glm::vec3(R * glm::vec4(0.f, 0.f, 1.f, 0.f));
		obj->upFPS = glm::vec3(R * glm::vec4(0.f, 1.f, 0.f, 0.f));
		obj->rightFPS = glm::cross(obj->lookFPS, obj->upFPS);

		obj->VM = glm::lookAt(obj->t->val_3, obj->t->val_3 + obj->lookFPS, obj->upFPS);
	}

	else if (obj->camLiType->val_s != "DIR")
	{
		obj->t->val_3 = glm::vec3(R * glm::vec4(0.f, 0.f, obj->distO, 0.f)) + obj->targO;
		obj->piv->val_3 = obj->t->val_3;

		obj->lookO = glm::normalize(obj->targO - obj->t->val_3);
		obj->rightO = glm::normalize(glm::cross(obj->lookO, myAbj.upWorld));
		obj->upO = glm::cross(obj->rightO, obj->lookO);

		obj->VM = glm::lookAt(obj->t->val_3, obj->targO, obj->upO);
		obj->VM *= glm::rotate(glm::mat4(), glm::radians(obj->r->val_3.z), glm::vec3(0.f, 0.f, 1.f));
	}

	//update VMgizSide if this object is the selcam
	if (obj == myAbj.selCamLi)
	{
		// UPDATE VM for GIZSIDE
		auto R = glm::yawPitchRoll(glm::radians(obj->r->val_3.x), glm::radians(obj->r->val_3.y), 0.f);
		auto posG = glm::vec3(R * glm::vec4(0.f, 0.f, myAbj.gizSideS.z, 0.f));
		auto lookG = glm::normalize(-posG);
		auto rightG = glm::cross(lookG, myAbj.upWorld);
		auto upG = glm::cross(rightG, lookG);

		if (obj->camLiType->val_s == "FPS") posG *= -1.f;
		myAbj.VMgizSide = glm::lookAt(posG, glm::vec3(0.f), upG);
	}

	obj->dirtyVM = false;

	if (obj->camLiTypeGet("light"))
		myAbj.UBO_light_needsUp = true;

	if (myAbj.myFSQ->clearBehav->val_s == "OnVMup")
	{
		cout << "baking (and clearing) canvas on VMup" << endl;

		myAbj.bakeNow = true;
		//clearCanvas();
	}
	
	myAbj.selCamLi->buildFrustumPlanes();
}

void mpfTimerStart(Abj &myAbj) //DEBUG - move to chrono in its own struct
{
	//120fps = 8.333
	//60fps = 16.667
	//30fps = 33.333

	myAbj.tick_frames++;
	myAbj.tick_new = GetTickCount();
	myAbj.tick_diff = myAbj.tick_new - myAbj.tick_old;

	if (myAbj.tick_diff >= 1000)
	{
		stringstream ss;
		ss << fixed << setprecision(3) << 1000.f / myAbj.tick_frames;
		myAbj.mpf = ss.str();

		myAbj.mpfTime = 1000.f / myAbj.tick_frames;
		myAbj.dTime = myAbj.mpfTime * myAbj.myFSQ->adaptTime->val_f * .01f;

		myAbj.tick_frames = 0;
		myAbj.tick_old = myAbj.tick_new;
	}
}

bool fboPrep(Abj &myAbj)
{
	myAbj.gBufN = gBufN_create(myAbj);

	myAbj.depthRevN = depthRevN_create(myAbj.width, myAbj.height);
	up64N(myAbj.depthRevN, 1);

	myAbj.bgN = bgN_create(myAbj.width, myAbj.height); //giz / grid / sky
	up64N(myAbj.bgN, 1);

	myAbj.tonemapN = tonemapN_create(myAbj);
	up64N(myAbj.tonemapN, 1);

	myAbj.alphaGaussN = dualN_create("alphaGauss", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.alphaGaussN, 1);

	myAbj.alphaGaussN = dualN_create("alphaGauss", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.alphaGaussN, 1);

	myAbj.bloomCN = singleN_create("bloomC", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.bloomCN, 1);

	myAbj.deferredN = singleN_create("def", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.deferredN, 1);

	myAbj.fxaaN = singleN_create("fxaa", GL_RGB16F, myAbj.width, myAbj.height);
	up64N(myAbj.fxaaN, 1);

	myAbj.brushN = dualN_create("brush", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.brushN, 1);

	myAbj.brushBGN = dualN_create("brushBG", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.brushBGN, 1);

	myAbj.brushTempN = dualN_create("brushTemp", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.brushTempN, 1);

	myAbj.eraserN = dualN_create("eraser", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.eraserN, 1);

	myAbj.cursorN = singleN_create("cursor", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.cursorN, 1);

	myAbj.sobelN = singleN_create("sobel", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.sobelN, 1);

	myAbj.ssaoN = singleN_create("ssao", GL_R16F, myAbj.width, myAbj.height);
	up64N(myAbj.ssaoN, 1);

	myAbj.ssaoGaussN = dualN_create("ssaoGauss", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.ssaoGaussN, 1);

	myAbj.ssrN = singleN_create("ssr", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.ssrN, 1);

	myAbj.lumaInitN = singleN_create("lumaInit", GL_R16F, 1024, 1024);

	for (int i = 0; i < 2; ++i)
	{
		string conc = "lumaAdapt";
		conc.append(to_string(i));

		myAbj.lumaAdaptN[i] = singleN_create(conc, GL_R16F, 1024, 1024);
	}

	bloomBufferCreate(myAbj);

	return true;
}

AbjNode gBufN_create(Abj &myAbj)
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
			glTextureStorage2D(setupRTT[i], 1, GL_RGBA32F, myAbj.width, myAbj.height);

		else if (i == 6)
			glTextureStorage2D(setupRTT[i], 1, GL_RGBA16F, myAbj.width, myAbj.height);

		else if (i == 7)
			glTextureStorage2D(setupRTT[i], 1, GL_R16F, myAbj.width, myAbj.height);

		else
			glTextureStorage2D(setupRTT[i], 1, GL_RGBA32UI, myAbj.width, myAbj.height);

		glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glNamedFramebufferTexture(fboNew, DrawBuffers[i], setupRTT[i], 0);
	}

	myAbj.gBuf0_32 = setupRTT[0];
	up64T(myAbj.gBuf0_32, myAbj.gBuf0_64, 1);

	myAbj.gBuf1_32 = setupRTT[1];
	up64T(myAbj.gBuf1_32, myAbj.gBuf1_64, 1);

	myAbj.gBuf2_32 = setupRTT[2];
	up64T(myAbj.gBuf2_32, myAbj.gBuf2_64, 1);

	myAbj.gBuf3_32 = setupRTT[3];
	up64T(myAbj.gBuf3_32, myAbj.gBuf3_64, 1);

	myAbj.gBuf4_32 = setupRTT[4];
	up64T(myAbj.gBuf4_32, myAbj.gBuf4_64, 1);

	myAbj.gBuf5_32 = setupRTT[5];
	up64T(myAbj.gBuf5_32, myAbj.gBuf5_64, 1);

	myAbj.gBuf6_32 = setupRTT[6];
	up64T(myAbj.gBuf6_32, myAbj.gBuf6_64, 1);

	myAbj.gBuf7_32 = setupRTT[7];
	up64T(myAbj.gBuf7_32, myAbj.gBuf7_64, 1);

	//DEPTH STEN
	glCreateTextures(GL_TEXTURE_2D, 1, &myAbj.gBuf_DS_32);
	glTextureStorage2D(myAbj.gBuf_DS_32, 1, GL_DEPTH32F_STENCIL8, myAbj.width, myAbj.height);
	glTextureParameteri(myAbj.gBuf_DS_32, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(myAbj.gBuf_DS_32, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(myAbj.gBuf_DS_32, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(myAbj.gBuf_DS_32, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myAbj.gBuf_DS_32, 0);
	glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

	if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "error with gBufN_create" << endl;

	return { "def", myAbj.width, myAbj.height, fboNew };
}

AbjNode depthRevN_create(int width, int height)
{
	GLuint fboNew;
	glCreateFramebuffers(1, &fboNew);

	//DEPTH STEN
	GLuint myDS;
	glCreateTextures(GL_TEXTURE_2D, 1, &myDS);
	glTextureStorage2D(myDS, 1, GL_DEPTH32F_STENCIL8, width, height);
	glTextureParameteri(myDS, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(myDS, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(myDS, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(myDS, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glNamedFramebufferTexture(fboNew, GL_DEPTH_ATTACHMENT, myDS, 0);
	glNamedFramebufferDrawBuffer(fboNew, GL_NONE);

	if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "error with depthRevN_create" << endl;

	return { "depthRev", width, height, fboNew, 0, 0, 0, 0, 0, myDS };
}

AbjNode bgN_create(int width, int height)
{
	GLuint fboNew;
	glCreateFramebuffers(1, &fboNew);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	const auto cAttachNum = arraySize(DrawBuffers);

	GLuint setupRTT[cAttachNum];
	glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

	for (int i = 0; i < cAttachNum; ++i)
	{
		glTextureStorage2D(setupRTT[i], 1, GL_RGBA16F, width, height);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glNamedFramebufferTexture(fboNew, DrawBuffers[i], setupRTT[i], 0);
	}

	//DEPTH STEN
	GLuint myDS;
	glCreateTextures(GL_TEXTURE_2D, 1, &myDS);
	glTextureStorage2D(myDS, 1, GL_DEPTH32F_STENCIL8, width, height);
	glTextureParameteri(myDS, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(myDS, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(myDS, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(myDS, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glNamedFramebufferTexture(fboNew, GL_DEPTH_STENCIL_ATTACHMENT, myDS, 0);
	glNamedFramebufferDrawBuffers(fboNew, cAttachNum, DrawBuffers);

	if (glCheckNamedFramebufferStatus(fboNew, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "error with bgN_create" << endl;

	return { "bg", width, height, fboNew, setupRTT[0], 0, 0, setupRTT[1], 0, myDS };
}

AbjNode tonemapN_create(Abj &myAbj)
{
	GLuint fboNew;
	glCreateFramebuffers(1, &fboNew);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	const auto cAttachNum = arraySize(DrawBuffers);

	GLuint setupRTT[cAttachNum];
	glCreateTextures(GL_TEXTURE_2D, cAttachNum, setupRTT);

	for (int i = 0; i < cAttachNum; ++i)
	{
		glTextureStorage2D(setupRTT[i], 1, GL_RGB16F, myAbj.width, myAbj.height);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(setupRTT[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (myAbj.myFSQ->fxaaBlur->val_b)
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

	return { "tonemap", myAbj.width, myAbj.height, fboNew, setupRTT[0] , 0, 0, setupRTT[1] };
}

AbjNode singleN_create(string name, GLenum format, int width, int height)
{
	GLuint fboNew;
	glCreateFramebuffers(1, &fboNew);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	const auto cAttachNum = arraySize(DrawBuffers);

	GLuint texNew[cAttachNum];
	glCreateTextures(GL_TEXTURE_2D, cAttachNum, texNew);

	for (int i = 0; i < cAttachNum; ++i)
	{
		auto numMip = 1 + int(floor(log2(glm::max(width, height))));
		glTextureStorage2D(texNew[i], numMip, format, width, height);

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

	return { name, width, height, fboNew, texNew[0] };
}

AbjNode dualN_create(string name, GLenum format, int width, int height)
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
			glTextureStorage2D(setupRTT[i], 1, format, width, height);
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

	return { name, width, height, nodeNew[0], setupRTT[0], 0, nodeNew[1], setupRTT[1] };
}

void bloomBufferCreate(Abj &myAbj)
{
	myAbj.bloomN = singleN_create("bloom", GL_RGBA16F, myAbj.width, myAbj.height);
	up64N(myAbj.bloomN, 1);

	auto widthB = myAbj.width / 2;
	auto heightB = myAbj.height / 2;

	for (int i = 0; i < 6; ++i)
	{
		string conc = "down";
		conc.append(to_string(i));

		myAbj.downN[i] = singleN_create(conc, GL_RGBA16F, widthB, heightB);
		up64N(myAbj.downN[i], 1);

		conc = "bloomGauss";
		conc.append(to_string(i));

		myAbj.bloomGaussN[i] = dualN_create(conc, GL_RGBA16F, widthB, heightB);
		up64N(myAbj.bloomGaussN[i], 1);

		widthB /= 2;
		heightB /= 2;
	}
}

void resizeTexClearMem(Abj &myAbj)
{
	//GBUF FBO
	up64T(myAbj.gBuf0_32, myAbj.gBuf0_64, 0);
	up64T(myAbj.gBuf1_32, myAbj.gBuf1_64, 0);
	up64T(myAbj.gBuf2_32, myAbj.gBuf2_64, 0);
	up64T(myAbj.gBuf3_32, myAbj.gBuf3_64, 0);
	up64T(myAbj.gBuf4_32, myAbj.gBuf4_64, 0);
	up64T(myAbj.gBuf5_32, myAbj.gBuf5_64, 0);
	up64T(myAbj.gBuf6_32, myAbj.gBuf6_64, 0);
	up64T(myAbj.gBuf7_32, myAbj.gBuf7_64, 0);
	up64T(myAbj.gBuf_DS_32, myAbj.gBuf_DS_64, 0);
	glDeleteFramebuffers(1, &myAbj.gBufN.fbo0);

	up64N(myAbj.alphaGaussN, 0);
	up64N(myAbj.bgN, 0);
	up64N(myAbj.bloomN, 0);
	up64N(myAbj.bloomCN, 0);
	up64N(myAbj.deferredN, 0);
	up64N(myAbj.fxaaN, 0);
	up64N(myAbj.brushN, 0);
	up64N(myAbj.brushBGN, 0);
	up64N(myAbj.brushTempN, 0);
	up64N(myAbj.sobelN, 0);
	up64N(myAbj.eraserN, 0);
	up64N(myAbj.cursorN, 0);
	up64N(myAbj.ssaoN, 0);
	up64N(myAbj.ssrN, 0);
	up64N(myAbj.ssaoGaussN, 0);
	up64N(myAbj.tonemapN, 0);

	up64N(myAbj.lumaInitN, 0);

	for (int i = 0; i < 2; ++i)
		up64N(myAbj.lumaAdaptN[i], 0);

	myAbj.fboReady = false;
}

bool searchUp(shared_ptr<Object> obj, Abj &myAbj)
{
	searchUp_rec(obj, myAbj);

	if (myAbj.searchB)
		return true;

	return false;
}

void searchUp_rec(shared_ptr<Object> obj, Abj &myAbj)
{
	if (obj->v->val_b)
	{
		if (obj->parentTo)
		{
			if (obj->parentTo->v->val_b)
			{
				auto found = false;
				shared_ptr<Object> tempObj;

				for (auto &i : myAbj.allObj)
				{
					if (obj->parentTo->parentTo == i)
					{
						found = true;
						tempObj = i;

						continue;
					}
				}

				if (!found)
					myAbj.searchB = true;

				else
					searchUp_rec(tempObj, myAbj);
			}

			else
				myAbj.searchB = false;
		}

		else
			myAbj.searchB = true;
	}

	else
		myAbj.searchB = false;
}

void clearCursor(bool inPaintMode, Abj &myAbj)
{
	if (inPaintMode) // don't clear if not in paint mode
	{
		glBindFramebuffer(GL_FRAMEBUFFER, myAbj.cursorN.fbo0);
		glViewport(0, 0, myAbj.cursorN.width, myAbj.cursorN.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void clearCanvas(Abj &myAbj)
{
	GLint resetCol[] = { 0, 0, 0, 0 };

	glClearNamedFramebufferiv(myAbj.brushN.fbo1, GL_COLOR, 0, resetCol);
	glClearNamedFramebufferiv(myAbj.brushBGN.fbo0, GL_COLOR, 0, resetCol);
	glClearNamedFramebufferiv(myAbj.brushBGN.fbo1, GL_COLOR, 0, resetCol);
}

void gizShow(string gizTypeShow, Abj &myAbj)
{
	if (myAbj.selB)
	{
		for (auto &i : myAbj.allObj)
		{
			if (gizTypeShow == "T")
			{
				if (i->type == "GIZ_CIRC") i->v->val_b = false;
				else if (i->type == "GIZ_CIRC_HALF") i->v->val_b = false;
				else if (i->type == "GIZ_CONE") i->v->val_b = true;
				else if (i->type == "GIZ_CUBE") i->v->val_b = false;
				else if (i->type == "GIZ_DUAL_HANDLE") i->v->val_b = true;
				else if (i->type == "GIZ_LINE") i->v->val_b = true;
			}

			else if (gizTypeShow == "R")
			{
				if (i->type == "GIZ_CIRC") i->v->val_b = true;
				else if (i->type == "GIZ_CIRC_HALF") i->v->val_b = true;
				else if (i->type == "GIZ_CONE") i->v->val_b = false;
				else if (i->type == "GIZ_CUBE") i->v->val_b = false;
				else if (i->type == "GIZ_DUAL_HANDLE") i->v->val_b = false;
				else if (i->type == "GIZ_LINE") i->v->val_b = false;
			}

			else if (gizTypeShow == "S")
			{
				if (i->type == "GIZ_CIRC") i->v->val_b = false;
				else if (i->type == "GIZ_CIRC_HALF") i->v->val_b = false;
				else if (i->type == "GIZ_CONE") i->v->val_b = false;
				else if (i->type == "GIZ_CUBE" && i != myAbj.myPivot) i->v->val_b = true;
				else if (i->type == "GIZ_DUAL_HANDLE") i->v->val_b = true;
				else if (i->type == "GIZ_LINE") i->v->val_b = true;
			}
		}

		myAbj.myGizNull->parentTo = myAbj.selB;
		myAbj.myGizNull->v->val_b = true;
		myAbj.myGizNull->gizType = gizTypeShow;

		//myOutliner->refreshOutliner(1);
	}
}

void pivTgl(Abj &myAbj)
{
	if (myAbj.selB && myAbj.selB != myAbj.myPivot)
	{
		myAbj.myPivot->parentTo = myAbj.selB;
		myAbj.myPivot->v->val_b = !myAbj.myPivot->v->val_b;
	}

	//myOutliner->refreshOutliner(1);
}

void clearSel(Abj &myAbj)
{
	myAbj.selB = 0;

	for (auto &i : myAbj.allObj)
		i->selected = false;
}

