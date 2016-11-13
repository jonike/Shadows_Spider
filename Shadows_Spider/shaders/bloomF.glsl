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
    http://forum.unity3d.com/threads/se-natural-bloom-dirty-lens.243430/
*/

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D bloom_gauss0;
layout(bindless_sampler, location = 1) uniform sampler2D bloom_gauss1;
layout(bindless_sampler, location = 2) uniform sampler2D bloom_gauss2;
layout(bindless_sampler, location = 3) uniform sampler2D bloom_gauss3;
layout(bindless_sampler, location = 4) uniform sampler2D bloom_gauss4;
layout(bindless_sampler, location = 5) uniform sampler2D bloom_gauss5;
layout(bindless_sampler, location = 6) uniform sampler2D def;
layout(bindless_sampler, location = 7) uniform sampler2D lens;

layout(location = 0) out vec4 Ci;

uniform vec4 comboU0; //vec4(bloomInten, bloomLensInten, 0.f, 0.f)

vec3 b0 = texture(bloom_gauss0, v.uv).rgb;
vec3 b1 = texture(bloom_gauss1, v.uv).rgb;
vec3 b2 = texture(bloom_gauss2, v.uv).rgb;
vec3 b3 = texture(bloom_gauss3, v.uv).rgb;
vec3 b4 = texture(bloom_gauss4, v.uv).rgb;
vec3 b5 = texture(bloom_gauss5, v.uv).rgb;
vec4 defT = texture(def, v.uv);
vec4 lensT = texture(lens, v.uv);

void main()
{
    Ci = defT;

    vec3 bloom = (b0*.5f + b1*.6f + b2*.6f + b3*.45f + b4*.35f + b5*.23f) / 2.2f;
    vec3 lensBloom = (b0 + b1*.8f + b2*.6f + b3*.45f + b4*.35f + b5*.23f) / 3.2f;

    Ci.rgb = mix(Ci.rgb, bloom, comboU0.x);

    Ci.r = mix(Ci.r, lensBloom.r, lensT.r * comboU0.y);
    Ci.g = mix(Ci.g, lensBloom.g, lensT.g * comboU0.y);
    Ci.b = mix(Ci.b, lensBloom.b, lensT.b * comboU0.y);
}
