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
 * Header file for Fixture.
 */

#include "TestResult.hpp"
#include "util/Stopwatch.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace myhayai {

/// Base test class.
/// @ref SetUp is invoked before each run, and @ref TearDown is invoked
/// once the run is finished.
/// The default test class does not contain any actual code in the
/// SetUp and TearDown methods, which means that tests can inherit
/// this class directly for non-fixture based benchmarking tests.

struct Test
{
        explicit Test(std::function<void()> body     = std::function<void()>(),
                      std::function<void()> setup    = std::function<void()>(),
                      std::function<void()> teardown = std::function<void()>())
            : m_body(std::move(body)), m_setup(std::move(setup)), m_teardown(std::move(teardown))
        {
        }

        uint64_t Run()
        {
                if (m_setup) {
                        m_setup();
                }
                stride::util::Stopwatch<> clock("bench", true);
                if (m_body) {
                        m_body();
                }
                clock.Stop();
                if (m_teardown) {
                        m_teardown();
                }
                return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(clock.Get()).count());
        }

        std::function<void()> m_body;
        std::function<void()> m_setup;
        std::function<void()> m_teardown;
};

} // namespace myhayai
