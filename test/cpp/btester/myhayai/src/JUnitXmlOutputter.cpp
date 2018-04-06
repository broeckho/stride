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
 *  The original copyright, to be found in the directory one level higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for JUnitXmlOutputter.
 */

#include "myhayai/JUnitXmlOutputter.hpp"

#include <iomanip>
#include <map>
#include <ostream>
#include <sstream>
#include <vector>

using namespace std;

namespace myhayai {

void JUnitXmlOutputter::Begin(const std::size_t& enabledCount, const std::size_t& disabledCount)
{
        (void)enabledCount;
        (void)disabledCount;
}

void JUnitXmlOutputter::End(const std::size_t& executedCount, const std::size_t& disabledCount)
{
        (void)executedCount;
        (void)disabledCount;

        // Write the header.
        _stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl << "<testsuites>" << std::endl;

        // Write out each test suite (fixture.)
        for (const auto& testSuite : _testSuites) {
                _stream << "    <testsuite name=\"" << testSuite.first << "\" tests=\"" << testSuite.second.size()
                        << "\">" << std::endl;

                // Write out each test case.
                for (const auto& testCase : testSuite.second) {
                        _stream << "        <testcase name=\"";
                        WriteEscapedString(testCase.Name);
                        _stream << "\"";

                        if (!testCase.Skipped)
                                _stream << " time=\"" << testCase.Time << "\" />" << endl;
                        else {
                                _stream << ">" << endl
                                        << "            <skipped />" << endl
                                        << "        </testcase>" << endl;
                        }
                }

                _stream << "    </testsuite>" << endl;
        }

        _stream << "</testsuites>" << endl;
}

void JUnitXmlOutputter::BeginTest(const string& fixtureName, const string& testName,
                                  const TestParametersDescriptor& parameters, const size_t& runsCount)
{
        (void)fixtureName;
        (void)testName;
        (void)parameters;
        (void)runsCount;
}

void JUnitXmlOutputter::SkipDisabledTest(const string& fixtureName, const string& testName,
                                         const TestParametersDescriptor& parameters, const size_t& runsCount)
{
        (void)fixtureName;
        (void)testName;
        (void)parameters;
        (void)runsCount;
}

void JUnitXmlOutputter::EndTest(const string& fixtureName, const string& testName,
                                const TestParametersDescriptor& parameters, const TestResult& result)
{
        (void)fixtureName;
        (void)testName;
        (void)parameters;

        auto fixtureIt = _testSuites.find(fixtureName);
        if (fixtureIt == _testSuites.end()) {
                _testSuites[fixtureName] = vector<TestCase>();
                fixtureIt                = _testSuites.find(fixtureName);
        }

        vector<TestCase>& testCases = fixtureIt->second;
        testCases.emplace_back(TestCase(fixtureName, testName, parameters, &result));
}

void JUnitXmlOutputter::WriteEscapedString(const string& str)
{
        string::const_iterator it = str.begin();
        while (it != str.end()) {
                char c = *it++;
                switch (c) {
                case '"': _stream << "&quot;"; break;
                case '\'': _stream << "&apos;"; break;
                case '<': _stream << "&lt;"; break;
                case '>': _stream << "&gt;"; break;
                case '&': _stream << "&amp;"; break;
                default: _stream << c; break;
                }
        }
}

} // namespace myhayai
