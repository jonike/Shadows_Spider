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

// Simple Text OpenGL Demo by Philip Rideout
// Licensed under the Creative Commons Attribution 3.0 Unported License.
// http://creativecommons.org/licenses/by/3.0/
//http://github.prideout.net/strings-inside-vertex-buffers/
//updated to gl 4.5 abj 3/19/15

#version 450 core
#extension GL_ARB_bindless_texture : require

in Geo
{
    vec2 uv;
} g;

layout(bindless_sampler, location = 0) uniform sampler2D txtAtlasT;
out vec4 Ci;

uniform vec3 Ctxt;

void main()
{
    float A = texture(txtAtlasT, g.uv).r;

    Ci = vec4(Ctxt, A);
}
