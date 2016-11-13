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

#include "Setup_obj.h"

void gridInit(Abj &myAbj)
{
	GridSetup myGridSetup[] =
	{
		{ "grid_front", { 0.f, 0.f, 0.f } },
		{ "grid_frontB", { 0.f, 0.f, 90.f } }, //

		{ "grid_left", { 0.f, 90.f, 0.f } },
		{ "grid_leftB", { 90.f, 90.f, 0.f } },

		{ "grid_top", { 90.f, 0.f, 0.f } },
		{ "grid_topB", { 0.f, 90.f, 90.f } }, //ok
	};

	for (auto &i : myGridSetup)
	{
		myAbj.newObj = VBOup("999", "GRID", i.name, 0, myAbj); //

		for (auto &j : myAbj.newObj)
		{
			j->deletable = false;
			j->selectable = false;
			j->ignoreOutliner = true;

			j->r->val_3 = i.r;
			float gridSizeD = 15.f;
			//j->s->val_3 = glm::vec3(attrTable->gridSize->val_f);
			j->s->val_3 = glm::vec3(gridSizeD);
			myAbj.allObj.push_back(j);
			myAbj.allGrid.push_back(j);
		}
	}
}

void camInit(Abj &myAbj)
{
	gridInit(myAbj);

	CamSetup myCamSetup[] =
	{
		{ "ORTHO", "back", { 0.f, 0.f, -5.f } },
		{ "ORTHO", "bottom", { 0.f, -5.f, .002f } }, //
		{ "ORTHO", "front", { 0.f, 0.f, 5.f } },
		{ "ORTHO", "left", { 5.f, 0.f, 0.f } },
		{ "ORTHO", "right", { -5.f, 0.f, 0.f } },
		{ "ORTHO", "top", { 0.f, 5.f, .002f } }, //

		{ "PERSP", "persp", { 10.f, 10.f, 10.f } },
		//{ "PERSP", "persp", { 0.f, 0.f, 0.001f } },
		{ "PERSP", "persp1", { -5.f, 2.f, 5.f } },
	};

	for (auto &i : myCamSetup)
	{
		myAbj.newObj = VBOup("999", i.type, i.name, 0, myAbj);

		for (auto &j : myAbj.newObj)
		{
			j->Cgiz = glm::vec3(1.f, 0.f, 0.f);
			j->t->val_3 = i.t;
			j->piv->val_3 = j->t->val_3;

			if (i.type == "ORTHO") //sets in VBOup()
			{
				j->nearClip->val_f = -10000.f;
				j->farClip->val_f = 10000.f;
				j->orthoType->val_s = stringToUpper(i.name);
				j->ignoreOutliner = true;
				j->deletable = false;
				j->ref = true;
			}

			j->setTarg(j->targO, 0.f);
			j->v->val_b = (j->camLiType->val_s != "ORTHO") ? true : false;

			myAbj.allObj.push_back(j);
			myAbj.allCamLi.push_back(j);

			if (j->name->val_s == "persp")
				myAbj.selCamLi = j;
		}
	}
}

