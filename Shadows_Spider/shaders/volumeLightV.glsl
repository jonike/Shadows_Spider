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

layout(location = 0) in vec3 pE;
layout(location = 1) in vec2 uvE;
layout(location = 2) in vec3 tE;
layout(location = 3) in vec3 nE;

uniform mat3 NM;
uniform mat4 MVP, MV;
uniform vec4 parentCl;

out Vert
{
    vec2 uv;
    vec3 parentCl, n, p, i;
} v;

void main()
{
    gl_Position = MVP * vec4(pE, 1.f);
    v.p = vec3(MV * vec4(pE, 1.f));

    v.n = normalize(NM * nE);

    v.parentCl = parentCl.rgb;
    v.uv = uvE;

    v.i = vec3(MV * vec4(pE, 1.f));
}
