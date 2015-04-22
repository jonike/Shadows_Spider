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

#include "RadPop.h"

RadPop::RadPop(MainWin &myWinTemp, QWidget *parent) : QMainWindow(parent), myWin(myWinTemp)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground, 1);
    setStyleSheet(myWin.myStyle); //

    QDesktopWidget *dw = QApplication::desktop();
    setFixedSize(dw->width(), dw->height());

    QShortcut *quit_s = new QShortcut(tr("CTRL+Q"), this);
    connect(quit_s, SIGNAL(activated()), qApp, SLOT(quit()));

    QTimer *upTimer = new QTimer(this);
    connect(upTimer, SIGNAL(timeout()), this, SLOT(update()));
    upTimer->start(16); //update every 16ms
}

void RadPop::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        quitTgl = 1;
}

void RadPop::mouseMoveEvent(QMouseEvent *e)
{
    newP = myWin.toVec2(e->pos());

    hoverHighlight();
}

void RadPop::mouseReleaseEvent(QMouseEvent *)
{
    for (unsigned int i = 0; i < allPop.size(); ++i)
    {
        if (allPop[i].hovered)
        {
            hitFind(allPop[i].name);

            break;
        }
    }

    quitTgl = 1;
}

void RadPop::paintEvent(QPaintEvent *)
{
    // START 2D
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //draw a circle at mouse pos
    QRect rectCircPopupCenter(0, 0, myWin.glslTable->popCenterXY->val_i, myWin.glslTable->popCenterXY->val_i);
    rectCircPopupCenter.moveCenter(myWin.toQP(startP));

    painter.setPen(Qt::black);
    painter.drawEllipse((rectCircPopupCenter));

    //draw rects
    for (unsigned int i = 0; i < allPop.size(); ++i)
    {
        QRect myRect(0, 0, allPop[i].sx, allPop[i].sy);
        myRect.moveCenter(myWin.toQP(startP + allPop[i].move));

        //colors
        glm::vec3 Cadj = myWin.gammaCsel();
        QColor Chigh = QColor::fromRgbF(Cadj.r, Cadj.g, Cadj.b);
        QColor CrectBG;

        if (myRect.contains(myWin.toQP(newP)) || allPop[i].hovered)
            CrectBG = Chigh;

        else
            CrectBG = QColor(Qt::gray);

        painter.setBrush(QBrush(CrectBG, Qt::SolidPattern));
        painter.setPen(Qt::black); //
        painter.drawRect(myRect);

        //set text color for camli items
        if (selCamLiTextColor(allPop[i].name, "cam")) painter.setPen(Qt::blue);
        else if (selCamLiTextColor(allPop[i].name, "light")) painter.setPen(Qt::yellow);

        //set text color for rotOrder items
        if (allPop[i].name == "XZY" || allPop[i].name == "YZX")
            painter.setPen(Qt::green);

        int usableTxtSize;
        if (allPop[i].name.size() > 8) usableTxtSize = 8;
        else if (allPop[i].name.size() > 15) usableTxtSize = 5;
        else usableTxtSize = 11;

//        qDebug() << "allPop[i].name / len = " << allPop[i].name << allPop[i].name.size() << usableTxtSize;

        painter.setFont(QFont("DejaVu Sans Mono", usableTxtSize, 75));
        painter.drawText(myRect, Qt::AlignCenter, allPop[i].name);

//        //draw tris debug
//        painter.setPen(Qt::red); //
//        painter.drawPolygon(allPop[i].hoverTri.translated(startP));
    }

    //draw line to cursor
    painter.setPen(QPen(Qt::black, 3));
    painter.drawLine(myWin.toQP(startP), myWin.toQP(newP));

    if (quitTgl)
    {
        painter.setCompositionMode (QPainter::CompositionMode_Source);
        painter.fillRect(rect(), Qt::transparent);
        painter.setCompositionMode (QPainter::CompositionMode_SourceOver);

        hide();

        for (unsigned int i = 0; i < myWin.allGL.size(); ++i)
        {
            myWin.allGL[i]->rmbTgl = 0;
            myWin.allGL[i]->shiftTgl = 0;
        }

        quitTgl = 0;
    }

    painter.end();
}

bool RadPop::selCamLiTextColor(QString name, QString type)
{
    QStringList lowerCase;

    if (type == "cam")
        lowerCase << "back" << "bottom" << "front" << "left" << "right" << "top" << "back" << "persp" << "ortho" << "fps";

    else if (type == "light")
        lowerCase << "area" << "dir" << "point" << "spot";

    for (int i = 0; i < lowerCase.size(); ++i)
    {
        if (name == lowerCase[i] || name == lowerCase[i].toUpper())
            return 1;
    }

    return 0;
}