void gizInit(Abj &myAbj)
{
	myAbj.newObj = VBOup("999", "GIZ", "giz", 0, myAbj);
	for (auto &i : myAbj.newObj)
	{
		i->v->val_b = false;
		i->ignoreOutliner = true;
		i->deletable = false;
		i->selectable = false;
		myAbj.allObj.push_back(i);
		myAbj.myGizNull = i; // parent to this
	}

	float dualP = .5f; // dual axis handle P

	GizSetup myGizSetup[] =
	{
		{ "GIZ_CIRC", "gizCirc", { .2f, .2f, .2f } },
		{ "GIZ_CIRC_HALF", "gizCircHalfX", { 1.f, 0.f, 0.f } },
		{ "GIZ_CIRC_HALF", "gizCircHalfY", { 0.f, 1.f, 0.f } },
		{ "GIZ_CIRC_HALF", "gizCircHalfZ", { 0.f, 0.f, 1.f } },

		{ "GIZ_CONE", "gizConeX", { 1.f, 0.f, 0.f } },
		{ "GIZ_CONE", "gizConeY", { 0.f, 1.f, 0.f } },
		{ "GIZ_CONE", "gizConeZ", { 0.f, 0.f, 1.f } },

		{ "GIZ_CUBE", "gizCubeX", { 1.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f } },
		{ "GIZ_CUBE", "gizCubeY", { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f } },
		{ "GIZ_CUBE", "gizCubeZ", { 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 1.f } },
		{ "GIZ_CUBE", "gizCubeXYZ", { 0.f, 0.f, 0.f } },

		{ "GIZ_DUAL_HANDLE", "gizDualXZ", { 0.f, 1.f, 0.f }, { 90.f, 0.f, 0.f }, { dualP, 0.f, dualP } },
		{ "GIZ_DUAL_HANDLE", "gizDualXY", { 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f }, { dualP, dualP, 0.f } },
		{ "GIZ_DUAL_HANDLE", "gizDualYZ", { 1.f, 0.f, 0.f }, { 0.f, 90.f, 0.f }, { 0.f, dualP, dualP } },

		{ "GIZ_LINE", "gizLineX", { 1.f, 0.f, 0.f } },
		{ "GIZ_LINE", "gizLineY", { 0.f, 1.f, 0.f } },
		{ "GIZ_LINE", "gizLineZ", { 0.f, 0.f, 1.f } },
	};

	for (auto &i : myGizSetup)
	{
		myAbj.newObj = VBOup("999", i.type, i.name, 0, myAbj);

		for (auto &j : myAbj.newObj)
		{
			j->Cgiz = i.Cgiz;
			j->Cgiz_stored = i.Cgiz;

			if (i.type == "GIZ_CUBE")
				j->s->val_3 = glm::vec3(.15f);

			else if (i.type == "GIZ_DUAL_HANDLE")
				j->s->val_3 = glm::vec3(.1f);

			j->r->val_3 = i.r;
			j->t->val_3 = i.t;
			j->piv->val_3 = i.t;
			j->deletable = false;
			j->selectable = false;
			j->ignoreOutliner = true;
			j->parentObj("giz");
			myAbj.allObj.push_back(j);
			myAbj.allGiz.push_back(j);
		}
	}

	GizSetup myGizSetupSide[] =
	{
		{ "GIZ_LINE", "gizLineXSide", { 1.f, 0.f, 0.f } },
		{ "GIZ_LINE", "gizLineYSide", { 0.f, 1.f, 0.f } },
		{ "GIZ_LINE", "gizLineZSide", { 0.f, 0.f, 1.f } },
	};

	for (auto &i : myGizSetupSide)
	{
		myAbj.newObj = VBOup("999", i.type, i.name, 0, myAbj);

		for (auto &j : myAbj.newObj)
		{
			j->Cgiz = i.Cgiz;
			j->Cgiz_stored = i.Cgiz;
			j->deletable = false;
			j->selectable = false;
			j->ignoreOutliner = true;
			j->gizSideObj = true;
			myAbj.allObj.push_back(j);
			myAbj.allGizSide.push_back(j);
		}
	}
}

void objInit(Abj &myAbj)
{
	myAbj.newObj = VBOup("999", "BB", "bb", 0, myAbj);

	for (auto &i : myAbj.newObj)
	{
		i->deletable = false;
		i->ignoreOutliner = true;
		i->selectable = false;
		i->v->val_b = false;
		myAbj.allObj.push_back(i);
		myAbj.myBB = i;
	}

	myAbj.newObj = VBOup("999", "FBO", "fsq", 0, myAbj);
	for (auto &i : myAbj.newObj)
	{
		i->deletable = false;
		i->ignoreOutliner = true;
		i->selectable = false;

		for (auto &j : myAbj.allMaps)
		{
			if (j.name == "ennis")
			{
				if (j.type == "CUBE_SPEC")
					myAbj.cubeM_specular_32 = j.layer[0].tex0_32;

				else if (j.type == "CUBE_IRRAD")
					myAbj.cubeM_irradiance_32 = j.layer[0].tex0_32;

				i->cubeM->val_s = j.name;
			}
		}

		myAbj.allObj.push_back(i);
		myAbj.myFSQ = i;
	}

	myAbj.newObj = VBOup("999", "FBO", "paintStroke", 0, myAbj);
	for (auto &i : myAbj.newObj)
	{
		i->deletable = false;
		i->ignoreOutliner = true;
		i->selectable = false;

		myAbj.allObj.push_back(i);
		myAbj.paintStroke = i;
	}

	myAbj.newObj = VBOup("999", "SELRECT", "selRect", 0, myAbj);
	for (auto &i : myAbj.newObj)
	{
		i->deletable = false;
		i->ignoreOutliner = true;
		i->selectable = false;
		myAbj.allObj.push_back(i);
		myAbj.mySelRect = i;
	}

	myAbj.newObj = VBOup("999", "TXT", "txt", 0, myAbj);
	for (auto &i : myAbj.newObj)
	{
		i->deletable = false;
		i->ignoreOutliner = true;
		i->selectable = false;
		myAbj.allObj.push_back(i);
		myAbj.myTxt = i;
	}

	myAbj.newObj = VBOup("999", "GIZ_CUBE", "pivot", 0, myAbj);
	for (auto &i : myAbj.newObj)
	{
		i->v->val_b = false;
		i->s->val_3 = glm::vec3(.3f);
		i->Cgiz = glm::vec3(1.f, 1.f, 0.f);
		i->deletable = false;
		i->ignoreOutliner = true;
		i->selectable = false;
		myAbj.allObj.push_back(i);
		myAbj.myPivot = i;
	}
}

