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

out Vert
{
    vec2 uv;
} v;

uniform mat4 MVP;

void main()
{
    const vec4 vertices[] = vec4[](
            vec4(-1.f, -1.f, 0.f, 1.f),
            vec4( 1.f, -1.f, 0.f, 1.f),
            vec4(-1.f,  1.f, 0.f, 1.f),
            vec4( 1.f,  1.f, 0.f, 1.f));

    gl_Position = MVP * vertices[gl_VertexID];
    v.uv = vertices[gl_VertexID].xy * .5f + .5f;
}
