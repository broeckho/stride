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
 * Header file for TestParameterDescriptor.
 */

#include "myhayai/Benchmarker.hpp"

#include "myhayai/ConsoleOutputter.hpp"
#include "myhayai/TestDescriptor.hpp"
#include "myhayai/TestFactory.hpp"
#include "myhayai/TestResult.hpp"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace myhayai {

Benchmarker& Benchmarker::Instance()
{
        static Benchmarker singleton;
        return singleton;
}

TestDescriptor* Benchmarker::RegisterTest(const char* fixtureName, const char* testName, size_t runs,
                                          TestFactory* testFactory, TestParametersDescriptor parameters)
{
        // Determine if the test has been disabled.
        static const char* disabledPrefix = "DISABLED_";
        bool               isDisabled     = ((::strlen(testName) >= 9) && (!::memcmp(testName, disabledPrefix, 9)));

        if (isDisabled)
                testName += 9;

        // Add the descriptor.
        TestDescriptor* descriptor =
            new TestDescriptor(fixtureName, testName, runs, testFactory, parameters, isDisabled);
        Instance()._tests.push_back(descriptor);
        return descriptor;
}

void Benchmarker::AddOutputter(Outputter& outputter) { Instance()._outputters.push_back(&outputter); }

void Benchmarker::ApplyPatternFilter(const char* pattern)
{
        Benchmarker& instance = Instance();

        // Split the filter at '-' if it exists.
        const char* const dash = strchr(pattern, '-');
        string       positive;
        string       negative;
        if (dash == nullptr) {
                positive = pattern;
        } else {
                positive = string(pattern, dash);
                negative = string(dash + 1);
                if (positive.empty())
                        positive = "*";
        }

        // Iterate across all tests and test them against the patterns.
        size_t index = 0;
        while (index < instance._tests.size()) {
                TestDescriptor* desc = instance._tests[index];
                if ((!FilterMatchesString(positive.c_str(), desc->CanonicalName)) ||
                    (FilterMatchesString(negative.c_str(), desc->CanonicalName))) {
                        instance._tests.erase(instance._tests.begin() +
                                              vector<TestDescriptor*>::difference_type(index));
                        delete desc;
                } else {
                        ++index;
                }
        }
}

void Benchmarker::RunAllTests()
{
        ConsoleOutputter        defaultOutputter;
        vector<Outputter*> defaultOutputters;
        defaultOutputters.push_back(&defaultOutputter);
        Benchmarker&             instance   = Instance();
        vector<Outputter*>& outputters = (instance._outputters.empty() ? defaultOutputters : instance._outputters);

        // Get the tests for execution.
        vector<TestDescriptor*>                 tests         = instance.GetTests();
        const size_t                            totalCount    = tests.size();
        size_t                                  disabledCount = 0;
        vector<TestDescriptor*>::const_iterator testsIt       = tests.begin();

        while (testsIt != tests.end()) {
                if ((*testsIt)->IsDisabled)
                        ++disabledCount;
                ++testsIt;
        }
        const size_t enabledCount = totalCount - disabledCount;

        // Begin output.
        for (size_t outputterIndex = 0; outputterIndex < outputters.size(); outputterIndex++) {
                outputters[outputterIndex]->Begin(enabledCount, disabledCount);
        }

        // Run through all the tests in ascending order.
        size_t index = 0;
        while (index < tests.size()) {
                // Get the test descriptor.
                TestDescriptor* descriptor = tests[index++];

                // Check if test matches include filters
                if (instance._include.size() > 0) {
                        bool        included = false;
                        string name     = descriptor->FixtureName + "." + descriptor->TestName;
                        for (size_t i = 0; i < instance._include.size(); i++) {
                                if (name.find(instance._include[i]) != string::npos) {
                                        included = true;
                                        break;
                                }
                        }
                        if (!included)
                                continue;
                }

                // Check if test is not disabled.
                if (descriptor->IsDisabled) {
                        for (size_t outputterIndex = 0; outputterIndex < outputters.size(); outputterIndex++)
                                outputters[outputterIndex]->SkipDisabledTest(descriptor->FixtureName,
                                                                             descriptor->TestName,
                                                                             descriptor->Parameters, descriptor->Runs);
                        continue;
                }

                // Describe the beginning of the run.
                for (size_t outputterIndex = 0; outputterIndex < outputters.size(); outputterIndex++)
                        outputters[outputterIndex]->BeginTest(descriptor->FixtureName, descriptor->TestName,
                                                              descriptor->Parameters, descriptor->Runs);

                // Execute each individual run.
                vector<uint64_t> runTimes(descriptor->Runs);
                size_t           run = 0;
                while (run < descriptor->Runs) {
                        Fixture*    test = descriptor->Factory->CreateTest();
                        uint64_t time = test->Run();
                        runTimes[run] = time;
                        delete test;
                        ++run;
                }

                // Calculate the test result.
                TestResult testResult(runTimes);

                // Describe the end of the run.
                for (size_t outputterIndex = 0; outputterIndex < outputters.size(); outputterIndex++) {
                        outputters[outputterIndex]->EndTest(descriptor->FixtureName, descriptor->TestName,
                                                            descriptor->Parameters, testResult);
                }
        }

        // End output.
        for (size_t outputterIndex = 0; outputterIndex < outputters.size(); outputterIndex++) {
                outputters[outputterIndex]->End(enabledCount, disabledCount);
        }
}

vector<const TestDescriptor*> Benchmarker::ListTests()
{
        vector<const TestDescriptor*> tests;
        Benchmarker&                       instance = Instance();
        size_t                        index    = 0;
        while (index < instance._tests.size())
                tests.push_back(instance._tests[index++]);
        return tests;
}

void Benchmarker::ShuffleTests()
{
        Benchmarker&       instance = Instance();
        random_device rd;
        mt19937       g(rd());
        shuffle(instance._tests.begin(), instance._tests.end(), g);
}

Benchmarker::~Benchmarker()
{
        // Release all test descriptors.
        size_t index = _tests.size();
        while (index--) {
                delete _tests[index];
        }
}

vector<TestDescriptor*> Benchmarker::GetTests() const
{
        vector<TestDescriptor*> tests;
        size_t                  index = 0;
        while (index < _tests.size()) {
                tests.push_back(_tests[index++]);
        }
        return tests;
}

bool Benchmarker::FilterMatchesString(const char* filter, const string& str)
{
        const char* patternStart = filter;
        while (true) {
                if (PatternMatchesString(patternStart, str.c_str()))
                        return true;
                // Finds the next pattern in the filter.
                patternStart = strchr(patternStart, ':');
                // Returns if no more pattern can be found.
                if (!patternStart)
                        return false;
                // Skips the pattern separater (the ':' character).
                patternStart++;
        }
}

bool Benchmarker::PatternMatchesString(const char* pattern, const char* str)
{
        switch (*pattern) {
        case '\0':
        case ':': return (*str == '\0');
        case '?': // Matches any single character.
                return ((*str != '\0') && (PatternMatchesString(pattern + 1, str + 1)));
        case '*': // Matches any string (possibly empty) of characters.
                return (((*str != '\0') && (PatternMatchesString(pattern, str + 1))) ||
                        (PatternMatchesString(pattern + 1, str)));
        default: return ((*pattern == *str) && (PatternMatchesString(pattern + 1, str + 1)));
        }
}

} // namespace myhayai
