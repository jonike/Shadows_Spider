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
Copyright (c) 2014, Morgan McGuire and Michael Mara
Copyright (c) 2015, Ben Hopkins (kode80)
All rights reserved.

From McGuire and Mara, Efficient GPU Screen-Space Ray Tracing,
Journal of Computer Graphics Techniques, 2014

From http://www.kode80.com/blog/2015/03/11/screen-space-reflections-in-unity-5/

This software is open source under the "BSD 2-clause license":

Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following
conditions are met:

1. Redistributions of source code must retain the above
copyright notice, this list of conditions and the following
disclaimer.

2. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
reference:
    http://casual-effects.blogspot.com/2014/08/screen-space-ray-tracing.html
    http://www.kode80.com/blog/2015/03/11/screen-space-reflections-in-unity-5/
    http://www.gamedev.net/topic/638355-screen-space-reflections-issues/
*/

#version 450 core
#extension GL_ARB_bindless_texture : require

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform usampler2D gBuf2; //N_VS;
layout(bindless_sampler, location = 1) uniform usampler2D gBuf3; //ruffM
layout(binding = 2) uniform sampler2D gBuf_DS;
layout(bindless_sampler, location = 3) uniform sampler2D depthRev_DS;
layout(bindless_sampler, location = 4) uniform sampler2D tonemap_noGiz;

layout(location = 0) out vec4 Ci;

uniform mat4 PM, PMinv, PM_SS_d3d, VM;
vec2 screenSize = textureSize(gBuf2, 0);

uniform vec4 comboU0; //vec4(farClip, nearClip, ssrIter, ssrRefine)
uniform vec4 comboU1; //vec4(ssrPixStride, ssrPixStrideZ, ssrPixZSize, ssrMaxRayDist)
uniform vec4 comboU2; //vec4(ssrEdgeFade, ssrEyeFade0, ssrEyeFade1, debug0)

uvec4 data3 = texelFetch(gBuf3, ivec2(gl_FragCoord.xy), 0);
float ruffM = unpackHalf2x16(data3.z).x;

vec3 reconstructP(vec2 UV)
{
    vec4 vProjectedPos = vec4(1.f);
    vProjectedPos.rg = UV * 2.f - 1.f;
    vProjectedPos.b = texture(gBuf_DS, UV).r * 2.f - 1.f;
    vec4 vPositionVS = PMinv * vProjectedPos;

    return vPositionVS.rgb / vPositionVS.a;
}

float Linear01Depth(float myDepth)
{
    vec2 _ZBufferParams;
    float fn = comboU0.x / comboU0.y;

    _ZBufferParams.x = 1.f - fn;
    _ZBufferParams.y = fn;

    return 1.f / (_ZBufferParams.x * myDepth + _ZBufferParams.y);
}

float distanceSquared(vec2 a, vec2 b)
{
    a -= b;
    return dot(a, a);
}

void swapIfBigger(inout float aa, inout float bb)
{
    if (aa > bb)
    {
        float tmp = aa;
        aa = bb;
        bb = tmp;
    }
}

bool rayIntersectsDepthBF(float zA, float zB, vec2 UV)
{
    float cameraZ = Linear01Depth(texture(gBuf_DS, UV).r) * -comboU0.x;
    float backZ = Linear01Depth(texture(depthRev_DS, UV).r) * -comboU0.x;

    return zB <= cameraZ && zA >= backZ - comboU1.z;
}

float calculateAlphaForIntersect(bool intersect, float iterCt, vec2 hitPix, vec3 hitPt, vec3 rayOrigin_VS, vec3 rayDir_VS)
{
    float alpha = min(1.f, 1.f - ruffM);
    alpha *= 1.f - (iterCt / comboU0.z); //fade ray hits that approach the max iters

    //fade ray hits that approach the screen edge
    float screenFade = comboU2.x;
    vec2 hitPixelNDC = (hitPix * 2.f - 1.f);
    float maxDim = min(1.f, max(abs(hitPixelNDC.x), abs(hitPixelNDC.y)));
    alpha *= 1.f - (max(0.f, maxDim - screenFade) / (1.f - screenFade));

    //fade ray hits based on how much they face the camera
    float eyeDir = clamp(rayDir_VS.z, comboU2.y, comboU2.z);
    alpha *= 1.f - ((eyeDir - comboU2.y) / (comboU2.z - comboU2.y));

    //fade ray hits based on dist from ray origin
    alpha *= 1.f - clamp(distance(rayOrigin_VS, hitPt) / comboU1.w, 0.f, 1.f);
    alpha *= int(intersect);

    return alpha;
}

