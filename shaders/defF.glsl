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

in Vert
{
    vec2 uv;
} v;

struct light_t
{
    vec4 Cl_type, falloff, lDirRot, lP;
    //vec4 ShadowCoord;
};

layout (std140, binding = 0) uniform lightUBO
{
    light_t light[50];
};

layout(bindless_sampler, location = 0) uniform sampler2D gbuf0_64;
layout(bindless_sampler, location = 1) uniform sampler2D gbuf1_64;
layout(bindless_sampler, location = 2) uniform usampler2D gbuf2_64;
layout(bindless_sampler, location = 3) uniform usampler2D gbuf3_64;
layout(bindless_sampler, location = 4) uniform usampler2D gbuf4_64;
layout(bindless_sampler, location = 5) uniform usampler2D gbuf5_64;
layout(bindless_sampler, location = 7) uniform sampler2D ssao_64;
layout(bindless_sampler, location = 8) uniform sampler2D simp_sky_64;
layout(binding = 9) uniform samplerCube cubeSpecMs;
layout(binding = 10) uniform samplerCube cubeIrrMs;

uniform bool vignette;
uniform int NUM_LIGHTS;
uniform float debugF;
uniform mat3 VMinv;
uniform mat4 VM, MV;

layout(location = 0) out vec4 Ci;

const float PI = 3.14159;

//32bit
vec3 P_view = texture(gbuf0_64, v.uv).rgb;
vec3 N_tan = texture(gbuf1_64, v.uv).rgb;

//packed 16bit
uvec4 data2 = texelFetch(gbuf2_64, ivec2(gl_FragCoord.xy), 0);
vec3 T_view = vec3(unpackHalf2x16(data2.x), unpackHalf2x16(data2.y).x);
vec3 N_view = vec3(unpackHalf2x16(data2.y).y, unpackHalf2x16(data2.z));
vec3 B_view = cross(N_view, T_view);

uvec4 data3 = texelFetch(gbuf3_64, ivec2(gl_FragCoord.xy), 0);
vec3 albedoM = vec3(unpackHalf2x16(data3.x), unpackHalf2x16(data3.y).x);

float metallicM = unpackHalf2x16(data3.y).y;
float ruffM = unpackHalf2x16(data3.z).x;
float alphaM = unpackHalf2x16(data3.z).y;

uvec4 data4 = texelFetch(gbuf4_64, ivec2(gl_FragCoord.xy), 0);
float Kr = unpackHalf2x16(data4.x).x;
float eta = unpackHalf2x16(data4.x).y;
float ruffD = unpackHalf2x16(data4.y).x;
float Ko = unpackHalf2x16(data4.y).y;

vec2 ruffA = unpackHalf2x16(data4.z);
float anisoTgl = unpackHalf2x16(data4.w).x;
float shadowCast = unpackHalf2x16(data4.w).y;

float ssao = clamp(texture(ssao_64, v.uv).r, 0.f, 1.f);

mat3 TBN = mat3(T_view, B_view, N_view);
vec3 incident_eye = normalize(P_view) * TBN;

vec3 V = normalize(vec3(dot(-P_view, T_view), dot(-P_view, B_view), dot(-P_view, N_view)));
float NdotV = dot(N_tan, V);

float fresnel(float NdotV) // schlick
{
    return eta + (1.f - eta) * pow(1.f - NdotV, 5.f);
}

float ggx(float NdotL, float NdotV, float HdotT, float HdotB, float HdotN, float HdotV)
{
    float ruffR2 = Kr * Kr;

    float v1i = 1.f / (NdotL + sqrt(ruffR2 + (1 - ruffR2) * NdotL * NdotL));
    float v1o = 1.f / (NdotV + sqrt(ruffR2 + (1 - ruffR2) * NdotV * NdotV));
    float G = v1i * v1o;

    float D;

    if (anisoTgl == 1.f)
    {
        float HdotX2 = pow(HdotT / ruffA.x, 2.f);
        float HdotY2 = pow(HdotB / ruffA.y, 2.f);

        float dDenom = (HdotX2 + HdotY2 / Kr) + (HdotN * HdotN);
        D = (1.f / PI) * (1.f / ruffR2) * (1.f / (dDenom * dDenom));

        return clamp(D * G, 0.f, 10.f);
    }

    else
    {
        D = ruffR2 / (PI * pow(((HdotN * HdotN) * (ruffR2 - 1.f) + 1.f), 2.f));
        D *= fresnel(NdotV);
    }

    return D * G;
}

