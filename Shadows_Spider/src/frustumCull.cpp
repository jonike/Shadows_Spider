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
#include "Object.h"

//////////////
/* SETUP (AFTER VMUP) */
//////////////

void Object::BBup()
{
	bbMin = (pE.empty()) ? glm::vec3(0.f) : pE[0];
	bbMax = (pE.empty()) ? glm::vec3(0.f) : pE[0];

	for (auto &i : pE)
	{
		bbMin = glm::min(i, bbMin);
		bbMax = glm::max(i, bbMax);
	}

	bbSize = bbMax - bbMin;
	bbCenter = .5f * (bbMin + bbMax);
	obbMVP = glm::translate(glm::mat4(), bbCenter) * glm::scale(glm::mat4(), bbSize);
}

void Object::setMM_BB()
{
	if (this != myAbj.paintStroke.get())
	{
		SM = glm::scale(glm::mat4(), s->val_3);

		if (type == "CAMLI" && camLiType->val_s != "DIR")
			RM = glm::yawPitchRoll(glm::radians(r->val_3.x), glm::radians(r->val_3.y), 0.f);

		else
			RM = rotOrderUse(rotOrder->val_s);

		TM = glm::translate(glm::mat4(), t->val_3);

		//    if (this == myAbj.paintStroke.get())
		//    {
		//        PM2D = glm::ortho(-1.f * myGL->aspect, 1.f * myGL->aspect, -1.f, 1.f, -1.f, 1.f);
		//        TM = glm::translate(glm::mat4(), t->val_3 * myGL->aspectXYZ);
		//    }

		if (parentTo != 0)
		{
			if (name->val_s == "giz")
			{
				if (myAbj.gizSpace == "local")
					RM = parentTo->RM;

				SM = glm::scale(glm::mat4(), glm::vec3(myAbj.gizScale));
			}

			if (name->val_s == "pivot" || name->val_s == "giz")
			{
				//TM = glm::translate(glm::mat4(), parentTo->piv->val_3);
				TM = glm::translate(glm::mat4(), parentTo->t->val_3); //
			}
		}

		//PIV
		pivM = glm::translate(glm::mat4(), t->val_3 - piv->val_3);
		pivRtM = glm::translate(glm::mat4(), pivRt);

		//MM
		//    if (piv->val_3 == glm::vec3(0.f)) MM = TM * RM * SM;
		//    else MM = TM * pivRtM * glm::inverse(pivM) * RM * SM * pivM;
		MM = TM * RM * SM;
	}
}

void Object::AABB_toWS()
{
	AABB_WS =
	{
		//near (maxZ)
		{ bbMin.x, bbMin.y, bbMax.z },
		{ bbMax.x, bbMin.y, bbMax.z },
		{ bbMax.x, bbMax.y, bbMax.z },
		{ bbMin.x, bbMax.y, bbMax.z },

		//far (minZ)
		{ bbMin.x, bbMin.y, bbMin.z },
		{ bbMax.x, bbMin.y, bbMin.z },
		{ bbMax.x, bbMax.y, bbMin.z },
		{ bbMin.x, bbMax.y, bbMin.z },
	};

	//    for (int i = 0; i < 8; ++i)
	//        cout << i <<  " OS = " << glm::to_string(AABB_WS[i]) << endl;

	// transform into WS
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 3; ++j)
			AABB_WS[i][j] = (AABB_WS[i].x * MM[0][j]) + (AABB_WS[i].y * MM[1][j]) + (AABB_WS[i].z * MM[2][j]) + MM[3][j];
	}

	//    for (int i = 0; i < 8; ++i)
	//        cout << i <<  " WS = " << glm::to_string(AABB_WS[i]) << endl;
}

//////////////
/* PER FRAME (GL_RENDER)*/
//////////////
bool Object::isAABBinFrustum()
{
	for (int i = 0; i < 6; ++i)
	{
		bool inside = false;

		for (int j = 0; j < 8; ++j)
		{
			if (glm::dot(AABB_WS[j], glm::vec3(myAbj.selCamLi->fPlanes[i])) + myAbj.selCamLi->fPlanes[i].w > 0.f)
			{
				inside = true;
				break;
			}
		}

		if (inside == false)
			return false;
	}

	return true;
}

void Object::buildFrustumPlanes()
{
	//NEAR
	nc = t->val_3 + lookO * nearClip->val_f;
	Hnear = 2.f * tan(glm::radians(fov->val_f / 2.f)) * nearClip->val_f;
	Wnear = Hnear * myAbj.aspect;
	ntl = nc + (upO * Hnear / 2.f) - (rightO * Wnear / 2.f);
	ntr = nc + (upO * Hnear / 2.f) + (rightO * Wnear / 2.f);
	nbl = nc - (upO * Hnear / 2.f) - (rightO * Wnear / 2.f);
	nbr = nc - (upO * Hnear / 2.f) + (rightO * Wnear / 2.f);

	//FAR
	fc = t->val_3 + lookO * farClip->val_f;
	Hfar = 2.f * tan(glm::radians(fov->val_f / 2.f)) * farClip->val_f;
	Wfar = Hfar * myAbj.aspect;
	ftl = fc + (upO * Hfar / 2.f) - (rightO * Wfar / 2.f);
	ftr = fc + (upO * Hfar / 2.f) + (rightO * Wfar / 2.f);
	fbl = fc - (upO * Hfar / 2.f) - (rightO * Wfar / 2.f);
	fbr = fc - (upO * Hfar / 2.f) + (rightO * Wfar / 2.f);

	fPlanes_temp =
	{
		{ ntr, ntl, ftl }, //U
		{ nbl, nbr, fbr }, //D
		{ ntl, nbl, fbl }, //L
		{ nbr, ntr, fbr }, //R
		{ ntl, ntr, nbr }, //N
		{ ftr, ftl, fbl }, //F
	};

	for (int i = 0; i < 6; ++i)
	{
		glm::vec3 e1 = fPlanes_temp[i].v2 - fPlanes_temp[i].v1;
		glm::vec3 e2 = fPlanes_temp[i].v3 - fPlanes_temp[i].v1;

		glm::vec3 N = glm::normalize(glm::cross(e1, e2));
		float d = -glm::dot(N, fPlanes_temp[i].v2);

		fPlanes[i] = glm::vec4(N, d);
	}
}