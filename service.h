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

#ifndef SERVICE_H
#define SERVICE_H

#include <QString>
#include <QTime>
#include <QUrl>
#include <QMap>

class Connection;
class Service
{
   public:
   Service(const QString& fileName = QString());
   ~Service();

   QString name();
   QUrl url();
   QString method();
   QString key(const QString& key);

   bool isLoaded();
   bool load(const QString& fileName);
   QList<Connection> parse(const QString& data);

   private:
   struct Private;
   Private *d;
};

class Connection {

   public:

   // Transit structure
   struct Transit {
      QString    from;
      QString    mean;
      QTime   arrival;
      QTime departure;
   };

   // Transits
   const QList<Transit>& transits() {
      return mTransits;
   }

   void setTransits(QList<Transit>& transits) {
      mTransits = transits;
   }

   void addTransit(Transit trans) {
      mTransits.append(trans);
   }

   // Parameters
   const QString& value(const QString& key) {
      return mParams[key];
   }

   void setValue(const QString& key, const QString& value) {
      mParams[key] = value;
   }

   private:
   QMap<QString,QString> mParams;
   QList<Transit> mTransits;
};

#endif // SERVICE_H
