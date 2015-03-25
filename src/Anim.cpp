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

#include "Anim.h"

Anim::Anim(MainWin &myWinTemp) : myWin(myWinTemp)
{

}

float Anim::bounce_(float t)
{
    return t * t * 8.f;
}

float Anim::anticp_(float t, float s)
{
    return t * t * ((s + 1) * t - s);
}

float Anim::overSh_(float t, float s)
{
    return t * t * ((s + 1) * t + s);
}

float Anim::cubicHerm_(float t, float p0, float p1, float m0, float m1)
{
    float t2 = t * t;
    float t3 = t2 * t;
    float herm = (2.f * t3 - 3.f * t2 + 1.f) * p0 + (t3 - 2.f * t2 + t) * m0 + (-2.f * t3 + 3.f * t2) * p1 + (t3 - t2) * m1;

    return herm;
}

float Anim::dynAnim(string type)
{
    float cycles = 1.f;
    float time = myWin.GetTickCount2() / 1000.f;
    float lin = fmodf(time, cycles);

    if (type == "linear")
        return lin;

    if (type == "smoothStep")
    {
        float smoothStep = lin * lin * (3.f - 2.f * lin);
        return smoothStep;
    }

    if (type == "spring")
    {
        float springF = .2f;
        float spring = pow(2.f, -10.f * lin) * sin((lin - springF / 4.f) * (2.f * PI) / springF) + 1.f;
        return spring;
    }

    if (type == "cycle")
    {
        float cycle = sin(2.f * cycles * PI * time);
//        float cycle = sin(2.f * cycles * PI * time) * 4;
        return cycle;
    }

    if (type == "accelDecel")
    {
        float accelDecel = cos((lin + 1.f) * PI) / 2.f + .5f;
        return accelDecel;
    }

    if (type == "bounce")
    {
        float bounce;

        if (lin < .3535f)
            bounce = bounce_(lin);
        else if (lin < .7408f)
            bounce = bounce_(lin - 0.54719f) + .7f;
        else if (lin < .9644f)
            bounce = bounce_(lin - 0.8526f) + .9f;
        else
            bounce = bounce_(lin - 1.0435f) + .95f;

        return bounce;
    }

    if (type == "accel")
    {
        float accelF = 2.f;
        float accel;

        if (accelF == 1.f) accel = lin * lin;
        else accel = pow(lin, 2.f * accelF);

        return accel;
    }

    if (type == "decel")
    {
        float decelF = 1.f;
        float decel;

        if (decelF == 1.f)
            decel = (1.f - (1.f - lin) * (1.f - lin));
        else
            decel = (1.f - pow((1.f - lin), 2.f * decelF));

        return decel;
    }

    if (type == "anticip")
    {
        float anticipF = 4.f;
        float anticp = lin * lin * ((anticipF + 1.f) * lin - anticipF);

        return anticp;
    }

    if (type == "anticipOverShoot")
    {
        float anticipOverShootF = 4.f * 1.5f;
        float anticipOverShoot;

        if (lin < .5f)
            anticipOverShoot = .5f * anticp_(lin * 2.f, anticipOverShootF);
        else
            anticipOverShoot = .5f * (overSh_(lin * 2.f - 2.f, anticipOverShootF) + 2.f);

        return anticipOverShoot;
    }

    if (type == "cubicHerm")
    {
        float cubicHerm = cubicHerm_(lin, 0.f, 1.f, 4.f, 4.f);// * cycle;

        return cubicHerm;
    }

    return 0.f;
}
