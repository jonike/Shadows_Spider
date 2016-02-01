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

#include "Outliner.h"

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

    keepSel = lmbTgl = rmbTgl = mmbTgl = ctrlTgl = shiftTgl = false;
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
        auto *ke = static_cast<QKeyEvent*>(e);

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
    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_Alt)
        altTgl = true;

    else if (e->key() == Qt::Key_Control)
        ctrlTgl = true;

    else if (e->key() == Qt::Key_Shift)
        shiftTgl = true;

    else if (e->key() == Qt::Key_I)
        myWin.PaintWinTgl(1, 0);

    else if (e->key() == Qt::Key_K)
        myWin.PaintWinTgl(1, 1);

    return QTreeWidget::keyPressEvent(e);
}

void Outliner::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_Alt)
        altTgl = false;

    else if (e->key() == Qt::Key_Control)
        ctrlTgl = false;

    else if (e->key() == Qt::Key_Shift)
        shiftTgl = false;

    else if (e->key() == Qt::Key_F)
    {
        if (altTgl && !ctrlTgl) // alt+F
        {
            auto *myItem = selectedItems()[0];

            if (myItem) //if item selected
                scrollToItem(myItem);
        }

        else if (!altTgl && ctrlTgl)
        {
            //search bar popup
            cout << "search bar pop" << endl;
        }
    }

    else if (e->key() == Qt::Key_I)
    {
        if (myWin.myPaintWin->stackedMain->currentIndex() == 0)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_K)
    {
        if (myWin.myPaintWin->stackedMain->currentIndex() == 1)
            myWin.PaintWinTgl(0, 999);
    }

    return QTreeWidget::keyReleaseEvent(e);
}

void Outliner::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        lmbTgl = true; rmbTgl = false; mmbTgl = false;
    }

    else if (e->button() == Qt::RightButton)
    {
        lmbTgl = false; rmbTgl = true; mmbTgl = false;
    }

    else if (e->button() == Qt::MiddleButton)
    {
        lmbTgl = false; rmbTgl = false; mmbTgl = true;
    }

    if (mmbTgl)
        setDragDropMode(QAbstractItemView::InternalMove);

    else
        setDragDropMode(QAbstractItemView::NoDragDrop);

    return QTreeWidget::mousePressEvent(e);
}

void Outliner::dropEvent(QDropEvent *e) // parentTo (drop) / unparent
{
    auto *myItem = itemAt(e->pos());

    if (myItem)
    {
        auto dragParentTo = myItem->text(0).toStdString();

        for (auto &i : myWin.allObj)
        {
            if (i->selected)
                i->parentObj(dragParentTo);
        }
    }

    else
    {
        for (auto &i : myWin.allObj)
        {
            if (i->selected)
                i->parentTo = 0;
        }
    }

    return QTreeWidget::dropEvent(e);
}