void lightInit(Abj &myAbj)
{
	LightSetup myLightSetup[] =
	{
		{ "AREA", "areaLight" },
		{ "DIR", "dirLight" },
		{ "POINT", "pointLight" },
		{ "SPOT", "spotLight" },
	};

	for (auto &i : myLightSetup)
	{
		myAbj.newObj = VBOup("999", i.type, i.name, 0, myAbj);
		for (auto &j : myAbj.newObj)
		{
			j->ignoreOutliner = true;
			j->selectable = 0;
			j->ref = true;
			j->v->val_b = false;
			myAbj.allObj.push_back(j);

			if (i.name == "areaLight") myAbj.myAreaLight = j;
			else if (i.name == "dirLight") myAbj.myDirLight = j;
			else if (i.name == "pointLight") myAbj.myPointLight = j;
			else if (i.name == "spotLight") myAbj.mySpotLight = j;
		}
	}
}

void startupScene(string name, Abj &myAbj)
{
	if (name == "BOIT")
	{
		myAbj.newObj = VBOup("999", "POINT", "light", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->t->val_3 = glm::vec3(5.f);
			i->piv->val_3 = i->t->val_3;
			i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
			i->setTarg(i->targO, 0.f);
			i->lInten->val_f = 5.f;
			i->nearShadow->val_f = 1.f;

			myAbj.allObj.push_back(i);
			myAbj.allCamLi.push_back(i);
		}

		/* OPAQUE */
		auto conc = myAbj.pathObj;
		conc.append("teapot.obj");

		myAbj.newObj = VBOup(conc, "OBJ", "teapot", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->metallicM->val_s = "WHITE";
			//i->ruffM->val_s = "white";
			//i->shadowCast->val_b = false;
			i->t->val_3 = glm::vec3(0.f, 0.f, -15.f);
			i->r->val_3 = glm::vec3(0.f, 90.f, 0.f);
			i->s->val_3 = glm::vec3(1.5f);
			myAbj.allObj.push_back(i);
		}

		/* TRANSPARENT - three planes */
		conc = myAbj.pathObj;
		conc.append("plane.obj");

		myAbj.newObj = VBOup(conc, "OBJ", "BOIT_blue", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->albedoM->val_s = "blue";
			i->Ko->val_f = 1.f;
			//i->shadowCast->val_b = false;
			i->t->val_3 = glm::vec3(10.f, 5.f, 0.f);
			i->piv->val_3 = i->t->val_3;
			i->r->val_3 = glm::vec3(90.f, 0.f, 0.f);
			i->s->val_3 = glm::vec3(20.f);
			myAbj.allObj.push_back(i);
		}

		myAbj.newObj = VBOup(conc, "OBJ", "BOIT_yellow", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->albedoM->val_s = "yellow";
			i->Ko->val_f = .75f;
			//i->shadowCast->val_b = false;
			i->t->val_3 = glm::vec3(5.f, 2.5f, -5.f);
			i->piv->val_3 = i->t->val_3;
			i->r->val_3 = glm::vec3(90.f, 0.f, 0.f);
			i->s->val_3 = glm::vec3(20.f);
			myAbj.allObj.push_back(i);
		}

		myAbj.newObj = VBOup(conc, "OBJ", "BOIT_red", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->albedoM->val_s = "red";
			i->Ko->val_f = .75f;
			//i->shadowCast->val_b = false;
			i->t->val_3 = glm::vec3(0.f, 0.f, -10.f);
			i->piv->val_3 = i->t->val_3;
			i->r->val_3 = glm::vec3(90.f, 0.f, 0.f);
			i->s->val_3 = glm::vec3(20.f);
			myAbj.allObj.push_back(i);
		}
	}


	else if (name == "teapotPlane")
	{
		//myFSQ->Kgi->val_f = 0.f;

		myAbj.newObj = VBOup("999", "POINT", "light", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->t->val_3 = glm::vec3(5.f);
			i->piv->val_3 = i->t->val_3;
			i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
			i->setTarg(i->targO, 0.f);
			i->lInten->val_f = 5.f;
			i->nearShadow->val_f = 1.f;

			myAbj.allObj.push_back(i);
			myAbj.allCamLi.push_back(i);
		}

		//myAbj.newObj = VBOup("999", "SPOT", "light", 0, myAbj);
		//for (auto &i : myAbj.newObj)
		//{
		//	i->t->val_3 = glm::vec3(0.f, 8.f, 8.f);
		//	i->piv->val_3 = i->t->val_3;
		//	i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
		//	i->setTarg(i->targO, 0.f);
		//	//i->lSpotO->val_f = 40.f;
		//	i->lInten->val_f = 5.f;
		//	i->nearShadow->val_f = 2.f;

		//	myAbj.allObj.push_back(i);
		//	myAbj.allCamLi.push_back(i);
		//}

		//myAbj.newObj = VBOup("999", "DIR", "light", 0, myAbj);
		//for (auto &i : myAbj.newObj)
		//{
		//	//translation shouldnt matter with DIR light
		//	i->t->val_3 = glm::vec3(8.f, 8.f, 8.f);
		//	//i->t->val_3 = glm::vec3(0.f, 8.f, -8.f);
		//	i->r->val_3 = glm::vec3(-15.f, 0.f, 0.f);
		//	//i->r->val_3 = glm::vec3(0.f, 0.f, 0.f);
		//	//i->r->val_3 = glm::vec3(-1.f, 0.f, 0.f);
		//	i->piv->val_3 = i->t->val_3;
		//	i->Cgiz = glm::vec3(1.f, 0.f, 0.f);
		//	//i->setTarg(i->targO, 0.f);
		//	//i->lSpotO->val_f = 40.f;
		//	i->lInten->val_f = 5.f;
		//	i->nearShadow->val_f = 2.f;

		//	myAbj.allObj.push_back(i);
		//	myAbj.allCamLi.push_back(i);
		//}


		auto conc = myAbj.pathObj;
		conc.append("teapot.obj");

		myAbj.newObj = VBOup(conc, "OBJ", "teapot", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->metallicM->val_s = "WHITE";
			//i->ruffM->val_s = "white";
			//i->shadowCast->val_b = false;
			i->r->val_3 = glm::vec3(0.f, 90.f, 0.f);
			i->s->val_3 = glm::vec3(1.5f);
			myAbj.allObj.push_back(i);
		}

		myAbj.newObj = VBOup(conc, "OBJ", "teapot", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			i->metallicM->val_s = "WHITE";
			//i->ruffM->val_s = "white";
			//i->shadowCast->val_b = false;
			i->r->val_3 = glm::vec3(0.f, 90.f, 0.f);
			i->s->val_3 = glm::vec3(1.5f);
			i->t->val_3 = glm::vec3(10.f, 0.f, 10.f);
			myAbj.allObj.push_back(i);
		}

		conc = myAbj.pathObj;
		conc.append("plane.obj");

		myAbj.newObj = VBOup(conc, "OBJ", "planeD", 0, myAbj);
		for (auto &i : myAbj.newObj)
		{
			//i->ruffM->val_s = "white";
			//i->shadowCast->val_b = false;
			//i->s->val_3 = glm::vec3(20.f);
			i->s->val_3 = glm::vec3(40.f);
			myAbj.allObj.push_back(i);
		}
	}
}

