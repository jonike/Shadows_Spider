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

in Vert
{
    vec2 uv;
    vec3 N_VS;
    vec4 bakeCoord;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D paintStroke;
layout(bindless_sampler, location = 1) uniform sampler2D layerOld;
layout(location = 0) out vec4 Ci;

void main()
{
    vec3 V_VS = normalize(scene.VM * vec4(10.f, 10.f, 10.f, 0.f)).xyz;
    vec4 paintStrokeT = textureProj(paintStroke, v.bakeCoord);
    vec4 layerOldT = texture(layerOld, v.uv);

    if (v.bakeCoord.q > 0.f) // prevents reverse projection
    {
        Ci = mix(layerOldT, paintStrokeT, paintStrokeT.a);

//        if (dot(v.N_VS, V_VS) > 0.f) //NdotV prevents backfacing proj
//        {
//            Ci = mix(layerOldT, paintStrokeT, paintStrokeT.a);
//        }
    }

    else //set original tex w/o stroke projection
    {
        Ci = layerOldT;
    }

    Ci.a = 1.f;
}
