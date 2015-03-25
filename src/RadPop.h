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

#ifndef RADPOP_H
#define RADPOP_H

#include "MainWin.h"

class RadPop : public QMainWindow
{
    Q_OBJECT
public:
    MainWin &myWin;
    RadPop(MainWin &, QWidget *parent = 0);

    vector<PopSetup> allPop;
    shared_ptr<GLWidget> myGL;

    bool quitTgl = 0;
    float radius = 100;

    glm::vec2 startP, newP;
    QPainter painter;
    QString popName;


    void buttonInit(vector<PopSetup>);

protected:
    void keyReleaseEvent(QKeyEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

    void hoverHighlight();
    void hitFind(QString);
    bool findCorrectAttrTable(QString);
    bool selCamLiTextColor(QString, QString);
};

#endif // RADPOP_H
