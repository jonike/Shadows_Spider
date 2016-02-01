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

#include "LayerSel.h"

LayerSel::LayerSel(MainWin &myWinTemp, QWidget *parent) : QListWidget(parent), myWin(myWinTemp)
{
    setFrameShadow(QFrame::Raised);
    setItemDelegate(myWin.myFocus);
    setAlternatingRowColors(1);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setSortingEnabled(0);
    setDefaultDropAction(Qt::MoveAction);
    setDragEnabled(1);

    labelF = QFont("DejaVu Sans Mono", 10, 75);
    sepF = QFont("DejaVu Sans Mono", 8, 75);
    valF = QFont("DejaVu Sans Mono", 9, 75);

    connect(this, &LayerSel::itemSelectionChanged, this, &LayerSel::layerSelChanged);

//    connect(this, &LayerSel::currentItemChanged, this, &LayerSel::currItemChangedF);
//    connect(this, &LayerSel::currentRowChanged, this, &LayerSel::currRowChangedF);
//    connect(this, &LayerSel::itemActivated, this, &LayerSel::ItemActivatedF);
//    connect(this, &LayerSel::itemChanged, this, &LayerSel::layerItemChangedF);

    altTgl = ctrlTgl = shiftTgl = qTgl = lmbTgl = rmbTgl = mmbTgl = camAttrWrongType = false;
    tabIdx = 0;

    installEventFilter(this);
}

//void LayerSel::currItemChangedF(QListWidgetItem *itemCurr, QListWidgetItem *itemPrev)
//{
//    cout << "in currItemChangedF" << endl;
//}

//void LayerSel::currRowChangedF(int row)
//{
//    cout << "in currRowChangedF" << endl;
//}

//void LayerSel::ItemActivatedF(QListWidgetItem *item)
//{
//    cout << "in ItemActivatedF" << endl;
//}

//void LayerSel::layerItemChangedF(QListWidgetItem *item)
//{
//    cout << "in layerItemChangedF" << endl;
//}

void LayerSel::dropEvent(QDropEvent *e)
{
    getNewOrder = true;

    return QListWidget::dropEvent(e);
}

void LayerSel::reorder(string type)
{
    //set the new idx in the layer
    for (auto &i : myWin.myGLWidgetSh->allMaps)
    {
        if (i.type == myWin.selB->texSel.type && i.name == myWin.selB->texSel.map.name)
        {
            for (auto &j : i.layer)
            {
                for (int k = 0; k < count(); ++k)
                {
                    QListWidgetItem *findItem = item(k);

                    if (j.name == findItem->text().toStdString())
                        j.idx = k;
                }
            }
        }
    }
}

bool LayerSel::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::KeyPress)
    {
        auto *ke = static_cast<QKeyEvent*>(e);

        if (ke->key() == Qt::Key_Space)
        {
            e->ignore();

            return true;
        }
    }

    return QWidget::eventFilter(obj, e);
}

void LayerSel::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control) ctrlTgl = true;
    else if (e->key() == Qt::Key_Q) qTgl = true;

    if (ctrlTgl && e->key() == Qt::Key_Q)
        myWin.quitConfirm();

    QListWidget::keyPressEvent(e);
}

void LayerSel::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat())
        return;

    else if (e->key() == Qt::Key_Alt) altTgl = false;
    else if (e->key() == Qt::Key_Control) ctrlTgl = false;
    else if (e->key() == Qt::Key_Shift) shiftTgl = false;

    else if (e->key() == Qt::Key_I)
    {
        //close layerSel
        if (myWin.myPaintWin->stackedMain->currentIndex() == 0)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_K)
    {
        //close colorPick
        if (myWin.myPaintWin->stackedMain->currentIndex() == 1)
            myWin.PaintWinTgl(0, 999);
    }

    else if (e->key() == Qt::Key_Space)
    {
        for (int i = 0; i < myWin.myFSQ->displMode->comboList.size(); ++i)
        {
            if (myWin.myFSQ->displMode->comboList[i] == myWin.myFSQ->displMode->val_s) //full / layer / map displ modes
            {
                int newIdx;

                if (i < myWin.myFSQ->displMode->comboList.size() - 1)
                    newIdx = ++i;

                else
                    newIdx = 0;

                myWin.myFSQ->displMode->val_s = myWin.myFSQ->displMode->comboList[newIdx];
                myWin.attrTable->refreshTable();
                //cout << "new displMode / newIdx : " << myWin.myFSQ->displMode->val_s << newIdx << endl;

                break;
            }
        }
    }

    setCursor(Qt::ArrowCursor);
}

void LayerSel::mousePressEvent(QMouseEvent *e)
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


   return QListWidget::mousePressEvent(e);
}

