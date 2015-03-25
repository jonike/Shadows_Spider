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

#include "Prefs.h"

Prefs::Prefs(MainWin &myWinTemp, QWidget *parent) : QTabWidget(parent), myWin(myWinTemp)
{
    setTabShape(QTabWidget::Triangular);
    setFont(QFont("DejaVu Sans Mono", 14, 75));
    setMovable(1);

    //GFX
    gfxTab_H = new QSplitter();
    gfxTab_H->setOrientation(Qt::Horizontal);

    gfxTab_H->insertWidget(0, myWin.cutTable);
    gfxTab_H->insertWidget(1, myWin.etcTable);
    gfxTab_H->insertWidget(2, myWin.glslTable);

    addTab(gfxTab_H, "gfx");

    //PATH
    pathTab_H = new QSplitter();
    pathTab_H->setOrientation(Qt::Horizontal);

    pathTab_H->insertWidget(0, myWin.pathTable);

    addTab(pathTab_H, "path");
}

void Prefs::enterEvent(QEvent *)
{
    setFocus();
}
