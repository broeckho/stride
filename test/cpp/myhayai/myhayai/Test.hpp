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
 */
/**
 * @file
 * Header file for Test.
 */

#include "TestResult.hpp"
#include "util/Stopwatch.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace myhayai {

/// Test.
struct Test
{
        explicit Test(std::function<void()> body     = std::function<void()>(),
                      std::function<void()> setup    = std::function<void()>(),
                      std::function<void()> teardown = std::function<void()>())
            : m_body(std::move(body)), m_setup(std::move(setup)), m_teardown(std::move(teardown))
        {
        }

        template <typename T = std::chrono::steady_clock>
        typename T::duration Run()
        {
                if (m_setup) {
                        m_setup();
                }
                const auto start = T::now();
                if (m_body) {
                        m_body();
                }
                const auto stop = T::now();
                if (m_teardown) {
                        m_teardown();
                }
                return stop - start;
        }

        std::function<void()> m_body;
        std::function<void()> m_setup;
        std::function<void()> m_teardown;
};

} // namespace myhayai