vector<shared_ptr<Object>> VBOup(string path, string type, string name, shared_ptr<Object> dupe, Abj &myAbj)
{
	if (!myAbj.newObj.empty())
		myAbj.newObj.clear();

	vector<shared_ptr<Object>> newO;

	if (path == "999")
	{
		auto obj = (dupe == 0) ? make_shared<Object>(myAbj) : dupe;
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
				// fbl, fbr,  ftr, ftl
				// nbl, nbr, ntr, ntl
				{ -.5f, -.5f, -.5f }, {  .5f, -.5f, -.5f }, {  .5f,  .5f, -.5f }, { -.5f,  .5f, -.5f },
				{ -.5f, -.5f,  .5f }, {  .5f, -.5f,  .5f }, {  .5f,  .5f,  .5f }, { -.5f,  .5f,  .5f },
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
				obj->pE.push_back(glm::vec3(-1.f * sX, 1.f * sY, 0.f));

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
			auto gridLines = myAbj.gridLines + 1;
			auto gridMinus = myAbj.gridLines / 2.f;

			vector<glm::vec3> gridVerts;
			gridVerts.resize(gridLines * gridLines);

			for (int i = 0; i < gridLines; ++i)
			{
				for (int j = 0; j < gridLines; ++j)
				{
					gridVerts[i + j * gridLines].x = (i - gridMinus) / gridMinus;
					gridVerts[i + j * gridLines].y = (j - gridMinus) / gridMinus;
				}
			}

			for (auto &i : gridVerts)
				obj->pE.push_back(i);

			//THICK MIDDLE LINE
			//for (auto &i : gridVerts)
			//{
			//    if (i.z == 0.f)
			//        obj->vboVC.push_back(glm::vec3(0.f));

			//    else
			//        obj->vboVC.push_back(myAbj.attrTable->Cgrid->val_3);
			//}
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

		auto obj = make_shared<Object>(myAbj);
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

			computeTangentBasis(vertices, uvs, normals, tangents, bitangents);
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
		for (auto it = myAbj.GLDataSh.begin(); it != myAbj.GLDataSh.end();)
		{
			if ((*it).obj == i)
				it = myAbj.GLDataSh.erase(it);

			else
				++it;
		}

		myAbj.GLDataSh.push_back({ i, VBO_P, VBO_UV, VBO_T, VBO_N, VBO_IDX });

		i->BBup();
		i->setMM_BB();
		i->AABB_toWS();
	}

	return newO;
}

