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

#ifndef SETUP_OBJ_H
#define SETUP_OBJ_H

#include "Core.h"

void gridInit(Abj &);
void camInit(Abj &);
void gizInit(Abj &);
void objInit(Abj &);
void lightInit(Abj &);

void startupScene(string, Abj &);

vector<shared_ptr<Object>> VBOup(string, string, string, shared_ptr<Object>, Abj &);

//OPENGL-TUTORIAL.ORG
bool loadNewOBJ(const char *, vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &);
void computeTangentBasis(vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<glm::vec3> &);
bool is_near(float, float);
bool getSimilarVertexIndex(glm::vec3 &, glm::vec2 &, glm::vec3 &, vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, unsigned short &);
void indexVBO_TBN(vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<unsigned short> &, vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<glm::vec3> &);
void lightArrowAdd(shared_ptr<Object>, float, float, float, string);

#endif