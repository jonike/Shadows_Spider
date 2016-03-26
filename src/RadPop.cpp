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

#include "RadPop.h"

RadPop::RadPop(MainWin &myWinTemp, QWidget *parent) : QMainWindow(parent), myWin(myWinTemp)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground, 1);
    setStyleSheet(QString::fromStdString(myWin.myStyle)); //

    auto *dw = QApplication::desktop();
    setFixedSize(dw->width(), dw->height());

    auto *quit_s = new QShortcut(tr("CTRL+Q"), this);
    connect(quit_s, SIGNAL(activated()), qApp, SLOT(quit()));

    auto *upTimer = new QTimer(this);
    connect(upTimer, SIGNAL(timeout()), this, SLOT(update()));
    upTimer->start(16); //update every 16ms
}

void RadPop::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        quitTgl = true;
}

void RadPop::mouseMoveEvent(QMouseEvent *e)
{
    newP = myWin.toVec2(e->pos());

    hoverHighlight();
}

void RadPop::mouseReleaseEvent(QMouseEvent *e)
{
     if (e->button() == Qt::RightButton)
     {
         for (auto &i : allPop)
         {
             if (i.hovered)
             {
                 hitFind(i.name);

                 break;
             }
         }

         quitTgl = true;
     }
}

void RadPop::paintEvent(QPaintEvent *)
{
    // START 2D
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //draw a circle at mouse pos
    QRect rectCircPopupCenter(0, 0, myWin.glslTable->popCenterXY->val_i, myWin.glslTable->popCenterXY->val_i);
    rectCircPopupCenter.moveCenter(myWin.toQP(startP));

    painter.setPen(myWin.toQC(glm::vec3(0.f)));
    painter.drawEllipse((rectCircPopupCenter));

    //draw rects
    for (auto &i : allPop)
    {
        QRect myRect(0, 0, i.sx, i.sy);
        myRect.moveCenter(myWin.toQP(startP + i.move));

        //colors
        glm::vec3 CrectBG;

        if (myRect.contains(myWin.toQP(newP)) || i.hovered)
            CrectBG = myWin.gammaCsel();

        else
            CrectBG = glm::vec3(.5f);

        painter.setBrush(QBrush(myWin.toQC(CrectBG), Qt::SolidPattern));
        painter.setPen(myWin.toQC(glm::vec3(0.f)));
        painter.drawRect(myRect);

        //set text color for camli items
        if (selCamLiTextColor(i.name, "cam")) painter.setPen(myWin.toQC(glm::vec3(0.f, 0.f, 1.f)));
        else if (selCamLiTextColor(i.name, "light")) painter.setPen(myWin.toQC(glm::vec3(1.f, 1.f, 0.f)));

        //set text color for rotOrder items
        if (i.name == "XZY" || i.name == "YZX")
            painter.setPen(myWin.toQC(glm::vec3(0.f, 1.f, 0.f)));

        int usableTxtSize;
        if (i.name.size() > 8) usableTxtSize = 8;
        else if (i.name.size() > 15) usableTxtSize = 5;
        else usableTxtSize = 11;

        //cout << "i.name / len = " << i.name << i.name.size() << usableTxtSize << endl;
        painter.setFont(QFont("DejaVu Sans Mono", usableTxtSize, 75));
        painter.drawText(myRect, Qt::AlignCenter, QString::fromStdString(i.name));

        /* draw tris debug */
        //painter.setPen(myWin.toQC(glm::vec3(1.f, 0.f, 0.f))); //
        //painter.drawPolygon(i.hoverTri.translated(startP));
    }

    //draw line to cursor
    painter.setPen(QPen(myWin.toQC(glm::vec3(0.f)), 3));
    painter.drawLine(myWin.toQP(startP), myWin.toQP(newP));

    if (quitTgl)
    {
        painter.setCompositionMode (QPainter::CompositionMode_Source);

        glm::vec4 transp(0.f);
        auto myTransp = qRgba(transp.r, transp.g, transp.b, transp.a);
        painter.fillRect(rect(), QColor::fromRgba(myTransp));

        painter.setCompositionMode (QPainter::CompositionMode_SourceOver);

        hide();

        myWin.myGL->rmbTgl = 0;
        myWin.myGL->shiftTgl = 0;

        quitTgl = false;
    }

    painter.end();
}

bool RadPop::selCamLiTextColor(string name, string type)
{
    vector<string> lowerCase;

    if (type == "cam")
        lowerCase = { "back", "bottom", "front", "left", "right", "top", "back", "persp", "ortho", "fps" };

    else if (type == "light")
        lowerCase = { "area", "dir", "point", "spot" };

    for (auto &i : lowerCase)
    {
        if (name == i || name == myWin.stringToUpper(i))
            return true;
    }

    return false;
}

