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

#version 450 core
#extension GL_ARB_bindless_texture : require

in Geo
{
    vec2 UV;
    vec3 T_VS, N_VS;
    vec4 shadowCoord0, shadowCoord1;
} g;

layout(bindless_sampler, location = 0) uniform sampler2D albedo;
layout(bindless_sampler, location = 1) uniform sampler2D alpha;
layout(bindless_sampler, location = 2) uniform sampler2D aniso;
layout(bindless_sampler, location = 3) uniform sampler2D metallic;
layout(bindless_sampler, location = 4) uniform sampler2D normal;
layout(bindless_sampler, location = 5) uniform sampler2D ruff;
layout(bindless_sampler, location = 6) uniform sampler2D sss;
layout(bindless_sampler, location = 7) uniform sampler2DShadow shadowM0;
layout(bindless_sampler, location = 8) uniform sampler2DShadow shadowM1;

layout(location = 0) out vec4 gBuf0;
layout(location = 1) out vec4 gBuf1;
layout(location = 2) out uvec4 gBuf2;
layout(location = 3) out uvec4 gBuf3;
layout(location = 4) out uvec4 gBuf4;
layout(location = 5) out uvec4 gBuf5;

uniform int anisoType;
uniform vec4 albedoAlphaTile, metallicRuffTile, normalTile, anisoSssTile;
uniform vec4 comboU0; //vec4(ior, ruffOren->val_f, Ko->val_f, debug0)
uniform vec4 comboU1; //vec4(ruffA->val_2, shadowCastF, sssSpread->val_f)

float getAniso(int type)
{
    if (type == 0) // anisoM->val_s == "BLANK"
        return -2.f;

    else if (type == 1) // anisoM->val_s == "VIEW"
        return -1.f;

    else if (type == 2) // anisoM->val_s == CUSTOM MAP
        return texture(aniso, anisoSssTile.xy * g.UV).r;

    return 0;
}

void main()
{
    vec3 albedoT = texture(albedo, albedoAlphaTile.rg * g.UV).rgb;
    albedoT = pow(albedoT, vec3(2.2f)); //gamma space input tex req decoding

    float anisoM  = getAniso(anisoType);
    float alphaT = texture(alpha, albedoAlphaTile.ba * g.UV).r * comboU0.z; // (w/ overall alpha param)
    float metallicT = texture(metallic, metallicRuffTile.rg * g.UV).r;

    float ruffT = texture(ruff, metallicRuffTile.ba * g.UV).r;

//    if (comboU0.w)
//        ruffT = pow(ruffT, 2.2f); ////gamma space input tex req decoding

    float sssT = texture(sss, anisoSssTile.ba * g.UV).r;

    vec3 N_TS = normalize(texture(normal, normalTile.rg * g.UV).rgb * 2.f - 1.f);
    N_TS = vec3(N_TS.r, -N_TS.g, N_TS.b);
    N_TS = vec3(N_TS.r * normalTile.b, N_TS.g * normalTile.b, N_TS.b);

    vec3 N_TS_mip = normalize(textureLod(normal, normalTile.rg * g.UV, 2).rgb * 2.f - 1.f);
    N_TS_mip = vec3(N_TS_mip.r, -N_TS_mip.g, N_TS_mip.b);
    N_TS_mip = vec3(N_TS_mip.r * normalTile.b, N_TS_mip.b * normalTile.b, N_TS_mip.b);

    //shadowmap
    float shadowRead0 = textureProj(shadowM0, g.shadowCoord0);
    float shadowRead1 = textureProj(shadowM1, g.shadowCoord1);

    ///////////
    gBuf0 = vec4(0.f);

    gBuf1 = vec4(N_TS, 1.f);
    if (!gl_FrontFacing) //determine front / back face
        gBuf1.a = 0.f;

    gBuf2.r = packHalf2x16(g.T_VS.rg);
    gBuf2.g = packHalf2x16(vec2(g.T_VS.b, g.N_VS.r));
    gBuf2.b = packHalf2x16(vec2(g.N_VS.g, g.N_VS.b));
    gBuf2.a = packHalf2x16(N_TS_mip.rg);

    gBuf3.r = packHalf2x16(albedoT.rg);
    gBuf3.g = packHalf2x16(vec2(albedoT.b, metallicT));
    gBuf3.b = packHalf2x16(vec2(ruffT, alphaT));
    //gBuf3.a = packHalf2x16(g.UV); //

    gBuf4.r = packHalf2x16(vec2(comboU0.xy));
    gBuf4.g = packHalf2x16(vec2(comboU0.zw));
    gBuf4.b = packHalf2x16(vec2(comboU1.xy));
    gBuf4.a = packHalf2x16(vec2(comboU1.zw));

    gBuf5.r = packHalf2x16(vec2(0.f, 0.f));
    gBuf5.g = packHalf2x16(vec2(sssT, N_TS_mip.b));
    gBuf5.b = packHalf2x16(vec2(anisoM, 0.f));
    gBuf5.a = packHalf2x16(vec2(shadowRead0, shadowRead1));
}
