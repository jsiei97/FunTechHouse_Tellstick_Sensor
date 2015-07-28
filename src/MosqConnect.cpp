/**
 * @file MosqConnect.cpp
 * @author Johan Simonsson
 * @brief Mosquitto interface
 */

/*
 * Copyright (C) 2013 Johan Simonsson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <cstring>

#include <QString>
#include <QDebug>
//#include <QRegExp>
//#include <QDateTime>
//#include <QUrl>

#include <mosquittopp.h>
#include <mosquitto.h>

#include "MosqConnect.h"


MosqConnect::MosqConnect(const char *id, const char *host, int port) : mosquittopp(id)
{
    int keepalive = 60;

    // Connect immediately.
    connect(host, port, keepalive);
};

MosqConnect::~MosqConnect(){
}

void MosqConnect::on_connect(int rc)
{
    printf("Connected with code %d.\n", rc);
    if(rc == 0)
    {
        // Only attempt to subscribe on a successful connect.
    }
}

void MosqConnect::on_message(const struct mosquitto_message *message)
{
    //struct mosquitto_message{ int mid; char *topic; void *payload; int payloadlen; int qos; bool retain; };
}

void MosqConnect::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    printf("Subscription succeeded. mid=%d qos=%d granter_qos=%d\n", mid, qos_count, *granted_qos);
}

void MosqConnect::pub(QString topic, char* subject, int sSize)
{
    publish(NULL, topic.toAscii(), sSize, subject);
}
void MosqConnect::pub(QString topic, QString subject )
{
//    qDebug()<<"publish"<<topic<<subject;
    publish(NULL, topic.toAscii(), subject.length(), subject.toAscii());
}
