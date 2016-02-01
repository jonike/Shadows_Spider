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

layout(location = 0) in vec3 pE;

uniform vec3 circRight;
uniform vec4 comboU0; //vec4(circUpOrFront, fullCirc)

const float PI = 3.14159;
uniform mat4 MVP;

void main()
{
    int seg = 30;
    float mult = (comboU0.w == 1.f) ? 2.f : 1.f;

    float theta = mult * PI * gl_VertexID / seg;

    vec3 rotPt = circRight * cos(theta);
    rotPt += comboU0.xyz * sin(theta);

    gl_Position = MVP * vec4(rotPt, 1.f);
}
