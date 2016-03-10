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

out Vert
{
    vec2 UV;
    vec3 T_VS, N_VS;
} v;

layout(location = 0) in vec3 pE;
layout(location = 1) in vec2 uvE;
layout(location = 2) in vec3 tE;
layout(location = 3) in vec3 nE;

uniform mat3 NM;
uniform mat4 MVP, MM;
uniform int NUM_LIGHTS;

vec4 P_WS = MM * vec4(pE, 1.f);

void main()
{
    gl_Position = MVP * vec4(pE, 1.f);

    v.UV = uvE;
    v.T_VS = normalize(NM * tE);
    v.N_VS = normalize(NM * nE);
}



/* Particle / Billboard */
//out Vert
//{
//    vec2 UV;
//    vec3 T_VS, N_VS;
//} v;

//layout(location = 0) in vec3 pE;
//layout(location = 1) in vec2 uvE;
//layout(location = 2) in vec3 tE;
//layout(location = 3) in vec3 nE;

//uniform mat3 NM;
//uniform mat4 MVP, MM, PV;
//uniform int NUM_LIGHTS;
//uniform vec3 camRight_WS, camUp_WS;

//vec4 P_WS = MM * vec4(pE, 1.f);

//void main()
//{
//    const vec4 vertices[] = vec4[](
//            vec4(-1.f, -1.f, 0.f, 1.f),
//            vec4( 1.f, -1.f, 0.f, 1.f),
//            vec4(-1.f,  1.f, 0.f, 1.f),
//            vec4( 1.f,  1.f, 0.f, 1.f));

////    const vec4 vertices[] = vec4[](
////            vec4(-0.5f, -0.5f, 0.0f, 1.f),
////            vec4( 0.5f, -0.5f, 0.0f, 1.f),
////            vec4(-0.5f,  0.5f, 0.0f, 1.f),
////            vec4( 0.5f,  0.5f, 0.0f, 1.f));

//    vec3 P_WS_d = vec3(0.f);
//    vec3 mySize = vec3(5.f);

////    vec3 vertexP_WS = P_WS_d + (camRight_WS * pE.x * mySize) + (camUp_WS * pE.z * mySize);
//    vec3 vertexP_WS = P_WS_d + (camRight_WS * vertices[gl_VertexID].x * mySize) + (camUp_WS * vertices[gl_VertexID].y * mySize);

//    gl_Position = PV * vec4(vertexP_WS, 1.f);

////    v.UV = uvE;
//    v.UV = vertices[gl_VertexID].xy * .5f + .5f;
////    v.T_VS = normalize(NM * tE);
////    v.N_VS = normalize(NM * nE);
//}
