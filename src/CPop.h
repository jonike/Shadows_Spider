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

#ifndef CPOP_H
#define CPOP_H

#include "MainWin.h"

class CPop : public QWidget
{
    Q_OBJECT
public:
    MainWin &myWin;
    CPop(MainWin &, QWidget *parent = 0);

    float hue, sat, val, usableSat, usableVal;
    QImage HSV_image;
    glm::vec2 CPopManipLeft, CPopManipRight, CPopManipTop; //
    bool changedManip, ctrlTgl, mmbTgl, setInitPos;

    shared_ptr<MultiAttr> targetAttr;


    void drawHueRect();

protected:
    void enterEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

};

#endif // CPOP_H
