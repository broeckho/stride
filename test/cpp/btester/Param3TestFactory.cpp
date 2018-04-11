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
 * Implementation for Param3TestFactory.
 */

#include "Param3TestFactory.h"
#include "SlowDeliveryMan.h"
#include "myhayai/Benchmark.hpp"

#include <memory>

using namespace std;
using namespace myhayai;

Test Param3TestFactory::operator()()
{
        auto f        = make_shared<SlowDeliveryMan>();
        auto body     = bind(&SlowDeliveryMan::DoThis, f, m_duration, m_distance);
        auto setup    = bind(&SlowDeliveryMan::SetUp, f, m_speed);
        auto teardown = bind(&SlowDeliveryMan::TearDown, f);
        return Test(body, setup, teardown);
}

namespace {
Benchmark b("Delivery", "Param3 - distance=2, duration=2, speed=2", 10, Param3TestFactory(2, 2, 2));
}
