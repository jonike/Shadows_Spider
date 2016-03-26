/*

Copyright 2015 Aleks Berg-Jones

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

#ifndef GLWIDGETSH
#define GLWIDGETSH

#include "MainWin.h"

class GLWidgetSh : public QGLWidget
{
    Q_OBJECT
public:
    MainWin &myWin;
    GLWidgetSh(MainWin &, QWidget *parent = 0);

    vector<GLSharedData> GLDataSh;
    vector<shared_ptr<Object>> newO;
    vector<Pro> allPro;
    vector<AbjNode> allShadow;
    vector<Map> allMaps;
    vector<shared_ptr<Brush>> allBrushes;

    GLuint pro, UBO_lights;
    string proN;
    vector<string> typeList;

    bool dontRename = false;
    bool UBO_light_needsUp = true; //update first by default
    bool brushOutlineUpB = false;

    shared_ptr<Brush> selBrush, selEraser;
    shared_ptr<Object> shadowObj;

    void UBO_update();
    void UBO_init();
    void brushInit();
    void mapInit();

    void writeShadow(shared_ptr<Object>);
    void addDeleteShadows(string);
    AbjNode shadowN_create(string, int, int);
    glm::mat4 PM_cube, VM_cube, MM_cube;

    void up64N(AbjNode &, bool);
    void up64T(GLuint &, GLuint64 &, bool);
    AbjNode topLayer(Map);

    void glUseProgram2(string);
    void proInit();

    AbjNode texN_create(string, string, string, int);
    vector<shared_ptr<Object>> VBOup(string, string, string, shared_ptr<Object>);

    //OPENGL-TUTORIAL.ORG
    bool loadNewOBJ(const char *, vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &);
    void computeTangentBasis(vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<glm::vec3> &);
    bool is_near(float, float);
    bool getSimilarVertexIndex(glm::vec3 &, glm::vec2 &, glm::vec3 &, vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, unsigned short &);
    void indexVBO_TBN(vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<unsigned short> &, vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &, vector<glm::vec3> &, vector<glm::vec3> &);

    void lightArrowAdd(shared_ptr<Object>, float, float, float, string);

    //CREATE PRO
    GLuint createIndy(const char *, GLenum);
    GLuint createProg(string, string, string);
    char* shader_read(const char *);
    void shader_error(GLuint);

    //SSAO NEW
    vector<glm::vec3> ssaoNoise, ssaoKernel32, ssaoKernel64;
    GLuint ssaoNoiseTex_32;
    GLuint64 ssaoNoiseTex_64;
    void ssaoUp_dynamic();
    float lerp(float, float, float);

protected:
    void initializeGL();
};

#endif // GLWIDGETSH
