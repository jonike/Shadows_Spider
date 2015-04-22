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

#ifndef PP_H
#define PP_H

#include "MainWin.h"

class PP
{
public:
    MainWin &myWin;
    PP(MainWin &);

    shared_ptr<GLWidget> myGL;

    bool fboPrep(shared_ptr<GLWidget>); //ep
    void bloomBufferCreate();
    AbjNode gaussNode_create(QString, GLenum, int, int);
    AbjNode gbuffer_node_create();
    AbjNode simp_node_create();
    AbjNode singleNode_create(QString, GLenum, int, int);

    void bloomRender();
    void downSampRender(AbjNode, AbjNode, int);

    GLuint gaussianLinear(GLuint, AbjNode);
    void gaussianRecursivePasses(GLuint, AbjNode, int, int);
    GLuint gaussianBlur(AbjNode, AbjNode, int);

    void postFX(shared_ptr<GLWidget>); //ep
    void resizeTexClearMem(shared_ptr<GLWidget>); //ep
};

#endif // PP_H
