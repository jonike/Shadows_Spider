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
    https://mynameismjp.wordpress.com/2009/03/10/reconstructing-position-from-depth/
*/

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

layout(bindless_sampler, location = 1) uniform sampler2D gbuf1_64;
layout(bindless_sampler, location = 2) uniform usampler2D gbuf2_64;
layout(bindless_sampler, location = 3) uniform usampler2D gbuf3_64;
layout(bindless_sampler, location = 4) uniform usampler2D gbuf4_64;
layout(bindless_sampler, location = 5) uniform usampler2D gbuf5_64;
layout(bindless_sampler, location = 7) uniform sampler2D ssao_64;
layout(bindless_sampler, location = 8) uniform sampler2D simp_sky_64;
layout(binding = 9) uniform samplerCube cubeSpecMs;
layout(binding = 10) uniform samplerCube cubeIrrMs;
layout(bindless_sampler, location = 11) uniform sampler2D sssLookup_64;
layout(bindless_sampler, location = 12) uniform sampler2D gbuf_DS_64;
layout(location = 0) out vec4 Ci;

uniform bool vignette;
uniform int NUM_LIGHTS;
uniform mat3 VMinv;
uniform mat4 PMinv, VM, MV;

vec3 getP(vec2 UV)
{
    vec4 vProjectedPos = vec4(1.f);
    vProjectedPos.xy = UV * 2.f - 1.f;
    vProjectedPos.z = texture(gbuf_DS_64, UV).x * 2.f - 1.f;
    vec4 vPositionVS = PMinv * vProjectedPos;

    return vPositionVS.xyz / vPositionVS.w;
}

uvec4 data2 = texelFetch(gbuf2_64, ivec2(gl_FragCoord.xy), 0);
vec3 T_VS = vec3(unpackHalf2x16(data2.x), unpackHalf2x16(data2.y).x);
vec3 N_VS = vec3(unpackHalf2x16(data2.y).y, unpackHalf2x16(data2.z));
vec3 B_VS = cross(N_VS, T_VS);

uvec4 data3 = texelFetch(gbuf3_64, ivec2(gl_FragCoord.xy), 0);
vec3 albedoM = vec3(unpackHalf2x16(data3.x), unpackHalf2x16(data3.y).x);

float metallicM = unpackHalf2x16(data3.y).y;
float ruffM = unpackHalf2x16(data3.z).x;
float alphaM = unpackHalf2x16(data3.z).y;

uvec4 data4 = texelFetch(gbuf4_64, ivec2(gl_FragCoord.xy), 0);
float Kr = unpackHalf2x16(data4.x).x;
float ior = unpackHalf2x16(data4.x).y;
float eta = 1.f / ior;
float ruffD = unpackHalf2x16(data4.y).x;
float Ko = unpackHalf2x16(data4.y).y;
vec2 ruffA = unpackHalf2x16(data4.z);
float anisoTgl = unpackHalf2x16(data4.w).x;
float shadowCast = unpackHalf2x16(data4.w).y;

uvec4 data5 = texelFetch(gbuf5_64, ivec2(gl_FragCoord.xy), 0);
float Ksss = unpackHalf2x16(data5.x).x;
float sssSpread = unpackHalf2x16(data5.x).y;
float sssM = unpackHalf2x16(data5.y).x;

vec3 N_TS = texture(gbuf1_64, v.uv).rgb;
vec3 N_TS_mip = vec3(unpackHalf2x16(data2.w), unpackHalf2x16(data5.y).y);
vec3 P_VS = getP(v.uv);

mat3 TBN = mat3(T_VS, B_VS, N_VS);
vec3 incident_eye = normalize(P_VS) * TBN;
vec3 V = normalize(vec3(dot(-P_VS, T_VS), dot(-P_VS, B_VS), dot(-P_VS, N_VS)));
float NdotV = dot(N_TS, V);

float schlick(float myEta, float NdotV)
{
    return myEta + (1.f - myEta) * pow(1.f - NdotV, 5.f);
}

float ggx(float NdotL, float NdotV, float HdotT, float HdotB, float HdotN, float HdotV)
{
    float PI = 3.1416f;
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
        D *= schlick(eta, NdotV);
    }

    return D * G;
}

float oren(float NdotL, float NdotV, vec3 L)
{
    float ruffD2 = ruffD * ruffD;
    float A = 1.f - .5f * (ruffD2 / (ruffD2 + .33f));
    float B = .45f * (ruffD2 / (ruffD2 + .09f));
    float acosNdotV = acos(NdotV);

    vec3 VperpN = normalize(V - N_TS * NdotV);
    float cos_phi = max(0.f, dot(VperpN, normalize(L - N_TS * NdotL)));
    float acosNdotL = acos(NdotL);
    float alpha = max(acosNdotL, acosNdotV);
    float beta = min(acosNdotL, acosNdotV);

    return NdotL * (A + B * cos_phi * sin(alpha) * tan(beta));
}

