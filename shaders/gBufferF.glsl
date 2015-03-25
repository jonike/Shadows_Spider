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
    vec3 p, n;
} v;

layout(location = 1) out vec4 rttN;
layout(location = 2) out vec4 rttUV;
layout(location = 3) out vec4 rttP;
layout(location = 4) out vec4 ssaoMask;


layout(bindless_sampler, location = 0) uniform sampler2D alphaMs;

uniform vec4 albedoAlphaTile;
uniform vec4 Kcombo; //(Kr->val_f, Ko->val_f, 1.f / ior->val_f, 0.f);

//etc
uniform bool ssaoTgl;

void main()
{
    float alphaM = texture(alphaMs, albedoAlphaTile.zw * v.uv).a;
    float setAlpha = Kcombo.y * alphaM;


    //RTT
    rttN = vec4(v.n, setAlpha);
    rttUV = vec4(v.uv, 0.f, setAlpha);
    rttP = vec4(v.p, setAlpha);
    ssaoMask = (ssaoTgl) ? vec4(1.f) : vec4(0.f);
}
