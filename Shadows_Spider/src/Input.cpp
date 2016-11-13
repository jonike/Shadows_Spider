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

#include "Input.h"

void keyCallback(GLFWwindow *myWin, int key, int scancode, int action, int mods)
{
	auto *myAbj = static_cast<Abj*>(glfwGetWindowUserPointer(myWin));

	/* PRESS */
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_LEFT_ALT)
		{
			myAbj->altTgl = true;
			myAbj->disableSelRect = true;
			clearCursor(myAbj->paintMode, *myAbj);
		}

		else if (key == GLFW_KEY_LEFT_CONTROL) myAbj->ctrlTgl = true;
		else if (key == GLFW_KEY_LEFT_SHIFT) myAbj->shiftTgl = true;
		else if (key == GLFW_KEY_EQUAL) myAbj->gizScale += .5f;
		else if (key == GLFW_KEY_MINUS) myAbj->gizScale -= .5f;

		else if (key == GLFW_KEY_A) myAbj->aTgl = true;
		else if (key == GLFW_KEY_B) myAbj->bTgl = true;
		else if (key == GLFW_KEY_C) myAbj->cTgl = true;
		else if (key == GLFW_KEY_D) myAbj->dTgl = true;
		else if (key == GLFW_KEY_E) myAbj->eTgl = true;
		else if (key == GLFW_KEY_F) myAbj->fTgl = true;
		else if (key == GLFW_KEY_G) myAbj->gTgl = true;
		else if (key == GLFW_KEY_H) myAbj->hTgl = true;
		
		else if (key == GLFW_KEY_I)
		{
			myAbj->iTgl = true;
			//PaintWinTgl(1, 0);
		}

		else if (key == GLFW_KEY_J) myAbj->jTgl = true;

		else if (key == GLFW_KEY_K)
		{
			myAbj->kTgl = true;
			//PaintWinTgl(1, 1);
		}
		
		else if (key == GLFW_KEY_L) myAbj->lTgl = true;
		else if (key == GLFW_KEY_M) myAbj->mTgl = true;
		else if (key == GLFW_KEY_N) myAbj->nTgl = true;
		else if (key == GLFW_KEY_O) myAbj->oTgl = true;
		else if (key == GLFW_KEY_P) myAbj->pTgl = true;
		else if (key == GLFW_KEY_Q) myAbj->qTgl = true;
		else if (key == GLFW_KEY_R) myAbj->rTgl = true;
		else if (key == GLFW_KEY_S) myAbj->sTgl = true;
		else if (key == GLFW_KEY_T) myAbj->tTgl = true;
		else if (key == GLFW_KEY_U) myAbj->uTgl = true;
		else if (key == GLFW_KEY_V) myAbj->vTgl = true;
		else if (key == GLFW_KEY_W) myAbj->wTgl = true;
		else if (key == GLFW_KEY_X) myAbj->xTgl = true;
		else if (key == GLFW_KEY_Y) myAbj->yTgl = true;
		//else if (key == GLFW_KEY_Z) myAbj->zTgl = true;
	}

	/* RELEASE */
	else if (action == GLFW_RELEASE)
	{
		//DELETE A CAMLI - remember to redo lightUBO

		//if (myAbj->paintMode)
		//{
		//}

		if		(key == GLFW_KEY_1) myAbj->selMode = "VERT";
		else if (key == GLFW_KEY_2) myAbj->selMode = "EDGE";
		else if (key == GLFW_KEY_3) myAbj->selMode = "FACE";
		else if (key == GLFW_KEY_4) myAbj->selMode = "OBJ";
		//else if (key == GLFW_KEY_5) wireOverTgl_swap();

		else if (key == GLFW_KEY_F3)
		{
			myAbj->debug0 = !myAbj->debug0;
			//refresh "attr table"
		}

		//F4 reserved for selecting myAbj->myFSQ

		else if (key == GLFW_KEY_GRAVE_ACCENT)
		{
			//show / hide prefs
		}

		if (key == GLFW_KEY_LEFT_ALT)
		{
			myAbj->altTgl = false;

			if (myAbj->selCamLi->camLiType->val_s != "FPS")
			{
				//if (myAbj->paintMode)
				//	setCursor(Qt::BlankCursor);

				//else
				//	setCursor(Qt::ArrowCursor);
			}

			if (!myAbj->lmbTgl)
				myAbj->disableSelRect = false;
		}

		else if (key == GLFW_KEY_LEFT_CONTROL)
		{
			myAbj->ctrlTgl = false;
			clearCursor(myAbj->paintMode, *myAbj);
		}

		else if (key == GLFW_KEY_LEFT_SHIFT) myAbj->shiftTgl = false;
		else if (key == GLFW_KEY_SPACE) myAbj->spaceTgl = false;
		
		else if (key == GLFW_KEY_ESCAPE)
		{
			myAbj->altTgl = false;
			myAbj->disableSelRect = true;
		}

		//////////////////
		else if (key == GLFW_KEY_A) myAbj->aTgl = false;
		
		else if (key == GLFW_KEY_B)
		{
			myAbj->bTgl = false;
			myAbj->bakeNow = true;
		}
		
		else if (key == GLFW_KEY_C) myAbj->cTgl = false;
		
		else if (key == GLFW_KEY_D)
		{
			myAbj->dTgl = false;

			if (myAbj->selCamLi->camLiType->val_s != "FPS" && !myAbj->ctrlTgl)
				pivTgl(*myAbj);
		}

		else if (key == GLFW_KEY_E)
		{
			myAbj->eTgl = false;

			if (myAbj->selCamLi->camLiType->val_s != "FPS")
				gizShow("R", *myAbj);
		}

		else if (key == GLFW_KEY_F)
		{
			myAbj->fTgl = false;

			if (myAbj->selCamLi->camLiType->val_s != "FPS" && myAbj->selB)
			{
				auto sizeByM = myAbj->selB->MM * glm::vec4(myAbj->selB->bbSize, 0.f);
				glm::vec3 size(sizeByM.x, sizeByM.y, sizeByM.z);
				auto dist = glm::sqrt(glm::dot(size, size)) / glm::sin(glm::radians(myAbj->selCamLi->fov->val_f * .5f));

				myAbj->selCamLi->setTarg(myAbj->selB->piv->val_3, dist * .5f);
			}
		}

		else if (key == GLFW_KEY_G)
		{
			myAbj->gTgl = false;
			myAbj->selCamLi->gridV = !myAbj->selCamLi->gridV;
		}

		else if (key == GLFW_KEY_H) myAbj->hTgl = false;

		else if (key == GLFW_KEY_I)
		{
			myAbj->iTgl = false;

			//if (myAbj->myPaintWin->stackedMain->currentIndex() == 0)
				//myAbj->PaintWinTgl(0, 999);
		}

		else if (key == GLFW_KEY_J) myAbj->jTgl = false;

		else if (key == GLFW_KEY_K)
		{
			myAbj->kTgl = false;

			//if (myAbj->myPaintWin->stackedMain->currentIndex() == 1)
			//myAbj->PaintWinTgl(0, 999);
		}

		else if (key == GLFW_KEY_L) myAbj->lTgl = false;
		else if (key == GLFW_KEY_M) myAbj->mTgl = false;
		else if (key == GLFW_KEY_N) myAbj->nTgl = false;
		else if (key == GLFW_KEY_O) myAbj->oTgl = false;

		else if (key == GLFW_KEY_P)
		{
			myAbj->pTgl = false;

			if (!myAbj->ctrlTgl)
			{
				if (myAbj->paintMode)
				{
					clearCursor(myAbj->paintMode, *myAbj);
					myAbj->paintMode = false;
					//setCursor(ArrowCursor);
				}

				else
				{
					myAbj->paintMode = true;
					//setCursor(BlankCursor);
				}
			}
		}

		else if (key == GLFW_KEY_Q)
		{
			myAbj->qTgl = false;

			if (myAbj->selCamLi->camLiType->val_s != "FPS")
				myAbj->myGizNull->v->val_b = false;

			clearCursor(myAbj->paintMode, *myAbj);
			myAbj->paintMode = false;
			//setcursor(arrowCursor);
		}

		else if (key == GLFW_KEY_R)
		{
			myAbj->rTgl = false;

			if (myAbj->selCamLi->camLiType->val_s != "FPS")
				gizShow("S", *myAbj);
		}

		else if (key == GLFW_KEY_S) myAbj->sTgl = false;
		else if (key == GLFW_KEY_T) myAbj->tTgl = false;
		else if (key == GLFW_KEY_U) myAbj->uTgl = false;
		else if (key == GLFW_KEY_V) myAbj->vTgl = false;

		else if (key == GLFW_KEY_W)
		{
			myAbj->wTgl = false;

			if (myAbj->selCamLi->camLiType->val_s != "FPS")
				gizShow("T", *myAbj);
		}

		else if (key == GLFW_KEY_X) myAbj->xTgl = false;
		else if (key == GLFW_KEY_Y) myAbj->yTgl = false;
		//else if (key == GLFW_KEY_Z) myAbj->zTgl = false;
		else if (key == GLFW_KEY_Z) myAbj->zTgl = !myAbj->zTgl;
	}
}

