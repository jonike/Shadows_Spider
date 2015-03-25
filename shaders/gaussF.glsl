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
#extension GL_ARB_bindless_texture : require

/*
reference:
    http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
*/

in Vert
{
    vec2 uv;
} v;

layout(bindless_sampler, location = 0) uniform sampler2D rttGaussIn32;
layout(location = 0) out vec4 Ci;

uniform int gaussStage;

float offset[3] = float[3](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[3](0.2270270270, 0.3162162162, 0.0702702703);
vec2 texelSize = 1.f / textureSize(rttGaussIn32, 0);

void main()
{
    Ci = texture(rttGaussIn32, v.uv) * weight[0];

    for (int i = 1; i < 3; ++i)
    {
        if (gaussStage == 0) //vertical
        {
            Ci += texture(rttGaussIn32, v.uv + vec2(0.f, offset[i]) * texelSize) * weight[i];
            Ci += texture(rttGaussIn32, v.uv - vec2(0.f, offset[i]) * texelSize) * weight[i];
        }

        else if (gaussStage == 1) //horizontal
        {
            Ci += texture(rttGaussIn32, v.uv + vec2(offset[i], 0.f) * texelSize) * weight[i];
            Ci += texture(rttGaussIn32, v.uv - vec2(offset[i], 0.f) * texelSize) * weight[i];
        }
    }
}
