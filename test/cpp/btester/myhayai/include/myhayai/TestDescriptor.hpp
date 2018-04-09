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
 * Header file for TestDescriptor.
 */

#include "TestFactory.hpp"
#include "TestParametersDescriptor.hpp"

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

namespace myhayai {

/// Test descriptor.
struct TestDescriptor
{
        /// Initialize a new test descriptor.
        /// @param fixture_name    Name of the fixture.
        /// @param test_name       Name of the test.
        /// @param num_runs        Number of runs for the test.
        /// @param iterations      Number of iterations per run.
        /// @param test_factory    Test factory implementation for the test.
        /// @param params_desc     Parametrized test parameters.
        TestDescriptor(const char* fixture_name, const char* test_name, std::size_t num_runs, TestFactory test_factory,
                       TestParametersDescriptor params_desc, bool is_disabled = false, bool is_in_filter = true)
            : m_fixture_name(fixture_name), m_test_name(test_name),
              m_canonical_name(std::string(fixture_name) + "." + test_name), m_num_runs(num_runs),
              m_test_factory(std::move(test_factory)), m_params_desc(params_desc), m_is_disabled(is_disabled),
              m_is_in_filter(is_in_filter)
        {
        }

        std::string              m_fixture_name;   ///< Fixture name.
        std::string              m_test_name;      ///< Test name.
        std::string              m_canonical_name; ///< Canonical name: <FixtureName>.<TestName>.
        std::size_t              m_num_runs;       ///< Number of test runs.
        TestFactory              m_test_factory;   ///< Test factory.
        TestParametersDescriptor m_params_desc;    ///< Parameters associated with the test.
        bool                     m_is_disabled;    ///< Disabled (or not).
        bool                     m_is_in_filter;   ///< Selected by filter (or not).
};

} // namespace myhayai
