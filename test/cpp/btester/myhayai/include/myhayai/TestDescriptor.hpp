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

#include "InfoFactory.hpp"
#include "TestFactory.hpp"

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
        TestDescriptor(const char* fixtureName, const char* testName, std::size_t numRuns, TestFactory testFactory,
                       InfoFactory infoFactory = InfoFactory(), bool isDisabled = false, bool isInFilter = true)
            : m_fixture_name(fixtureName), m_test_name(testName), m_num_runs(numRuns),
              m_test_factory(std::move(testFactory)), m_info_factory(std::move(infoFactory)),
              m_is_disabled(isDisabled), m_is_in_filter(isInFilter)
        {
        }

        std::string GetCanonicalName() const { return std::string(m_fixture_name).append(".").append(m_test_name); }

        std::string m_fixture_name;   ///< Fixture name.
        std::string m_test_name;      ///< Test name.
        std::size_t m_num_runs;       ///< Number of test runs.
        TestFactory m_test_factory;   ///< Test factory.
        InfoFactory m_info_factory;   ///< Generatesptree with info associated with the test.
        bool        m_is_disabled;    ///< Disabled (or not).
        bool        m_is_in_filter;   ///< Selected by filter (or not).
};

} // namespace myhayai
