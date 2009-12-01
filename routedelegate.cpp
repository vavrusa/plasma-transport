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

      // Resize frame
      d->svg->resizeFrame(option.rect.size());

      // Check column count
      if(index.model()->columnCount() > 1) {
         d->svg->setEnabledBorders(Plasma::FrameSvg::TopBorder | Plasma::FrameSvg::BottomBorder);
         if(index.column() == 0)
            d->svg->setEnabledBorders(d->svg->enabledBorders() | Plasma::FrameSvg::LeftBorder);
         if(index.column() == index.model()->columnCount() - 1)
            d->svg->setEnabledBorders(d->svg->enabledBorders() | Plasma::FrameSvg::RightBorder);
      }

      // Draw hover background
      if (option.state & (QStyle::State_Selected | QStyle::State_MouseOver)) {

         // Adjust by margin size
         QRectF rect(option.rect);
         rect.adjust(d->svg->marginSize(Plasma::LeftMargin) * 0.5, d->svg->marginSize(Plasma::TopMargin) * 0.5,
                     -d->svg->marginSize(Plasma::RightMargin) * 0.5, -d->svg->marginSize(Plasma::BottomMargin) * 0.5);

         // Fill base layer
         p->setOpacity(0.4);
         p->fillRect(rect, option.palette.highlight());
         p->setOpacity(1.0);
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

   // Efficiency
   if(index.data(EfficiencyRole).isValid()) {
      qreal ratio = index.data(EfficiencyRole).toReal();

      // Update pen color
      QPen pen(p->pen());
      pen.setColor(QColor(128 * ratio, 128 * (1.0 - ratio), 0));
      p->setPen(pen);
   }

   // Draw global text
   if(index.data(Qt::DisplayRole).isValid()) {

      // Count lines
      int margin = option.fontMetrics.averageCharWidth();
      int i = -1, lines = 1;
      QString text(index.data(Qt::DisplayRole).toString());
      while((i = text.indexOf('\n', i + 1)) != -1)
         ++lines;

      // Update contents rect
      QRect contentsRect(option.rect);
      contentsRect.adjust(margin, margin * 0.5, -margin, -margin * 0.5);
      contentsRect.setHeight(contentsRect.height() / lines);

      // Draw lines
      i = -1;
      for(int i = 0; i < lines; ++i) {

         // Update font
         if(index.data(EmphasisRole).isValid()) {
            QFont font(p->font());
            font.setBold(i + 1 == index.data(EmphasisRole).toInt());
            p->setFont(font);
         }

         // Draw line
         int k = text.indexOf('\n');
         p->drawText(contentsRect, alignFlags, text.left(k));
         contentsRect.moveTop(contentsRect.top() + contentsRect.height());
         text.remove(0, k + 1);
      }
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
   size.setHeight( 2.4 * option.fontMetrics.height());
   size.setWidth(option.fontMetrics.averageCharWidth() * 3 + maxw);
   return size;
}

