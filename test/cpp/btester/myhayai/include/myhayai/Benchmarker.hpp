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
#include "TestDescriptor.hpp"
#include "TestFactory.hpp"
#include "TestResult.hpp"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <random>
#include <string>
#include <vector>

namespace myhayai {

/// Benchmarking execution controller singleton.
class Benchmarker
{
public:
        /// Add an outputter.
        /// @param outputter Outputter. The caller must ensure that the
        /// outputter remains in existence for the entire benchmark run.
        static void AddOutputter(Outputter& outputter);

        /// Apply a pattern filter to the tests.
        /// --gtest_filter-compatible pattern:
        /// https://code.google.com/p/googletest/wiki/AdvancedGuide
        /// @param pattern Filter pattern compatible with gtest.
        static void ApplyPatternFilter(const char* pattern);

        /// Get the singleton instance of @ref Benchmarker.
        /// @returns a reference to the singleton instance of the
        /// benchmarker execution controller.
        static Benchmarker& Instance();

        /// List tests.
        static std::vector<const TestDescriptor*> ListTests();

        /// Register a test with the benchmarker instance.
        /// @param fixtureName    Name of the fixture.
        /// @param testName       Name of the test.
        /// @param runs           Number of runs for the test.
        /// @param testFactory    Test factory implementation for the test.
        /// @returns a pointer to a @ref TestDescriptor instance
        /// representing the given test.
        static TestDescriptor* RegisterTest(const char* fixtureName, const char* testName, std::size_t runs,
                                            TestFactory* testFactory,
                                            TestParametersDescriptor parameters = TestParametersDescriptor());
        /// Run all benchmarking tests.
        static void RunAllTests();

        /// Randomly shuffles the order of tests.
        static void ShuffleTests();

private:
        /// Private constructor.
        Benchmarker() = default;

        /// Private destructor.
        ~Benchmarker();

        /// Test if a filter matches a string.
        /// Adapted from gtest. All rights reserved by original authors.
        static bool FilterMatchesString(const char* filter, const std::string& str);

        /// Get the tests to be executed.
        std::vector<TestDescriptor*> GetTests() const;

        /// Test if pattern matches a string.
        /// Adapted from gtest. All rights reserved by original authors.
        static bool PatternMatchesString(const char* pattern, const char* str);

private:
        std::vector<std::string>     _include;    ///< Test filters.
        std::vector<Outputter*>      _outputters; ///< Registered outputters.
        std::vector<TestDescriptor*> _tests;      ///< Registered tests.
};

} // namespace myhayai
