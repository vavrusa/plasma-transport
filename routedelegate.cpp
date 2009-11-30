/***************************************************************************
*   Copyright (C) Marek Vavrusa <marek@vavrusa.com>                       *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License as       *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#include <QDebug>
#include <QPainter>
#include <Plasma/FrameSvg>
#include "routedelegate.h"

class RouteDelegate::Private
{
   public:
   Plasma::FrameSvg* svg;
};

RouteDelegate::RouteDelegate()
   : QItemDelegate(), d(new Private)
{
   d->svg = new Plasma::FrameSvg(this);
   d->svg->setImagePath("widgets/frame");
   d->svg->setElementPrefix("raised");
}

RouteDelegate::~RouteDelegate()
{
   delete d;
}

void RouteDelegate::drawBackground(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   // Text background
   if(index.data(FrameRole).isValid()) {
      d->svg->resizeFrame(option.rect.size());
      d->svg->paintFrame(p, option.rect.topLeft());
   }
}

void RouteDelegate::paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   // Draw background
   drawBackground(p, option, index);

   // Draw text
   if(index.data(TextRole).isValid()) {
      p->drawText(option.rect, Qt::AlignCenter, index.data(FormattedTextRole).toString());
   }
}

QSize RouteDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QItemDelegate::sizeHint(option, index);
    size.setHeight( 3 * option.fontMetrics.height());
    return size;
}

