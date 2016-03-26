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

#include "CamCombo.h"

CamCombo::CamCombo(MainWin &myWinTemp, QWidget *parent) : QComboBox(parent), myWin(myWinTemp)
{
    setItemDelegate(myWin.myFocus);
    setFont(QFont("DejaVu Sans Mono", 11, 75));

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    setProperty("isCamCombo", 1);

    clear();

    for (auto &i : myWin.allCamLi)
    {
        if (i->selectable)
        {
            if (i->camLiTypeGet("cam"))
                addItem(myWin.myIconCam, QString::fromStdString(i->name->val_s));

            else if (i->camLiTypeGet("light"))
                addItem(myWin.myIconLight, QString::fromStdString(i->name->val_s));
        }
    }

    connect(this, SIGNAL(activated(int)), this, SLOT(changeCam()));
}

void CamCombo::enterEvent(QEvent *)
{
    setFocus();
}

void CamCombo::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton)
        selActiveCam();

    else
        return QComboBox::mouseReleaseEvent(e);
}

void CamCombo::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

void CamCombo::selActiveCam()
{
    myWin.clearSel();

    for (auto &i : myWin.allCamLi)
    {
        if (i->name->val_s == currentText().toStdString())
        {
            i->selected = true;
            myWin.selB = i;

            break;
        }
    }

    myWin.myOutliner->refreshOutliner(1);
    myWin.attrTable->refreshTable();
}

void CamCombo::changeCam()
{
    for (auto &i : myWin.allCamLi)
    {
        if (i->name->val_s == currentText().toStdString())
        {
            myWin.myGL->selCamLi = i;
            myWin.myGL->selCamLi->setDirty();

            myWin.setLightsDirty();
        }
    }

    myWin.myGL->resizeGL(myWin.myGL->width(), myWin.myGL->height());
}

void CamCombo::refresh()
{
    clear();

    for (auto &i : myWin.allCamLi)
    {
        if (i->selectable)
        {
            if (i->camLiTypeGet("cam"))
                addItem(myWin.myIconCam, QString::fromStdString(i->name->val_s));

            else if (i->camLiTypeGet("light"))
                addItem(myWin.myIconLight, QString::fromStdString(i->name->val_s));
        }
    }

    auto match = false;

    for (int i = 0; i < count(); ++i)
    {
        if (itemText(i).toStdString() == myWin.myGL->selCamLi->name->val_s)
        {
            match = 1;
            setCurrentIndex(i); //set the index back to stored val

            break;
        }
    }

    if (!match)
    {
        for (auto &i : myWin.allCamLi)
        {
            if (i->name->val_s == "persp") //set persp by default
                myWin.myGL->selCamLi = i;
        }
    }

    myWin.myGL->resizeGL(myWin.myGL->width(), myWin.myGL->height());
}
