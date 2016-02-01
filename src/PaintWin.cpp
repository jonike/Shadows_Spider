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

#include "PaintWin.h"

/* MARI - DRAG L / R to set brush tip properties */
// R - radius (left == decrease / right == increase)
// O - opacity (left == decrease / right == increase)

PaintWin::PaintWin(MainWin &myWinTemp, QWidget *parent) : QMainWindow(parent), myWin(myWinTemp)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    resize(550, 350);
//    resize(500, 500);

    auto *topWidget = new QWidget;
    setCentralWidget(topWidget);
    setStyleSheet(QString::fromStdString(myWin.myStyle)); //

    stackedMain = new QStackedLayout(topWidget);

    //LAYER SEL
    layerSel_H = new QSplitter();
    layerSel_H->setOrientation(Qt::Horizontal);
    layerSel_H->insertWidget(0, myWin.objectLay);
    layerSel_H->insertWidget(1, myWin.typeLay);
    layerSel_H->insertWidget(2, myWin.mapLay);
    layerSel_H->insertWidget(3, myWin.layerLay);
    stackedMain->insertWidget(0, layerSel_H);

    //COLOR PICK
    colorPick_H = new QSplitter();
    colorPick_H->setOrientation(Qt::Horizontal);
    myCPop = new CPop(myWin);
    colorPick_H->insertWidget(0, myCPop);
    stackedMain->insertWidget(1, colorPick_H);

    setMouseTracking(1);
}

void PaintWin::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_I)
    {
        if (stackedMain->currentIndex() == 0)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_K)
    {
        if (stackedMain->currentIndex() == 1)
            myWin.PaintWinTgl(0, 999);
    }
}

void PaintWin::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Space)
    {
//        cout << "pressed Space from PaintWin" << endl;
    }
}
