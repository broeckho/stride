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
 * Header file for Benchmarker.
 */

#include "ConsoleOutputter.hpp"
#include "InfoFactory.hpp"
#include "TestDescriptors.hpp"
#include "TestFactory.hpp"
#include "TestResult.hpp"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace myhayai {

/// Benchmarking execution controller singleton.
class Benchmarker
{
public:
        /// Simple singleton: no copy construction allowed.
        Benchmarker(const Benchmarker&) = delete;

        /// Simple singleton: no copy assignment allowed.
        Benchmarker& operator=(const Benchmarker&) = delete;

        /// Get the singleton instance of Benchmarker.
        /// @returns a reference to the singleton instance.
        static Benchmarker& Instance();

public:
        /// Add an outputter.
        /// @param outputter Outputter. The caller must ensure that the
        /// outputter remains in existence for the entire benchmark run.
        static void AddOutputter(Outputter& outputter);

        /// Get the tests to be executed.
        const TestDescriptors& GetTestDescriptors() const;

        /// Register a test with the benchmarker instance.
        /// @param fixtureName    Name of the fixture.
        /// @param testName       Name of the test.
        /// @param runs           Number of runs for the test.
        /// @param testFactory    Test factory implementation for the test.
        /// @param infoFactory     Generates ptree with info on test
        /// @param disableTest   Disable the test (won't run evn if included in filter.
        /// @returns true in case of successful registration.
        bool static RegisterTest(const char* fixtureName, const char* testName, std::size_t runs,
                                           TestFactory testFactory, InfoFactory infoFactory = InfoFactory(),
                                           bool disableTest = false);

        /// Run all benchmarking tests.
        static void RunTests(const std::vector<std::string>& names);

private:
        /// Private constructor.
        Benchmarker() = default;

        /// Private destructor.
        ~Benchmarker() = default;

private:
        std::vector<Outputter*> m_outputters;       ///< Registered outputters.
        TestDescriptors         m_test_descriptors; ///< Descriptors for egistered tests.
};

} // namespace myhayai