void mouseButtonCallback(GLFWwindow *myWin, int button, int action, int mods)
{
	auto *myAbj = static_cast<Abj*>(glfwGetWindowUserPointer(myWin));

	if (action == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			myAbj->lmbTgl = true; myAbj->mmbTgl = false; myAbj->rmbTgl = false;

			if (!myAbj->altTgl)
			{
				myAbj->rayP = myAbj->pMouseNew; //

				if (myAbj->paintMode)
					myAbj->disableSelRect = true;

				else
					myAbj->disableSelRect = false;

				if (myAbj->selCamLi->camLiType->val_s != "FPS")
				{
					//if (paintMode)
						//setCursor(Qt::BlankCursor);

					//else
						//setCursor(Qt::ArrowCursor); //
				}
			}

			if (!myAbj->selRectPts_color.empty())
				myAbj->selRectPts_color.clear();
		}

		else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			myAbj->lmbTgl = false; myAbj->mmbTgl = true; myAbj->rmbTgl = false;

			//if (!myAbj->altTgl)
				//myAbj->gizTransType = myAbj->myGizmo->hover();

			clearCursor(myAbj->paintMode, *myAbj);
		}

		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			myAbj->lmbTgl = false; myAbj->mmbTgl = false; myAbj->rmbTgl = true;

			if (myAbj->selCamLi->camLiType->val_s != "FPS")
			{
				//if (!myAbj->altTgl && !myAbj->ctrlTgl && myAbj->shiftTgl)
					//radPop_GL("addObj");

				//if (!myAbj->altTgl && !myAbj->ctrlTgl && !myAbj->shiftTgl)
					//radPop_GL("viewChange");
			}

			clearCursor(myAbj->paintMode, *myAbj);
		}
	}

	else if (action == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			myAbj->lmbTgl = false;

			if (!myAbj->altTgl && !myAbj->paintMode) //SELOBJ RAYCAST
			{
				getPtsBetweenRect(*myAbj);
				myAbj->singleShotRC = false;

				if (myAbj->selRectPts_color.empty())
				{
					myAbj->singleShotRC = true;
					myAbj->selRectPts_color.push_back(myAbj->rayP);
				}

				if (!myAbj->ctrlTgl && !myAbj->shiftTgl)
					clearSel(*myAbj); //

				checkForHits(*myAbj);
			}

			else if (!myAbj->altTgl && myAbj->paintMode && myAbj->penOrientation == "ERASER")
			{
				myAbj->firstPress = 1;
			}

			else if (!myAbj->altTgl && myAbj->paintMode && myAbj->penOrientation == "PEN")
			{
				//CLEAR BG1
				glBindFramebuffer(GL_FRAMEBUFFER, myAbj->brushBGN.fbo0);
				glViewport(0, 0, myAbj->brushBGN.width, myAbj->brushBGN.height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//COPY PAINT DRAG TO BG2
				glDisable(GL_BLEND);
				glBindFramebuffer(GL_FRAMEBUFFER, myAbj->brushBGN.fbo1);
				glViewport(0, 0, myAbj->brushBGN.width, myAbj->brushBGN.height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram2("pCopyTex", *myAbj);

				myAbj->copyTgt = 5; //brushN2
				myAbj->myFSQ->render();

	            //cout << "resetting debug paint settings 0" << endl;
				myAbj->firstPress = 1;
			}

			//else
				//myAbj.attrTable->refreshTable();
		}

		else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			myAbj->mmbTgl = false;
			myAbj->gizTransType = "NONE";
			//myAbj.attrTable->refreshTable();
		}

		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			myAbj->rmbTgl = false;
			//myAbj.attrTable->refreshTable();
		}

		//setting cursor for paint / normal again to make sure...
		//if (selCamLi->camLiType->val_s != "FPS")
		//{
		//	if (paintMode)
		//		setCursor(Qt::BlankCursor);

		//	else
		//		setCursor(Qt::ArrowCursor);
		//}
	}
}

