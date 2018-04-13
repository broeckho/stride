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
 * Implementation for Param2TestFactory.
 */

#include "SlowDeliveryMan.h"
#include "myhayai/Benchmark.hpp"
#include "myhayai/Test.hpp"

#include <memory>

using namespace std;
using namespace myhayai;
/**
 * The factory produces a test with all three stages (setup, body, teardown).
 */
class Param2TestFactory
{
public:
        Param2TestFactory(unsigned int distance, unsigned int duration, unsigned int speed)
            : m_distance(distance), m_duration(duration), m_speed(speed)
        {
        }

        myhayai::Test operator()()
        {
                auto f        = make_shared<SlowDeliveryMan>();
                auto body     = [f, this]() { f->DoThis(m_duration, m_distance); };
                auto setup    = [f, this]() { f->SetUp(m_speed); };
                auto teardown = [f]() { f->TearDown(); };
                return Test(body, setup, teardown);
        }

private:
        unsigned int m_distance;
        unsigned int m_duration;
        unsigned int m_speed;
};

// Registration of the test.
namespace {
Benchmark b("Delivery", "Param2 - distance=6, duration=7, speed=2", 10, Param2TestFactory(6, 7, 2));
}
