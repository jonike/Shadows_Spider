/*

Copyright 2015 Aleksander Berg-Jones

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

/*
reference:
    http://forum.unity3d.com/threads/se-natural-bloom-dirty-lens.243430/
*/

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
layout(bindless_sampler, location = 6) uniform sampler2D defT;
layout(bindless_sampler, location = 7) uniform sampler2D lens;

layout(location = 0) out vec4 Ci;

uniform float bloomInten, bloomLensInten;

void main()
{
    vec3 b0 = texture(bloom_gauss0, v.uv).rgb;
    vec3 b1 = texture(bloom_gauss1, v.uv).rgb;
    vec3 b2 = texture(bloom_gauss2, v.uv).rgb;
    vec3 b3 = texture(bloom_gauss3, v.uv).rgb;
    vec3 b4 = texture(bloom_gauss4, v.uv).rgb;
    vec3 b5 = texture(bloom_gauss5, v.uv).rgb;
    vec4 lensT = texture(lens, v.uv);

    Ci = texture(defT, v.uv);

    vec3 bloom = (b0*.5f + b1*.6f + b2*.6f + b3*.45f + b4*.35f + b5*.23f) / 2.2f;
    vec3 lensBloom = (b0 + b1*.8f + b2*.6f + b3*.45f + b4*.35f + b5*.23f) / 3.2f;

    Ci.rgb = mix(Ci.rgb, bloom, bloomInten);

    Ci.r = mix(Ci.r, lensBloom.r, lensT.r * bloomLensInten);
    Ci.g = mix(Ci.g, lensBloom.g, lensT.g * bloomLensInten);
    Ci.b = mix(Ci.b, lensBloom.b, lensT.b * bloomLensInten);
}
