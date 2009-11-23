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

#include <Plasma/IconWidget>
#include <Plasma/ScrollWidget>
#include <Plasma/BusyWidget>
#include <Plasma/PushButton>
#include <Plasma/LineEdit>
#include <Plasma/Label>
#include <QGraphicsLinearLayout>
#include <QPainter>
#include <KConfigDialog>
#include <KStandardDirs>
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
   // Options
   QString home;

   // Widgets
   Plasma::LineEdit* searchLine;
   Plasma::PushButton* searchButton;
   QGraphicsLinearLayout* resultLayout;
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
   // Create main layout
   QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);

   // Create search layout
   QGraphicsLinearLayout* searchLayout = new QGraphicsLinearLayout(Qt::Horizontal, layout);

   // Search direction line
   d->searchLine = new Plasma::LineEdit(this);
   searchLayout->addItem(d->searchLine);

   // Search submit button
   d->searchButton = new Plasma::PushButton(this);
   d->searchButton->setText(tr("Search"));
   connect(d->searchButton, SIGNAL(clicked()), this, SLOT(search()));
   searchLayout->addItem(d->searchButton);
   layout->addItem(searchLayout);

   // Create results layout
   Plasma::ScrollWidget* scrollWidget = new Plasma::ScrollWidget(this);
   QGraphicsWidget* dataWidget = new QGraphicsWidget(scrollWidget);
   scrollWidget->setWidget(dataWidget);
   d->resultLayout = new QGraphicsLinearLayout(Qt::Vertical, dataWidget);
   layout->addItem(scrollWidget);
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

void Transport::loadConfig()
{
}

void Transport::loadService(const QString &service)
{
}

#include "transport.moc"
