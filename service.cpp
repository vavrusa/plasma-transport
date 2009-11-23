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
#include "service.h"

struct Service::Private {
   bool    isLoaded; // State
   QString     name; // Service name
   QDomDocument doc; // DOM document
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

const QString& Service::name()
{
   return d->name;
}
