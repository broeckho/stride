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

#include "SlowDeliveryMan.h"
#include "myhayai/Benchmark.hpp"
#include "myhayai/Test.hpp"

#include <memory>
#include <stdexcept>

using namespace std;
using namespace myhayai;

void class_delivery()
{
        //---------------------------------------------------------------------------------------------
        // Simple Test factory for a test with only a body.
        //---------------------------------------------------------------------------------------------
        class Param1TestFactory
        {
        public:
                explicit Param1TestFactory(unsigned int distance) : m_distance(distance) {}

                myhayai::Test operator()()
                {
                        auto p    = make_shared<DeliveryMan>(1);
                        auto body = [p, this]() { p->DeliverPackage(m_distance); };
                        return Test(body);
                }

        private:
                unsigned int m_distance;
        };

        // Instantiation of a Benchmark object triggers registration of the test.
        // You can check status afterwards.
        Benchmark b1("Delivery", "Param1 - distance=4", 10, Param1TestFactory(4));
        Benchmark b2("Delivery", "Param1 - distance=8", 5, Param1TestFactory(8), true);
        Benchmark b3("Delivery", "Param1 - distance=10", 10, Param1TestFactory(10));
        if (!b1 || !b2 || !b3) {
                throw runtime_error("Param1TestFactory test not successfully registered.");
        }

        //---------------------------------------------------------------------------------------------
        // The factory produces a test with all three stages (setup, body, teardown).
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

        // Directly reistering the text, and not bothering to verify wheteher it's successful.
        BenchmarkRunner::RegisterTest("Delivery", "Param2 - distance=6, duration=7, speed=2", 10,
                                      Param2TestFactory(6, 7, 2));

        //---------------------------------------------------------------------------------------------
        // Simuilar to Param2TestFactory but now using binders.
        //---------------------------------------------------------------------------------------------
        class Param3TestFactory
        {
        public:
                Param3TestFactory(unsigned int distance, unsigned int duration, unsigned int speed)
                    : m_distance(distance), m_duration(duration), m_speed(speed)
                {
                }

                myhayai::Test operator()()
                {
                        auto f        = make_shared<SlowDeliveryMan>();
                        auto body     = bind(&SlowDeliveryMan::DoThis, f, m_duration, m_distance);
                        auto setup    = bind(&SlowDeliveryMan::SetUp, f, m_speed);
                        auto teardown = bind(&SlowDeliveryMan::TearDown, f);
                        return Test(body, setup, teardown);
                }

        private:
                unsigned int m_distance;
                unsigned int m_duration;
                unsigned int m_speed;
        };

        BenchmarkRunner::RegisterTest("Delivery", "Param3 - distance=2, duration=2, speed=2", 10,
                                      Param3TestFactory(2, 2, 2));
}
