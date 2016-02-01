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

// Simple Text OpenGL Demo by Philip Rideout
// Licensed under the Creative Commons Attribution 3.0 Unported License.
// http://creativecommons.org/licenses/by/3.0/
//http://github.prideout.net/strings-inside-vertex-buffers/
//updated to gl 4.5 abj 3/19/15

#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in Vert
{
    int vCharacter, vPosition;
} v[];

uniform vec4 comboU0; //vec4(txtCellSize, txtCellOffset)
uniform vec4 comboU1; //vec4(txtSize, txtOrigin)

out Geo
{
    vec2 uv;
} g;

void main()
{
    // Determine the final quad's position and size:
    float x = comboU1.z + float(v[0].vPosition) * comboU1.x * 2;
    float y = comboU1.w;
    vec4 P = vec4(x, y, 0, 1);
    vec4 U = vec4(1, 0, 0, 0) * comboU1.x;
    vec4 V = vec4(0, 1, 0, 0) * comboU1.y;

    // Determine the texture coordinates:
    int letter = v[0].vCharacter;
    letter = clamp(letter - 32, 0, 96);
    int row = letter / 16 + 1;
    int col = letter % 16;
    float S0 = comboU0.z + comboU0.x * col;
    float T0 = comboU0.w + 1 - comboU0.y * row;
    float S1 = S0 + comboU0.x - comboU0.z;
    float T1 = T0 + comboU0.y;

    // Output the quad's vertices:
    g.uv = vec2(S0, 1 - T1); gl_Position = P - U - V; EmitVertex();
    g.uv = vec2(S1, 1 - T1); gl_Position = P + U - V; EmitVertex();
    g.uv = vec2(S0, 1 - T0); gl_Position = P - U + V; EmitVertex();
    g.uv = vec2(S1, 1 - T0); gl_Position = P + U + V; EmitVertex();

    EndPrimitive();
}
