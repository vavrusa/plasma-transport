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
#include <QTextCodec>
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
   QScriptValue keyScript = d->scriptObject.property("qmap");
   QScriptValueIterator i(keyScript);
   while(i.hasNext()) {
      i.next();
      if(i.name() == k)
         return i.value().toString();
   }

   return QString();
}

QList<Route> Service::parse(const QString& data)
{
   // Call script
   QScriptValue parseScript = d->scriptObject.property("parse");
   parseScript.call(d->scriptObject, QScriptValueList() << data);

   // Evaluate
   QList<Route> connList;
   QScriptValue result = d->scriptObject.property("result");
   QScriptValueIterator connIter(result);
   qDebug() << "Result :";

   // Parse result map
   while(connIter.hasNext()) {
      connIter.next();

      // Next connection
      connList.append(Route());
      Route& conn = connList.last();
      QScriptValueIterator iter(connIter.value());

      // Prepare Connection parameters
      QTextCodec* codec = QTextCodec::codecForName("UTF-8");
      while(iter.hasNext()) {
         iter.next();

         // Transit list
         if(iter.name() == "transits") {

            // Iterate transits
            QScriptValueIterator transIter(iter.value());
            while(transIter.hasNext()) {

               Transit transit;
               while(transIter.hasNext()) {
                  transIter.next();

                  // Parse array
                  QScriptValue arr = transIter.value();
                  transit.setFrom(codec->toUnicode(arr.property(0).toString().toAscii()));
                  transit.setMean(codec->toUnicode(arr.property(1).toString().toAscii()));
                  transit.setArrives(QTime::fromString(arr.property(2).toString(), "h:mm"));
                  transit.setDeparts(QTime::fromString(arr.property(3).toString(), "h:mm"));

                  // Add transit
                  qDebug() << "   transit: " << transit.from() << " " << transit.mean()
                        << transit.arrives().toString("h:mm") << " -> " << transit.departs().toString("h:mm");
                  conn.addTransit(transit);
               }
            }

            continue;
         }

         // Codec
         if(iter.name() == "codepage")
            codec = QTextCodec::codecForName(iter.value().toString().toLatin1());

         // String values
         if(iter.value().isString()) {
            conn.setValue(iter.name(), iter.value().toString());
            qDebug() << "   param: " << iter.name()  << " = " << iter.value().toString();
         }
      }
   }

   return connList;
}
