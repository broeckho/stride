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
 * Header file for ParamTestFactory.
 */

#include "DeliveryMan.h"
#include "myhayai/Benchmark.hpp"
#include "myhayai/Test.hpp"

#include <memory>

using namespace std;
using namespace myhayai;

/**
 * Simple Test factory for a test with only a body.
 * @return Test functor.
 */
class Param1TestFactory
{
public:
        ///
        explicit Param1TestFactory(unsigned int distance) : m_distance(distance) {}

        ///
        myhayai::Test operator()()
        {
                auto p    = make_shared<DeliveryMan>(1);
                auto body = [p, this]() { p->DeliverPackage(m_distance); };
                return Test(body);
        }

private:
        unsigned int m_distance;
};

// Test registration (groupname, testname, number of runs, test factory.
// The names of the benchmark variables are of no consequence. They are only
// created to trigger a call to BenchmarkRunner::RegisterTest.
//
// The test is (statically) parameterized through the factory constructor and
// the capture of the this pointer.
// The last call parameter (true) diables the test i.e. it is not run in the
// benchmarks.
namespace {
Benchmark b1("Delivery", "Param1 - distance=4", 10, Param1TestFactory(4));
Benchmark b2("Delivery", "Param1 - distance=8", 5, Param1TestFactory(8), true);
Benchmark b3("Delivery", "Param1 - distance=10", 10, Param1TestFactory(10));
} // namespace
