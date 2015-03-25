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

layout(location = 0) in vec3 pE;
layout(location = 1) in vec2 uvE;
layout(location = 2) in vec3 tE;
layout(location = 3) in vec3 bE;
layout(location = 4) in vec3 nE;

struct Light
{
    float atten;
    vec3 Cl, lDirRot, lP, l;
    vec4 fall, ShadowCoord;
};

uniform Light liIn1, liIn2, liIn3;
uniform int myLightType[4];
uniform mat3 NM;
uniform mat4 MVP, MV, VM;
uniform mat4 depthBiasMVP1, depthBiasMVP2, depthBiasMVP3;

out Vert
{
    vec2 uv;
    vec3 i, p, t, b, n;
    Light li1, li2, li3;
} v;

float calcSpot(Light myLi, vec3 dist)
{
    float angle = dot(normalize(dist), myLi.lDirRot);

    return clamp((angle - myLi.fall.z) / (myLi.fall.y - myLi.fall.z), 0.f, 1.f);
}

void main()
{
    gl_Position = MVP * vec4(pE, 1.f);

    v.p = vec3(MV * vec4(pE, 1.f));
    v.uv = uvE;

    v.t = normalize(NM * tE);
    v.b = normalize(NM * bE);
    v.n = normalize(NM * nE);
    v.i = normalize(vec3(dot(-v.p, v.t), dot(-v.p, v.b), dot(-v.p, v.n)));

    vec3 myDist;

    /* liIn1 */
    v.li1 = liIn1;
    myDist = vec3(VM * vec4(v.li1.lP, 1.f)) - v.p;

    float myAtten1 = v.li1.fall.x * 1.f / dot(myDist, myDist);
    v.li1.atten = 1.f;
    v.li1.l = myDist;

    if      (myLightType[1] == 1) v.li1.l = v.li1.lDirRot; //DIR
    else if (myLightType[1] == 2) v.li1.atten = myAtten1; //POINT
    else if (myLightType[1] == 3) v.li1.atten = calcSpot(v.li1, myDist) * myAtten1; //SPOT

    v.li1.l = normalize(vec3(dot(v.li1.l, v.t), dot(v.li1.l, v.b), dot(v.li1.l, v.n)));
    v.li1.ShadowCoord = depthBiasMVP1 * vec4(pE, 1.f);

    /* liIn2 */
    v.li2 = liIn2;
    myDist = vec3(VM * vec4(v.li2.lP, 1.f)) - v.p;

    float myAtten2 = v.li2.fall.x * 1.f / dot(myDist, myDist);
    v.li2.atten = 1.f;
    v.li2.l = myDist;

    if      (myLightType[2] == 1) v.li2.l = v.li2.lDirRot; //DIR
    else if (myLightType[2] == 2) v.li2.atten = myAtten2; //POINT
    else if (myLightType[2] == 3) v.li2.atten = calcSpot(v.li2, myDist) * myAtten2; //SPOT

    v.li2.l = normalize(vec3(dot(v.li2.l, v.t), dot(v.li2.l, v.b), dot(v.li2.l, v.n)));
    v.li2.ShadowCoord = depthBiasMVP2 * vec4(pE, 1.f);

    /* liIn3 */
    v.li3 = liIn3;
    myDist = vec3(VM * vec4(v.li3.lP, 1.f)) - v.p;

    float myAtten3 = v.li3.fall.x * 1.f / dot(myDist, myDist);
    v.li3.atten = 1.f;
    v.li3.l = myDist;

    if      (myLightType[1] == 1) v.li3.l = v.li3.lDirRot; //DIR
    else if (myLightType[1] == 2) v.li3.atten = myAtten3; //POINT
    else if (myLightType[1] == 3) v.li3.atten = calcSpot(v.li3, myDist) * myAtten3; //SPOT

    v.li3.l = normalize(vec3(dot(v.li3.l, v.t), dot(v.li3.l, v.b), dot(v.li3.l, v.n)));
    v.li3.ShadowCoord = depthBiasMVP2 * vec4(pE, 1.f);
}
