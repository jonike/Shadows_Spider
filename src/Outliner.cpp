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

#include "MainWin.h"

Outliner::Outliner(MainWin &myWinTemp, QWidget *parent) : QTreeWidget(parent), myWin(myWinTemp)
{
    setColumnCount(1);
    header()->setVisible(0);
    setFrameShadow(QFrame::Raised);
    setItemDelegate(myWin.myFocus);
    setAlternatingRowColors(1);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    setExpandsOnDoubleClick(0);
    setSortingEnabled(0);
    setDefaultDropAction(Qt::MoveAction);
    setProperty("isOutliner", 1);

    connect(this, &Outliner::itemChanged, this, &Outliner::changeName);
    connect(this, &Outliner::itemSelectionChanged, this, &Outliner::changeSel);
    connect(this, &Outliner::itemExpanded, this, &Outliner::fullyExpand);
    connect(this, &Outliner::itemCollapsed, this, &Outliner::fullyCollapse);
    connect(this, &Outliner::activated, this, &Outliner::disableCuts);
    connect(this, &Outliner::itemDoubleClicked, this, &Outliner::disableCuts);

    keepSel = lmbTgl = rmbTgl = mmbTgl = ctrlTgl = shiftTgl = 0;
    refreshOutliner(1);

    installEventFilter(this);
}

void Outliner::enterEvent(QEvent *)
{
    setFocus();
}

void Outliner::leaveEvent(QEvent *)
{
    myWin.cutTable->multiCutTgl(1);
}

bool Outliner::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);

        if (ke->key() == Qt::Key_Tab)
        {
            QModelIndex currIdx = currentIndex();
            QModelIndex nextIdx;

            if ((unsigned int)currIdx.row() + 1 == outLinerItemCt)
                nextIdx = model()->index(0, 0);

            else
                nextIdx = model()->index(currIdx.row() + 1, 0);

            selectionModel()->select(nextIdx, QItemSelectionModel::Select);
            setCurrentIndex(nextIdx);
            setFocus();
            edit(nextIdx);
        }
    }

    return QWidget::eventFilter(obj, e);
}

void Outliner::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control)
        ctrlTgl = 1;

    else if (e->key() == Qt::Key_Shift)
        shiftTgl = 1;

    return QTreeWidget::keyPressEvent(e);
}

void Outliner::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    if (e->key() == Qt::Key_Control)
        ctrlTgl = 0;

    else if (e->key() == Qt::Key_Shift)
        shiftTgl = 0;

    return QTreeWidget::keyReleaseEvent(e);
}

void Outliner::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = 1; mmbTgl = 0; rmbTgl = 0;

        QTreeWidget::mousePressEvent(e);
    }

    else if (e->button() == Qt::RightButton)
    {
        lmbTgl = 0; rmbTgl = 1; mmbTgl = 0;
    }

    else if (e->button() == Qt::MiddleButton)
    {
        lmbTgl = 0; rmbTgl = 0; mmbTgl = 1;
    }

    if (mmbTgl) setDragDropMode(QAbstractItemView::DragDrop);
    else setDragDropMode(QAbstractItemView::NoDragDrop);
}

void Outliner::dropEvent(QDropEvent *e)
{
    QTreeWidgetItem *myItem = itemAt(e->pos());

    if (myItem)
    {
        QString dragParentTo = myItem->text(0);

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->selected)
                myWin.allObj[i]->parentObj(dragParentTo);
        }
    }

    else
    {
        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->selected)
                myWin.allObj[i]->parentTo = 0;
        }
    }

    return QTreeWidget::dropEvent(e);
}

void Outliner::changeName(QTreeWidgetItem *item, int col)
{
    if (!ignoreChangeName)
    {
        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->selected)
            {
                myWin.allObj[i]->rename(item->text(col));

                if (myWin.allObj[i]->type == "CAMLI")
                {
                    for (unsigned int j = 0; j < myWin.allCamCombo.size(); ++j)
                        myWin.allCamCombo[j]->refresh();
                }
            }
        }

        refreshOutliner(1);
        myWin.attrTable->refreshTable();
    }
}

void Outliner::changeSel()
{
    if (!keepSel)
    {
        myWin.clearSel();

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            foreach (QTreeWidgetItem *singleItem, selectedItems())
            {
                if (singleItem->text(0) == myWin.allObj[i]->name->val_s)
                {
                    myWin.allObj[i]->selected = 1;
                    myWin.selB = myWin.allObj[i];

                    break;
                }
            }
        }

        myWin.gizPivAutoShow(); //
        myWin.attrTable->refreshTable();
    }

    myWin.cutTable->multiCutTgl(1);
}

void Outliner::disableCuts()
{
    myWin.cutTable->multiCutTgl(0);
}

