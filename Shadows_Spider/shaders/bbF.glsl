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

layout(location = 0) out vec4 rttC_or_rttGiz;

uniform vec4 comboU0; //vec4(Crand, 1.f) (COLORPICK) else vec4(Csel / Cwire, 1.f)

void main()
{
    rttC_or_rttGiz = comboU0;
}