float oren(float NdotL, float NdotV, vec3 L)
{
    float ruffD2 = ruffD * ruffD;
    float A = 1.f - .5f * (ruffD2 / (ruffD2 + .33f));
    float B = .45f * (ruffD2 / (ruffD2 + .09f));
    float acosNdotV = acos(NdotV);

    vec3 VperpN = normalize(V - N_tan * NdotV);
    float cos_phi = max(0.f, dot(VperpN, normalize(L - N_tan * NdotL)));
    float acosNdotL = acos(NdotL);
    float alpha = max(acosNdotL, acosNdotV);
    float beta = min(acosNdotL, acosNdotV);

    return NdotL * (A + B * cos_phi * sin(alpha) * tan(beta));
}

vec3 cubeRefl()
{
    return VMinv * TBN * reflect(incident_eye, N_tan);
}

vec3 cubeRefr(float ca)
{
    return VMinv * TBN * refract(incident_eye, N_tan, eta + ca);
}

float calcSpot(vec3 dist, vec3 LDirRot, vec3 falloff)
{
    float angle = dot(normalize(dist), LDirRot);

    return clamp((angle - falloff.z) / (falloff.y - falloff.z), 0.f, 1.f);
}

void main()
{
    float sumShadow;
    vec3 sumDirect, sumIndirect;

    /* DIRECT */
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        float shadowRead = 1.f;

        if (light[i].Cl_type.w == 1 || light[i].Cl_type.w == 3) //spot / dir
        {
            if (i == 1)
            {
//                light[i].ShadowCoord = aasdsad;
//                shadowRead = textureProj(shadow1T, light[i].ShadowCoord);
            }
        }

        vec3 myDist = vec3(VM * vec4(light[i].lP.xyz, 1.f)) - P_view;
        vec4 myLDirRot = -normalize(VM * light[i].lDirRot);

        float myAtten = light[i].falloff.x * 1.f / dot(myDist, myDist);
        float atten = 1.f;
        vec3 L = myDist;

        if      (light[i].Cl_type.w == 1.f) L = myLDirRot.xyz; //DIR
        else if (light[i].Cl_type.w == 2.f) atten = myAtten; //POINT
        else if (light[i].Cl_type.w == 3.f) atten = calcSpot(myDist, myLDirRot.xyz, light[i].falloff.xyz) * myAtten; //SPOT

        vec3 atten2 = atten * light[i].Cl_type.rgb;

        L = normalize(vec3(dot(L, T_view), dot(L, B_view), dot(L, N_view)));

        vec3 H = normalize(L + V);
        float HdotL = dot(H, L);
        float HdotN = dot(H, N_tan);
        float HdotV = dot(H, V);
        float HdotT = dot(H, T_view);
        float HdotB = dot(H, B_view);
        float NdotL = dot(N_tan, L);

        vec3 metallicTint = (metallicM == 1.f) ? albedoM : vec3(1.f);
        vec3 directDiff, directSpec;

        if (NdotL > 0.f)
        {
            directDiff = oren(NdotL, NdotV, L) * albedoM * (1.f - metallicM);
            directSpec = ggx(NdotL, NdotV, HdotT, HdotB, HdotN, HdotV) * metallicTint;
//            sumShadow += shadowRead; //
        }

        sumDirect += atten2 * (directSpec + directDiff);
//        sumDirect += atten2 * sumShadow * (directSpec + directDiff); //
    }

    /* INDIRECT */
    float mipIdx = Kr * (textureQueryLevels(cubeSpecMs) - 1.f);
    float fresnelRefl_PMREM = eta + (max(1.f -  Kr, eta) - eta) * pow(1.f - NdotV, 5.f);

    vec3 indirDiff = texture(cubeIrrMs, cubeRefl()).rgb * albedoM * (1.f - metallicM) * ssao;
    vec3 indirSpec = fresnelRefl_PMREM * textureLod(cubeSpecMs, cubeRefl(), mipIdx).rgb;
    vec3 indirRefr = textureLod(cubeSpecMs, cubeRefr(0.f), mipIdx).rgb;

    if (alphaM == 0.f)
    {
        indirDiff = vec3(0.f);
        indirSpec = vec3(0.f);
        indirRefr = vec3(0.f);
    }

    //    if (shadowCast == 1.f)
    //        sumIndirect = sumShadow * (indirDiff + indirSpec);

    //    else
        sumIndirect = (indirDiff + indirSpec);

    Ci = vec4(sumDirect + sumIndirect, alphaM);

    vec4 myCubeBG = texture(simp_sky_64, v.uv);
    Ci.rgb = mix(myCubeBG.rgb, Ci.rgb, Ci.a);
}

