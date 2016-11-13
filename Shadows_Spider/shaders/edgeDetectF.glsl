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

/*
reference:
    https://en.wikipedia.org/wiki/Sobel_operator
    GLSL Cookbook Chp 5 Image Processing and Screen Space Techniques pg 153 - 157
    http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
*/

#version 450 core
#extension GL_ARB_shading_language_include : enable
#include "Core_glsl.h"

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D tex;
out vec4 Ci;

uniform vec4 comboU0; // vec4(edgeDetect_mode, edgeThr, 0.f, 0.f)
vec2 texelSize = 1.f / textureSize(tex, 0);

uniform mat3 G2[2] = mat3[]
(
    mat3(1.f, 2.f, 1.f, 0.f, 0.f, 0.f, -1.f, -2.f, -1.f),
    mat3(1.f, 0.f, -1.f, 2.f, 0.f, -2.f, 1.f, 0.f, -1.f)
);

uniform mat3 G9[9] = mat3[]
(
    mat3(0.353553, 0.0, -0.353553, 0.5, 0.0, -0.5, 0.353553, 0.0, -0.353553),
    mat3(0.353553, 0.5, 0.353553, 0.0, 0.0, 0.0, -0.353553, -0.5, -0.353553),
    mat3(0.0, 0.353553, -0.5, -0.353553, 0.0, 0.353553, 0.5, -0.353553, 0.0),
    mat3(0.5, -0.353553, 0.0, -0.353553, 0.0, 0.353553, 0.0, 0.353553, -0.5),
    mat3(0.0, -0.5, 0.0, 0.5, 0.0, 0.5, 0.0, -0.5, 0.0),
    mat3(-0.5, 0.0, 0.5, 0.0, 0.0, 0.0, 0.5, 0.0, -0.5),
    mat3(0.166667, -0.333333, 0.166667, -0.333333, 0.666667, -0.333333, 0.166667, -0.333333, 0.166667),
    mat3(-0.333333, 0.166667, -0.333333, 0.166667, 0.666667, 0.166667, -0.333333, 0.166667, -0.333333),
    mat3(0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333, 0.333333)
);

void main()
{
    if (comboU0.x == 0.f) // SOBEL
    {
        mat3 I;
        float cnv[2];
        vec3 mySample;

        /* fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value */
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                mySample = texture(tex, v.uv + texelSize * vec2(i - 1.f, j - 1.f)).rgb;
                I[i][j] = length(mySample);
            }
        }

        /* calculate the convolution values for all the masks */
        for (int i = 0; i < 2; ++i)
        {
            float dp3 = dot(G2[i][0], I[0]) + dot(G2[i][1], I[1]) + dot(G2[i][2], I[2]);
            cnv[i] = dp3 * dp3;
        }

        float dist = sqrt(cnv[0] * cnv[0] + cnv[1] * cnv[1]);

        if (dist > comboU0.y)
            Ci = vec4(1.f);

        else
            Ci = vec4(0.f);
    }

    else if (comboU0.x == 1.f) // Rastergrid Frei-Chen with pre-computed mat3 values
    {
        mat3 I;
        float cnv[9];
        vec3 mySample;

        /* fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value */
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                mySample = texture(tex, v.uv + texelSize * vec2(i - 1.f, j - 1.f)).rgb;
                I[i][j] = length(mySample);
            }
        }

        /* calculate the convolution values for all the masks */
        for (int i = 0; i < 9; ++i)
        {
            float dp3 = dot(G9[i][0], I[0]) + dot(G9[i][1], I[1]) + dot(G9[i][2], I[2]);
            cnv[i] = dp3 * dp3;
        }

        float M = (cnv[0] + cnv[1]) + (cnv[2] + cnv[3]);
        float S = (cnv[4] + cnv[5]) + (cnv[6] + cnv[7]) + (cnv[8] + M);

        float dist = sqrt(M / S);

        if (dist > comboU0.y)
            Ci = vec4(1.f);

        else
            Ci = vec4(0.f);
    }
}