void Outliner::refreshOutliner(bool keep)
{
    if (keep)
        keepSel = 1;

    clear();

    outLinerItemCt = 0;

    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        if (!myWin.allObj[i]->ignoreOutliner)
        {
            QTreeWidgetItem *outlinerLabel = new QTreeWidgetItem();

            outlinerLabel->setData(0, 32, myWin.allObj[i]->name->val_s);

            if (myWin.allObj[i]->parentTo == 0) outlinerLabel->setData(0, 33, 0);
            else outlinerLabel->setData(0, 33, myWin.allObj[i]->parentTo->name->val_s); //parentTo

            outlinerLabel->setData(0, 34, myWin.allObj[i]->expand);
            outlinerLabel->setData(0, 35, myWin.allObj[i]->v->val_b);
            outlinerLabel->setData(0, 36, myWin.allObj[i]->type);

            outlinerLabel->setText(0, myWin.allObj[i]->name->val_s);

            QString myType = myWin.allObj[i]->type;

            if (myType == "CAMLI")
            {
                if (myWin.allObj[i]->camLiTypeGet("cam"))
                    myType = "CAM";

                else
                    myType = "LIGHT";
            }

            prepItems(outlinerLabel, myType);
            allLabels.push_back(outlinerLabel);

        }
    }

    outLinerItemCt = (unsigned int)allLabels.size();

    for (unsigned int i = 0; i < allLabels.size(); ++i)
    {
        if (allLabels[i]->data(0, 33) == 0)
            addTopLevelItem(allLabels[i]); //

        else
        {
            for (unsigned int j = 0; j < allLabels.size(); ++j)
            {
                if (allLabels[j]->text(0) == allLabels[i]->data(0, 33).toString())
                    allLabels[j]->addChild(allLabels[i]); //
            }
        }

        if (allLabels[i]->data(0, 34) == 1)
            allLabels[i]->setExpanded(1);

        for (unsigned int j = 0; j < myWin.allObj.size(); ++j) //WRONG - SAME COL FOR HIDDEN / VIZ
        {
            if (myWin.allObj[j]->selected && myWin.allObj[j]->name->val_s == allLabels[i]->data(0, 32))
                allLabels[i]->setSelected(1);
        }

        //CORRECT LABEL COLOR FOR HIDDEN / VIZ OBJ --- ADD THIS TO changeSel()
        //COULD ALSO USE SETPROPERTY() ON INDIVIDUAL SUBCLASSED CELL

//        for (int j = 0; j < myWin.allObj.size(); ++j)
//        {
//            if (myWin.allObj[j]->selected && myWin.allObj[j]->name->val_s == allLabels[i]->data(0, 32))
//            {
//                ignoreChangeName = 1;
//                allLabels[i]->setBackground(0, QBrush("#8e5511"));

//                if (myWin.allObj[j]->v->val_b)
//                    allLabels[i]->setForeground(0, QBrush("#B1B1B1")); //viz

//                else
//                    allLabels[i]->setForeground(0, QBrush(Qt::black)); //hidden

//                ignoreChangeName = 0;
//            }
//        }
    }

    allLabels.clear();
    keepSel = 0;
}

void Outliner::prepItems(QTreeWidgetItem *item, QString type)
{
    item->setFont(0, QFont("DejaVu Sans Mono", 10, 75));
    item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

    if (!item->data(0, 35).toBool()) item->setForeground(0, QBrush(Qt::black));

    QPixmap *myPixmap;

    if (type == "EMPTY" || type == "FBO" || type == "RTT" || type == "TXT")
    {
        myPixmap = new QPixmap(60, 60);
        myPixmap->fill(Qt::darkGray);
    }

    else if (type == "CAM")
    {
        myPixmap = new QPixmap("icons/camIcon.png");
    }

    else if (type == "LIGHT")
    {
        myPixmap = new QPixmap("icons/lightIcon.png");
    }

    else if (type == "GRID" || type == "OBJ")
        myPixmap = new QPixmap("icons/meshIcon.png");

    QIcon myIcon(*myPixmap);
    item->setIcon(0, myIcon);
}

void Outliner::visitTree(QStringList &list, QTreeWidgetItem *item)
{
    list << item->text(0);

    for (int i = 0; i < item->childCount(); ++i)
        visitTree(list, item->child(i));
}

QStringList Outliner::visitTree(QTreeWidget *tree)
{
    QStringList list;

    for(int i = 0; i < tree->topLevelItemCount(); ++i)
        visitTree(list, tree->topLevelItem(i));

    return list;
}

void Outliner::fullyCollapse(QTreeWidgetItem *item)
{
    if (shiftTgl)
    {
        for (int i = 0; i < item->childCount(); ++i)
        {
            item->child(i)->setExpanded(0);

            for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
            {
                if (item->data(0, 32) == myWin.allObj[j]->name->val_s)
                    myWin.allObj[j]->expand = 0;

                if (item->child(i)->data(0, 32) == myWin.allObj[j]->name->val_s)
                    myWin.allObj[j]->expand = 0;
            }
        }
    }

    else
    {
        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (item->data(0, 32) == myWin.allObj[i]->name->val_s)
                myWin.allObj[i]->expand = 0;
        }
    }
}

void Outliner::fullyExpand(QTreeWidgetItem *item)
{
    if (shiftTgl)
    {
        for (int i = 0; i < item->childCount(); ++i)
        {
            item->child(i)->setExpanded(1);

            for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
            {
                if (item->data(0, 32) == myWin.allObj[j]->name->val_s)
                    myWin.allObj[j]->expand = 1;

                else if (item->child(i)->data(0, 32) == myWin.allObj[j]->name->val_s)
                    myWin.allObj[j]->expand = 1;
            }
        }
    }

    else
    {
        for (unsigned int j = 0; j < myWin.allObj.size(); ++j)
        {
            if (item->data(0, 32) == myWin.allObj[j]->name->val_s)
                myWin.allObj[j]->expand = 1;
        }
    }
}
