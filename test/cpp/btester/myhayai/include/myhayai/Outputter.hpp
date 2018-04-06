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
 * Header file for Outputter.
 */

#include "TestDescriptor.hpp"
#include "TestResult.hpp"

#include <cstddef>
#include <iostream>

namespace myhayai {

/// Outputter. Abstract base class for outputters.
class Outputter
{
public:
        /// Base so needs to be virtual.
        virtual ~Outputter() = default;

        /// Begin benchmarking. The total number of benchmarks registred is the
        /// sum of the two counts passed to the outputter.
        /// @param enabledCount Number of benchmarks to be executed.
        /// @param disabledCount Number of disabled benchmarks to be skipped.
        virtual void Begin(const std::size_t& enabledCount, const std::size_t& disabledCount) = 0;

        /// End benchmarking.
        /// @param executedCount Number of benchmarks that have been executed.
        /// @param disabledCount Number of benchmarks that have been skipped because they are disabled.
        virtual void End(const std::size_t& executedCount, const std::size_t& disabledCount) = 0;

        /// Begin benchmark test run.
        /// @param fixtureName   Fixture name.
        /// @param testName      Test name.
        /// @param parameters    Test parameter description.
        /// @param runsCount Number of runs to be executed.
        virtual void BeginTest(const std::string& fixtureName, const std::string& testName,
                               const TestParametersDescriptor& parameters, const std::size_t& runsCount) = 0;

        /// End benchmark test run.
        /// @param fixtureName  Fixture name.
        /// @param testName     Test name.
        /// @param parameters   Test parameter description.
        /// @param result       Test result.
        virtual void EndTest(const std::string& fixtureName, const std::string& testName,
                             const TestParametersDescriptor& parameters, const TestResult& result) = 0;

        /// Skip disabled benchmark test run.
        /// @param fixtureName  Fixture name.
        /// @param testName     Test name.
        /// @param parameters   Test parameter description.
        /// @param runsCount    Number of runs to be executed.
        /// @param iterationsCount Number of iterations per run.
        virtual void SkipDisabledTest(const std::string& fixtureName, const std::string& testName,
                                      const TestParametersDescriptor& parameters, const std::size_t& runsCount) = 0;

protected:
        /// Write a nicely formatted test name to a stream.
        static void WriteTestNameToStream(std::ostream& stream, const std::string& fixtureName,
                                          const std::string& testName, const TestParametersDescriptor& parameters);
};

} // namespace myhayai
