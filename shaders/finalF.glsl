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
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D fxaa;
layout(bindless_sampler, location = 1) uniform sampler2D ssr;
layout(bindless_sampler, location = 2) uniform sampler2D cursor;
layout(bindless_sampler, location = 3) uniform sampler2D deferred;
layout(bindless_sampler, location = 4) uniform sampler2D brush;
layout(bindless_sampler, location = 5) uniform sampler2D accum; //
layout(bindless_sampler, location = 6) uniform sampler2D revealage; //

out vec3 Ci;

uniform vec4 LDRU;
uniform vec4 comboU0; //vec4(rezGateTgl, dragDrop, debug0, debug1)

float rezGateAlpha(vec2 uv, vec2 pLD, vec2 pRU, float dragDrop)
{
    if ((uv.x < pLD.x || uv.x > pRU.x || uv.y < pLD.y || uv.y > pRU.y) || (dragDrop == 1.f))
        return .2f;

    else
        return 1.f;

    return 0.f;
}

//void main() //SPEED
//{
////    Ci = texture(deferred, v.uv).rgb;
////    Ci = texture(fxaa, v.uv).rgb;
////    Ci = texture(ssr, v.uv).rgb;
//    Ci = texture(fxaa, v.uv).rgb + (revealageT * texture(ssr, v.uv).rgb);

////    if (comboU0.z == 1.f)

//    Ci = pow(Ci, vec3(1.f / 2.2f));
//}

void main() //3D
{
    vec4 brushT = texture(brush, v.uv);
    vec4 cursorT = texture(cursor, v.uv);
    float revealageT = texture(revealage, v.uv).r;

    //    Ci = texture(deferred, v.uv).rgb;
    //    Ci = texture(fxaa, v.uv).rgb;
    //    Ci = texture(ssr, v.uv).rgb;
    Ci = texture(fxaa, v.uv).rgb + (revealageT * texture(ssr, v.uv).rgb);

    Ci = mix(Ci, brushT.rgb, brushT.a); // paint
    Ci = mix(Ci, cursorT.rgb, cursorT.a); //paint cursor

    //    if (comboU0.z == 1.f)

    if (comboU0.x == 1.f || comboU0.y == 1.f)
        Ci *= rezGateAlpha(v.uv, LDRU.xy, LDRU.zw, comboU0.y);

    Ci = pow(Ci, vec3(1.f / 2.2f));
}

//void main() //2D
//{
//    vec4 brushT = texture(brush, v.uv);
//    vec4 cursorT = texture(cursor, v.uv);

//    Ci = brushT.rgb; //
//    Ci = mix(Ci, cursorT.rgb, cursorT.a); //

//    if (comboU0.z)
//    {
//    }

//    Ci = pow(Ci, vec3(1.f / 2.2f));
//}
