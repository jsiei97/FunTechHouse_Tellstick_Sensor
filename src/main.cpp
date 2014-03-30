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

#define DATA_LENGTH 160

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
    printf("%ld %s: data=%s controller=%d callback=%d\n", getUptime(), __func__, data, controllerId, callbackId);

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
}


int main(int argc, char *argv[])
{
    printf("FunTechHouse_Tellstick_Sensor\n");
    tdInit();

    tdRegisterRawDeviceEvent( &reactOnRaw, NULL );

    while(1)
    {
        printf(".");
        sleep(10);
    }

    printf("FunTechHouse_Tellstick_Sensor: Done\n");
    return 0;
}
