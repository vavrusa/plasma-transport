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

#include <QDomDocument>
#include <QDomElement>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QMap>
#include "service.h"

struct Service::Private {
   bool                  isLoaded; // State
   QString                   name; // Service name
   QUrl                       url; // Service URL
   QMap<QString,QString> queryMap; // Query param mapping
   QString                 method; // Query method
   QDomDocument               doc; // DOM document
};

Service::Service(const QString& fileName)
   : d(new Private)
{
   // Implicit load
   d->isLoaded = false;
   if(!fileName.isEmpty())
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
   d->isLoaded = d->doc.setContent(file.readAll());
   file.close();

   // Parse header
   QDomElement elem = d->doc.firstChildElement("service");
   if(elem.isNull())
      return d->isLoaded = false;
   d->name = elem.attribute("name");

   return d->isLoaded;
}

bool Service::parse()
{
   // Load query data
   QDomElement root = d->doc.firstChildElement("service");
   QDomElement head = root.firstChildElement("query");
   QDomElement elem = head;

   // Query method
   if(head.attribute("method", "GET").toUpper() == "GET")
      d->method = "GET";
   else
      d->method = "POST";

   // Url
   d->url.setUrl(head.firstChildElement("url").text());
   elem = head.firstChildElement("data");
   for(elem = elem.firstChildElement(); !elem.isNull(); elem = elem.nextSiblingElement()) {
      d->url.addQueryItem(elem.tagName(), elem.text());
      qDebug() << "Data: " << elem.tagName() << " = " << elem.text();
   }
   qDebug() << "Url: " << d->url.toString();

   // Load parameter mapping
   d->queryMap.clear();
   elem = head.firstChildElement("map");
   for(elem = elem.firstChildElement(); !elem.isNull(); elem = elem.nextSiblingElement()) {
      d->queryMap.insert(elem.tagName(), elem.text());
      qDebug() << "Map: " << elem.tagName() << " -> " << elem.text();
   }

   return true;
}

const QString& Service::name()
{
   return d->name;
}

const QUrl& Service::url()
{
   return d->url;
}

const QString& Service::method()
{
   return d->method;
}

const QString& Service::param(const QString& key)
{
   return d->queryMap[key];
}