void cursorPosCallback(GLFWwindow *myWin, double xPos, double yPos)
{
	auto *myAbj = static_cast<Abj*>(glfwGetWindowUserPointer(myWin));

	myAbj->pMouseOld = myAbj->pMouseNew;
	myAbj->pMouseNew = glm::vec2(xPos, yPos);
	myAbj->pMouseDiff = myAbj->pMouseNew - myAbj->pMouseOld;

	//if (myAbj->myGizNull->parentTo)
	//{
	//	if (myAbj->gizTransType == "NONE")
	//	{
	//		myAbj->gizHoverType = myAbj.myGizmo->hover();
	//		myAbj.myGizmo->resetSetCol(gizHoverType);
	//	}

	//	else
	//		myAbj.myGizmo->transform();
	//}

	if (myAbj->paintMode && myAbj->paintCursorResizeTgl)
	{
		auto paintCursorScaleSpeed = .005f;
		myAbj->selBrush->scale = clamp(myAbj->selBrush->scale + myAbj->pMouseDiff.x * paintCursorScaleSpeed, glm::vec3(0.25f), glm::vec3(1.f));
	}

	if (myAbj->selCamLi->camLiType->val_s != "FPS")
	{
		if (myAbj->lmbTgl && myAbj->altTgl)
		{
			//setCursor(Qt::SizeAllCursor);

			if (!(myAbj->selCamLi->camLiType->val_s == "ORTHO" && !myAbj->selCamLi->orthoFree->val_b))
			{
				myAbj->selCamLi->r->val_3.x -= myAbj->tumbleSpeed * myAbj->pMouseDiff.x;
				myAbj->selCamLi->r->val_3.y = glm::clamp(myAbj->selCamLi->r->val_3.y - myAbj->tumbleSpeed * myAbj->pMouseDiff.y, -89.99f, 89.99f); //no flip
			}

			myAbj->selCamLi->setDirty();
			//updateCamAttrs("rotate");
		}

		else if (myAbj->mmbTgl && myAbj->ctrlTgl && myAbj->shiftTgl)
		{
			myAbj->selCamLi->r->val_3.z += (float)myAbj->pMouseDiff.x;

			myAbj->selCamLi->setDirty();
			//updateCamAttrs("rotate");
		}

		else if ((myAbj->mmbTgl && !myAbj->myGizNull->v->val_b) || (myAbj->mmbTgl && myAbj->altTgl))
		{
			//setCursor(Qt::ClosedHandCursor);

			if (myAbj->selCamLi->camLiTypeGet("dirOrtho"))
			{
				myAbj->selCamLi->targO -= myAbj->selCamLi->rightO * (float)myAbj->pMouseDiff.x * myAbj->panSpeed_ortho;
				myAbj->selCamLi->targO += myAbj->selCamLi->upO * (float)myAbj->pMouseDiff.y * myAbj->panSpeed_ortho;
			}

			else
			{
				myAbj->selCamLi->targO -= myAbj->selCamLi->rightO * (float)myAbj->pMouseDiff.x * myAbj->panSpeed;
				myAbj->selCamLi->targO += myAbj->selCamLi->upO * (float)myAbj->pMouseDiff.y * myAbj->panSpeed;
			}

			myAbj->selCamLi->setDirty();
			//updateCamAttrs("translate");
		}

		else if (myAbj->rmbTgl && myAbj->altTgl)
		{
			//setCursor(Qt::SizeVerCursor);

			if (myAbj->selCamLi->camLiTypeGet("dirOrtho"))
			{
				myAbj->selCamLi->orthoZoom->val_f = glm::max(.001f, myAbj->selCamLi->orthoZoom->val_f - myAbj->pMouseDiff.x * myAbj->dollySpeed_ortho); //no flip

				PMupOrtho(*myAbj);
			}

			else
				myAbj->selCamLi->distO = glm::max(.01f, myAbj->selCamLi->distO - myAbj->pMouseDiff.x * myAbj->dollySpeed); //no flip

			myAbj->selCamLi->setDirty();
			//updateCamAttrs("zoom");
		}
	}

	else
	{
		glfwSetCursorPos(myWin, myAbj->width / 2, myAbj->height / 2);

		myAbj->selCamLi->r->val_3.x += float(myAbj->width / 2 - myAbj->pMouseNew.x) * myAbj->fpsMouseSpeed;
		myAbj->selCamLi->r->val_3.y -= float(myAbj->height / 2 - myAbj->pMouseNew.y) * myAbj->fpsMouseSpeed;

		myAbj->selCamLi->setDirty(); //VM
	}
}

