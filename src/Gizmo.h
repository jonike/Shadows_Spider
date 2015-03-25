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

#ifndef GIZMO_H
#define GIZMO_H

/*
reference:
    https://github.com/CedricGuillemet/LibGizmo
*/

#include "MainWin.h"

class Gizmo
{
public:
    MainWin &myWin;
    Gizmo(MainWin &);

    shared_ptr<GLWidget> myGL;

    string hover(shared_ptr<GLWidget>); //ep
    void transform(shared_ptr<GLWidget>); //ep

    glm::vec3 rayCast(glm::vec2);
    glm::vec3 rayCast2(glm::vec2);
    glm::vec3 rayInter(glm::vec3, bool, string);
    bool rotatePlane(glm::vec3, glm::vec3);

    void resetSetCol(string); //ep
    glm::vec3 dotInterAx(glm::vec2, glm::vec3, glm::vec3, glm::vec3, glm::vec3, string);
};

#endif // GIZMO_H
