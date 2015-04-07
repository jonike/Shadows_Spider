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

#ifndef GLWIDGETSH
#define GLWIDGETSH

#include "MainWin.h"

typedef struct
{
    QString name;
    GLuint pro;
} Pro;

typedef struct
{
    QString name, type;
    GLuint tex_32;
    GLuint64 tex_64;
} Tex;

typedef struct
{
    unsigned int dwSize;
    unsigned int dwFlags;
    unsigned int dwHeight;
    unsigned int dwWidth;
    unsigned int dwPitchOrLinearSize;
    unsigned int dwDepth;
    unsigned int dwMipMapCount;
    unsigned int dwReserved1[11];

    struct
    {
        unsigned int dwSize;
        unsigned int dwFlags;
        unsigned int dwFourCC;
        unsigned int dwRGBBitCount;
        unsigned int dwRBitMask;
        unsigned int dwGBitMask;
        unsigned int dwBBitMask;
        unsigned int dwAlphaBitMask;
    } sPixelFormat;

    struct
    {
        unsigned int dwCaps1;
        unsigned int dwCaps2;
        unsigned int dwDDSX;
        unsigned int dwReserved;
    } sCaps;

    unsigned int dwReserved2;

} ddsHeader;

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
    vector<Tex> allTex;

    GLuint pro;
    QString proN;
    bool dontRename = 0;

    //LIGHT
    int shadowIdx;
    glm::mat4 negCubeCenter, negCubeCenter2, VMcubeShadow;

    void texInit();
    GLuint dds16fUp(string);
    GLuint texUp(string);
    void writeShadow(int);
    void addDeleteShadows(string);
    AbjNode shadowNode_create(QString, int, int);
    AbjNode cubeDynNode_create(QString, int, int);
    void cubemapGen();

    AbjNode cubeFBO_node;
    glm::mat4 dynVM;

    void up64N(AbjNode &, bool);
    void up64T(GLuint &, GLuint64 &, bool);

    //PAINT
    void paintSlow(shared_ptr<GLWidget>);

    void glUseProgram2(QString);
    void proUp();

    void VAOup(shared_ptr<Object>);
    vector<shared_ptr<Object>> VBOup(QString, QString, QString, shared_ptr<Object>);

    //OPENGL-TUTORIAL.ORG
    bool loadOBJ(const char *, vector<glm::vec3> &, vector<glm::vec2> &, vector<glm::vec3> &);
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

protected:
    void initializeGL();

};

#endif // GLWIDGETSH
