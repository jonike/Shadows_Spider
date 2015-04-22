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
    http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753

    http://anki3d.org/screen-space-ambient-occlusion/
*/

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D gbuf0_64; //P_view;
layout(bindless_sampler, location = 1) uniform usampler2D gbuf2_64; //N_obj;
layout(bindless_sampler, location = 2) uniform sampler2D nRandT;
layout(location = 0) out float Ci;

uniform bool vignette;
uniform float ssaoBias, ssaoInten, ssaoRad;
uniform int ssaoRand;
vec2 screenSize = textureSize(gbuf0_64, 0);

vec3 getPos(in vec2 UV)
{
    return texture(gbuf0_64, UV).rgb;
}

float doSSAO(in vec2 tcoord, in vec2 uv, in vec3 p, in vec3 cnorm)
{
    vec3 diff = getPos(tcoord + uv) - p;
    vec3 v = normalize(diff);
    float d = length(diff);

    return max(0.f, dot(cnorm, v) - ssaoBias) * (ssaoInten / (1.f + d));
}

float ssao()
{
    vec3 P = getPos(v.uv);

    uvec4 data2 = texelFetch(gbuf2_64, ivec2(gl_FragCoord.xy), 0);
    vec3 N = vec3(unpackHalf2x16(data2.y).y, unpackHalf2x16(data2.z));

    vec2 nRand = normalize(texture(nRandT, screenSize * v.uv / ssaoRand).xy * 2.f - 1.f);

    const vec2 samples[16] = vec2[](vec2(0.53812504, 0.18565957), vec2(0.13790712, 0.24864247), vec2(0.33715037, 0.56794053), vec2(-0.6999805, -0.04511441), vec2(0.06896307, -0.15983082), vec2(0.056099437, 0.006954967), vec2(-0.014653638, 0.14027752), vec2(0.010019933, -0.1924225), vec2(-0.35775623, -0.5301969), vec2(-0.3169221, 0.106360726), vec2(0.010350345, -0.58698344), vec2(-0.08972908, -0.49408212), vec2(0.7119986, -0.0154690035), vec2(-0.053382345, 0.059675813), vec2(0.035267662, -0.063188605), vec2(-0.47761092, 0.2847911));

    float ao = 0.f;
    float rad = ssaoRad / P.z;

    for (int i = 0; i < 16; ++i)
    {
        vec2 newCoord = reflect(samples[i], nRand) * rad;

        ao += doSSAO(v.uv, newCoord, P, N);
    }

    return 1.f - (ao / 16.f);
}

void main()
{
    Ci = ssao();
}
