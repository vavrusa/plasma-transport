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
#include "service.h"

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
   if(!index.data(NoFrameRole).isValid()) {
      d->svg->resizeFrame(option.rect.size());

      // Check column count
      if(index.model()->columnCount() > 1) {
         d->svg->setEnabledBorders(Plasma::FrameSvg::TopBorder | Plasma::FrameSvg::BottomBorder);
         if(index.column() == 0)
            d->svg->setEnabledBorders(d->svg->enabledBorders() | Plasma::FrameSvg::LeftBorder);
         if(index.column() == index.model()->columnCount() - 1)
            d->svg->setEnabledBorders(d->svg->enabledBorders() | Plasma::FrameSvg::RightBorder);
         }

      d->svg->paintFrame(p, option.rect.topLeft());
   }
}

void RouteDelegate::paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   // Draw background
   drawBackground(p, option, index);
   p->save();

   // Alignment
   int alignFlags = Qt::AlignVCenter;
   if(index.data(Qt::TextAlignmentRole).isValid())
      alignFlags |= index.data(Qt::TextAlignmentRole).toInt();

   // Draw global text
   int margin = option.fontMetrics.averageCharWidth();
   QRect contentsRect(option.rect);
   contentsRect.adjust(margin,0,-margin,0);
   if(index.data(Qt::DisplayRole).isValid()) {
      p->drawText(contentsRect, alignFlags, index.data(Qt::DisplayRole).toString());
   }

   // Restore painter
   p->restore();
}

QSize RouteDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QString text(index.data(Qt::DisplayRole).toString());

   // Get longest line
   int i = -1, maxw = 0;
   while((i = text.indexOf('\n', i + 1)) != -1) {
      int w = option.fontMetrics.width(text.left(i));
      text.remove(0, i);
      if(w > maxw)
         maxw = w;
   }

   // Last attempt to measure line
   if(maxw == 0) {
      maxw = option.fontMetrics.width(text);
   }

   QSize size = QItemDelegate::sizeHint(option, index);
   size.setHeight( 2 * option.fontMetrics.height() + 4);
   size.setWidth(option.fontMetrics.averageCharWidth() * 2 + maxw);
   return size;
}

