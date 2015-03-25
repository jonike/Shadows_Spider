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
    http://john-chapman-graphics.blogspot.com/2013/01/good-enough-volumetrics-for-spotlights.html
*/

in Vert
{
    vec2 uv;
    vec3 parentCl, n, p, i;
} v;

layout(location = 0) out vec4 rttC;
layout(location = 4) out vec4 ssaoMask;
out vec4 Ci;

uniform bool ssaoTgl;
uniform float volDist;
uniform vec3 volTip, volTipView;

void main()
{
    vec3 Nn = normalize(v.n);
    vec3 V = -normalize(v.i);

    float softEdge = abs(dot(Nn, V));
    softEdge = pow(softEdge, 4);

    float dist = distance(v.p, volTipView);
    float atten = 1.f - (dist / volDist);
    atten = clamp(atten, 0.f, 1.f);

    rttC = vec4(v.parentCl * atten * softEdge, 1.f);

    //RTT
    ssaoMask = (ssaoTgl) ? vec4(1.f) : vec4(0.f);
}
