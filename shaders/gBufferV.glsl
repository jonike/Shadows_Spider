/*

Copyright 2015 Aleks Berg-Jones

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

#version 450 core
#extension GL_ARB_bindless_texture : require

out Vert
{
    vec2 UV;
    vec3 T_VS, N_VS;
    vec4 shadowCoord0, shadowCoord1;
} v;

struct light_t
{
    vec4 Cl_type, falloff, lDirRot, lP;
    mat4 ShadowCoord;
};

layout (std140, binding = 0) uniform lightUBO
{
    light_t light[50];
};

layout(location = 0) in vec3 pE;
layout(location = 1) in vec2 uvE;
layout(location = 2) in vec3 tE;
layout(location = 3) in vec3 nE;

uniform mat3 NM;
uniform mat4 MVP;
uniform int NUM_LIGHTS;

void main()
{
    gl_Position = MVP * vec4(pE, 1.f);

    v.UV = uvE;
    v.T_VS = normalize(NM * tE);
    v.N_VS = normalize(NM * nE);

    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        if (i == 0)
            v.shadowCoord0 = light[i].ShadowCoord * vec4(pE, 1.f);

        if (i == 1)
            v.shadowCoord1 = light[i].ShadowCoord * vec4(pE, 1.f);
    }
}
