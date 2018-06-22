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

#include "InfoFactory.h"
#include "TestFactory.h"

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

namespace myhayai {

/**
 * Benchmark test descriptor.
 *
 * Benchmark tests descriptors are specified by
 * \li the group the belong to in combination with their individual name. This
 *     combination is referred to as the canonical name.
 * \li the number of times the benchmark has to be executed
 * \li the factory that produces test instances through its call operator
 * \li the factory that produces info about the test (intended to be useful for viewers)
 * \li a status variable that lets you (temporarily) disable a tests so it remains
 *     registered but wil not run when the benchmarks execute.
 */

struct TestDescriptor
{
        /// Construct empty test descriptor.
        TestDescriptor()
            : m_group_name(), m_test_name(), m_num_runs(0), m_test_factory(), m_info_factory(), m_is_disabled(true)
        {
        }

        /// Initialize a new test descriptor.
        /// @param groupName       Name of the test group.
        /// @param testName        Name of the test.
        /// @param numRuns         Number of runs for the test.
        /// @param testFactory     Test factory implementation for the test.
        /// @param infoFactory     Provides extra info that can be recorded by viewers
        /// @param isDiabled       Lets you put a test in place but (temporarily) disable it.
        TestDescriptor(std::string groupName, std::string testName, std::size_t numRuns, TestFactory testFactory,
                       InfoFactory infoFactory = InfoFactory(), bool isDisabled = false)
            : m_group_name(std::move(groupName)), m_test_name(std::move(testName)), m_num_runs(numRuns),
              m_test_factory(std::move(testFactory)), m_info_factory(std::move(infoFactory)), m_is_disabled(isDisabled)
        {
        }

        std::string GetCanonicalName() const { return std::string(m_group_name).append(".").append(m_test_name); }

        std::string m_group_name;   ///< Group name.
        std::string m_test_name;    ///< Test name.
        std::size_t m_num_runs;     ///< Number of test runs.
        TestFactory m_test_factory; ///< Test factory.
        InfoFactory m_info_factory; ///< Generates ptree with info associated with the test.
        bool        m_is_disabled;  ///< Disabled (or not).
};

} // namespace myhayai
