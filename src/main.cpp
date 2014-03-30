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

#define DATA_LENGTH 160

//TDRawDeviceEvent
void reactOnRaw(const char *data, int controllerId, int callbackId, void *context)
{
    printf("%s: data=%s controller=%d callback=%d\n", __func__, data, controllerId, callbackId);
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
