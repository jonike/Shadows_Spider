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
    float myDepth;
} v;

layout(location = 0) in vec3 pE;
layout(location = 1) in vec2 uvE;

uniform mat4 MVP, MV;

void main()
{
    gl_Position = MVP * vec4(pE, 1.f);
    v.myDepth = -(MV * vec4(pE, 1.f)).z;

    v.UV = uvE;
}