// Trace a ray in screen-space from rayOrigin (in camera space) pointing in rayDirection (in camera space) using jitter to offset the ray based on (jitter * ssrPixStride). Returns true if the ray hits a pixel in the depth buffer and outputs the hitPix (in UV space), the hitPt (in camera space) and the number of iterations it took to get there.
bool traceScreenSpaceRay(vec3 rayOrigin_VS, vec3 R_VS, float jitter, out vec2 hitPix, out vec3 hitPt, out float iterCt)
{
    // Clip to the near plane
    float rayLen = ((rayOrigin_VS.z + R_VS.z * comboU1.w) > -comboU0.y) ? (-comboU0.y - rayOrigin_VS.z) / R_VS.z : comboU1.w;
    vec3 rayEnd = rayOrigin_VS + R_VS * rayLen;

    // Project into homogeneous clip space
    vec4 H0 = vec4(rayOrigin_VS, 1.f) * PM_SS_d3d;
    float K0 = 1.f / H0.w;

    vec4 H1 = vec4(rayEnd, 1.f) * PM_SS_d3d;
    float K1 = 1.f / H1.w;

    // The interpolated homogeneous version of the camera-space points
    vec3 Q0 = rayOrigin_VS * K0;
    vec3 Q1  = rayEnd * K1;

    // Screen-space endpoints
    vec2 P0 = H0.xy * K0;
    vec2 P1 = H1.xy * K1;

    // If the line is degenerate, make it cover at least one pixel to avoid handling zero-pixel extent as a special case later
    P1 += (distanceSquared(P0, P1) < .0001) ? .01f : 0.f;

    vec2 delta = P1 - P0;

    // Permute so that the primary iteration is in x to collapse all quadrant-specific DDA cases later
    bool permute = false;

    if (abs(delta.x) < abs(delta.y))
    {
        // This is a more-vertical line
        permute = true;
        delta = delta.yx;
        P0 = P0.yx;
        P1 = P1.yx;
    }

    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;

    // Track the derivatives of Q and K
    vec3 dQ = (Q1 - Q0) * invdx;
    float dK = (K1 - K0) * invdx;
    vec2 dP = vec2(stepDir, delta.y * invdx);

    // Calculate pixel stride based on distance of ray origin from camera. Since perspective means distant objects will be smaller in screen space we can use this to have higher quality reflections for far away objects while still using a large pixel stride for near objects (and increase performance) this also helps mitigate artifacts on distant reflections when we use a large pixel stride.
    float strideScaler = 1.f - min(1.f, -rayOrigin_VS.z / comboU1.y);
    float pixelStride = 1.f + strideScaler * comboU1.x;

    // Scale derivatives by the desired pixel stride and then offset the starting values by the jitter fraction
    dP *= pixelStride;
    dQ *= pixelStride;
    dK *= pixelStride;

    P0 += dP * jitter;
    Q0 += dQ * jitter;
    K0 += dK * jitter;

    float i;
    float zA = 0.f;
    float zB = 0.f;

    //track ray step and derivatives to parallelize
    vec4 pqk = vec4(P0, Q0.z, K0);
    vec4 dPQK = vec4(dP, dQ.z, dK);
    bool intersect = false;

    for (i = 0; i < comboU0.z && intersect == false; ++i)
    {
        pqk += dPQK;

        zA = zB;
        zB = (dPQK.z * .5f + pqk.z) / (dPQK.w * .5f + pqk.w);
        swapIfBigger(zB, zA);

        hitPix = permute ? pqk.yx : pqk.xy;
        hitPix *= 1.f / screenSize;

        intersect = rayIntersectsDepthBF(zA, zB, hitPix);
    }

    if (pixelStride > 1.f && intersect) // Binary search refinement
    {
        pqk -= dPQK;
        dPQK /= pixelStride;

        float originalStride = pixelStride * .5f;
        float stride = originalStride;

        zA = pqk.z / pqk.w;
        zB = zA;

        for (float j = 0; j < comboU0.w; ++j)
        {
            pqk += dPQK * stride;

            zA = zB;
            zB = (dPQK.z * -.5f + pqk.z) / (dPQK.w * -.5f + pqk.w);
            swapIfBigger(zB, zA);

            hitPix = permute ? pqk.yx : pqk.xy;
            hitPix *= 1.f / screenSize;

            originalStride *= .5f;
            stride = rayIntersectsDepthBF(zA, zB, hitPix) ? -originalStride : originalStride;
        }
    }

    Q0.xy += dQ.xy * i;
    Q0.z = pqk.z;
    hitPt = Q0 / pqk.w;
    iterCt = i;

    return intersect;
}

void main()
{
    vec3 P_VS = reconstructP(v.uv);

    uvec4 data2 = texelFetch(gBuf2, ivec2(gl_FragCoord.xy), 0);
    vec3 N_VS = vec3(unpackHalf2x16(data2.g).g, unpackHalf2x16(data2.b));

    vec3 R_VS = normalize(reflect(normalize(P_VS), normalize(N_VS)));

    float iterCt;
    vec2 hitPix;
    vec3 hitPt;

    vec2 uv2 = v.uv * screenSize;
    float c = (uv2.r + uv2.g) * .25f;
    float jitter = mod(c, 1.f);

    bool intersect = traceScreenSpaceRay(P_VS, R_VS, jitter, hitPix, hitPt, iterCt);
    float alpha = calculateAlphaForIntersect(intersect, iterCt, hitPix, hitPt, P_VS, R_VS);

    hitPix = mix(v.uv, hitPix, intersect);
    Ci = vec4(texture(tonemap_noGiz, hitPix).rgb, alpha);
}

