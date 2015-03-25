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

struct Light
{
    float atten;
    vec3 Cl, lDirRot, lP, l;
    vec4 fall, ShadowCoord;
};

in Geo
{
    vec2 uv;
    vec3 i, p, t, b, n, wire;
    Light li1, li2, li3;
} g;

layout(bindless_sampler, location = 0) uniform sampler2DShadow shadow1T;
layout(bindless_sampler, location = 1) uniform sampler2DShadow shadow2T;
layout(bindless_sampler, location = 2) uniform sampler2DShadow shadow3T;
layout(bindless_sampler, location = 3) uniform sampler2D albedoMs;
layout(bindless_sampler, location = 4) uniform sampler2D alphaMs;
layout(bindless_sampler, location = 5) uniform sampler2D metallicMs;
layout(bindless_sampler, location = 6) uniform sampler2D ruffMs;
layout(bindless_sampler, location = 7) uniform sampler2D normalMs;
layout(binding = 8) uniform samplerCube cubeSpecMs;
layout(binding = 9) uniform samplerCube cubeIrrMs;

layout(location = 0) out vec4 rttC;
layout(location = 1) out vec4 rttN;
layout(location = 2) out vec4 rttUV;
layout(location = 3) out vec4 rttP;
layout(location = 4) out vec4 ssaoMask;
layout(location = 5) out vec4 rttGiz;

//UV tile
uniform vec4 albedoAlphaTile, metallicRuffTile, normalTile;

////refl
uniform vec4 Kcombo; //(Kr->val_f, Ko->val_f, 1.f / ior->val_f, 0.f);
uniform vec4 ruffCombo; //(ruffA->val_2, ruffD->val_f, anisoTgl);

//fog
uniform vec4 CfogCombo; //(myWin.myFSQ->Cfog->val_3, fogTgl);
uniform float fogDen;

uniform vec4 wireCombo; //(CwireUse->val_3, wireMode);

//etc
uniform bool CpickTgl, ssaoTgl, shadowCast, vignette;
const float PI = 3.14159;
uniform int myLightType[4];
uniform mat4 VMinv;
uniform vec3 Crand;

float fresnel(float eta, float NdotV) // schlick
{
    return eta + (1.f - eta) * pow(1.f - NdotV, 5.f);
}

float ggx(float NdotL, float NdotV, float HdotT, float HdotB, float HdotN, float HdotV, float ruffR)
{
    float ruffR2 = ruffR * ruffR;

    float v1i = 1.f / (NdotL + sqrt(ruffR2 + (1 - ruffR2) * NdotL * NdotL));
    float v1o = 1.f / (NdotV + sqrt(ruffR2 + (1 - ruffR2) * NdotV * NdotV));
    float G = v1i * v1o;

    float D;

    if (ruffCombo.w == 1.f) //aniso
    {
        float HdotX2 = pow(HdotT / ruffCombo.xy.x, 2.f);
        float HdotY2 = pow(HdotB / ruffCombo.xy.y, 2.f);

        float dDenom = (HdotX2 + HdotY2 / ruffR) + (HdotN * HdotN);
        D = (1.f / PI) * (1.f / ruffR2) * (1.f / (dDenom * dDenom));

        return clamp(D * G, 0.f, 10.f);
    }

    else
    {
        D = ruffR2 / (PI * pow(((HdotN * HdotN) * (ruffR2 - 1.f) + 1.f), 2.f));
        D *= fresnel(Kcombo.z, NdotV);
    }

    return D * G;
}

float oren(float NdotL, float NdotV, vec3 Ln, vec3 Nn, vec3 V)
{
    float ruffD2 = ruffCombo.z * ruffCombo.z;
    float A = 1.f - .5f * (ruffD2 / (ruffD2 + .33f));
    float B = .45f * (ruffD2 / (ruffD2 + .09f));
    float acosNdotV = acos(NdotV);

    vec3 VperpN = normalize(V - Nn * NdotV);
    float cos_phi = max(0.f, dot(VperpN, normalize(Ln - Nn * NdotL)));
    float acosNdotL = acos(NdotL);
    float alpha = max(acosNdotL, acosNdotV);
    float beta = min(acosNdotL, acosNdotV);

    return NdotL * (A + B * cos_phi * sin(alpha) * tan(beta));
}

vec3 cubeRefl(vec3 Nn, vec3 V, mat4 VMinv, mat3 TBN)
{
    return mat3(VMinv) * TBN * reflect(V, Nn);
}

vec3 cubeRefr(float ca, vec3 Nn, vec3 V, mat4 VMinv, mat3 TBN)
{
    return mat3(VMinv) * TBN * refract(V, Nn, Kcombo.z + ca);
}

float fog(vec3 p, float den)
{
    float dist = length(p);
    return clamp(exp(-dist * den), 0.f, 1.f);
}

float wire(vec3 wire, float wireThick)
{
    float wireD = min(wire.x, min(wire.y, wire.z));

    float mixVal;
    if (wireD < wireThick - 1.f) mixVal = 1.f;
    else if (wireD > wireThick + 1.f) mixVal = 0.f;

    else
    {
        float x = wireD - (wireThick - 1.f);
        mixVal = exp2(-2.f * x * x);
    }

    return mixVal;
}

