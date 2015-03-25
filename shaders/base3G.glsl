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

//GLSL cookbook pg 198 Drawing a Wireframe on top of a shaded mesh

struct Light
{
    float atten;
    vec3 Cl, lDirRot, lP, l;
    vec4 fall, ShadowCoord;
};

in Vert
{
    vec2 uv;
    vec3 i, p, t, b, n;
    Light li1, li2, li3;
} v[];

out Geo
{
    vec2 uv;
    vec3 i, p, t, b, n, wire;
    Light li1, li2, li3;
} g;

uniform mat4 VPM;

vec3 wireFrame(mat4 VPM, vec4 pos0, vec4 pos1, vec4 pos2)
{
    vec2 p0 = vec2(VPM * (pos0 / pos0.w));
    vec2 p1 = vec2(VPM * (pos1 / pos1.w));
    vec2 p2 = vec2(VPM * (pos2 / pos2.w));

    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos((b*b + c*c - a*a) / (2.f * b * c));
    float beta = acos((a*a + c*c - b*b) / (2.f * a * c));
    float ha = abs(c * sin(beta));
    float hb = abs(c * sin(alpha));
    float hc = abs(b * sin(alpha));

    return vec3(ha, hb, hc);
}

void main()
{
    vec3 alt = wireFrame(VPM, gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);

    for (int i = 0; i < 3; ++i)
    {
        g.i = v[i].i;
        g.uv = v[i].uv;
        g.p = v[i].p;
        g.t = v[i].t;
        g.b = v[i].b;
        g.n = v[i].n;
        g.li1 = v[i].li1;
        g.li2 = v[i].li2;
        g.li3 = v[i].li3;

        if (i == 0) g.wire = vec3(alt.x, 0.f, 0.f);
        else if (i == 1) g.wire = vec3(0.f, alt.y, 0.f);
        else if (i == 2) g.wire = vec3(0.f, 0.f, alt.z);

        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();

}
