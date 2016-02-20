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

/*
reference:
    http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
    http://anki3d.org/screen-space-ambient-occlusion/
    https://mynameismjp.wordpress.com/2009/03/10/reconstructing-position-from-depth/
    http://www.learnopengl.com/#!Advanced-Lighting/SSAO
*/

#version 450 core
#extension GL_ARB_bindless_texture : require

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform usampler2D gBuf_1; //N_VS
layout(binding = 1) uniform sampler2D gBuf_DS;
layout(bindless_sampler, location = 2) uniform sampler2D noise;

layout(location = 0) out float Ci;

uniform vec4 comboU0; //vec4(ssaoBias, ssaoInten, ssaoRad)
uniform mat4 PM, PMinv;

vec3 reconstructP(vec2 UV)
{
    vec4 vProjectedPos = vec4(1.f);
    vProjectedPos.rg = UV * 2.f - 1.f;
    vProjectedPos.b = texture(gBuf_DS, UV).r * 2.f - 1.f;
    vec4 vPositionVS = PMinv * vProjectedPos;

    return vPositionVS.rgb / vPositionVS.a;
}

const vec3 kernel32[32] = vec3[](
    vec3(0.049771, -0.044709, 0.049963),
    vec3(0.014670, 0.016640, 0.002253),
    vec3(-0.041710, -0.019881, 0.032768),
    vec3(0.014580, -0.096910, 0.043305),
    vec3(0.061694, 0.065883, 0.063534),
    vec3(0.106679, 0.051196, 0.017865),
    vec3(-0.002488, -0.066365, 0.081411),
    vec3(-0.000427, -0.000242, 0.000477),
    vec3(0.068554, -0.063904, 0.034774),
    vec3(0.055410, 0.045635, 0.047766),
    vec3(-0.049113, 0.031515, 0.034683),
    vec3(0.090800, -0.060271, 0.088827),
    vec3(0.098741, -0.038793, 0.130012),
    vec3(-0.029161, -0.006830, 0.100537),
    vec3(-0.047639, -0.047251, 0.047484),
    vec3(-0.067123, 0.042622, 0.050614),
    vec3(-0.036462, 0.029922, 0.011124),
    vec3(0.158832, 0.242610, 0.023848),
    vec3(-0.099019, -0.202919, 0.188084),
    vec3(-0.193788, -0.003917, 0.197766),
    vec3(-0.025008, -0.078990, 0.046319),
    vec3(0.007952, -0.012090, 0.010311),
    vec3(-0.018799, -0.167624, 0.171605),
    vec3(0.245956, -0.020897, 0.374521),
    vec3(0.205594, 0.339749, 0.286315),
    vec3(0.001074, 0.001230, 0.000825),
    vec3(-0.292679, 0.182783, 0.284144),
    vec3(-0.012673, -0.340605, 0.460926),
    vec3(-0.216063, 0.099839, 0.649584),
    vec3(-0.008128, 0.009069, 0.008612),
    vec3(-0.324718, 0.425938, 0.498051),
    vec3(0.142327, 0.314624, 0.180863)
);

void main()
{
    vec3 P_VS = reconstructP(v.uv);

    uvec4 data2 = texelFetch(gBuf_1, ivec2(gl_FragCoord.xy), 0);
    vec3 N_VS = vec3(unpackHalf2x16(data2.y).y, unpackHalf2x16(data2.z));

    vec2 screenSize = textureSize(gBuf_1, 0);
    vec3 randomVec = normalize(texture(noise, v.uv * screenSize / 4).rgb * 2.f - 1.f);

    // Create TBN change-of-basis matrix: from tangent-space to view-space
//    vec3 tangent = normalize(randomVec - N_VS * dot(randomVec, N_VS));
    vec3 tangent = randomVec - N_VS * dot(randomVec, N_VS);
    vec3 bitangent = cross(N_VS, tangent);
    mat3 TBN = mat3(tangent, bitangent, N_VS);

    float ao = 0.f;

    for (int i = 0; i < 32; ++i)
    {
        vec3 sampleP = TBN * kernel32[i]; // From tangent to view-space
        sampleP = sampleP * comboU0.z + P_VS;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = PM * vec4(sampleP, 1.f); // from view to clip-space
        offset.xy /= offset.w; // perspective divide
        offset.xy = offset.xy * .5f + .5f; // transform to range 0.f - 1.f

        float sampleDepth = reconstructP(offset.xy).z;
        float rangeCheck = smoothstep(0.f, 1.f, comboU0.z / abs(P_VS.z - sampleDepth));

        ao += (sampleDepth - comboU0.x >= sampleP.z ? 1.f : 0.f) * rangeCheck * comboU0.y;
    }

    Ci = 1.f - (ao / 32.f);
}
