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
 * Implementation file for SlowDeliveryMan.
 */

#include <chrono>
#include <thread>

#include "SlowDeliveryMan.h"

using namespace std;

void SlowDeliveryMan::SetUp(unsigned int speed) { this->m_man = new DeliveryMan(speed); }

void SlowDeliveryMan::TearDown() { delete this->m_man; }

void SlowDeliveryMan::DoThis(unsigned int dist, unsigned int dur)
{
        Sleep(dur);
        m_man->DeliverPackage(dist);
}

void SlowDeliveryMan::DoThat(unsigned int dur)
{
        Sleep(dur);
        m_man->DeliverPackage(2);
}

void SlowDeliveryMan::Sleep(unsigned int dur)
{
        using namespace std::chrono_literals;
        const auto d1 = 10ms;
        this_thread::sleep_for(dur * d1);
}
