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

/*
reference:
    http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
*/

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D tex;
layout(location = 0) out vec4 Ci;

uniform int gaussStage;

float offset[3] = float[3](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[3](0.2270270270, 0.3162162162, 0.0702702703);
vec2 texelSize = 1.f / textureSize(tex, 0);

void main()
{
    Ci = texture(tex, v.uv) * weight[0];

    for (int i = 1; i < 3; ++i)
    {
        if (gaussStage == 0) //vertical
        {
            Ci += texture(tex, v.uv + vec2(0.f, offset[i]) * texelSize) * weight[i];
            Ci += texture(tex, v.uv - vec2(0.f, offset[i]) * texelSize) * weight[i];
        }

        else if (gaussStage == 1) //horizontal
        {
            Ci += texture(tex, v.uv + vec2(offset[i], 0.f) * texelSize) * weight[i];
            Ci += texture(tex, v.uv - vec2(offset[i], 0.f) * texelSize) * weight[i];
        }
    }
}
