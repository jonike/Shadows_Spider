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

layout(bindless_sampler, location = 0) uniform sampler2D fxaa_64;
layout(bindless_sampler, location = 1) uniform sampler2D ssr_64;

uniform bool dragDrop, rezGateTgl, vign;
uniform vec2 rezGate_LD, rezGate_RU;

out vec3 Ci;

float rezGateAlpha(vec2 uv, vec2 pLD, vec2 pRU, bool dragDrop)
{
    if ((uv.x < pLD.x || uv.x > pRU.x || uv.y < pLD.y || uv.y > pRU.y) || (dragDrop == true))
        return .2f;

    else
        return 1.f;

    return 0.f;
}

void main()
{
    Ci = texture(fxaa_64, v.uv).rgb + texture(ssr_64, v.uv).rgb;

    if (rezGateTgl || dragDrop)
        Ci *= rezGateAlpha(v.uv, rezGate_LD, rezGate_RU, dragDrop);

    Ci = pow(Ci.rgb, vec3(1.f / 2.2f));
}
