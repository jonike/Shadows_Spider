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

#ifndef OUTLINER_H
#define OUTLINER_H

#include "MainWin.h"

class Outliner : public QTreeWidget
{
    Q_OBJECT
public:
    MainWin &myWin;
    Outliner(MainWin &, QWidget *parent = 0);

    bool keepSel, lmbTgl, rmbTgl, mmbTgl, ctrlTgl, shiftTgl;
    bool ignoreChangeName = 0;
    unsigned int outLinerItemCt;

    vector<QTreeWidgetItem*> allLabels;


    void refreshOutliner(bool);

public slots:
    void changeName(QTreeWidgetItem *, int);
    void changeSel();
    void disableCuts();
    void fullyExpand(QTreeWidgetItem *);
    void fullyCollapse(QTreeWidgetItem *);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void dropEvent(QDropEvent *); //
    bool eventFilter(QObject *, QEvent *);

    void prepItems(QTreeWidgetItem *, QString);

    void visitTree(QStringList &list, QTreeWidgetItem *item);
    QStringList visitTree(QTreeWidget *tree);
};

#endif // OUTLINER_H