bool checkForHits(Abj &myAbj)
{
	myAbj.colorPickTgl = true;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram2("pBB", myAbj); //gets Crand

	for (auto &i : myAbj.frustumObjs)
	{
		if (i->type == "CAMLI" || i->type == "OBJ")
		{
			i->mvpGet();
			i->render();
		}
	}

	glDisable(GL_DEPTH_TEST);

	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	auto foundNonOBJ = false;

	for (auto it = myAbj.selRectPts_color.begin(); it != myAbj.selRectPts_color.end(); )
	{
		unsigned char data[4];
		glReadPixels(int((*it).x), myAbj.height - int((*it).y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;

		if (pickedID != 0x00ffffff)
		{
			int r = (pickedID & 0x000000FF) >> 0;
			int g = (pickedID & 0x0000FF00) >> 8;
			int b = (pickedID & 0x00FF0000) >> 16;

			glm::vec3 compareID(r / 255.f, g / 255.f, b / 255.f);

			for (auto &i : myAbj.frustumObjs)
			{
				if (i->Crand == compareID)
				{
					if (i->selectable)
						matchFoundRaycast(i, myAbj);
				}
			}
		}

		if (myAbj.singleShotRC)
			++it;

		else
		{
			size_t it_curr = it - myAbj.selRectPts_color.begin();
			size_t it_end = myAbj.selRectPts_color.size();

			if (it_curr + myAbj.selAccuracy <= it_end)
				advance(it, myAbj.selAccuracy);

			else
				++it;
		}
	}

	/* CLEANUP */
	glEnable(GL_DEPTH_TEST);
	myAbj.colorPickTgl = false;
	//gizPivAutoShow(); //
	//myAbj.myOutliner->refreshOutliner(1);
	//myAbj.attrTable->refreshTable();

	return foundNonOBJ;
}


void matchFoundRaycast(shared_ptr<Object> obj, Abj &myAbj)
{
	if (obj->selected && myAbj.ctrlTgl)
		obj->selected = false;

	if (!myAbj.ctrlTgl)
	{
		auto type = obj->type;

		obj->selected = true;
		myAbj.selB = obj; //

		if (type == "GIZ_CIRC" || type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_LINE")
			obj->parentTo->parentTo->selected = 1; //keep obj selected w/ manips
	}
}

bool quitCombo(GLFWwindow *myWin)
{
	bool ctrlTgl, qTgl;
	ctrlTgl = qTgl = false;

	if (glfwGetKey(myWin, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		ctrlTgl = true;

	if (glfwGetKey(myWin, GLFW_KEY_Q) == GLFW_PRESS)
		qTgl = true;

	if (ctrlTgl == true && qTgl == true)
		return true;

	return false;
}