vec3 cubeRefl()
{
    return VMinv * TBN * reflect(incident_eye, N_TS);
}

vec3 cubeRefr(float ca)
{
    return VMinv * TBN * refract(incident_eye, N_TS, eta + ca);
}

float calcSpot(vec3 dist, vec3 LDirRot, vec3 falloff)
{
    float angle = dot(normalize(dist), LDirRot);

    return clamp((angle - falloff.z) / (falloff.y - falloff.z), 0.f, 1.f);
}

float luma(vec3 col)
{
    return max(dot(col, vec3(.2125f, .7154f, .0721f)), .0001f);
}

vec3 calcSSS(float NdotL, vec3 myCl)
{
    vec4 P_WS = inverse(VM) * vec4(P_VS, 1.f);
    vec3 N_tan_mip_WS = TBN * N_TS_mip;

    float Curvature = (length(fwidth(N_tan_mip_WS)) / length(fwidth(P_WS))) * sssSpread;
    vec2 brdfUV = vec2(NdotL * .5f +.5f, Curvature * luma(myCl));
    vec3 brdf = texture(sssLookup_64, brdfUV).rgb;

    return albedoM * brdf * Ksss;
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

        vec3 L = vec3(VM * vec4(light[i].lP.xyz, 1.f)) - P_VS;
        vec4 myLDirRot = -normalize(VM * light[i].lDirRot);

        float myAtten = light[i].falloff.x * 1.f / dot(L, L);
        float atten = 1.f;

        if      (light[i].Cl_type.w == 1.f) L = myLDirRot.xyz; //DIR
        else if (light[i].Cl_type.w == 2.f) atten = myAtten; //POINT
        else if (light[i].Cl_type.w == 3.f) atten = calcSpot(L, myLDirRot.xyz, light[i].falloff.xyz) * myAtten; //SPOT

        vec3 atten2 = atten * light[i].Cl_type.rgb;

        L = normalize(vec3(dot(L, T_VS), dot(L, B_VS), dot(L, N_VS)));
        vec3 H = normalize(L + V);
        float HdotL = dot(H, L);
        float HdotN = dot(H, N_TS);
        float HdotV = dot(H, V);
        float HdotT = dot(H, T_VS);
        float HdotB = dot(H, B_VS);
        float NdotL = dot(N_TS, L);

        vec3 metallicTint = (metallicM == 1.f) ? albedoM : vec3(1.f);
        vec3 directDiff, directSpec;

        if (NdotL > 0.f)
        {
            directDiff = oren(NdotL, NdotV, L) * albedoM * (1.f - metallicM);
            directSpec = ggx(NdotL, NdotV, HdotT, HdotB, HdotN, HdotV) * metallicTint;
            //sumShadow += shadowRead; //
        }

        vec3 sss = calcSSS(NdotL, light[i].Cl_type.rgb);

        directDiff = mix(directDiff, sss, sssM);
        sumDirect += atten2 * (directSpec + directDiff);
        //sumDirect += atten2 * sumShadow * (directSpec + directDiff); //
    }

    /* INDIRECT */
    float mipIdx = Kr * (textureQueryLevels(cubeSpecMs) - 1.f);
    float fresnelRefl_PMREM = eta + (max(1.f -  Kr, eta) - eta) * pow(1.f - NdotV, 5.f);

    float ssao = clamp(texture(ssao_64, v.uv).r, 0.f, 1.f);
    vec3 indirDiff = texture(cubeIrrMs, cubeRefl()).rgb * albedoM * (1.f - metallicM) * ssao;
    vec3 indirSpec = fresnelRefl_PMREM * textureLod(cubeSpecMs, cubeRefl(), mipIdx).rgb;
    vec3 indirRefr = textureLod(cubeSpecMs, cubeRefr(0.f), mipIdx).rgb;

    if (alphaM == 0.f)
    {
        indirDiff = vec3(0.f);
        indirSpec = vec3(0.f);
        indirRefr = vec3(0.f);
    }

    if (shadowCast == 1.f)
    {
        sumShadow = 1.f; //
        sumIndirect = sumShadow * (indirDiff + indirSpec);
    }

    else
        sumIndirect = (indirDiff + indirSpec);

//    Ci = vec4(sumDirect + sumIndirect, alphaM);
    Ci = vec4(sumDirect, alphaM);

    vec4 myCubeBG = texture(simp_sky_64, v.uv);
    Ci.rgb = mix(myCubeBG.rgb, Ci.rgb, Ci.a);
}
