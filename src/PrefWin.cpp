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

#include "PrefWin.h"

PrefWin::PrefWin(MainWin &myWinTemp, QWidget *parent) : QMainWindow(parent), myWin(myWinTemp)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    resize(650, 550);

    QWidget *topWidget = new QWidget;
    setCentralWidget(topWidget);
    setStyleSheet(myWin.myStyle); //

    stackedMain = new QStackedLayout(topWidget);

    myPrefs = new Prefs(myWin);

    QSplitter *hsvSplit = new QSplitter(Qt::Horizontal);
    hsvSplit->insertWidget(0, myPrefs);

    stackedMain->insertWidget(0, hsvSplit);
    stackedMain->setCurrentIndex(0);

    QShortcut *HideThis_s = new QShortcut(Qt::Key_QuoteLeft, this);
    connect(HideThis_s, SIGNAL(activated()), this, SLOT(HideThis()));
}

void PrefWin::HideThis()
{
    myWin.PrefWinTgl = 0;
    hide();
}

void PrefWin::mousePressEvent(QMouseEvent *e)
{
    mpos = myWin.toVec2(e->pos());
}

void PrefWin::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() && Qt::LeftButton)
        move(e->globalX() - mpos.x, e->globalY() - mpos.y);
}
