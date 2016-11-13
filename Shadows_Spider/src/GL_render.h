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

#ifndef GL_RENDER_H
#define GL_RENDER_H

#include "Core.h"

void render_frame(Abj &);
void mpfTimerStart(Abj &); //DEBUG - move to chrono in its own struct

void resize(Abj &);
void resizeTexClearMem(Abj &);
void PMupOrtho(Abj &);
void VMup(shared_ptr<Object> obj, Abj &);
void dupeStenFix_check(shared_ptr<Object>, Abj &);
bool gridMatch(shared_ptr<Object>, Abj &);
void GBuffer_BOIT(Abj &);
void postFX(Abj &);
void overlay2D(Abj &);

bool fboPrep(Abj &);
AbjNode gBufN_create(Abj &);
AbjNode depthRevN_create(int, int);
AbjNode bgN_create(int, int);
AbjNode tonemapN_create(Abj &);
AbjNode singleN_create(string, GLenum, int, int);
AbjNode dualN_create(string, GLenum, int, int);
void bloomBufferCreate(Abj &);

bool searchUp(shared_ptr<Object>, Abj &);
void searchUp_rec(shared_ptr<Object>, Abj &);

void clearCursor(bool, Abj &);
void clearCanvas(Abj &);

void gizShow(string, Abj &);
void pivTgl(Abj &myAbj);

void clearSel(Abj &);



#endif