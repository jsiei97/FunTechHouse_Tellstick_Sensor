/**
 * @file main.c
 * @author Johan Simonsson
 * @brief Main file with callback from the tellstick lib.
 */

/*
 * Copyright (C) 2014 Johan Simonsson
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <telldus-core.h>
#include <sys/sysinfo.h>

#include <QString>
#include <QDebug>
#include <QStringList>
#include <QHash>

#include "HumiditySensor.h"
#include "MosqConnect.h"
#include <mosquitto.h>

#define DATA_LENGTH 160
#define MAX_SIZE 100
MosqConnect *mqtt;
QHash<QString, HumiditySensor *> sensorListRH;
char str[MAX_SIZE];

long getUptime()
{
    struct sysinfo s_info;
    int error;
    error = sysinfo(&s_info);
    if(error != 0)
    {
        //What to do?
        printf("code error = %d\n", error);
        return 0;
    }
    return s_info.uptime;
}


//TDRawDeviceEvent
void reactOnRaw(const char *data, int controllerId, int callbackId, void *context)
{
    //printf("%ld %s: data=%s controller=%d callback=%d\n", getUptime(), __func__, data, controllerId, callbackId);

    //This prints things like:
    //The low spec humidity sensor (http://www.clasohlson.com/se/Temperaturgivare-hygrometer/36-1797)
    //360448 reactOnRaw: data=class:sensor;protocol:mandolyn;id:11;model:temperaturehumidity;temp:19.2;humidity:47; controller=-1 callback=1

    //if class:sensor;protocol:mandolyn )
    // model:temperaturehumidity; -> mqtt string syntax
    // temp:19.2;humidity:47;     -> The data :)
    // id:11; -> make id part of the topic since it is uniq


    //The internal test sensor...?
    //360060 reactOnRaw: data=class:sensor;protocol:fineoffset;id:0;model:temperaturehumidity;humidity:0;temp:0.0; controller=-1 callback=1

    // The nexa remote D3 turn off
    //360544 reactOnRaw: data=class:command;protocol:arctech;model:codeswitch;house:D;unit:3;method:turnoff; controller=-1 callback=1
    //360544 reactOnRaw: data=class:command;protocol:sartano;model:codeswitch;code:0011101110;method:turnoff; controller=-1 callback=1

    // The nexa remote D3 turn on
    //360768 reactOnRaw: data=class:command;protocol:arctech;model:codeswitch;house:D;unit:3;method:turnon; controller=-1 callback=1
    //360768 reactOnRaw: data=class:command;protocol:waveman;model:codeswitch;house:D;unit:3;method:turnon; controller=-1 callback=1


    QString inData(data);
    //qDebug() << inData;


    QString id;

    bool isSensor=false;
    bool isTempHum=false;
    bool isMandolyn=false;

    QString temperature;
    QString humidity;

    QStringList list = inData.split(";", QString::SkipEmptyParts);
    for (int i = 0; i < list.size(); ++i)
    {
        QString part = list.at(i).trimmed();
        //qDebug() << part;

        QStringList parts = part.split(":", QString::SkipEmptyParts);
        //qDebug() << part << parts.size();
        if(parts.size() == 2)
        {
            if(parts.at(0).compare("id")==0)
            {
                bool ok;
                if(!((parts.at(1).toInt(&ok, 10) == 0) && ok))
                {
                    //qDebug() << "ok id" << parts.at(1);
                    id=parts.at(1);
                }
            }
            else if((parts.at(0).compare("class")==0) &&
                    (parts.at(1).compare("sensor")==0))
            {
                isSensor=true;
            }
            else if((parts.at(0).compare("protocol")==0) &&
                    (parts.at(1).compare("mandolyn")==0))
            {
                isMandolyn=true;
            }
            else if((parts.at(0).compare("model")==0) &&
                    (parts.at(1).compare("temperaturehumidity")==0))
            {
                isTempHum=true;
            }
            else if(parts.at(0).compare("temp")==0)
            {
                temperature = parts.at(1);
            }
            else if(parts.at(0).compare("humidity")==0)
            {
                humidity = parts.at(1);
            }
        }
    }

    //If this was a temperature humidity sensor then we send it to the server.
    if( (!id.isEmpty()) && isSensor && isTempHum && isMandolyn)
    {
        //qDebug() << "good data";
        if( (!temperature.isEmpty()) && (!humidity.isEmpty()) )
        {
            //Tmp debug string...
            qDebug() << getUptime() << "Data to server: Temperature:" << temperature << " Humidity:" << humidity;

            QString topic("/FunTechHouse/tellstick/");
            topic.append(id);

            if(!sensorListRH.contains(id))
            {
                sensorListRH.insert(id, new HumiditySensor());
            }

            /*
               QString subject("temperature=");
               subject.append(temperature);
               subject.append(" ; rh=");
               subject.append(humidity);
               subject.append("\%");
               qDebug() << topic << " - " << subject;
           */

            HumiditySensor *rh = sensorListRH.value(id);
            if(rh->timeToSend(temperature, humidity, str, MAX_SIZE))
            {
                qDebug() << topic << " - " << str;
                mqtt->pub(topic, str, strlen(str));
            }
        }
    }
}


int main()
{
    printf("FunTechHouse_Tellstick_Sensor\n");
    tdInit();

    int rc;

    mosqpp::lib_init();

    mqtt = new MosqConnect(
            "FunTechHouse_Tellstick_Sensor",
            "mosqhub",
            1883
            );

    tdRegisterRawDeviceEvent( &reactOnRaw, NULL );

    while(1)
    {
		rc = mqtt->loop();
		if(rc){
			mqtt->reconnect();
		}
        printf(".");
        sleep(10);
    }

    mosqpp::lib_cleanup();

    printf("FunTechHouse_Tellstick_Sensor: Done\n");
    return 0;
}
