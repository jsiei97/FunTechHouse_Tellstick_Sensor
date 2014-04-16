/**
 * @file HumiditySensor.h
 * @author Johan Simonsson
 * @brief A humidity and temperature sensor class with alarm logic
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

#ifndef  __HUMIDITYSENSOR_H
#define  __HUMIDITYSENSOR_H

#include "Sensor.h"

// If value is the "same" for "cnt" questions, then send anyway.
// If sleep is 1s (1000ms) and there is 1 question per rotation
// then we have 600/1s=600s or always send every 10min
// 1200/1s/60s=20min
#define ALWAYS_SEND_CNT 1200

class HumiditySensor : public Sensor
{
    private:
        int    valueSendCnt;///< Always send after "cnt time" even if there is no change

        //Variables for the temperature part
        double temperatureWork;   ///< Active temperature that we work with right now
        double temperatureSent;   ///< Last temperature sent to the server
        double temperatureDiffMax;///< temperature should diff more than this to be sent to the server
        double temperatureOffset; ///< Offset calibration temperature, this will just be added to the messured temperature

        //Variables for the humidity part
        double humidityWork;   ///< Active humidity that we work with right now
        double humiditySent;   ///< Last humidity sent to the server
        double humidityDiffMax;///< humidity should diff more than this to be sent to the server
        double humidityOffset; ///< Offset calibration humidity, this will just be added to the messured humidity

        //Varibles for temperature alarm high
        double alarmHighTemperature;      ///< Values higher than this will trigger an alarm
        bool   alarmHighTemperatureActive;///< Is high alarm activated.
        bool   alarmHighTemperatureSent;  ///< Have we sent this alarm? We only send alarm ones

        //Varibles for humidity alarm high
        double alarmHighHumidity;      ///< Values higher than this will trigger an alarm
        bool   alarmHighHumidityActive;///< Is high alarm activated.
        bool   alarmHighHumiditySent;  ///< Have we sent this alarm? We only send alarm ones

        //Variables for temperature alarm low
        double alarmLowTemperature;      ///< Values lower than this will trigger an alarm
        bool   alarmLowTemperatureActive;///< Is low alarm activated.
        bool   alarmLowTemperatureSent;  ///< Have we sent this alarm? We only send alarm ones

        //Variables for humidity alarm low
        double alarmLowHumidity;      ///< Values lower than this will trigger an alarm
        bool   alarmLowHumidityActive;///< Is low alarm activated.
        bool   alarmLowHumiditySent;  ///< Have we sent this alarm? We only send alarm ones

        double alarmHystTemperature; ///< Temperature alarm level must go back this much to be reseted
        double alarmHystHumidity;    ///< Humidity alarm level must go back this much to be reseted


    public:
        HumiditySensor();

        bool valueTimeToSend(double temperature, double humidity);
        void setDiffToSend(double temperature, double humidity);
        void setValueOffset(double temperature, double humidity);

        void setAlarmLevels(
                double highTemperature, bool highTemperatureActive,
                double lowTemperature,  bool lowTemperatureActive,
                double highHumidity,    bool highHumidityActive,
                double lowHumidity,     bool lowHumidityActive);
        void setAlarmHyst(double hystTemperature, double hystHumidity);

        bool alarmCheck(char* responce, int maxSize);
        void alarmFailed();

};

#endif  // __HUMIDITYSENSOR_H
