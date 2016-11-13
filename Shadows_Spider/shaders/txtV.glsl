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

// Simple Text OpenGL Demo by Philip Rideout
// Licensed under the Creative Commons Attribution 3.0 Unported License.
// http://creativecommons.org/licenses/by/3.0/
//http://github.prideout.net/strings-inside-vertex-buffers/
//updated to gl 4.5 abj 3/19/15

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

layout(location = 0) in int Character;

out Vert
{
    int vCharacter, vPosition;
} v;

void main()
{
    v.vCharacter = Character;
    v.vPosition = gl_VertexID;

    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
}

