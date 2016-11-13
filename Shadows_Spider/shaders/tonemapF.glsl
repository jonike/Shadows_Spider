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
    http://content.gpwiki.org/index.php/D3DBook:High-Dynamic_Range_Rendering#Luminance_History_Function e10 - auto exp
*/

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D bloomC;
layout(bindless_sampler, location = 1) uniform sampler2D luma;
layout(bindless_sampler, location = 2) uniform sampler2D giz;
layout(location = 0) out vec3 Ci;
layout(location = 1) out vec3 Ci_noGiz;

uniform vec4 comboU0; //vec4(expo, adaptAuto, vign, vignDist)

float log10(float x)
{
    return log2(x) / log2(10);
}

vec3 filmicHabel(vec3 x)
{
    float A = .15f;
    float B = .5f;
    float C = .1f;
    float D = .2f;
    float E = .02f;
    float F = .3f;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float vignetteOverlay(vec2 pos, float inner, float outer)
{
    float r = length(pos);

    return 1.f - smoothstep(inner, outer, r);
}

//void main()
//{
//    vec4 bloomCT = texture(bloomC, v.uv);
//    Ci = bloomCT.rgb;

//    vec4 gizT = texture(giz, v.uv);
//    Ci = mix(Ci.rgb, gizT.rgb, gizT.a);
//}

void main()
{
    vec4 bloomCT = texture(bloomC, v.uv);
    float lumaT = max(exp(textureLod(luma, v.uv, 10.f).r), .0001f);

    if (comboU0.z == 1.f)
        bloomCT.rgb *= vignetteOverlay(v.uv * 2.f - 1.f, .55f, comboU0.w);

    float expoAdapt = comboU0.x;

    if (comboU0.y == 1.f)
        expoAdapt = 1.03f - (2.f / (2.f + log10(lumaT + 1.f)));

    Ci = bloomCT.rgb * expoAdapt / lumaT;
    Ci = filmicHabel(Ci) / filmicHabel(vec3(11.2f));
    Ci_noGiz = Ci;

    vec4 gizT = texture(giz, v.uv);
    Ci = mix(Ci.rgb, gizT.rgb, gizT.a);
}
