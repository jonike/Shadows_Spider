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

/*
reference:
    OpenGL SuperBible Sixth Edition - pg 325 - 328
*/

layout(location = 0) in vec3 pE;
layout(location = 4) in vec3 nE;

out Vert
{
    vec3 n;
} v;

void main()
{
    gl_Position = vec4(pE, 1.f);

    v.n = nE;
}
