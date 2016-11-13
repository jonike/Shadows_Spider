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

#ifndef CORE_H
#define CORE_H

inline size_t uboAligned(size_t size) { return ((size + 255) / 256) * 256; }
#define arraySize(ar) (sizeof(ar) / sizeof(ar[0]))
#define PI 3.14159f
#define versionNum 3.000

#include <windows.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/compatibility.hpp>
#include <gtx/color_space.hpp>
#include <gtx/component_wise.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <chrono>
#include <random>
#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>
#include <cstddef>

#include "Core_typedefs.h"
#include "../shaders/Core_glsl.h"

using namespace std;

class Object;

typedef struct
{
	string name;
	int width, height;
	GLuint fbo0, tex0_32;
	GLuint64 tex0_64;
	GLuint fbo1, tex1_32;
	GLuint64 tex1_64;
	GLuint DS_32;
	GLuint64 DS_64;
	int idx;
	unsigned int ID;
} AbjNode;

typedef struct
{
	string type, name;
	glm::vec3 t;
} CamSetup;

typedef struct
{
	glm::vec3 v1, v2, v3;
} FPlane;

typedef struct
{
	string name;
	glm::vec3 r;
} GridSetup;

typedef struct
{
	string type, name;
	glm::vec3 Cgiz, r, t;
} GizSetup;

typedef struct
{
	string type, name;
} LightSetup;

typedef struct
{
	string attr;
	float attrVal;
} DynSelCamAttrs;

typedef struct
{
	shared_ptr<Object> obj;
	GLuint VBO_P, VBO_UV, VBO_T, VBO_N, VBO_IDX;
} GLSharedData;

typedef struct
{
	glm::vec3 targ, up;
} CubemapDirections;

//typedef struct
//{
//	string name, type;
//	int sx, sy, offX, offY;
//	QPolygon hoverTri;
//	glm::vec2 move;
//	bool hovered;
//} PopSetup;

//typedef struct
//{
//	QPushButton *button;
//	string name;
//} TableButtons;

typedef struct
{
	int row, col;
} TableRowCol;

typedef struct
{
	string name;
	GLuint pro;
} Pro;

typedef struct
{
	string name, type, flat;
	unsigned int ID;
	vector<AbjNode> layer;
} Map;

typedef struct
{
	string name;
	glm::vec3 scale;
	float opac;
	unsigned int ID;
	glm::vec2 coord;
} Brush;

typedef struct
{
	glm::vec2 coord;
	glm::vec3 scale;
	float opac;
} Stroke;

typedef struct
{
	string name;
	unsigned int ID;
} NameID;

typedef struct
{
	NameID x, y;
} TwoNameID;

typedef struct
{
	string type;
	NameID map, layer;
	vector<TwoNameID> typeMap, mapLayer;
} TexSel;

typedef struct
{
	glm::vec2 mouse, UV;
} MouseToUV;

typedef struct
{
	string type, name;
} DebugUsrPtr;

typedef struct
{
	unsigned int dwSize;
	unsigned int dwFlags;
	unsigned int dwHeight;
	unsigned int dwWidth;
	unsigned int dwPitchOrLinearSize;
	unsigned int dwDepth;
	unsigned int dwMipMapCount;
	unsigned int dwReserved1[11];

	struct
	{
		unsigned int dwSize;
		unsigned int dwFlags;
		unsigned int dwFourCC;
		unsigned int dwRGBBitCount;
		unsigned int dwRBitMask;
		unsigned int dwGBitMask;
		unsigned int dwBBitMask;
		unsigned int dwAlphaBitMask;
	} sPixelFormat;

	struct
	{
		unsigned int dwCaps1;
		unsigned int dwCaps2;
		unsigned int dwDDSX;
		unsigned int dwReserved;
	} sCaps;

	unsigned int dwReserved2;

} ddsHeader;

