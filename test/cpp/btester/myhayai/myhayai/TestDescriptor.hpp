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
        TestDescriptor()
            : m_group_name(), m_test_name(), m_num_runs(0), m_test_factory(TestFactory()),
              m_info_factory(InfoFactory()), m_is_disabled(true), m_is_included(false)
        {
        }

        /// Initialize a new test descriptor.
        /// @param fixture_name    Name of the fixture.
        /// @param test_name       Name of the test.
        /// @param num_runs        Number of runs for the test.
        /// @param iterations      Number of iterations per run.
        /// @param test_factory    Test factory implementation for the test.
        /// @param params_desc     Parametrized test parameters.
        TestDescriptor(std::string groupName, std::string testName, std::size_t numRuns, TestFactory testFactory,
                       InfoFactory infoFactory = InfoFactory(), bool isDisabled = false, bool isInFilter = true)
            : m_group_name(std::move(groupName)), m_test_name(std::move(testName)), m_num_runs(numRuns),
              m_test_factory(std::move(testFactory)), m_info_factory(std::move(infoFactory)), m_is_disabled(isDisabled),
              m_is_included(isInFilter)
        {
        }

        std::string GetCanonicalName() const { return std::string(m_group_name).append(".").append(m_test_name); }

        std::string m_group_name;   ///< Group name.
        std::string m_test_name;    ///< Test name.
        std::size_t m_num_runs;     ///< Number of test runs.
        TestFactory m_test_factory; ///< Test factory.
        InfoFactory m_info_factory; ///< Generates ptree with info associated with the test.
        bool        m_is_disabled;  ///< Disabled (or not).
        bool        m_is_included;  ///< Included by filter (or not).
};

} // namespace myhayai
