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
 * Implementation file for Benchmarker.
 */

#include "myhayai/Benchmarker.hpp"

#include "myhayai/ConsoleOutputter.hpp"
#include "myhayai/Fixture.hpp"
#include "myhayai/TestFactory.hpp"
#include "myhayai/InfoFactory.hpp"
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

void Benchmarker::AddOutputter(Outputter& outputter) { Instance().m_outputters.push_back(&outputter); }

void Benchmarker::ApplyPatternFilter(const char* pattern)
{
        Benchmarker& instance = Instance();

        // Split the filter at '-' if it exists.
        const char* const dash = strchr(pattern, '-');
        string            positive;
        string            negative;
        if (dash == nullptr) {
                positive = pattern;
        } else {
                positive = string(pattern, dash);
                negative = string(dash + 1);
                if (positive.empty())
                        positive = "*";
        }

        // Iterate across all tests and test them against the patterns.
        for (auto& desc : instance.m_test_descriptors) {
                if ((!FilterMatchesString(positive.c_str(), desc.GetCanonicalName())) ||
                    (FilterMatchesString(negative.c_str(), desc.GetCanonicalName()))) {
                        desc.m_is_in_filter = false;
                }
        }
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

const TestDescriptors& Benchmarker::GetTestDescriptors() const { return m_test_descriptors; }

Benchmarker& Benchmarker::Instance()
{
        static Benchmarker singleton;
        return singleton;
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

TestDescriptor Benchmarker::RegisterTest(const char* fixture_name, const char* test_name, size_t runs,
                                         TestFactory test_factory, InfoFactory infoFactory, bool disabled)
{
        TestDescriptor descriptor(fixture_name, test_name, runs, std::move(test_factory), infoFactory, disabled);
        Instance().m_test_descriptors.emplace_back(descriptor);
        return descriptor;
}

void Benchmarker::RunAllTests()
{
        ConsoleOutputter   defaultOutputter;
        vector<Outputter*> defaultOutputters;
        defaultOutputters.push_back(&defaultOutputter);
        Benchmarker&        instance   = Instance();
        vector<Outputter*>& outputters = (instance.m_outputters.empty() ? defaultOutputters : instance.m_outputters);

        // Get the test_descriptors for execution.
        auto         test_descriptors = instance.GetTestDescriptors();
        const size_t totalCount       = test_descriptors.size();
        const size_t disabledCount    = test_descriptors.CountDisabled();
        const size_t enabledCount     = totalCount - disabledCount;

        // Begin output.
        for (auto& o : outputters) {
                o->Begin(enabledCount, disabledCount);
        }

        // Run through all the test_descriptors in ascending order.
        for (const auto& t_d : test_descriptors) {

                // If test is not in filter, just skip (not outputting info for now).
                if (t_d.m_is_disabled) {
                        continue;
                }

                // If test is disabled output and skip.
                if (t_d.m_is_disabled) {
                        for (auto& o : outputters)
                                o->SkipDisabledTest(t_d.m_fixture_name, t_d.m_test_name, t_d.m_info_factory,
                                                    t_d.m_num_runs);
                        continue;
                }

                // Describe the beginning of the run.
                for (auto& o : outputters)
                        o->BeginTest(t_d.m_fixture_name, t_d.m_test_name, t_d.m_info_factory, t_d.m_num_runs);

                // Execute each individual run.
                vector<uint64_t> run_times(t_d.m_num_runs);
                for (auto& rt : run_times) {
                        Fixture test = t_d.m_test_factory();
                        rt           = test.Run();
                }

                // Calculate the test result.
                TestResult results(run_times);

                // Describe the end of the run.
                for (auto& o : outputters) {
                        o->EndTest(t_d.m_fixture_name, t_d.m_test_name, t_d.m_info_factory, results);
                }
        }

        // End output.
        for (auto& o : outputters) {
                o->End(enabledCount, disabledCount);
        }
}

void Benchmarker::ShuffleTests()
{
        Benchmarker&  instance = Instance();
        random_device rd;
        mt19937       g(rd());
        shuffle(instance.m_test_descriptors.begin(), instance.m_test_descriptors.end(), g);
}

} // namespace myhayai
