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

#include <Plasma/ScrollWidget>
#include <Plasma/BusyWidget>
#include <QGraphicsLinearLayout>
#include <QPainter>
#include <KConfigDialog>
#include <QHttp>
#include <QUrl>
#include "transport.h"

// State tracking
enum State {
   Idle        = 0x00, // Idle
   Searching   = 0x01, // Searching
};

struct Transport::Private
{
   // Config
   QString home;
};

Transport::Transport(QObject *parent, const QVariantList &args)
   : Plasma::Applet(parent, args), d(new Private)
{
   // Plasmoid defaults
   setBackgroundHints(DefaultBackground);
   resize(300, 250);
}


Transport::~Transport()
{
   delete d;
}

void Transport::init()
{
}

void Transport::search(const QString &destination, const QDateTime &dt)
{

}

void Transport::createConfigurationInterface(KConfigDialog *parent)
{
}

void Transport::configAccepted()
{
}

#include "transport.moc"
