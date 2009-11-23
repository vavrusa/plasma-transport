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

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <Plasma/Applet>

class Transport : public Plasma::Applet
{
   Q_OBJECT
   public:
        Transport(QObject *parent, const QVariantList &args);
        ~Transport();

   public slots:
        void init();
        void search(const QString& destination = QString(), const QDateTime& dt = QDateTime::currentDateTime());

     protected slots:
        void loadConfig();
        void configAccepted();
        void searchResult(int id, bool error);

   protected:
        void createConfigurationInterface(KConfigDialog *parent);

   private:
        struct Private;
        Private *d;
};

K_EXPORT_PLASMA_APPLET(transport, Transport)
#endif
