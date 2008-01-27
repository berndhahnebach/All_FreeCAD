/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#include <Gui/Qt4All.h>

#include "Flag.h"

using namespace Gui;

Flag::Flag(QWidget* parent)
  : QLabel(parent)
{
    // use white background by default
    setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window,Qt::white);
    this->setPalette(pal);
    this->setIndent(10);
    this->setFixedHeight(20);
}

Flag::~Flag()
{
}

void Flag::paintEvent(QPaintEvent* e)
{
    QLabel::paintEvent(e);
}

void Flag::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        move(e->globalPos() - dragPosition);
        e->accept();
    }
}

void Flag::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        dragPosition = e->globalPos() - frameGeometry().topLeft();
        e->accept();
    }
}

QSize Flag::sizeHint() const
{
    return QSize(100, 20);
}

// ------------------------------------------------------------------------

FlagLayout::FlagLayout(QWidget *parent, int margin, int spacing)
    : QLayout(parent)
{
    setMargin(margin);
    setSpacing(spacing);
}

FlagLayout::FlagLayout(int spacing)
{
    setSpacing(spacing);
}

FlagLayout::~FlagLayout()
{
    QLayoutItem *l;
    while ((l = takeAt(0)))
        delete l;
}

void FlagLayout::addItem(QLayoutItem *item)
{
    add(item, TopLeft);
}

void FlagLayout::addWidget(QWidget *widget, Position position)
{
    add(new QWidgetItem(widget), position);
}

Qt::Orientations FlagLayout::expandingDirections() const
{
    return Qt::Horizontal | Qt::Vertical;
}

bool FlagLayout::hasHeightForWidth() const
{
    return false;
}

int FlagLayout::count() const
{
    return list.size();
}

QLayoutItem *FlagLayout::itemAt(int index) const
{
    ItemWrapper *wrapper = list.value(index);
    if (wrapper)
        return wrapper->item;
    else
        return 0;
}

QSize FlagLayout::minimumSize() const
{
    return calculateSize(MinimumSize);
}

void FlagLayout::setGeometry(const QRect &rect)
{
    int topHeight = 0;
    int bottomHeight = 0;

    QLayout::setGeometry(rect);

    // left side
    for (int i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        Position position = wrapper->position;

        if (position == TopLeft) {
            topHeight += spacing();
            item->setGeometry(QRect(rect.x() + spacing(), topHeight, 
                                    item->sizeHint().width(), item->sizeHint().height()));

            topHeight += item->geometry().height();
        } else if (position == BottomLeft) {
            bottomHeight += item->geometry().height() + spacing();
            item->setGeometry(QRect(rect.x() + spacing(), rect.height() - bottomHeight, 
                                    item->sizeHint().width(), item->sizeHint().height()));
        }
    }

    // right side
    topHeight = 0;
    bottomHeight = 0;
    for (int i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        Position position = wrapper->position;

        int rightpos = item->sizeHint().width() + spacing();
        if (position == TopRight) {
            topHeight += spacing();
            item->setGeometry(QRect(rect.x() + rect.width() - rightpos, topHeight, 
                                    item->sizeHint().width(), item->sizeHint().height()));

            topHeight += item->geometry().height();
        } else if (position == BottomRight) {
            bottomHeight += item->geometry().height() + spacing();
            item->setGeometry(QRect(rect.x() + rect.width() - rightpos, rect.height() - bottomHeight, 
                                    item->sizeHint().width(), item->sizeHint().height()));
        }
    }
}

QSize FlagLayout::sizeHint() const
{
    return calculateSize(SizeHint);
}

QLayoutItem *FlagLayout::takeAt(int index)
{
    if (index >= 0 && index < list.size()) {
        ItemWrapper *layoutStruct = list.takeAt(index);
        return layoutStruct->item;
    }
    return 0;
}

void FlagLayout::add(QLayoutItem *item, Position position)
{
    list.append(new ItemWrapper(item, position));
}

QSize FlagLayout::calculateSize(SizeType sizeType) const
{
    QSize totalSize;

    for (int i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        Position position = wrapper->position;
        QSize itemSize;

        if (sizeType == MinimumSize)
            itemSize = wrapper->item->minimumSize();
        else // (sizeType == SizeHint)
            itemSize = wrapper->item->sizeHint();

        totalSize.rheight() += itemSize.height();
        totalSize.rwidth() = qMax<int>(totalSize.width(),itemSize.width());
    }
    return totalSize;
}

#include "moc_Flag.cpp"