//void main()
//{
//    float sumShadow;
//    vec3 sumDirect, sumIndirect;

//    /* DIRECT */
//    for (int i = 0; i < NUM_LIGHTS; ++i)
//    {
//        float shadowRead = 1.f;

//        if (light[i].Cl_type.w == 1 || light[i].Cl_type.w == 3) //spot / dir
//        {
//            if (i == 1)
//            {
//                //                light[i].ShadowCoord = aasdsad;
//                //                shadowRead = textureProj(shadow1T, light[i].ShadowCoord);
//            }
//        }

//        vec3 myDist = vec3(VM * vec4(light[i].lP.xyz, 1.f)) - P_view;
//        vec4 myLDirRot = -normalize(VM * light[i].lDirRot);

//        float myAtten = light[i].falloff.x * 1.f / dot(myDist, myDist);
//        float atten = 1.f;
//        vec3 L = myDist;

//        if      (light[i].Cl_type.w == 1.f) L = myLDirRot.xyz; //DIR
//        else if (light[i].Cl_type.w == 2.f) atten = myAtten; //POINT
//        else if (light[i].Cl_type.w == 3.f) atten = calcSpot(myDist, myLDirRot.xyz, light[i].falloff.xyz) * myAtten; //SPOT

//        vec3 atten2 = atten * light[i].Cl_type.rgb;

//        L = normalize(vec3(dot(L, T_view), dot(L, B_view), dot(L, N_view)));

//        vec3 H = normalize(L + V);
//        float HdotL = dot(H, L);
//        float HdotN = dot(H, N_tan);
//        float HdotV = dot(H, V);
//        float HdotT = dot(H, T_view);
//        float HdotB = dot(H, B_view);
//        float NdotL = dot(N_tan, L);

//        vec3 metallicTint = (metallicM == 1.f) ? albedoM : vec3(1.f);
//        vec3 directDiff, directSpec;

//        if (NdotL > 0.f)
//        {
//            directDiff = oren(NdotL, NdotV, L) * albedoM * (1.f - metallicM);
//            directSpec = ggx(NdotL, NdotV, HdotT, HdotB, HdotN, HdotV) * metallicTint;
//            //            sumShadow += shadowRead; //
//        }

//        sumDirect += atten2 * (directSpec + directDiff);
//        //        sumDirect += atten2 * sumShadow * (directSpec + directDiff); //
//    }

//    /* INDIRECT */
//    float mipIdx = Kr * (textureQueryLevels(cubeSpecMs) - 1.f);
//    float fresnelRefl_PMREM = eta + (max(1.f -  Kr, eta) - eta) * pow(1.f - NdotV, 5.f);

//    vec3 indirDiff = texture(cubeIrrMs, cubeRefl()).rgb * albedoM * (1.f - metallicM);
//    vec3 indirSpec = fresnelRefl_PMREM * textureLod(cubeSpecMs, cubeRefl(), mipIdx).rgb;
//    vec3 indirRefr = textureLod(cubeSpecMs, cubeRefr(0.f), mipIdx).rgb;

//    if (alphaM == 0.f)
//    {
//        indirDiff = vec3(0.f);
//        indirSpec = vec3(0.f);
//        indirRefr = vec3(0.f);
//    }

//    //    if (shadowCast == 1.f)
//    //        sumIndirect = sumShadow * (indirDiff + indirSpec);

//    //    else
//    sumIndirect = (indirDiff + indirSpec);

//    Ci = vec4(sumDirect + sumIndirect, alphaM);

//    vec4 myCubeBG = texture(simp_sky_64, v.uv);
//    Ci.rgb = mix(myCubeBG.rgb, Ci.rgb, Ci.a);
//}