void Outliner::changeName(QTreeWidgetItem *item, int col)
{
    if (!ignoreChangeName)
    {
        for (auto &i : myWin.allObj)
        {
            if (i->selected)
            {
                i->rename(item->text(col).toStdString());

                if (i->type == "CAMLI")
                {
                    for (auto &j : myWin.allCamCombo)
                        j->refresh();
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

        for (auto &i : myWin.allObj)
        {
            for (QTreeWidgetItem *singleItem : selectedItems())
            {
                if (singleItem->text(0).toStdString() == i->name->val_s)
                {
                    i->selected = true;
                    myWin.selB = i;

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
        keepSel = true;

    clear();

    outLinerItemCt = 0;

    for (auto &i : myWin.allObj)
    {
        if (!i->ignoreOutliner)
        {
            auto *outlinerLabel = new QTreeWidgetItem();

            outlinerLabel->setData(0, 32, QString::fromStdString(i->name->val_s));

            if (i->parentTo == 0) outlinerLabel->setData(0, 33, 0);
            else outlinerLabel->setData(0, 33, QString::fromStdString(i->parentTo->name->val_s)); //parentTo

            outlinerLabel->setData(0, 34, i->expand);
            outlinerLabel->setData(0, 35, i->v->val_b);
            outlinerLabel->setData(0, 36, QString::fromStdString(i->type));

            outlinerLabel->setText(0, QString::fromStdString(i->name->val_s));

            auto myType = i->type;

            if (myType == "CAMLI")
            {
                if (i->camLiTypeGet("cam"))
                    myType = "CAM";

                else
                    myType = "LIGHT";
            }

            prepItems(outlinerLabel, myType);
            allLabels.push_back(outlinerLabel);
        }
    }

    outLinerItemCt = (unsigned int)allLabels.size();

    for (auto &i : allLabels)
    {
        if (i->data(0, 33) == 0)
            addTopLevelItem(i); //

        else
        {
            for (auto &j : allLabels)
            {
                if (j->text(0) == i->data(0, 33).toString())
                    j->addChild(i); //
            }
        }

        if (i->data(0, 34) == 1)
            i->setExpanded(1);

        for (auto &j : myWin.allObj) // WRONG RESULTING COLOR .... GIVES SAME COL FOR HIDDEN / VIZ OBJECTS
        {
            if (j->selected && i->data(0, 32).toString().toStdString() == j->name->val_s)
                i->setSelected(1);
        }

        //CORRECT LABEL COLOR FOR HIDDEN / VIZ OBJ --- ADD THIS TO changeSel()
        //COULD ALSO USE SETPROPERTY() ON INDIVIDUAL SUBCLASSED CELL

//        for (auto &j : myWin.allObj)
//        {
//            if (j->selected && j->name->val_s == i->data(0, 32).toString().toStdString())
//            {
//                ignoreChangeName = true;
//                i->setBackground(0, QBrush("#8e5511"));

//                if (j->v->val_b)
//                    i->setForeground(0, QBrush("#B1B1B1")); //viz

//                else
//                    i->setForeground(0, QBrush(myWin.toQC(glm::vec3(0.f)))); //hidden

//                ignoreChangeName = false;
//            }
//        }
    }

    allLabels.clear();
    keepSel = false;
}

void Outliner::prepItems(QTreeWidgetItem *item, string type)
{
    item->setFont(0, QFont("DejaVu Sans Mono", 10, 75));
    item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

    if (!item->data(0, 35).toBool()) item->setForeground(0, QBrush(myWin.toQC(glm::vec3(0.f))));

    QPixmap *myPixmap;

    if (type == "EMPTY" || type == "FBO" || type == "TXT")
    {
        myPixmap = new QPixmap(60, 60);
        myPixmap->fill(myWin.toQC(glm::vec3(.5f)));
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

void Outliner::fullyCollapse(QTreeWidgetItem *item)
{
    if (shiftTgl)
    {
        for (int i = 0; i < item->childCount(); ++i)
        {
            item->child(i)->setExpanded(0);

            for (auto &j : myWin.allObj)
            {
                if (item->data(0, 32).toString().toStdString() == j->name->val_s)
                    j->expand = false;

                if (item->child(i)->data(0, 32).toString().toStdString() == j->name->val_s)
                    j->expand = false;
            }
        }
    }

    else
    {
        for (auto &i : myWin.allObj)
        {
            if (item->data(0, 32).toString().toStdString() == i->name->val_s)
                i->expand = false;
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

            for (auto &j : myWin.allObj)
            {
                if (item->data(0, 32).toString().toStdString() == j->name->val_s)
                    j->expand = true;

                else if (item->child(i)->data(0, 32).toString().toStdString() == j->name->val_s)
                    j->expand = true;
            }
        }
    }

    else
    {
        for (auto &j : myWin.allObj)
        {
            if (item->data(0, 32).toString().toStdString() == j->name->val_s)
                j->expand = true;
        }
    }
}
