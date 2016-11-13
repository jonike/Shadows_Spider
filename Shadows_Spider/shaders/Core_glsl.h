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

#if defined(GL_core_profile)
#extension GL_ARB_bindless_texture : require
#endif

struct LightData
{
	vec4 Cl_type, falloff, DirRot, P_WS;
	mat4 ShadowCoord;
};

struct SceneData
{
	mat4 PM, PMinv, VM, VMinv, VPM;
	vec4 comboU0; //vec4(Kgi, NUM_LIGHTS, debug0, 0.f)
};

struct DebugData
{
	vec4 comboU0, comboU1;
};

//
//struct ObjData //GBUFFER
//{
//	mat4 MVP, MM, NM; //64
//	sampler2D albedo, alpha, aniso, metallic, normal, ruff, sss, align8_a; //8
//	sampler2D gBuf0, gBuf_DS;
//
//	vec2 texelSize, align8_b; //8
//	vec4 Cs; //16
//	float anisoType, floatPadA, floatPadB, floatPadC;
//	vec4 albedoAlphaTile, metallicRuffTile, normalTile, anisoSssTile;
//	vec4 comboU0; //vec4(ior, ruffOren->val_f, Ko->val_f, debug0)
//	vec4 comboU1; //vec4(ruffA->val_2, shadowCastF, sssSpread->val_f)
//};
//


#if defined(GL_core_profile)
layout(std140, binding = 0) uniform lightBuffer
{
	LightData light[50]; //indexed
};

layout(std140, binding = 1) uniform sceneBuffer
{
	SceneData scene;
};

layout(std140, binding = 2) uniform debugBuffer
{
	DebugData debug;
};

//layout(std140, binding = 1) uniform objectBuffer
//{
//	ObjData object;
//};
//
#endif

