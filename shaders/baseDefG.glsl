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

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in Vert
{
    vec2 UV;
    vec3 T_VS, N_VS;
} v[];

out Geo
{
    vec2 UV;
    vec3 T_VS, N_VS;
} g;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        g.UV = v[i].UV;
        g.T_VS = v[i].T_VS;
        g.N_VS = v[i].N_VS;

        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
