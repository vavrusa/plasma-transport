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

#include <QStringList>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QMap>
#include "service.h"

struct Service::Private {
   bool                  isLoaded; // State
   QScriptEngine           engine; // Engine
   QScriptValue      scriptObject; // Scriptable object
};

Service::Service(const QString& fileName)
   : d(new Private)
{
   // Implicit load
   d->isLoaded = false;
   load(fileName);
}

Service::~Service()
{
   delete d;
}

bool Service::isLoaded()
{
   return d->isLoaded;
}

bool Service::load(const QString& fileName)
{
   // Reset loaded state
   d->isLoaded = false;

   // Open file
   if(fileName.isEmpty())
      return false;

   QFile file(fileName);
   if(!file.open(QFile::ReadOnly))
      return false;

   // Load content
   qDebug() << "Script: " << fileName;
   QScriptValue cls = d->engine.evaluate(file.readAll(), fileName);
   d->isLoaded = !d->engine.hasUncaughtException();
   file.close();

   // Debug
   if(d->engine.hasUncaughtException()) {
      qDebug() << "Failed to load, backtrace: ";
      foreach(const QString& line, d->engine.uncaughtExceptionBacktrace())
         qDebug() << "  # " << line;
   }
   else {
      d->scriptObject = d->engine.globalObject().property("Service");
   }

   return d->isLoaded;
}

QString Service::name()
{
   return d->scriptObject.property("name").toString();
}

QUrl Service::url()
{
   return QUrl(d->scriptObject.property("url").toString());
}

QString Service::method()
{
   return d->scriptObject.property("method").toString();
}

QString Service::key(const QString& k)
{
   QScriptValue keyScript = d->scriptObject.property("key");
   return keyScript.call(d->scriptObject, QScriptValueList() << k).toString();
}

QList<Connection> Service::parse(const QString& data)
{
   // Call script
   QScriptValue parseScript = d->scriptObject.property("parse");
   parseScript.call(d->scriptObject, QScriptValueList() << data);

   // Evaluate
   QList<Connection> connList;
   QScriptValue result = d->scriptObject.property("result");
   QScriptValueIterator connIter(result);
   qDebug() << "Result :";
   while(connIter.hasNext()) {
      connIter.next();

      // Next connection
      connList.append(Connection());
      Connection& conn = connList.last();
      QScriptValueIterator iter(connIter.value());
      qDebug() << " + Spojeni";
      while(iter.hasNext()) {
         iter.next();

         // Transit list
         if(iter.name() == "transits") {
            QScriptValueIterator transIter(iter.value());
            Connection::Transit transit;
            while(transIter.hasNext()) {
               transIter.next();

               // Parse array
               QScriptValue arr = transIter.value();
               transit.from = arr.property(0).toString();
               transit.mean = arr.property(1).toString();
               transit.arrival = QTime::fromString(arr.property(2).toString(), "h:mm");
               transit.departure = QTime::fromString(arr.property(3).toString(), "h:mm");

               // Add transit
               qDebug() << "   transit: " << transit.from << " " << transit.mean
                     << transit.arrival.toString("h:mm") << " -> " << transit.departure.toString("h:mm");
               conn.addTransit(transit);;
            }
            continue;
         }

         // String values
         if(iter.value().isString()) {
            conn.setValue(iter.name(), iter.value().toString());
            qDebug() << "   param: " << iter.name()  << " = " << iter.value().toString();
         }
      }
   }

   return connList;
}
