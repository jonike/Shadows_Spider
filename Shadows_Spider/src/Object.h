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

#ifndef OBJECT_H
#define OBJECT_H

#include "Core.h"

class Object
{
	Abj &myAbj;
public:
	Object(Abj &);
	//Object(const Object &);
	//shared_ptr<Object> Clone() const { return(shared_ptr<Object>(CloneImpl())); }
	//virtual Object* CloneImpl() const { return(new Object(*this)); }

	bool VAO_loaded;
	GLuint VAO;
	vector<glm::vec3> pE, nE, tE;
	vector<glm::vec2> uvE;
	vector<GLushort> idxE;
	shared_ptr<Object> parentTo;

	vector<shared_ptr<MultiAttr>> multiObj;
	vector<MouseToUV> mouseToUV;

	shared_ptr<MultiAttr> name, transformSep, t, r, s, v, bb;
	shared_ptr<MultiAttr> shaderSep, albedoM, alphaM, anisoM, cubeM, lensM, metallicM, normalM, ruffM, sssM;
	shared_ptr<MultiAttr> Ko, ior, ruffA, ruffOren, sssSpread;
	shared_ptr<MultiAttr> shadowCast, twoSided, Cwire, normWt, piv, rotOrder;
	shared_ptr<MultiAttr> tileSep;

	shared_ptr<MultiAttr> camLiType;
	shared_ptr<MultiAttr> camSep, fov, nearClip, farClip, nearShadow, farShadow, orthoFree, orthoType, orthoZoom;

	shared_ptr<MultiAttr> lightSep, Cl, lInten, lSpotI, lSpotO;
	shared_ptr<MultiAttr> volSep, volCone, volDist, volS;

	shared_ptr<MultiAttr> bloomSep, bloomInten, bloomLensInten;
	shared_ptr<MultiAttr> hdrSep, adaptTime, adaptAuto, expo, Kgi, vign, vignDist;
	shared_ptr<MultiAttr> fxaaSep, fxaaBlur, fxaaSubPix, fxaaEdgeThr, fxaaEdgeThrMin;
	shared_ptr<MultiAttr> ssaoSep, ssaoBias, ssaoInten, ssaoKernel, ssaoRad;
	shared_ptr<MultiAttr> paintSep, clearBehav, displMode, edgeThr;
	shared_ptr<MultiAttr> ssrSep, ssrIter, ssrRefine, ssrPixStride, ssrPixZSize, ssrPixStrideZ, ssrMaxRayDist, ssrEdgeFade, ssrEyeFade0, ssrEyeFade1;
	shared_ptr<MultiAttr> transpSep, transpW;

	float distO;
	glm::vec3 lookO, rightO, upO, targO, lookFPS, rightFPS, upFPS;

	bool dirtyShadow, dirtyVM, expand, ignoreOutliner, ref, selected, deletable, gridV, selectable;

	int vertsOnObj;
	string dupeStenFix = "999";
	string type;

	glm::mat4 biasM, PM2D;
	glm::vec3 Crand;
	bool showN = false;
	int gaussStage;
	TexSel texSel;

	glm::vec3 Cgiz, Cgiz_stored;
	string gizType;
	bool gizSideObj = false;

	//frustum D
	glm::vec3 nc, ntl, ntr, nbl, nbr, fc, ftl, ftr, fbl, fbr;
	float Hnear, Wnear, Hfar, Wfar;
	vector<FPlane> fPlanes_temp;
	glm::vec4 fPlanes[6];
	vector<glm::vec3> AABB_WS;
	void setMM_BB();
	void AABB_toWS();
	void buildFrustumPlanes();
	bool isAABBinFrustum();
	//void frustumCull();

	const char *txt2D;
	glm::vec2 txtOrigin;

	glm::mat3 NM;
	glm::mat4 MVP, MV, MM, PM, VM, TM, RM, SM;
	glm::mat4 bbScaleM, bbrotM, bbtransM, obbMVP;
	glm::mat4 pivM, pivRtM, rpM, rM;
	glm::vec3 bbMin, bbMax, bbCenter, bbSize, bbSizeFull;
	glm::vec3 pivRt;

	void VAO_load();
	void render();
	void shadowPass();

	void BBup();
	//void deleteVAO_VBO();
	void glErrorPrint(const char *);
	void rename(string);
	void parentObj(string);
	void setTarg(glm::vec3, float);
	bool camLiTypeGet(string);
	void setDirty();
	glm::mat4 rotOrderUse(string);
	void tileMaps(GLuint, string);
	void mvpGet();
	//void populateTexCombos();
	//void setTexSel_init();



protected:
	void proUse();
//
	glm::vec3 genColorID();
	string getIncName(string);
};


#endif
