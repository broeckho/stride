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
 * Implementation file for DeliveryMan.
 */

#include "DeliveryMan.h"

#include <chrono>
#include <thread>

using namespace std;

void DeliveryMan::DeliverPackage(unsigned int dist)
{
        // Waste some clock cycles here.
        using namespace std::chrono_literals;
        const auto d1 = 10ms;
        this_thread::sleep_for(dist * d1);
}

void DeliveryMan::Prepare(unsigned int number)
{
        // Waste some clock cycles here.
        using namespace std::chrono_literals;
        const auto d1 = 10ms;
        this_thread::sleep_for(number * d1);
}

void DeliveryMan::Sleep(unsigned int dur)
{
        using namespace std::chrono_literals;
        const auto d1 = 10ms;
        this_thread::sleep_for(dur * d1);
}
