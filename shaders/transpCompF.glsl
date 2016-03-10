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

/*
reference:
    http://casual-effects.blogspot.com/2014/03/weighted-blended-order-independent.html
    http://casual-effects.blogspot.com/2015/03/implemented-weighted-blended-order.html
    http://casual-effects.blogspot.com/2015/03/colored-blended-order-independent.html
    http://jcgt.org/published/0002/02/09/
*/

#version 450 core
#extension GL_ARB_bindless_texture : require

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D accum;
layout(bindless_sampler, location = 1) uniform sampler2D revealage;
layout(location = 0) out vec4 Ci;

uniform vec4 comboU0; //vec4(depthScale, debug0, 0.f, 0.f)

float maxComponent(vec3 comp)
{
    return max(max(comp.x, comp.y), comp.z);
}

void main()
{
    float revealageT = texture(revealage, v.uv).r;

//    if (revealageT == 1.f)
//        discard;

    vec4 accumT = texture(accum, v.uv);

    vec3 myTest = abs(accumT.rgb);

    if (isinf(maxComponent(abs(accumT.rgb))))
        accumT.rgb = vec3(accumT.a);

    Ci = vec4(accumT.rgb / max(accumT.a, 0.00001), revealageT);
}
