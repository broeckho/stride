#pragma once
/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */
/**
 * @file
 * Header Param3TestFactory.
 */

#include "myhayai/include/myhayai/Test.hpp"

class Param3TestFactory
{
public:
        Param3TestFactory(unsigned int distance, unsigned int duration, unsigned int speed)
            : m_distance(distance), m_duration(duration), m_speed(speed)
        {
        }

        myhayai::Test operator()();

private:
        unsigned int m_distance;
        unsigned int m_duration;
        unsigned int m_speed;
};
