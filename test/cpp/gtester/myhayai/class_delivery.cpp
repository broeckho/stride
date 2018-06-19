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

#include "class_delivery.h"

#include "DeliveryMan.h"
#include "myhayai/Benchmark.h"
#include "myhayai/Test.h"

#include <memory>
#include <stdexcept>

using namespace std;
using namespace myhayai;

namespace Tests {

void class_delivery()
{
        //---------------------------------------------------------------------------------------------
        // The factory produces a test with all three stages (setup, body, teardown).
        //---------------------------------------------------------------------------------------------
        class Param1TestFactory
        {
        public:
                Param1TestFactory(unsigned int distance, unsigned int duration, unsigned int speed)
                    : m_distance(distance), m_duration(duration), m_speed(speed)
                {
                }

                myhayai::Test operator()()
                {
                        auto f        = make_shared<DeliveryMan>();
                        auto body     = [f, this]() { f->DoThis(m_duration, m_distance); };
                        auto setup    = [f, this]() { f->Prepare(m_speed); };
                        auto teardown = [f]() { f->TearDown(); };
                        return Test(body, setup, teardown);
                }

        private:
                unsigned int m_distance;
                unsigned int m_duration;
                unsigned int m_speed;
        };

        Benchmark b("Delivery", "Param2 - distance=6, duration=5, speed=2", 5, Param1TestFactory(6, 5, 2));
        if (!b) {
                throw runtime_error("Param1TestFactory test not successfully registered.");
        }
        //---------------------------------------------------------------------------------------------
        // Simuilar to Param1TestFactory but now using binders.
        //---------------------------------------------------------------------------------------------
        class Param2TestFactory
        {
        public:
                Param2TestFactory(unsigned int distance, unsigned int duration, unsigned int speed)
                    : m_distance(distance), m_duration(duration), m_speed(speed)
                {
                }

                myhayai::Test operator()()
                {
                        auto f        = make_shared<DeliveryMan>();
                        auto body     = bind(&DeliveryMan::DoThis, f, m_duration, m_distance);
                        auto setup    = bind(&DeliveryMan::Prepare, f, m_speed);
                        auto teardown = bind(&DeliveryMan::TearDown, f);
                        return Test(body, setup, teardown);
                }

        private:
                unsigned int m_distance;
                unsigned int m_duration;
                unsigned int m_speed;
        };

        // Directly registering the test, and not bothering to verify wheteher it's successful.
        BenchmarkRunner::RegisterTest("Delivery", "Param3 - distance=2, duration=2, speed=2", 3,
                                      Param2TestFactory(2, 2, 2));
}

} // namespace Tests
