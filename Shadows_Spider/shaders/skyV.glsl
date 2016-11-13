/*

Copyright 2016 Aleks Berg-Jones

This file is part of Shadows Spider.

Shadows Spider is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Shadows Spider is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Shadows Spider.  If not, see <http://www.gnu.org/licenses/>.

*/

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

out Vert
{
    vec3 uv;
} v;

uniform mat4 PMinv_cube, VM;

void main()
{
    const vec4 vertices[] = vec4[](
            vec4(-1.f, -1.f, 1.f, 1.f),
            vec4( 1.f, -1.f, 1.f, 1.f),
            vec4(-1.f,  1.f, 1.f, 1.f),
            vec4( 1.f,  1.f, 1.f, 1.f));

    gl_Position = vertices[gl_VertexID];

    vec3 unproject = (PMinv_cube * vertices[gl_VertexID]).xyz;
    v.uv = transpose(mat3(scene.VM)) * unproject;
}
