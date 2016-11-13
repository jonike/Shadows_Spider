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

#ifndef INPUT_H
#define INPUT_H

#include "Core.h"

bool quitCombo(GLFWwindow *);
void keyCallback(GLFWwindow*, int, int, int, int);
void cursorPosCallback(GLFWwindow *, double, double);
void mouseButtonCallback(GLFWwindow *, int, int, int);

bool checkForHits(Abj &);
void matchFoundRaycast(shared_ptr<Object>, Abj &);

#endif