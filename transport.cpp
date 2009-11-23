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
#include <QMap>
#include "transport.h"
#include "service.h"
#include "ui_config.h"

// State tracking
enum State {
   Idle        = 0x00, // Idle
   Searching   = 0x01, // Searching
};

struct Transport::Private
{
   // Options
   QString home;
   Service service;
   QMap<int,QString> serviceMap;

   // Transports
   QHttp http;
   int connId;

   // Widgets
   Plasma::LineEdit* searchLine;
   Plasma::PushButton* searchButton;
   QGraphicsLinearLayout* resultLayout;
   Ui::config configUi;
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

   // Connect search result
   connect(&d->http, SIGNAL(requestFinished(int,bool)), this, SLOT(searchResult(int,bool)));

   // Load config
   loadConfig();
}

void Transport::search(const QString &destination, const QDateTime &dt)
{
}

void Transport::searchResult(int id, bool error)
{
}

void Transport::createConfigurationInterface(KConfigDialog *parent)
{
   // Create configuration
   QWidget* configWidget = new QWidget(parent);
   d->configUi.setupUi(configWidget);
   d->configUi.home->setText(d->home);
   
   // Fill services
   Service serv;
   d->serviceMap.clear();
   QStringList services = KGlobal::dirs()->findAllResources( "data", "plasma_engine_transport/services/*.xml" );
   foreach(const QString& service, services) {
      if(serv.load(service)) {
         d->configUi.service->addItem(serv.name());
         d->serviceMap.insert(d->configUi.service->count() - 1, service);
         if(serv.name() == d->service.name())
            d->configUi.service->setCurrentIndex(d->configUi.service->count() - 1);
      }
   }

   // Create page
   connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
   parent->setButtons(KDialog::Ok | KDialog::Cancel);
   parent->addPage(configWidget, i18n("Configuration"), icon());
}

void Transport::configAccepted()
{
   // Save config
   KConfigGroup configGroup = config();
   configGroup.writeEntry("home", d->configUi.home->text());
   configGroup.writeEntry("service", d->serviceMap[d->configUi.service->currentIndex()]);
   configGroup.sync();
   d->serviceMap.clear();

   // Reload config
   loadConfig();
}

void Transport::loadConfig()
{
   // Load config
   KConfigGroup configGroup = config();
   d->home = configGroup.readEntry("home");
   if(d->service.load(configGroup.readEntry("service")))
      d->service.parse();

   qDebug() << "Config: from " << d->home << " (" << d->service.name() << ")";
}

#include "transport.moc"
