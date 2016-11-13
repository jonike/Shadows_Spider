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

#include "Core.h"

void quitSeq(Abj &myAbj)
{
	////DELETE ALL OBJECTS AND THEIR VBOS
	//for (auto &i : myAbj.allObj)
	//	i->selected = true;

	//delete_();

	////DELETE ALL FBOS AND THEIR ATTACHED TEXTURES
	//resizeTexClearMem();

	////DELETE ALL SHADOWMAPS
	//addDeleteShadows("delete");

	////DELETE ALL  "REG" TEXTURES
	//for (auto &i : myAbj.allMaps)
	//{
	//	for (auto &j : i.layer)
	//	{
	//		glMakeTextureHandleNonResidentARB(j.tex0_64);
	//		glMakeTextureHandleNonResidentARB(j.tex1_64);
	//		glDeleteTextures(1, &j.tex0_32);
	//		glDeleteTextures(1, &j.tex1_32);
	//	}
	//}

	////DELETE ALL PROGRAMS
	//for (auto &i : myAbj.allPro)
	//	glDeleteProgram(i.pro);

	////DELETE REMAINING ABJ NODES THAT HAVE FBOS INCLUDING ONES IN GLWIDGETSH
	////delete ubo
	//glDeleteBuffers(1, &myAbj.pro);
	//glDeleteBuffers(1, &myAbj.uboScene);
	//glDeleteBuffers(1, &myAbj.uboLight);
}

void myVInit(Abj &myAbj)
{
	myAbj.pathGLSL = "./shaders/";
	myAbj.pathObj = "C:/users/bergj/desktop/obj/";
	myAbj.pathTex = "C:/users/bergj/desktop/tex/";

	myAbj.gamma = 1.f / 2.2f;
	myAbj.gizScale = 1.75f;
	myAbj.gizSpace = "world";
	myAbj.upWorld = glm::vec3(0.f, 1.f, 0.f);
	myAbj.lightCt = 0;
	myAbj.brushSize = 10;
	myAbj.brushHard = 100;
	myAbj.mpfTgl = true;
	myAbj.fboReady = false;
	myAbj.currLum = 0;
	myAbj.gizSideS = glm::vec3(40.f, 40.f, 3.f);
	myAbj.UBO_light_needsUp = true; //update first by default
	myAbj.brushOutlineUpB = false;
	myAbj.gridLines = 20;

	myAbj.bakeNow = false;
	myAbj.penOrientation = "PEN";
	myAbj.paintType = "NONE";
	myAbj.paintMode = false;
	myAbj.dragDrop = false;
	myAbj.edgeDetect_mode = 0;
	myAbj.selMode = "OBJ";
	myAbj.disableSelRect = false;

	myAbj.colorPickTgl = false;
	myAbj.statsTgl = myAbj.wireOverTgl = false;
	myAbj.mpfTgl = myAbj.gizSideTgl = myAbj.gizSpaceTgl = true;
	myAbj.rezGateTgl = myAbj.rezGateTgl_sel = false; //

	myAbj.Csel = glm::vec3(.3f, 1.f, 1.f);
	myAbj.CselManip = glm::vec3(.557f, .333f, .067f);
	myAbj.Ctxt = glm::vec3(1.f, 1.f, 0.f);
	myAbj.Cwire = glm::vec3(0.f);
	myAbj.Cgrid = glm::vec3(0.f);

	myAbj.altTgl = myAbj.ctrlTgl = myAbj.shiftTgl = myAbj.lmbTgl = myAbj.mmbTgl = myAbj.rmbTgl = myAbj.spaceTgl = myAbj.colorPickTgl = false;
	myAbj.wTgl = myAbj.sTgl = myAbj.aTgl = myAbj.dTgl = myAbj.rTgl = myAbj.fTgl = myAbj.vTgl = false;

	myAbj.debug0 = false;
	myAbj.copyTgt = 0;

	myAbj.blendModeD = 999;
	myAbj.moveDist_paint = false;
	myAbj.firstPress = true;
	myAbj.doEraserDebug = true;
	myAbj.paintCursorResizeTgl = false;
	myAbj.distDragPaint = 15.f;
	myAbj.fpsMouseSpeed = .3f;
	myAbj.tumbleSpeed = .5f;
	myAbj.panSpeed = .07f;
	myAbj.panSpeed_ortho = .03f;
	myAbj.dollySpeed = .05f;
	myAbj.dollySpeed_ortho = .01f;
	myAbj.selAccuracy = 30;

	myAbj.gizHoverType = "NONE";
	myAbj.gizTransType = "NONE";
}

int main(int argc, char *argv[])
{
	srand(static_cast<int>(time(0)));

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 120);

	bool brushOutlineUpB = false;
	Abj myAbj;
	myAbj.width = 600;
	myAbj.height = 600;

	GLFWwindow *GLFWwin;
	GLFWwin = glfwCreateWindow(myAbj.width, myAbj.height, "Shadows Spider", 0, 0);
	glfwMakeContextCurrent(GLFWwin);
	glewExperimental = true;
	glewInit();
	glfwSetInputMode(GLFWwin, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(GLFWwin, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //GLFW_CURSOR_HIDDEN
	glfwSetKeyCallback(GLFWwin, keyCallback);
	glfwSetCursorPosCallback(GLFWwin, cursorPosCallback);
	glfwSetMouseButtonCallback(GLFWwin, mouseButtonCallback);
	glfwSetWindowUserPointer(GLFWwin, &myAbj);
	double mpf_oldTime = glfwGetTime();
	int mpf_framesPerSec = 0;
	glfwSwapInterval(0); //vsync off

	///////////
	///////////

	myVInit(myAbj);
	mapInit(myAbj);
	proInit(myAbj);
	camInit(myAbj);
	gizInit(myAbj);
	objInit(myAbj);
	lightInit(myAbj);
	startupScene("teapotPlane", myAbj);
	//startupScene("BOIT", myAbj);
	//myOutliner->refreshOutliner(1);

	for (auto &i : myAbj.allCamLi)
	{
		if (i->camLiType->val_s == "DIR")
			i->RM = i->rotOrderUse(i->rotOrder->val_s); //init RM for dir lights
	}

	glClearColor(0.f, 0.f, 0.f, 0.f);

	addDeleteShadows("add", myAbj);
	uboInit(myAbj);
	myAbj.lightCt = countLights(myAbj);

	resize(myAbj);

	do
	{
		/* MPF TIMER */
		{
			double mpf_currentTime = glfwGetTime();
			++mpf_framesPerSec;

			if (mpf_currentTime - mpf_oldTime >= 1.f) //change title / reset
			{
				stringstream ss;
				ss << 1000.0 / mpf_framesPerSec;
				string mpfStr = ss.str();
				string myWinTitle = "Shadows Spider";
				myWinTitle += " | " + mpfStr + " [ms]";
				const char *myMPF = myWinTitle.c_str();

				glfwSetWindowTitle(GLFWwin, myMPF);

				mpf_framesPerSec = 0;
				mpf_oldTime = glfwGetTime();
			}
		}

		render_frame(myAbj);

		glfwSwapBuffers(GLFWwin);
		glfwPollEvents();
	}

	while (!quitCombo(GLFWwin) && glfwWindowShouldClose(GLFWwin) == 0);

	quitSeq(myAbj);
	glfwTerminate();

	return 0;
}