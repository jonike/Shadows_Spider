
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

layout(bindless_sampler, location = 0) uniform sampler2D bg;
layout(bindless_sampler, location = 1) uniform sampler2D eraser;
layout(location = 0) out vec4 Ci;

void main()
{
    vec4 bgT = texture(bg, v.uv);
    vec4 eraserT = texture(eraser, v.uv);

    if (eraserT.a > 0.f)
        Ci = vec4(0.f);

    else
        Ci = bgT;
}