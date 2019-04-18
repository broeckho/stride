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
 * Header file for Benchmarker.
 */

#pragma once

#include "InfoFactory.h"
#include "Payload.h"
#include "TestDescriptors.h"
#include "TestFactory.h"
#include "TestResult.h"
#include "util/Subject.h"

#include <string>
#include <vector>

namespace myhayai {

/**
 * BenchmarkRunner registers all tests (hence the singleton construction) and runs
 * the tests specified by their canonical names (<test_group_name>.<test_name>).
 */
class BenchmarkRunner : public stride::util::Subject<event::Payload>
{
public:
        /// Simple singleton: no copy construction allowed.
        BenchmarkRunner(const BenchmarkRunner&) = delete;

        /// Simple singleton: no copy assignment allowed.
        BenchmarkRunner& operator=(const BenchmarkRunner&) = delete;

        /// Get the singleton instance of Benchmarker.
        /// @returns a reference to the singleton instance.
        static BenchmarkRunner& Instance();

public:
        /// Get the tests to be executed.
        const TestDescriptors& GetTestDescriptors() const;

        /// Register a test with the benchmarker instance. The combination of
        /// "<groupName>.<testName>" is the canonical name and must be unique.
        /// @param groupName      Name of the group.
        /// @param testName       Name of the test.
        /// @param runs           Number of runs for the test.
        /// @param testFactory    Test factory implementation for the test.
        /// @param infoFactory     Generates ptree with info on test
        /// @param disableTest   Disable the test (won't run evn if included in filter.
        /// @returns true in case of successful registration.
        bool static RegisterTest(const std::string& groupName, const std::string& testName, std::size_t runs,
                                 TestFactory testFactory, InfoFactory infoFactory = InfoFactory(),
                                 bool disableTest = false);

        /// Run tests specified by their canonical names.
        void RunTests(const std::vector<std::string>& canonicalNames);

private:
        /// Private constructor.
        BenchmarkRunner() : m_test_descriptors() {}

        /// Private destructor.
        ~BenchmarkRunner() final = default;

private:
        TestDescriptors m_test_descriptors; ///< Descriptors for registered tests.
};

} // namespace myhayai