/* OPENGL-TUTORIAL.ORG */
bool loadNewOBJ(const char *path, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<glm::vec3> temp_vertices;
	vector<glm::vec2> temp_uvs;
	vector<glm::vec3> temp_normals;

	auto *file = fopen(path, "r");

	if (!file)
		return 0;

	while (1) // read the first word of the line
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

void computeTangentBasis(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals, vector<glm::vec3> &tangents, vector<glm::vec3> &bitangents)
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
bool is_near(float v1, float v2)
{
	return fabs(v1 - v2) < .01f;
}

// Searches through all already-exported vertices for a similar one. Similar = same position + same UVs + same normal
bool getSimilarVertexIndex(glm::vec3 &in_vertex, glm::vec2 &in_uv, glm::vec3 &in_normal, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals, unsigned short &result
)
{
	// Lame linear search
	for (unsigned int i = 0; i < out_vertices.size(); ++i)
	{
		if (is_near(in_vertex.x, out_vertices[i].x) && is_near(in_vertex.y, out_vertices[i].y) && is_near(in_vertex.z, out_vertices[i].z) &&
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

void indexVBO_TBN(vector<glm::vec3> &in_vertices, vector<glm::vec2> &in_uvs, vector<glm::vec3> &in_normals, vector<glm::vec3> &in_tangents, vector<glm::vec3> &in_bitangents, vector<unsigned short> &out_indices, vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals, vector<glm::vec3> &out_tangents, vector<glm::vec3> &out_bitangents
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

void lightArrowAdd(shared_ptr<Object> obj, float arrX, float arrY, float arrZ, string orient)
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
