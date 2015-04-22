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

#include "CPop.h"

CPop::CPop(MainWin &myWinTemp, QWidget *parent) : QWidget(parent), myWin(myWinTemp)
{
    changedManip = ctrlTgl = mmbTgl = 0;
    setInitPos = 1;

    hue = 1.f; // start at 1 to scroll down by default
    sat = val = 0.f;

    QShortcut *quit_s = new QShortcut(tr("CTRL+Q"), this);
    connect(quit_s, SIGNAL(activated()), qApp, SLOT(quit()));
}

void CPop::enterEvent(QEvent *)
{
    setFocus();
}

void CPop::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton)
        mmbTgl = 1;
}

void CPop::mouseReleaseEvent(QMouseEvent *e)
{
    mmbTgl = 0;

    if (e->button() == Qt::LeftButton)
        myWin.TglCPopWin();

    else if (e->button() == Qt::MiddleButton && targetAttr != 0)
    {
        myWin.attrTable->refreshTable();
        myWin.glslTable->refreshTable();
    }
}

void CPop::mouseMoveEvent(QMouseEvent *e)
{
    if (mmbTgl)
    {
        sat = glm::max(0.f, min((float)e->pos().x() / (float)width(), 1.f));
        val = glm::max(0.f, min((float)e->pos().y() / (float)width(), 1.f));

        update();
        myWin.myGLWidgetSh->UBO_light_needsUp = 1; //
    }
}

void CPop::wheelEvent(QWheelEvent *e)
{
    float dragBoost = 1.f;

    if (ctrlTgl)
        dragBoost = myWin.etcTable->dragBoostHSV->val_f;

    float usableD = (float)e->delta() * .00008f * dragBoost;
    hue = glm::max(0.f, min(hue + usableD, 1.f));

    drawHueRect();
    update();
    myWin.myGLWidgetSh->UBO_light_needsUp = 1; //

    if (targetAttr != 0)
    {
        myWin.attrTable->refreshTable();
        myWin.glslTable->refreshTable();
    }
}

void CPop::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control)
        ctrlTgl = 1;
}

void CPop::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        myWin.TglCPopWin();

    else if (e->key() == Qt::Key_Control)
        ctrlTgl = 0;

    else if (e->key() == Qt::Key_C)
        myWin.TglCPopWin();

    else if (e->key() == Qt::Key_QuoteLeft)
        myWin.PrefWinOpen();
}

void CPop::resizeEvent(QResizeEvent *)
{
    drawHueRect();
}

void CPop::drawHueRect()
{
    HSV_image = QImage(width(), width(), QImage::Format_RGB32);

    for (int i = 0; i < width(); ++i)
    {
        for (int j = 0; j < width(); ++j)
        {
            QColor HSV_to_RGB = QColor::fromHsvF(hue, float(i) / width(), float(j) / width()).toHsv();

            QColor sRGB;
            sRGB.setRgbF(pow(HSV_to_RGB.redF(), myWin.gamma), pow(HSV_to_RGB.greenF(), myWin.gamma), pow(HSV_to_RGB.blueF(), myWin.gamma));

            HSV_image.setPixel(i, j, sRGB.rgb());
        }
    }
}

void CPop::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawImage(rect(), HSV_image);

    //manip
    if (mmbTgl || setInitPos || changedManip)
    {
        usableSat = sat * width();
        usableVal = val * width();

        glm::vec2 p(usableSat, usableVal);
        int triSize = myWin.glslTable->CPopManipSize->val_i;

        CPopManipLeft = glm::vec2(p.x - triSize, p.y + triSize);
        CPopManipRight = glm::vec2(p.x + triSize, p.y + triSize);
        CPopManipTop = glm::vec2(p.x, p.y - triSize);

        if (setInitPos)
            setInitPos = 0;

        if (changedManip)
            changedManip = 0;
    }

    QPoint ChooseRectManipPts[3] =
    {
        myWin.toQP(CPopManipLeft), myWin.toQP(CPopManipRight), myWin.toQP(CPopManipTop),
    };

    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter.drawPolygon(ChooseRectManipPts, 3);

    if (targetAttr != 0)
    {
        QColor toRGB = QColor::fromHsvF(hue, sat, val).toRgb();

        myWin.myCPopWin->myCPop->targetAttr->val_3 = glm::vec3(toRGB.redF(), toRGB.greenF(), toRGB.blueF());
    }
}
