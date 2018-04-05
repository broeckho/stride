
#include "myhayai/JUnitXmlOutputter.hpp"

#include <iomanip>
#include <map>
#include <ostream>
#include <sstream>
#include <vector>

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
        for (TestSuiteMap::iterator testSuiteIt = _testSuites.begin(); testSuiteIt != _testSuites.end();
             ++testSuiteIt) {
                _stream << "    <testsuite name=\"" << testSuiteIt->first << "\" tests=\"" << testSuiteIt->second.size()
                        << "\">" << std::endl;

                // Write out each test case.
                for (std::vector<TestCase>::iterator testCaseIt = testSuiteIt->second.begin();
                     testCaseIt != testSuiteIt->second.end(); ++testCaseIt) {
                        _stream << "        <testcase name=\"";
                        WriteEscapedString(testCaseIt->Name);
                        _stream << "\"";

                        if (!testCaseIt->Skipped)
                                _stream << " time=\"" << testCaseIt->Time << "\" />" << std::endl;
                        else {
                                _stream << ">" << std::endl
                                        << "            <skipped />" << std::endl
                                        << "        </testcase>" << std::endl;
                        }
                }

                _stream << "    </testsuite>" << std::endl;
        }

        _stream << "</testsuites>" << std::endl;
}

void JUnitXmlOutputter::BeginTest(const std::string& fixtureName, const std::string& testName,
                                  const TestParametersDescriptor& parameters, const std::size_t& runsCount)
{
        (void)fixtureName;
        (void)testName;
        (void)parameters;
        (void)runsCount;
}

void JUnitXmlOutputter::SkipDisabledTest(const std::string& fixtureName, const std::string& testName,
                                         const TestParametersDescriptor& parameters, const std::size_t& runsCount)
{
        (void)fixtureName;
        (void)testName;
        (void)parameters;
        (void)runsCount;
}

void JUnitXmlOutputter::EndTest(const std::string& fixtureName, const std::string& testName,
                                const TestParametersDescriptor& parameters, const TestResult& result)
{
        (void)fixtureName;
        (void)testName;
        (void)parameters;

        auto fixtureIt = _testSuites.find(fixtureName);
        if (fixtureIt == _testSuites.end()) {
                _testSuites[fixtureName] = std::vector<TestCase>();
                fixtureIt                = _testSuites.find(fixtureName);
        }

        std::vector<TestCase>& testCases = fixtureIt->second;
        testCases.emplace_back(TestCase(fixtureName, testName, parameters, &result));
}

void JUnitXmlOutputter::WriteEscapedString(const std::string& str)
{
        std::string::const_iterator it = str.begin();
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
