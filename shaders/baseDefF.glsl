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

in Geo
{
    vec2 UV;
    vec3 T_VS, N_VS;
} g;

layout(bindless_sampler, location = 0) uniform sampler2D albedoMs;
layout(bindless_sampler, location = 1) uniform sampler2D alphaMs;
layout(bindless_sampler, location = 2) uniform sampler2D metallicMs;
layout(bindless_sampler, location = 3) uniform sampler2D normalMs;
layout(bindless_sampler, location = 4) uniform sampler2D ruffMs;
layout(bindless_sampler, location = 5) uniform sampler2D sssMs;

//layout(location = 0) out vec4 gbuf0;
layout(location = 1) out vec4 gbuf1;
layout(location = 2) out uvec4 gbuf2;
layout(location = 3) out uvec4 gbuf3;
layout(location = 4) out uvec4 gbuf4;
layout(location = 5) out uvec4 gbuf5;

uniform vec4 albedoAlphaTile, metallicRuffTile, normalTile, sssTile;
uniform vec4 matPrefs0; //(Kr->val_f, eta, ruffD->val_f, Ko->val_f);
uniform vec4 matPrefs1; //(ruffA->val_2, anisoTglF, shadowCastF);
uniform vec4 matPrefs2; //(Ksss->val_f, sssSpread->val_f, 0.f, 0.f);

void main()
{
    vec3 albedoM = texture(albedoMs, albedoAlphaTile.xy * g.UV).rgb;
    float alphaM = texture(alphaMs, albedoAlphaTile.zw * g.UV).a * matPrefs0.w;
    float metallicM = texture(metallicMs, metallicRuffTile.xy * g.UV).r;
    float ruffM = texture(ruffMs, metallicRuffTile.zw * g.UV).r;
    float sssM = texture(sssMs, sssTile.xy * g.UV).r;

    vec3 N_TS = normalize(texture(normalMs, normalTile.xy * g.UV).rgb * 2.f - 1.f);
    N_TS = vec3(N_TS.r, -N_TS.g, N_TS.b);
    N_TS = vec3(N_TS.x * normalTile.z, N_TS.y * normalTile.z, N_TS.z);

    vec3 N_TS_mip = normalize(textureLod(normalMs, normalTile.xy * g.UV, 2).rgb * 2.f - 1.f);
    N_TS_mip = vec3(N_TS_mip.r, -N_TS_mip.g, N_TS_mip.b);
    N_TS_mip = vec3(N_TS_mip.x * normalTile.z, N_TS_mip.y * normalTile.z, N_TS_mip.z);

    /////////////
//    gbuf0 = vec4(0.f, 0.f, 0.f, 0.f);
    gbuf1 = vec4(N_TS, 0.f);

    gbuf2.x = packHalf2x16(g.T_VS.xy);
    gbuf2.y = packHalf2x16(vec2(g.T_VS.z, g.N_VS.x));
    gbuf2.z = packHalf2x16(vec2(g.N_VS.y, g.N_VS.z));
    gbuf2.w = packHalf2x16(N_TS_mip.xy);

    gbuf3.x = packHalf2x16(albedoM.xy);
    gbuf3.y = packHalf2x16(vec2(albedoM.z, metallicM));
    gbuf3.z = packHalf2x16(vec2(ruffM, alphaM));
    gbuf3.w = packHalf2x16(g.UV);

    gbuf4.x = packHalf2x16(vec2(matPrefs0.xy));
    gbuf4.y = packHalf2x16(vec2(matPrefs0.zw));
    gbuf4.z = packHalf2x16(vec2(matPrefs1.xy));
    gbuf4.w = packHalf2x16(vec2(matPrefs1.zw));

    gbuf5.x = packHalf2x16(vec2(matPrefs2.xy));
    gbuf5.y = packHalf2x16(vec2(sssM, N_TS_mip.z));
//    gbuf5.z = packHalf2x16(vec2(0.f, 0.f));
//    gbuf5.w = packHalf2x16(vec2(0.f, 0.f));
}
