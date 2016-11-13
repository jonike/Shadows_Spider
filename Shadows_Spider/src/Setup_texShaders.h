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

#ifndef SETUP_TEXSHADERS_H
#define SETUP_TEXSHADERS_H

#include "Core.h"

void uboInit(Abj &);
void uboUp(Abj &);
//void brushInit();
//void mapInit();

void mapInit(Abj &);
AbjNode texN_create(string, string, string, int);
void up64N(AbjNode &, bool);
void up64T(GLuint &, GLuint64 &, bool);

void proInit(Abj &);
GLuint createProg(string, string, string);
GLuint createIndy(const char *, GLenum);
char* shader_read(const char *);
void shader_error(GLuint);
void glUseProgram2(string, Abj &);


void addDeleteShadows(string, Abj &);
AbjNode shadowN_create(string, int, int);
void writeShadow(shared_ptr<Object>, Abj &);
AbjNode topLayer(Map);

//ETC
int countLights(Abj &);
glm::vec2 toNDC(glm::vec2, Abj &, string);
void getPtsBetweenRect(Abj &);

//PAINTING
void brushOutlineUp(Abj &);

//PP
GLuint gaussianLinear(GLuint, AbjNode, Abj &);
void gaussianRecursivePasses(GLuint, AbjNode, int, int, Abj &);
GLuint gaussianBlur(AbjNode, AbjNode, int, Abj &);
GLuint gaussianBlur2(GLuint, AbjNode, int, Abj &);
void downSampRender(AbjNode, AbjNode, int, Abj &);
void bloomRender(Abj &);

#endif