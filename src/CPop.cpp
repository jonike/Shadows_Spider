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

#include "CPop.h"

CPop::CPop(MainWin &myWinTemp, QWidget *parent) : QWidget(parent), myWin(myWinTemp)
{
    changedManip = ctrlTgl = mmbTgl = false;
    setInitPos = true;

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
        mmbTgl = true;
}

void CPop::mouseReleaseEvent(QMouseEvent *e)
{
    mmbTgl = false;

    if (e->button() == Qt::MiddleButton && targetAttr != 0)
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

    if (targetAttr != 0)
    {
        myWin.attrTable->refreshTable();
        myWin.glslTable->refreshTable();
    }
}

void CPop::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control)
        ctrlTgl = true;
}

void CPop::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_Control)
        ctrlTgl = false;

    else if (e->key() == Qt::Key_K)
        myWin.PaintWinTgl(0, 999);
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
            QColor HSV_to_RGB = QColor::fromHsvF(hue, float(i) / width(), float(j) / width());

            QColor sRGB = QColor::fromRgbF(pow(HSV_to_RGB.redF(), 1.f), pow(HSV_to_RGB.greenF(), 1.f), pow(HSV_to_RGB.blueF(), 1.f));
            //QColor sRGB = QColor::fromRgbF(pow(HSV_to_RGB.redF(), myWin.gamma), pow(HSV_to_RGB.greenF(), myWin.gamma), pow(HSV_to_RGB.blueF(), myWin.gamma));

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
            setInitPos = true;

        if (changedManip)
            changedManip = false;
    }

    glm::vec2 ChooseRectManipPts0[3] = { CPopManipLeft, CPopManipRight, CPopManipTop };

    QPoint ChooseRectManipPts[3];
    for (int i = 0; i < 3; ++i)
        ChooseRectManipPts[i] = myWin.toQP(ChooseRectManipPts0[i]);

    painter.setPen(QPen(myWin.toQC(glm::vec3(0.f)), 2, Qt::SolidLine));
    painter.drawPolygon(ChooseRectManipPts, 3);

    if (targetAttr != 0)
    {
        targetAttr->val_3 = glm::rgbColor(glm::hsvColor(glm::vec3(hue, sat, val)));
    }
}