void RadPop::buttonInit(vector<PopSetup> inPop)
{
    vector<PopSetup> radPop, linPopN, linPopE, linPopS, linPopW;

    for (auto &i : inPop)
    {
        if      (i.type == "RAD") radPop.push_back(i);
        else if (i.type == "N") linPopN.push_back(i);
        else if (i.type == "E") linPopE.push_back(i);
        else if (i.type == "S") linPopS.push_back(i);
        else if (i.type == "W") linPopW.push_back(i);
    }

    //RAD
    float triBetween = 360 / (radPop.size() * 2);
    float radiusBoost = radius * 100;

    for (size_t i = 0; i < 360; ++i)
    {
        for (auto &j : radPop)
        {
            float theta = 2.f * PI * float(i) / 360;
            glm::vec2 myPt(radius * cos(theta), radius * sin(theta));

            j.move = myPt;

            auto usableI = (i == 0) ? 360 : i; //first angle == 360 if 0

            auto thetaTri1 = 2.f * PI * float(usableI - triBetween) / 360;
            glm::vec2 triPt1(radiusBoost * cos(thetaTri1), radiusBoost * sin(thetaTri1));

            auto thetaTri2 = 2.f * PI * float(i + triBetween) / 360;
            glm::vec2 triPt2(radiusBoost * cos(thetaTri2), radiusBoost * sin(thetaTri2));

            j.hoverTri << myWin.toQP(glm::vec2(0.f)) << myWin.toQP(triPt1) << myWin.toQP(triPt2);

            //get "full" adj angles
            //cout << "name = " << j.name << "move = " <<  glm::to_string(j.move) << "angle = " << i << endl;
            //cout << "triBetween = " << triBetween << "tri1Ang = " << usableI - triBetween << "tri2Ang = " << i + triBetween << endl;
            //cout << "triPt1 = " << glm::to_string(triPt1) << "triPt2 = " << glm::to_string(triPt2) << endl;

            i += 360 / radPop.size();
        }
    }

    if (!allPop.empty())
        allPop.clear();

    for (unsigned int i = 0; i < radPop.size(); ++i)
        allPop.push_back(radPop[i]);

    for (unsigned int i = 0; i < linPopN.size(); ++i)
    {
        linPopN[i].move = glm::vec2(linPopN[i].offX, 2.5f * radius + i * linPopN[i].sy + linPopN[i].offY);
        allPop.push_back(linPopN[i]);
    }

    for (unsigned int i = 0; i < linPopE.size(); ++i)
    {
        linPopE[i].move = glm::vec2(linPopE[i].offX, 2.5f * radius + i * linPopE[i].sy + linPopE[i].offY);
        allPop.push_back(linPopE[i]);
    }

    for (unsigned int i = 0; i < linPopS.size(); ++i)
    {
        linPopS[i].move = glm::vec2(linPopS[i].offX, 2.5f * radius + i * linPopS[i].sy + linPopS[i].offY);
        allPop.push_back(linPopS[i]);
    }

    for (unsigned int i = 0; i < linPopW.size(); ++i)
    {
        linPopW[i].move = glm::vec2(linPopW[i].offX, 2.5f * radius + i * linPopW[i].sy + linPopW[i].offY);
        allPop.push_back(linPopW[i]);
    }
}

bool RadPop::findCorrectAttrTable(string hit)
{
    for (unsigned int i = 0; i < myWin.cutTable->multiCut.size(); ++i)
    {
        if (popName == myWin.cutTable->multiCut[i]->name)
        {
            myWin.cutTable->changeEnumName = popName;
            myWin.cutTable->changeEnum(hit);
            myWin.cutTable->changeEnumName = "";

            return true;
        }
    }

    for (unsigned int i = 0; i < myWin.etcTable->multiEtc.size(); ++i)
    {
        if (popName == myWin.etcTable->multiEtc[i]->name)
        {
            myWin.etcTable->changeEnumName = popName;
            myWin.etcTable->changeEnum(hit);
            myWin.etcTable->changeEnumName = "";

            return true;
        }
    }

    for (unsigned int i = 0; i < myWin.glslTable->multiGL.size(); ++i)
    {
        if (popName == myWin.glslTable->multiGL[i]->name)
        {
            myWin.glslTable->changeEnumName = popName;
            myWin.glslTable->changeEnum(hit);
            myWin.glslTable->changeEnumName = "";

            return true;
        }
    }

    myWin.attrTable->changeEnumName = popName;
    myWin.attrTable->changeEnum(hit);
    myWin.attrTable->changeEnumName = "";

    return true;
}

void RadPop::hitFind(string hit)
{
    if (popName != "addObj" && popName != "viewChange") // enum
        findCorrectAttrTable(hit);

    else if (popName == "addObj")
    {
        if (hit == "camLi")
            myWin.camLiAdd();

        else
            myWin.objAdd(hit, "POP");
    }

    else if (popName == "viewChange")
    {
        if (hit == "left" || hit == "front" || hit == "top" || hit == "persp")
        {
            for (auto &i : myWin.allCamCombo)
            {
                int camIdx = i->findText(QString::fromStdString(hit));
                i->setCurrentIndex(camIdx);
                i->activated(camIdx);
            }
        }

        if (hit == "gizSpace") myWin.myGL->gizSpaceTgl_swap();
        else if (hit == "mpf") myWin.myGL->mpfTgl_swap();
        else if (hit == "rez") myWin.myGL->rezGateTgl_swap();
        else if (hit == "stats") myWin.myGL->statsTgl_swap();
    }
}

void RadPop::hoverHighlight()
{
    QRect rectCircPopupCenter(0, 0, myWin.glslTable->popCenterXY->val_i, myWin.glslTable->popCenterXY->val_i);
    rectCircPopupCenter.moveCenter(myWin.toQP(startP));

    if (rectCircPopupCenter.contains(myWin.toQP(newP)))
    {
        for (auto &i : allPop)
            i.hovered = false;
    }

    else
    {
        for (auto &i : allPop)
        {
            if (i.type != "rad")
            {
                QRect rectLin(0, 0, i.sx, i.sy);
                rectLin.moveCenter(myWin.toQP(startP + i.move));

                if (rectLin.contains(myWin.toQP(newP)))
                {
                    i.hovered = true;

                    for (auto &j : allPop)
                    {
                        if (i.name != j.name)
                            j.hovered = false;
                    }

                    continue;
                }
            }

            else
            {
                if (i.hoverTri.translated(myWin.toQP(startP)).containsPoint(myWin.toQP(newP), Qt::OddEvenFill))
                {
                    i.hovered = true;

                    for (auto &j : allPop)
                    {
                        if (i.name != j.name)
                            j.hovered = false;
                    }

                    continue;
                }
            }
        }
    }
}
