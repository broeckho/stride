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
 * Header file for JUnitXmlOutputter.
 */

#include "Outputter.hpp"
#include "TestParametersDescriptor.hpp"

#include <iomanip>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace myhayai {

/// JUnit-compatible XML outputter.
class JUnitXmlOutputter : public Outputter
{
private:
        /// Test case.
        class TestCase
        {
        public:
                TestCase(const std::string& fixtureName, const std::string& testName,
                         const TestParametersDescriptor& parameters, const TestResult* result)
                {
                        // Derive a pretty name.
                        std::stringstream nameStream;
                        WriteTestNameToStream(nameStream, fixtureName, testName, parameters);
                        Name = nameStream.str();

                        // Derive the result.
                        Skipped = !result;

                        if (result) {
                                std::stringstream timeStream;
                                timeStream << std::fixed << std::setprecision(9) << (result->RunTimeAverage() / 1e9);
                                Time = timeStream.str();
                        }
                }

                std::string Name;
                std::string Time;
                bool        Skipped;
        };

        /// Test suite map.
        using TestSuiteMap = std::map<std::string, std::vector<TestCase>>;

public:
        /// Initialize outputter.
        /// @param stream Output stream. Must exist for the entire duration of
        /// the outputter's use.
        explicit JUnitXmlOutputter(std::ostream& stream) : _stream(stream) {}

        void Begin(const std::size_t& enabledCount, const std::size_t& disabledCount) override;

        void End(const std::size_t& executedCount, const std::size_t& disabledCount) override;

        void BeginTest(const std::string& fixtureName, const std::string& testName,
                       const TestParametersDescriptor& parameters, const std::size_t& runsCount) override;

        void SkipDisabledTest(const std::string& fixtureName, const std::string& testName,
                              const TestParametersDescriptor& parameters, const std::size_t& runsCount) override;

        void EndTest(const std::string& fixtureName, const std::string& testName,
                     const TestParametersDescriptor& parameters, const TestResult& result) override;

private:
        /// Write an escaped string. The escaping is currently very
        /// rudimentary and assumes that names, parameters etc. are ASCII.
        /// @param str String to write.
        void WriteEscapedString(const std::string& str);

private:
        std::ostream& _stream;
        TestSuiteMap  _testSuites;
};

} // namespace myhayai
