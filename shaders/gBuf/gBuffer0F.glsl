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

in Vert
{
    vec2 UV;
    vec3 T_VS, N_VS;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D albedo;
layout(bindless_sampler, location = 1) uniform sampler2D alpha;
layout(bindless_sampler, location = 2) uniform sampler2D aniso;
layout(bindless_sampler, location = 3) uniform sampler2D metallic;
layout(bindless_sampler, location = 4) uniform sampler2D normal;
layout(bindless_sampler, location = 5) uniform sampler2D ruff;
layout(bindless_sampler, location = 6) uniform sampler2D sss;

layout(location = 0) out vec4 gBuf0;
layout(location = 1) out uvec4 gBuf1;
layout(location = 2) out uvec4 gBuf2;
layout(location = 3) out uvec4 gBuf3;
layout(location = 4) out uvec4 gBuf4;
//layout(location = 5) out uvec4 gBuf5;
//layout(location = 6) out uvec4 gBuf6;
//layout(location = 7) out uvec4 gBuf7;

uniform int NUM_LIGHTS;
uniform float anisoType;
uniform vec4 albedoAlphaTile, metallicRuffTile, normalTile, anisoSssTile;
uniform vec4 comboU0; //vec4(ior, ruffOren->val_f, Ko->val_f, debug0)
uniform vec4 comboU1; //vec4(ruffA->val_2, shadowCastF, sssSpread->val_f)

float getAniso(float type)
{
    if (type == 2.f) // CUSTOM MAP
        return texture(aniso, anisoSssTile.xy * v.UV).r;

    else
        return type; //BLANK / VIEW
}

void main()
{
    vec3 albedoT = texture(albedo, albedoAlphaTile.rg * v.UV).rgb;
    albedoT = pow(albedoT, vec3(2.2f)); //gamma space input tex req decoding

    float anisoM  = getAniso(anisoType);
    float alphaT = texture(alpha, albedoAlphaTile.ba * v.UV).r * comboU0.z; // (w/ overall alpha param)
    float metallicT = texture(metallic, metallicRuffTile.rg * v.UV).r;

    float ruffT = texture(ruff, metallicRuffTile.ba * v.UV).r;

//    if (comboU0.w)
//        ruffT = pow(ruffT, 2.2f); ////gamma space input tex req decoding

    float sssT = texture(sss, anisoSssTile.ba * v.UV).r;

    vec3 N_TS = normalize(texture(normal, normalTile.rg * v.UV).rgb * 2.f - 1.f);
    N_TS = vec3(N_TS.r, -N_TS.g, N_TS.b);
    N_TS = vec3(N_TS.r * normalTile.b, N_TS.g * normalTile.b, N_TS.b);

    vec3 N_TS_mip = normalize(textureLod(normal, normalTile.rg * v.UV, 2).rgb * 2.f - 1.f);
    N_TS_mip = vec3(N_TS_mip.r, -N_TS_mip.g, N_TS_mip.b);
    N_TS_mip = vec3(N_TS_mip.r * normalTile.b, N_TS_mip.b * normalTile.b, N_TS_mip.b);

    ///////////
    gBuf0 = vec4(N_TS, 1.f);
    if (!gl_FrontFacing) //determine front / back face
        gBuf0.w = 0.f;

    gBuf1.x = packHalf2x16(v.T_VS.rg);
    gBuf1.y = packHalf2x16(vec2(v.T_VS.b, v.N_VS.r));
    gBuf1.z = packHalf2x16(vec2(v.N_VS.g, v.N_VS.b));
    gBuf1.w = packHalf2x16(N_TS_mip.rg);

    gBuf2.x = packHalf2x16(albedoT.rg);
    gBuf2.y = packHalf2x16(vec2(albedoT.b, metallicT));
    gBuf2.z = packHalf2x16(vec2(ruffT, alphaT));
//    gBuf2.w = packHalf2x16(v.UV); //

    gBuf3.x = packHalf2x16(vec2(comboU0.xy));
    gBuf3.y = packHalf2x16(vec2(comboU0.zw));
    gBuf3.z = packHalf2x16(vec2(comboU1.xy));
    gBuf3.w = packHalf2x16(vec2(comboU1.zw));

    gBuf4.x = packHalf2x16(vec2(sssT, N_TS_mip.b));
    gBuf4.y = packHalf2x16(vec2(anisoM, 0.f));
    gBuf4.z = packHalf2x16(vec2(0.f, 0.f));
    gBuf4.w = packHalf2x16(vec2(0.f, 0.f));

    //    gBuf5.x = packHalf2x16(vec2(0.f));
    //    gBuf5.y = packHalf2x16(vec2(0.f));
    //    gBuf5.z = packHalf2x16(vec2(0.f));
    //    gBuf5.w = packHalf2x16(vec2(0.f));

    //    gBuf6.x = packHalf2x16(vec2(0.f));
    //    gBuf6.y = packHalf2x16(vec2(0.f));
    //    gBuf6.z = packHalf2x16(vec2(0.f));
    //    gBuf6.w = packHalf2x16(vec2(0.f));

    //    gBuf7.x = packHalf2x16(vec2(0.f));
    //    gBuf7.y = packHalf2x16(vec2(0.f));
    //    gBuf7.z = packHalf2x16(vec2(0.f));
    //    gBuf7.w = packHalf2x16(vec2(0.f));
}
