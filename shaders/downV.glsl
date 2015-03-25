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

out Vert
{
    vec2 uv, uv2, uv3, uv4, uv5;
} v;

uniform vec2 texelSize;

void main()
{
    const vec4 vertices[] = vec4[](
            vec4(-1.f, -1.f, 0.f, 1.f),
            vec4( 1.f, -1.f, 0.f, 1.f),
            vec4(-1.f,  1.f, 0.f, 1.f),
            vec4( 1.f,  1.f, 0.f, 1.f));

    gl_Position = vertices[gl_VertexID];

    v.uv = vertices[gl_VertexID].xy * .5f + .5f;

    v.uv2 = v.uv;
    v.uv3 = v.uv + vec2(texelSize.x, 0);
    v.uv4 = v.uv + vec2(texelSize.x, texelSize.y);
    v.uv5 = v.uv + vec2(0, texelSize.y);
}
