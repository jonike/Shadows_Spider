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

#ifndef LAYERSEL_H
#define LAYERSEL_H

#include "MainWin.h"

class LayerSel : public QListWidget
{
    Q_OBJECT
public:
    MainWin &myWin;
    LayerSel(MainWin &, QWidget *parent = 0);

    bool altTgl, ctrlTgl, shiftTgl, qTgl;
    bool lmbTgl, rmbTgl, mmbTgl;
    bool setupTgl, refreshAttrTgl, camAttrWrongType;
    bool getNewOrder = false;

    int useThisRow, lastRow, storedStackedIdx, tabIdx;

    vector<string> mySelLayList;

    string attrName, attrType;
    int attrIdx;
    QFont labelF, sepF, valF;

    vector<shared_ptr<Object>> selObjs;
    vector <shared_ptr<MultiAttr>> multiVec;

    bool refreshTable();
    void reorder(string);

public slots:
    void layerSelChanged();

//    void currItemChangedF(QListWidgetItem *, QListWidgetItem *);
//    void currRowChangedF(int);
//    void ItemActivatedF(QListWidgetItem *);
//    void layerItemChangedF(QListWidgetItem *);

protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *, QEvent *);

    void prepItems(QListWidgetItem *, string);
    void tableWidgetItem_add();
    void setSelection();
    void setRows();
};

#endif // LAYERSEL_H
