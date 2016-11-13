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
    http://casual-effects.blogspot.com/2014/03/weighted-blended-order-independent.html
    http://casual-effects.blogspot.com/2015/03/implemented-weighted-blended-order.html
    http://casual-effects.blogspot.com/2015/03/colored-blended-order-independent.html
    http://jcgt.org/published/0002/02/09/
*/

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

in Vert
{
    vec2 UV;
    float myDepth;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D albedo;
layout(bindless_sampler, location = 1) uniform sampler2D alpha;

layout(location = 0) out vec4 sumColor;
layout(location = 1) out float sumWeight;

uniform vec4 comboU0; //vec4(depthScale, debug0, Ko->val_f, 0.f)
uniform vec4 albedoAlphaTile, metallicRuffTile, normalTile, anisoSssTile;

void main()
{
    vec3 albedoT = texture(albedo, albedoAlphaTile.rg * v.UV).rgb;
    albedoT = pow(albedoT, vec3(2.2f)); //gamma space input tex req decoding

    float alphaT = texture(alpha, albedoAlphaTile.ba * v.UV).r * comboU0.z;

    //eq 9
    //if the PM was a perspective projection, can use gl_FragCoord.w
    //otherwise, use depth from v

    float viewDepth = v.myDepth;
//    float viewDepth = abs(1.f / gl_FragCoord.w);

    float linearDepth = viewDepth * comboU0.r;
    float weight = clamp(.03f / (1e-5 + pow(linearDepth, 4.f)), 1e-2, 3e3);

    vec4 Cs = vec4(albedoT, alphaT);
    sumColor = vec4(Cs.rgb * Cs.a, Cs.a) * weight;
    sumWeight = Cs.a;
}
