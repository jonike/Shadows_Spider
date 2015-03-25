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

#include "CPopWin.h"

CPopWin::CPopWin(MainWin &myWinTemp, QWidget *parent) : QMainWindow(parent), myWin(myWinTemp)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    resize(myWin.glslTable->CPopSize->val_i, myWin.glslTable->CPopSize->val_i);

    QWidget *topWidget = new QWidget;
    setCentralWidget(topWidget);
    setStyleSheet(myWin.myStyle); //
    stackedMain = new QStackedLayout(topWidget);

    myCPop = new CPop(myWin);

    QSplitter *hsvSplit = new QSplitter(Qt::Horizontal);
    hsvSplit->insertWidget(0, myCPop);

    stackedMain->insertWidget(0, hsvSplit);
    stackedMain->setCurrentIndex(0);
}

void CPopWin::resizeEvent(QResizeEvent *)
{
    resize(myWin.glslTable->CPopSize->val_i, myWin.glslTable->CPopSize->val_i);
}