void RadPop::buttonInit(vector<PopSetup> inPop)
{
    vector<PopSetup> radPop, linPopN, linPopE, linPopS, linPopW;

    for (unsigned int i = 0; i < inPop.size(); ++i)
    {
        if (inPop[i].type == "RAD") radPop.push_back(inPop[i]);
        if (inPop[i].type == "N") linPopN.push_back(inPop[i]);
        if (inPop[i].type == "E") linPopE.push_back(inPop[i]);
        if (inPop[i].type == "S") linPopS.push_back(inPop[i]);
        if (inPop[i].type == "W") linPopW.push_back(inPop[i]);
    }

    //RAD
    float triBetween = 360 / (radPop.size() * 2);
    float radiusBoost = radius * 100;

    for (size_t i = 0; i < 360; ++i)
    {
        for (unsigned int j = 0; j < radPop.size(); ++j)
        {
            float theta = 2.f * PI * float(i) / 360;
            glm::vec2 myPt(radius * cos(theta), radius * sin(theta));

            radPop[j].move = myPt;

            size_t usableI = (i == 0) ? 360 : i; //first angle == 360 if 0

            float thetaTri1 = 2.f * PI * float(usableI - triBetween) / 360;
            glm::vec2 triPt1(radiusBoost * cos(thetaTri1), radiusBoost * sin(thetaTri1));

            float thetaTri2 = 2.f * PI * float(i + triBetween) / 360;
            glm::vec2 triPt2(radiusBoost * cos(thetaTri2), radiusBoost * sin(thetaTri2));

            radPop[j].hoverTri << QPoint(0, 0) << myWin.toQP(triPt1) << myWin.toQP(triPt2);

            //get "full" adj angles
            //qDebug() << "name = " << radPop[j].name << "move = " <<  radPop[j].move << "angle = " << i;
            //qDebug() << "triBetween = " << triBetween << "tri1Ang = " << usableI - triBetween << "tri2Ang = " << i + triBetween;
            //qDebug() << "triPt1 = " << triPt1 << "triPt2 = " << triPt2;
            //qDebug() << endl;

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

bool RadPop::findCorrectAttrTable(QString hit)
{
    for (unsigned int i = 0; i < myWin.cutTable->multiCut.size(); ++i)
    {
        if (popName == myWin.cutTable->multiCut[i]->name)
        {
            myWin.cutTable->changeEnumName = popName;
            myWin.cutTable->changeEnum(hit);
            myWin.cutTable->changeEnumName = "";

            return 1;
        }
    }

    for (unsigned int i = 0; i < myWin.etcTable->multiEtc.size(); ++i)
    {
        if (popName == myWin.etcTable->multiEtc[i]->name)
        {
            myWin.etcTable->changeEnumName = popName;
            myWin.etcTable->changeEnum(hit);
            myWin.etcTable->changeEnumName = "";

            return 1;
        }
    }

    for (unsigned int i = 0; i < myWin.glslTable->multiGL.size(); ++i)
    {
        if (popName == myWin.glslTable->multiGL[i]->name)
        {
            myWin.glslTable->changeEnumName = popName;
            myWin.glslTable->changeEnum(hit);
            myWin.glslTable->changeEnumName = "";

            return 1;
        }
    }

    myWin.attrTable->changeEnumName = popName;
    myWin.attrTable->changeEnum(hit);
    myWin.attrTable->changeEnumName = "";

    return 1;

}

void RadPop::hitFind(QString hit)
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
            for (unsigned int j = 0; j < myWin.allCamCombo.size(); ++j)
            {
                if (myWin.allCamCombo[j]->myGL.get() == myGL.get())
                {
                    int camIdx = myWin.allCamCombo[j]->findText(hit);
                    myWin.allCamCombo[j]->setCurrentIndex(camIdx);
                    myWin.allCamCombo[j]->activated(camIdx);
                }
            }
        }

        if (hit == "gizSpace") myGL.get()->gizSpaceTgl_();
        else if (hit == "mpf") myGL.get()->mpfTgl_();
        else if (hit == "rez") myGL.get()->rezGateTgl_();
        else if (hit == "rtt") myGL.get()->rttVizTgl_();
        else if (hit == "stats") myGL.get()->statsTgl_();

        else if (hit == "hLay") myWin.hLayTgl();
        else if (hit == "gridLay") myWin.gridLayTgl();
        else if (hit == "vLay") myWin.vLayTgl();
    }
}

void RadPop::hoverHighlight()
{
    QRect rectCircPopupCenter(0, 0, myWin.glslTable->popCenterXY->val_i, myWin.glslTable->popCenterXY->val_i);
    rectCircPopupCenter.moveCenter(myWin.toQP(startP));

    if (rectCircPopupCenter.contains(myWin.toQP(newP)))
    {
        for (unsigned int i = 0; i < allPop.size(); ++i)
            allPop[i].hovered = 0;
    }

    else
    {
        for (unsigned int i = 0; i < allPop.size(); ++i)
        {
            if (allPop[i].type != "rad")
            {
                QRect rectLin(0, 0, allPop[i].sx, allPop[i].sy);
                rectLin.moveCenter(myWin.toQP(startP + allPop[i].move));

                if (rectLin.contains(myWin.toQP(newP)))
                {
                    allPop[i].hovered = 1;

                    for (unsigned int j = 0; j < allPop.size(); ++j)
                    {
                        if (j != i)
                            allPop[j].hovered = 0;
                    }

                    continue;
                }
            }

            else
            {
                if (allPop[i].hoverTri.translated(myWin.toQP(startP)).containsPoint(myWin.toQP(newP), Qt::OddEvenFill))
                {
                    allPop[i].hovered = 1;

                    for (unsigned int j = 0; j < allPop.size(); ++j)
                    {
                        if (j != i)
                            allPop[j].hovered = 0;
                    }

                    continue;
                }
            }
        }
    }
}