typedef struct
{
	int width, height;

	vector<Map> allMaps;
	vector<Pro> allPro;
	//vector<shared_ptr<CamCombo>> allCamCombo;
	vector<shared_ptr<Object>> allCamLi, allGiz, allGizSide, allGrid, allObj, newObj, frustumObjs;

	vector<AbjNode> allShadow;
	vector<shared_ptr<Brush>> allBrushes;

	shared_ptr<Object> selB, selCamLi, myBB, myTxt, myFSQ, myPivot, myGizNull, myAreaLight, myDirLight, myPointLight, mySpotLight, myVolCone, paintStroke, mySelRect, shadowObj;
	shared_ptr<Brush> selBrush, selEraser;
	vector<GLSharedData> GLDataSh;

	float gamma, gizScale;
	string pathGLSL, pathObj, pathTex, gizSpace;
	int lightCt, brushSize, brushHard;
	GLuint cubeM_specular_32, cubeM_irradiance_32;

	GLuint pro, uboLight, uboScene, uboDebug;
	GLuint64 uboLight_64, uboScene_64, uboDebug_64;
	string proN;

	//timer (make sep struct)
	bool mpfTgl;
	string mpf;
	unsigned int tick_frames;
	double tick_new, tick_old, tick_diff;
	float mpfTime, dTime;

	bool searchB;
	bool lmbTgl, mmbTgl, rmbTgl, altTgl, ctrlTgl, shiftTgl, spaceTgl;
	bool aTgl, bTgl, cTgl, dTgl, eTgl, fTgl, gTgl, hTgl, iTgl, jTgl, kTgl, lTgl, mTgl, nTgl, oTgl, pTgl, qTgl, rTgl, sTgl, tTgl, uTgl, vTgl, wTgl, xTgl, yTgl, zTgl;
	bool colorPickTgl, disableSelRect, gizSideTgl, gizSpaceTgl, rezGateTgl, rezGateTgl_sel, singleShotRC, statsTgl, wireOverTgl;

	//FBO
	bool fboReady;
	int currLum;

	GLuint gBuf0_32, gBuf1_32, gBuf2_32, gBuf3_32, gBuf4_32, gBuf5_32, gBuf6_32, gBuf7_32, gBuf_DS_32;
	GLuint64 gBuf0_64, gBuf1_64, gBuf2_64, gBuf3_64, gBuf4_64, gBuf5_64, gBuf6_64, gBuf7_64, gBuf_DS_64;

	GLuint rttGaussIn32, tempGauss;
	GLuint64 rttGaussIn64, downSamp_64, extractHLfromSrc_64;

	AbjNode gBufN, bloomCN, bloomN, bloomGaussN[6], deferredN, depthRevN, downN[6], fxaaN, lumaAdaptN[2], lumaInitN, bgN, ssaoN, ssaoGaussN, ssrN, tonemapN, alphaGaussN;
	AbjNode brushN, brushBGN, brushTempN, eraserN, cursorN, sobelN;
	bool bakeNow;

	glm::mat4 PMd, PMstored, PM_cube, VM_cube, MM_cube;

	float tabletPressure;
	string penOrientation, paintType;
	bool paintMode, dragDrop;
	int edgeDetect_mode;

	//GL
	float aspect, aspectSide, NDC_x, NDC_y, rayIntersectDist, fpsMouseSpeed, tumbleSpeed, panSpeed, panSpeed_ortho, dollySpeed, dollySpeed_ortho;
	glm::mat4 VMgizSide, PM, PMgizSide, PMinv_cube;
	glm::vec2 pMouseNew, pMouseNew_stored, pMouseOld, pMouseDiff, rayP, texelSize;
	glm::vec2 selRect_LD, selRect_RU, rezGate_LD, rezGate_RU;
	glm::vec3 upWorld, from_OBB, to_OBB, aspectXYZ;
	vector<Stroke> strokes_brush, strokes_eraser, strokes_brush_cursor, strokes_eraser_cursor, strokes_cursor;
	string BOIT_name_D, selMode;

	glm::vec3 gizSideS, Csel, CselManip, Ctxt, Cwire, Cgrid;
	int gridLines;

	bool UBO_light_needsUp, brushOutlineUpB, debug0;
	int copyTgt;
	string pDefDyn;

	glm::vec3 brushRGB;
	glm::vec4 brushRGBA;
	float brushA;

	vector<glm::vec2> selRectPts_color, selRectPts_usable;

	string gizHoverType, gizTransType;
	glm::vec3 gizN, gizMouseDown, giz_rayOrigin, giz_rayDir, gizHoverCheckX, gizHoverCheckY, gizHoverCheckZ;

	int blendModeD, selAccuracy;
	bool moveDist_paint, firstPress, doEraserDebug, paintCursorResizeTgl;
	float distDragPaint;
	glm::vec2 paintCursorResize_p;
	glm::vec2 P_currF, P_prevF;
	Map myLayerIdx, sobelMap;


} Abj;




class MultiAttr
{
public:
	MultiAttr() { ; }
	shared_ptr<MultiAttr> Clone() const { return(shared_ptr<MultiAttr>(CloneImpl())); }

	string typeX = "ALL";
	string grp, name, type, val_s;

	bool repop = true;
	bool val_b;
	int val_i, idx_3;
	int tab = 0;
	int idx;
	float val_f;
	float min = -9999.f;
	float max = 9999.f;
	glm::vec2 val_2;
	glm::vec3 val_3;
	glm::vec4 val_4;
	vector<string> comboList;
	string cut;

//protected:
	virtual MultiAttr* CloneImpl() const { return(new MultiAttr(*this)); }
};


#include "Object.h"
#include "StringManip.h"
#include "Input.h"
#include "Setup_texShaders.h"
#include "Setup_obj.h"
#include "GL_render.h"

#endif