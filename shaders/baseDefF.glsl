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
    vec3 P_view, T_view, N_view;
} g;

layout(bindless_sampler, location = 0) uniform sampler2D albedoMs;
layout(bindless_sampler, location = 1) uniform sampler2D alphaMs;
layout(bindless_sampler, location = 2) uniform sampler2D metallicMs;
layout(bindless_sampler, location = 3) uniform sampler2D ruffMs;
layout(bindless_sampler, location = 4) uniform sampler2D normalMs;

layout(location = 0) out vec4 gbuf0;
layout(location = 1) out vec4 gbuf1;
layout(location = 2) out uvec4 gbuf2;
layout(location = 3) out uvec4 gbuf3;
layout(location = 4) out uvec4 gbuf4;
layout(location = 5) out uvec4 gbuf5;

uniform vec4 albedoAlphaTile, metallicRuffTile, normalTile;
uniform vec4 matPrefs0; //(Kr->val_f, eta, ruffD->val_f, Ko->val_f);
uniform vec4 matPrefs1; //(ruffA->val_2, anisoTglF, shadowCastF);

void main()
{
    vec3 albedoM = texture(albedoMs, albedoAlphaTile.xy * g.UV).rgb;
    float alphaM = texture(alphaMs, albedoAlphaTile.zw * g.UV).a * matPrefs0.w;

    float metallicM = texture(metallicMs, metallicRuffTile.xy * g.UV).r;
    float ruffM = texture(ruffMs, metallicRuffTile.zw * g.UV).r;

    vec3 N_tan = normalize(texture(normalMs, normalTile.xy * g.UV).rgb * 2.f - 1.f);
    N_tan = vec3(N_tan.r, -N_tan.g, N_tan.b);
    N_tan = vec3(N_tan.x * normalTile.z, N_tan.y * normalTile.z, N_tan.z);

    /////////////
    gbuf0 = vec4(g.P_view, 0.f);
    gbuf1 = vec4(N_tan, 0.f);

    gbuf2.x = packHalf2x16(g.T_view.xy);
    gbuf2.y = packHalf2x16(vec2(g.T_view.z, g.N_view.x));
    gbuf2.z = packHalf2x16(vec2(g.N_view.y, g.N_view.z));
//    gbuf2.w = packHalf2x16(vec2(0.f, 0.f));

    gbuf3.x = packHalf2x16(albedoM.xy);
    gbuf3.y = packHalf2x16(vec2(albedoM.z, metallicM));
    gbuf3.z = packHalf2x16(vec2(ruffM, alphaM));
    gbuf3.w = packHalf2x16(g.UV);

    //matPrefs0 / matPrefs1
    gbuf4.x = packHalf2x16(vec2(matPrefs0.xy));
    gbuf4.y = packHalf2x16(vec2(matPrefs0.zw));
    gbuf4.z = packHalf2x16(vec2(matPrefs1.xy));
    gbuf4.w = packHalf2x16(vec2(matPrefs1.zw));
}
