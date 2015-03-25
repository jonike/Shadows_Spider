/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Affero Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2011 Florian Boesch
 * All rights reserved.
 *
 * Contributor(s): Aleks Berg-Jones
 */

#version 450 core

in Vert
{
    vec2 uv;
} v;

layout(location = 0) out vec4 rttC;
layout(location = 4) out vec4 ssaoMask;

uniform vec3 sunDir;
uniform vec2 viewport;
uniform mat4 PMinv_cube;
uniform mat4 VMinv;

vec3 KrAir = vec3(0.18867780436772762, 0.4978442963618773, 0.6616065586417131); // air

uniform float rayleigh_brightness = 3.3f;
uniform float mie_brightness = .1f;
uniform float spot_brightness = 800.f;
uniform float scatter_strength = .028f;
uniform float rayleigh_strength = .139f;
uniform float mie_strength = .264f;

uniform float rayleigh_collection_power = .81f;
uniform float mie_collection_power = .39f;
uniform float mie_distribution = .63f;

float surface_height = .99f;
float range = .01f;
float intensity = 2.f;
const int step_count = 16;

uniform bool ssaoTgl;

vec3 get_world_normal()
{
    vec2 frag_coord = gl_FragCoord.xy / viewport;
    frag_coord = (frag_coord - .5f) * 2.f;
    vec4 device_normal = vec4(frag_coord, 0.f, 1.f);
    vec3 eye_normal = normalize((PMinv_cube * device_normal).xyz);
    vec3 world_normal = normalize(mat3(VMinv) * eye_normal);

    return world_normal;
}

float atmospheric_depth(vec3 position, vec3 dir)
{
    float a = dot(dir, dir);
    float b = 2.f * dot(dir, position);
    float c = dot(position, position) - 1.f;
    float det = b * b - 4.f * a * c;
    float detSqrt = sqrt(det);
    float q = (-b - detSqrt) / 2.f;
    float t1 = c / q;

    return t1;
}

float phase(float alpha, float g)
{
    float a = 3.f * (1.f - g * g);
    float b = 2.f * (2.f + g * g);
    float c = 1.f + alpha * alpha;
    float d = pow(1.f + g * g - 2.f * g * alpha, 1.5f);

    return (a / b) * (c / d);
}

float horizon_extinction(vec3 position, vec3 dir, float radius)
{
    float u = dot(dir, -position);
    if (u < 0.f) return 1.f;

    vec3 near = position + u * dir;
    if (length(near) < radius) return 0.f;

    else
    {
        vec3 v2 = normalize(near) * radius - position;
        float diff = acos(dot(normalize(v2), dir));

        return smoothstep(0.f, 1.f, pow(diff * 2.f, 3.f));
    }
}

vec3 absorb(float dist, vec3 color, float factor)
{
    return color - color * pow(KrAir, vec3(factor / dist));
}

void main()
{
    vec3 eyedir = get_world_normal();
    float alpha = dot(eyedir, sunDir);

    float rayleigh_factor = phase(alpha, -.01f) * rayleigh_brightness;
    float mie_factor = phase(alpha, mie_distribution) * mie_brightness;
    float spot = smoothstep(0.f, 15.f, phase(alpha, .9995f)) * spot_brightness;

    vec3 eye_position = vec3(0.f, surface_height, 0.f);
    float eye_depth = atmospheric_depth(eye_position, eyedir);
    float step_length = eye_depth / float(step_count);
    float eye_extinction = horizon_extinction(eye_position, eyedir, surface_height - .15f);

    vec3 rayleigh_collected = vec3(0.f, 0.f, 0.f);
    vec3 mie_collected = vec3(0.f, 0.f, 0.f);

    for (int i = 0; i < step_count; ++i)
    {
        float sample_distance = step_length * float(i);
        vec3 position = eye_position + eyedir * sample_distance;
        float extinction = horizon_extinction(position, sunDir, surface_height - .35f);
        float sample_depth = atmospheric_depth(position, sunDir);
        vec3 influx = absorb(sample_depth, vec3(intensity), scatter_strength) * extinction;
        rayleigh_collected += absorb(sample_distance, KrAir * influx, rayleigh_strength);
        mie_collected += absorb(sample_distance, influx, mie_strength);
    }

    rayleigh_collected = (rayleigh_collected * eye_extinction * pow(eye_depth, rayleigh_collection_power)) / float(step_count);
    mie_collected = (mie_collected * eye_extinction * pow(eye_depth, mie_collection_power)) / float(step_count);

    vec3 color = vec3(spot * mie_collected + mie_factor * mie_collected + rayleigh_factor * rayleigh_collected);

    rttC = vec4(pow(color, vec3(1.f / 2.2f)), 1.f);
	
    ssaoMask = (ssaoTgl) ? vec4(1.f) : vec4(0.f);
}