float VectorToDepth(vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float n = 1.f;
    float f = 1000.f;

    float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
    return (NormZComp + 1.f) * .5f;
}

void main()
{
    vec4 albedoM = texture(albedoMs, albedoAlphaTile.xy * g.uv);
    float alphaM = texture(alphaMs, albedoAlphaTile.zw * g.uv).a;
    float metallicM = texture(metallicMs, metallicRuffTile.xy * g.uv).r;
    float ruffM = texture(ruffMs, metallicRuffTile.zw * g.uv).r;
    ruffM = .1f; //DEBUG

    vec3 Nn = normalize(texture(normalMs, normalTile.xy * g.uv).rgb * 2.f - 1.f);
    Nn = vec3(Nn.r, -Nn.g, Nn.b);
    Nn = vec3(Nn.x * normalTile.z, Nn.y * normalTile.z, Nn.z);

    mat3 TBN = mat3(g.t, g.b, g.n);
    vec3 V_TBN = normalize(g.p) * TBN;

    vec3 V = normalize(g.i);
    float NdotV = dot(Nn, V);

    //indirect
    float mipIdx = Kcombo.x * (textureQueryLevels(cubeSpecMs) - 1.f);
    float fresnelRefl_PMREM2 = Kcombo.z + (max(1.f -  Kcombo.x, Kcombo.z) - Kcombo.z) * pow(1.f - NdotV, 5.f);
    vec3 myIndirDiff = texture(cubeIrrMs, cubeRefl(Nn, V_TBN, VMinv, TBN)).rgb;
    vec3 myIndirSpec = fresnelRefl_PMREM2 * textureLod(cubeSpecMs, cubeRefl(Nn, V_TBN, VMinv, TBN), mipIdx).rgb;
    //vec3 myIndirSpec = textureLod(cubeSpecMs, cubeRefr(0.f, Nn, V_TBN, VMinv, TBN), mipIdx);

    float sumShadow;

    for (int i = 1; i < 4; ++i)
    {
        Light usableLi;
        float shadowRead = 1.f;

        if (myLightType[i] == 1 || myLightType[i] == 3)
        {
            if (i == 1)
            {
                usableLi = g.li1;
                shadowRead = textureProj(shadow1T, usableLi.ShadowCoord);
            }

            else if (i == 2)
            {
                usableLi = g.li2;
                shadowRead = textureProj(shadow2T, usableLi.ShadowCoord);
            }

            else if (i == 3)
            {
                usableLi = g.li3;
                shadowRead = textureProj(shadow3T, usableLi.ShadowCoord);
            }
        }

        vec3 Ln = normalize(usableLi.l);
        vec3 H = normalize(Ln + V);
        float HdotL = dot(H, Ln);
        float HdotN = dot(H, Nn);
        float HdotV = dot(H, V);
        float HdotT = dot(H, g.t);
        float HdotB = dot(H, g.b);
        float NdotL = dot(Nn, Ln);

        //direct
        vec3 metallicTint = (metallicM == 1.f) ? albedoM.rgb : vec3(1.f);
        vec3 directDiff, directSpec;

        if (NdotL > 0.f)
        {
            directDiff = oren(NdotL, NdotV, Ln, Nn, V) * albedoM.rgb * (1.f - metallicM);
            directSpec = ggx(NdotL, NdotV, HdotT, HdotB, HdotN, HdotV,  Kcombo.x) * metallicTint * usableLi.Cl;
            sumShadow += shadowRead;
//            sumShadow += usableLi.atten * shadowRead;
        }

        rttC.rgb += usableLi.atten * shadowRead * (directDiff + directSpec);
    }

    //indirect
    vec3 indirSpec, indirDiff;

    indirSpec = myIndirSpec;
    indirDiff = myIndirDiff * albedoM.rgb * (1.f - metallicM);
//    indirDiff = myIndirDiff * albedoM.rgb;

    if (shadowCast == true)
        rttC.rgb += sumShadow * (indirDiff + indirSpec);

    else
        rttC.rgb += (indirDiff + indirSpec);

    rttC.a = Kcombo.y * alphaM;

    if (CpickTgl)
        rttC = vec4(Crand, 1.f);

    //FX
    if (wireCombo.a == 1) rttC.rgb = mix(rttC.rgb, wireCombo.rgb, wire(g.wire, .75f));
    else if (wireCombo.a == 2) rttC = vec4(wireCombo.rgb, wire(g.wire, .75f));

    if (CfogCombo.a == 1.f) rttC.rgb = mix(CfogCombo.rgb, rttC.rgb, fog(g.p, fogDen));

    //RTT
    rttUV = vec4(g.uv, 0.f, rttC.a);
    rttN = vec4(g.n, rttC.a);
    rttGiz = vec4(0.f);
    rttP = vec4(g.p, rttC.a);
    ssaoMask = (ssaoTgl) ? vec4(1.f) : vec4(0.f);
}
