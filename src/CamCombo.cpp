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

#include "CamCombo.h"

CamCombo::CamCombo(MainWin &myWinTemp, shared_ptr<GLWidget> myGLTemp, QWidget *parent) : QComboBox(parent), myWin(myWinTemp), myGL(myGLTemp)
{
    setItemDelegate(myWin.myFocus);
    setFont(QFont("DejaVu Sans Mono", 11, 75));

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    setProperty("isCamCombo", 1);

    clear();

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->selectable)
        {
            if (myWin.allObj[i]->camLiTypeGet("cam"))
                addItem(myWin.myIconCam, myWin.allObj[i]->name->val_s);

            else if (myWin.allObj[i]->camLiTypeGet("light"))
                addItem(myWin.myIconLight, myWin.allObj[i]->name->val_s);
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

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->name->val_s == currentText())
        {
            myWin.allObj[i]->selected = 1;
            myWin.selB = myWin.allObj[i];

            break;
        }
    }

    myWin.myOutliner->refreshOutliner(1);
    myWin.attrTable->refreshTable();
}

void CamCombo::changeCam()
{
    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->name->val_s == currentText())
        {
            myGL.get()->selCamLi = myWin.allObj[i];
            myGL.get()->selCamLi->setDirty();

            myWin.setLightsDirty();
        }
    }

    myGL.get()->resizeGL(myGL.get()->width(), myGL.get()->height());
}

void CamCombo::refresh()
{
    clear();

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (myWin.allObj[i]->selectable)
        {
            if (myWin.allObj[i]->camLiTypeGet("cam"))
                addItem(myWin.myIconCam, myWin.allObj[i]->name->val_s);

            else if (myWin.allObj[i]->camLiTypeGet("light"))
                addItem(myWin.myIconLight, myWin.allObj[i]->name->val_s);
        }
    }


    bool match = 0;

    for (int i = 0; i < count(); ++i)
    {
        if (itemText(i) == myGL.get()->selCamLi->name->val_s)
        {
            match = 1;
            setCurrentIndex(i); //set the index back to stored val

            break;
        }
    }

    if (!match)
    {
        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->name->val_s == "persp") //set persp by default
                myGL.get()->selCamLi = myWin.allObj[i];
        }
    }

    myGL.get()->resizeGL(myGL.get()->width(), myGL.get()->height());
}
