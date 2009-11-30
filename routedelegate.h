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

#ifndef ROUTEDELEGATE_H
#define ROUTEDELEGATE_H
#include <QItemDelegate>

class RouteDelegate : public QItemDelegate
{
   public:
   RouteDelegate();
   ~RouteDelegate();

   enum SpecificRoles {
       TextRole =  Qt::UserRole + 1, // User text
       FrameRole = Qt::UserRole + 2, // Custom SVG frame
   };

   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const;

   protected:
   void paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const;
   void drawBackground(QPainter * p, const QStyleOptionViewItem& option, const QModelIndex& index) const;

   private:
   class Private;
   Private *d;
};

#endif
