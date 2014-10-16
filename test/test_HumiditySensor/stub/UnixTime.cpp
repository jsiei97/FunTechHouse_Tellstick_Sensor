/**
 * @file UnixTime.cpp
 * @author Johan Simonsson
 * @brief Unix timestamp wrapper
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

#include <QDateTime>
#include "UnixTime.h"


unsigned int MY_FEJK_TIME = 0;

/**
 * Returns a timestamp with the number of seconds that have passed since 1970-01-01T00:00:00,
 * Coordinated Universal Time (Qt::UTC).
 *
 * @return unixtime timestamp
 */
unsigned int UnixTime::get()
{
    return MY_FEJK_TIME;
}