void LayerSel::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        lmbTgl = false;

    setCursor(Qt::ArrowCursor);

    return QListWidget::mouseReleaseEvent(e);
}

void LayerSel::layerSelChanged()
{
    vector<QListWidgetItem *> selItems;

    for (auto &i : selectedItems())
        selItems.push_back(i);

    if (!setupTgl && !selItems.empty())
    {
        string selText = selItems[0]->text().toStdString();

        if (this == myWin.objectLay)
        {
            myWin.clearSel();

            for (auto &i : myWin.allObj)
            {
                if (i->name->val_s == selText)
                {
                    i->selected = true;
                    myWin.selB = i; //

                    myWin.myOutliner->refreshOutliner(1);
                    myWin.attrTable->refreshTable();

                    break;
                }
            }
        }

        else if (this == myWin.typeLay) //change type / map / layer
        {
            myWin.selB->texSel.type = selText;

            for (auto &i : myWin.myGLWidgetSh->allMaps)
            {
                if (i.type == myWin.selB->texSel.type)
                {
                    for (auto &j : myWin.selB->texSel.typeMap)
                    {
                        if (j.x.name == myWin.selB->texSel.type)
                        {
                            //texSel (on init) is: type, ID (0), name, ID

                            myWin.selB->texSel.map.name = j.y.name;
                            myWin.selB->texSel.map.ID = j.y.ID;

                            for (auto &k : myWin.selB->texSel.mapLayer)
                            {
                                if (k.x.ID == j.y.ID)
                                {
                                    myWin.selB->texSel.layer.name = k.y.name;
                                    myWin.selB->texSel.layer.ID = k.y.ID;
                                }
                            }
                        }
                    }
                }
            }
        }

        else if (this == myWin.mapLay) // change map / layer
        {
            myWin.selB->texSel.map.name = selText;

            for (auto &i : myWin.myGLWidgetSh->allMaps)
            {
                if (i.type == myWin.selB->texSel.type && i.name == myWin.selB->texSel.map.name)
                {
                    myWin.selB->texSel.map.ID = i.ID;

                    for (auto &j : myWin.selB->texSel.typeMap)
                    {
                        if (j.x.name == myWin.selB->texSel.type)
                        {
                            j.y.name = myWin.selB->texSel.map.name;
                            j.y.ID = i.ID;

                            for (auto &k : myWin.selB->texSel.mapLayer)
                            {
                                if (k.x.ID == j.y.ID)
                                {
                                    k.x.name = myWin.selB->texSel.map.name;
                                    k.x.ID = i.ID;

                                    myWin.selB->texSel.layer.name = k.y.name;
                                    myWin.selB->texSel.layer.ID = k.y.ID;
                                }
                            }
                        }
                    }
                }
            }
        }

        else if (this == myWin.layerLay) //change layer
        {
            if (selItems.size() == 1) //REG, painting
            {
                mySelLayList.clear();

                myWin.selB->texSel.layer.name = selText;

                for (auto &i : myWin.myGLWidgetSh->allMaps)
                {
                    //if (i.ID == myWin.selB->texSel.map.ID) // map shortcut
                    if (i.type == myWin.selB->texSel.type)
                    {
                        for (auto &j : i.layer)
                        {
                            if (j.name == myWin.selB->texSel.layer.name)
                            {
                                for (auto &k : myWin.selB->texSel.typeMap)
                                {
                                    if (k.x.name == myWin.selB->texSel.type) //type match for typeMap and texSel.type
                                    {
                                        for (auto &l : myWin.selB->texSel.mapLayer)
                                        {
                                            if (l.x.ID == i.ID && l.x.ID == k.y.ID)
                                            {
                                                l.y.name = myWin.selB->texSel.layer.name;
                                                l.y.ID = j.ID;

//                                                cout << "name / ID = " << l.y.name << l.y.ID << endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            else if (selItems.size() > 1) //multiple sel, moving, deleting, etc
            {
                mySelLayList.clear();

                if (this == myWin.layerLay)
                {
                    for (auto  &item : selectedItems())
                    {
                        //cout << "name of selItem = " << item->text().toStdString() << endl;
                        mySelLayList.push_back(item->text().toStdString());
                    }
                }
            }
        }

        myWin.refreshAllPaintTables();
    }
}

bool LayerSel::refreshTable()
{
    clear();

    refreshAttrTgl = true;

    lastRow = 0;
    useThisRow = 0;
    setupTgl = true;
    multiVec.clear();

    setRows();

    for (unsigned int i = 0; i < multiVec.size(); ++i)
    {
        attrIdx = multiVec[i]->idx;
        attrName = multiVec[i]->name;
        attrType = multiVec[i]->type;

        useThisRow = i;
//        setRowHeight(i, myWin.etcTable->rowH->val_i);

        tableWidgetItem_add();
    }

    setSelection();

    useThisRow = 0;
    setupTgl = false;
    refreshAttrTgl = false;

    return true;
}

void LayerSel::setRows()
{
    if (this == myWin.objectLay) // ADD ALL OBJS
    {
        for (auto &i : myWin.allObj)
        {
            if (i->type == "OBJ")
            {
                shared_ptr<MultiAttr> attrNew;
                attrNew = make_shared<MultiAttr>();
                attrNew->name = i->name->val_s;
                attrNew->type = "string";
                attrNew->val_s = i->name->val_s;
                multiVec.push_back(attrNew);
            }
        }
    }

    else if (this == myWin.typeLay) // ADD ALL TYPES
    {
        //typeList = { "ALBEDO", "ALPHA", "ANISO", "LENS", "METALLIC", "RUFF", "SSS" };

        for (auto &i : myWin.myGLWidgetSh->typeList)
        {
            shared_ptr<MultiAttr> attrNew;
            attrNew = make_shared<MultiAttr>();
            attrNew->name = i;
            attrNew->type = "string";
            attrNew->val_s = i;
            multiVec.push_back(attrNew);
        }
    }

    else if (this == myWin.mapLay) //ADD MAPS FOR SELECTED TYPE
    {
        for (auto &i : myWin.myGLWidgetSh->allMaps)
        {
            if (i.type == myWin.selB->texSel.type)
            {
                shared_ptr<MultiAttr> attrNew;
                attrNew = make_shared<MultiAttr>();
                attrNew->name = i.name;
                attrNew->type = "string";
                attrNew->val_s = i.name;
                multiVec.push_back(attrNew);
            }
        }
    }

    else if (this == myWin.layerLay) //ADD LAYERS FOR SELECTED MAP
    {
        for (auto &i : myWin.myGLWidgetSh->allMaps)
        {
            if (i.type == myWin.selB->texSel.type)
            {
                if (i.name == myWin.selB->texSel.map.name) //
                {
                    for (auto &j : i.layer)
                    {
                        shared_ptr<MultiAttr> attrNew;
                        attrNew = make_shared<MultiAttr>();
                        attrNew->idx = j.idx;
                        attrNew->name = j.name;
                        attrNew->type = "string";
                        attrNew->val_s = j.name;
                        multiVec.push_back(attrNew);
                    }
                }
            }
        }
    }
}

void LayerSel::setSelection()
{
    vector<QListWidgetItem *> findItem;

    if (this == myWin.layerLay && mySelLayList.size() > 1)
    {
        for (auto &i : mySelLayList)
        {
            for (int j = 0; j < findItems(QString::fromStdString(i), Qt::MatchExactly).size(); ++j)
                findItem.push_back(selectedItems()[j]);

            if (!findItem.empty())
            {
                for (auto &j : findItem)
                    j->setSelected(1);

                findItem.clear();
            }
        }
    }

    else
    {
        string toFind;

        if      (this == myWin.objectLay) toFind = myWin.selB->name->val_s;
        else if (this == myWin.typeLay) toFind = myWin.selB->texSel.type;
        else if (this == myWin.mapLay) toFind = myWin.selB->texSel.map.name;
        else if (this == myWin.layerLay) toFind = myWin.selB->texSel.layer.name;

        for (int i = 0; i < findItems(QString::fromStdString(toFind), Qt::MatchExactly).size(); ++i)
            findItem.push_back(findItems(QString::fromStdString(toFind), Qt::MatchExactly)[i]);

        if (!findItem.empty())
        {
            for (auto &i : findItem)
                i->setSelected(1);

            findItem.clear();
        }
    }
}

void LayerSel::tableWidgetItem_add()
{
    auto *itemLabel = new QListWidgetItem(QString::fromStdString(attrName));
    prepItems(itemLabel, "label");
}

void LayerSel::prepItems(QListWidgetItem *item, string type)
{
    if (type == "label")
    {
        item->setFont(labelF);
        item->setTextAlignment(Qt::AlignCenter| Qt::AlignVCenter);
        item->setFlags(item->flags()^Qt::ItemIsEditable);

        if (this == myWin.typeLay)
        {
            //make adjustable in prefs
            item->setSizeHint(QSize(item->sizeHint().width(), 45));
        }

        item->setData(32, QString::fromStdString(attrType));
        item->setData(33, QString::fromStdString(attrName));

        if (this == myWin.layerLay)
        {
//            insertItem(useThisRow, item);
            insertItem(attrIdx, item);
        }

        else
            insertItem(useThisRow, item);
    }
}
