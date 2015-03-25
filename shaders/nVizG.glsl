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

/*
reference:
    OpenGL SuperBible Sixth Edition - pg 325 - 328
*/

layout(triangles) in;
layout(line_strip, max_vertices = 8) out;

uniform float normVLen;
uniform int nType;
uniform mat4 MVP;

in Vert
{
    vec3 n;
} v[];

out Geo
{
    vec3 Cv;
} g;

void main()
{
    if (nType == 1 || nType == 3) //fN
    {
        vec3 Cfn = vec3(0.f, 1.f, 0.f);

        vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 faceN = normalize(cross(ab, ac));

        vec4 faceCent = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.f;

        gl_Position = MVP * faceCent;
        g.Cv = Cfn;
        EmitVertex();

        gl_Position = MVP * (faceCent + vec4(faceN * normVLen, 0.f));
        g.Cv = Cfn;
        EmitVertex();

        EndPrimitive();
    }

    if (nType == 2 || nType == 3) //vN
    {
        vec3 Cvn = vec3(0.f, 0.f, 1.f);

        for (int i = 0; i < gl_in.length(); ++i)
        {
            vec3 N = v[i].n;

            gl_Position = MVP * gl_in[i].gl_Position;
            g.Cv = Cvn;
            EmitVertex();

            gl_Position = MVP * (gl_in[i].gl_Position + vec4(N * normVLen, 0.f));
            g.Cv = Cvn;
            EmitVertex();

            EndPrimitive();
        }
    }
}
