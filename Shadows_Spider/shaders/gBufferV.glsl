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

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

out Vert
{
    vec2 UV;
    vec3 T_VS, N_VS;
    vec4 shadowCoord0;
} v;

layout(location = 0) in vec3 pE;
layout(location = 1) in vec2 uvE;
layout(location = 2) in vec3 tE;
layout(location = 3) in vec3 nE;

uniform mat3 NM;
uniform mat4 MVP, MM;
uniform int NUM_LIGHTS;

vec4 P_WS = MM * vec4(pE, 1.f);

float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 100.f;
    const float n = 1.f;

    float NormZComp = (f + n) / (f - n) - (2.f * f * n) / (f - n) / LocalZcomp;

    return (NormZComp + 1.f) * .5f;
}

vec4 getShadowCoords(int idx)
{
    vec4 myShadowCoords;

    if (light[idx].Cl_type.w == 2.f) //POINT
    {
        vec3 lightDir = P_WS.xyz - light[idx].P_WS.xyz;

        myShadowCoords = vec4(lightDir, VectorToDepth(lightDir));
    }

    else //DIR / SPOT
        myShadowCoords = light[idx].ShadowCoord * vec4(pE, 1.f);

    return myShadowCoords;
}

void main()
{
    gl_Position = MVP * vec4(pE, 1.f);

    v.UV = uvE;
    v.T_VS = normalize(NM * tE);
    v.N_VS = normalize(NM * nE);

    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        if (i == 0)
            v.shadowCoord0 = getShadowCoords(i);
    }
}
