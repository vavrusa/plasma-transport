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

#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <QTreeView>
#include <QPainter>
#include <KConfigDialog>
#include <KStandardDirs>
#include <KLineEdit>
#include <Plasma/IconWidget>
#include <Plasma/TreeView>
#include <Plasma/BusyWidget>
#include <Plasma/LineEdit>
#include <QHttp>
#include <QMap>
#include "transport.h"
#include "service.h"
#include "routedelegate.h"
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
   Plasma::IconWidget* searchButton;
   Plasma::TreeView* dataView;
   QStandardItemModel* dataModel;
   Ui::config configUi;
};

Transport::Transport(QObject *parent, const QVariantList &args)
   : Plasma::Applet(parent, args), d(new Private)
{
   // Plasmoid defaults
   setAspectRatioMode(Plasma::IgnoreAspectRatio);
   setBackgroundHints(DefaultBackground);
   resize(320, 250);
}


Transport::~Transport()
{
   delete d;
}

void Transport::init()
{
   // Create main layout
   QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
   layout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   // Create search layout
   QGraphicsLinearLayout* searchLayout = new QGraphicsLinearLayout(Qt::Horizontal, layout);

   // Search direction line
   d->searchLine = new Plasma::LineEdit(this);
   connect(d->searchLine, SIGNAL(returnPressed()), this, SLOT(search()));
   searchLayout->addItem(d->searchLine);

   // Search submit button
   d->searchButton = new Plasma::IconWidget(this);
   d->searchButton->setIcon("page-zoom");
   connect(d->searchButton, SIGNAL(clicked()), this, SLOT(search()));
   searchLayout->addItem(d->searchButton);
   layout->addItem(searchLayout);

   // Create results model and view
   d->dataModel = new QStandardItemModel(0, 1, this);
   d->dataView = new Plasma::TreeView(this);
   d->dataView->setModel(d->dataModel);
   d->dataView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   // Set QTreeView properties
   QTreeView* view = d->dataView->nativeWidget();
   view->setItemDelegate(new RouteDelegate());
   view->setRootIsDecorated(false); // No indentation line
   view->setHeaderHidden(true);
   view->setWordWrap(true);
   view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   // Set transparent background
   QPalette pal = palette();
   pal.setColor( QPalette::Background, Qt::transparent);
   pal.setColor( QPalette::Base, Qt::transparent);
   view->setPalette(pal);

   // Add view to layout
   layout->addItem(d->dataView);

   // Connect search result
   connect(&d->http, SIGNAL(requestFinished(int,bool)), this, SLOT(searchResult(int,bool)));

   // Load config
   loadConfig();
}

void Transport::search(const QString &destination, const QDateTime &dt)
{
   // Use default destination
   QString to(destination);
   if(to.isEmpty())
      to = d->searchLine->text();

   // If search line is empty, focus
   if(to.isEmpty()) {
      d->searchLine->setFocus();
      return;
   }

   // Create Http request
   QUrl url = d->service.url();
   url.addQueryItem(d->service.key("date"), dt.date().toString("d.M.yyyy"));
   url.addQueryItem(d->service.key("from"), d->home);
   url.addQueryItem(d->service.key("to"), to);
   url.addQueryItem(d->service.key("time"), dt.time().toString("h:mm"));

   QHttpRequestHeader header(d->service.method(), url.path());
   header.setValue("Host", url.host());
   header.setValue("User-Agent", "Mozilla/5.0"); // We're not IE
   header.setValue("Referer", url.host() + url.path());
   header.setContentType("application/x-www-form-urlencoded");

   // Query service
   d->http.setHost(url.host());
   d->connId = d->http.request(header, url.encodedQuery());
   qDebug() << "Query: " << url.toString();
}

void Transport::searchResult(int id, bool error)
{
   // Check current search only
   if(d->connId != id)
      return;

   // Check error
   if(error) {
      qWarning("Http: error in response \'%d\'", id);
      return;
   }

   // Check for redirection
   QHttpResponseHeader response = d->http.lastResponse();
   qDebug() << "Received: code " << response.statusCode() << " (" << d->http.bytesAvailable() << " bytes)";
   if(response.statusCode() > 299 && response.statusCode() < 400) {
      qDebug() << "Redirect to: " << response.value("Location");
      d->http.setHost(d->service.url().host());
      d->connId = d->http.get(QString("/") + response.value("Location"));
      return;
   }

   // Parse result
   qDebug() << "Received: " << d->http.bytesAvailable() << " bytes";
   QString data(d->http.readAll());
   QList<Route> list = d->service.parse(data);

   // Get results
   d->dataModel->clear();
   QList<Route>::iterator it;
   for(it = list.begin(); it != list.end(); ++it) {
       QStandardItem* item = new QStandardItem();
       const Transit& start = it->transits().front();
       const Transit& end = it->transits().back();
       int duration = start.departs().secsTo(end.arrives());
       item->setData(start.departs().toString("h:mm") + " " +
                     start.from() + " - " +
                     end.from() + " ("  +
                     QTime().addSecs(duration).toString("h'h' m'm'") + ")",
                     RouteDelegate::TextRole);
       item->setData(true, RouteDelegate::FrameRole);

      d->dataModel->appendRow(item);
   }
}

void Transport::createConfigurationInterface(KConfigDialog *parent)
{
   // Create configuration
   QWidget* configWidget = new QWidget(parent);
   d->configUi.setupUi(configWidget);
   d->configUi.home->setText(d->home);

   // Fill services
   d->serviceMap.clear();
   QStringList services = KGlobal::dirs()->findAllResources( "data", "plasma_engine_transport/services/*.js" );
   foreach(const QString& service, services) {
      Service serv;
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
   parent->addPage(configWidget, tr("Configuration"), icon());
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
   d->service.load(configGroup.readEntry("service"));

   // Update text edit
   d->searchLine->nativeWidget()->setClickMessage(tr("Search from ") + d->home);

   // Update focus
   d->searchLine->setFocus();
}

#include "transport.moc"
