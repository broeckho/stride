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
 * Implementation file for ConsoleOutputter.
 */

#include "myhayai/ConsoleOutputter.hpp"
#include "myhayai/BoxPlotData.hpp"

#include <iomanip>

using namespace std;

namespace myhayai {

void ConsoleOutputter::Begin(const size_t& enabledCount, const size_t& disabledCount)
{
        m_stream << fixed;
        m_stream << Console::TextGreen << "[==========]" << Console::TextDefault << " Running " << enabledCount
                 << (enabledCount == 1 ? " benchmark." : " benchmarks") << "." << endl;
}

void ConsoleOutputter::End(const size_t& executedCount, const size_t& disabledCount)
{
        m_stream << Console::TextGreen << "[==========]" << Console::TextDefault << " Ran " << executedCount
                 << (executedCount == 1 ? " benchmark." : " benchmarks");

        if (disabledCount) {
                m_stream << ", skipped " << disabledCount << (disabledCount == 1 ? " benchmark." : " benchmarks");
        } else {
                m_stream << ".";
        }
        m_stream << endl;
}

void ConsoleOutputter::BeginOrSkipTest(const string& fixtureName, const string& testName,
                                       const InfoFactory& infoFactory, const size_t& runsCount, const bool skip)
{
        if (skip) {
                m_stream << Console::TextCyan << "[ DISABLED ]";
        } else {
                m_stream << Console::TextGreen << "[ RUN      ]";
        }
        m_stream << Console::TextYellow << " ";
        m_stream << fixtureName << "." << testName;
        m_stream << Console::TextDefault << " (" << runsCount << (runsCount == 1 ? " run" : " runs") << ")" << endl;
}

void ConsoleOutputter::BeginTest(const string& fixtureName, const string& testName, const InfoFactory& infoFactory,
                                 const size_t& runsCount)
{
        BeginOrSkipTest(fixtureName, testName, infoFactory, runsCount, false);
}

void ConsoleOutputter::SkipDisabledTest(const string& fixtureName, const string& testName,
                                        const InfoFactory& infoFactory, const size_t& runsCount)
{
        BeginOrSkipTest(fixtureName, testName, infoFactory, runsCount, true);
}

void ConsoleOutputter::EndTest(const string& fixtureName, const string& testName, const InfoFactory& infoFactory,
                               const TestResult& result)
{
        const auto stats = BoxPlotData::Calculate(result);

        m_stream << Console::TextGreen << "[     DONE ]" << Console::TextYellow << " ";
        m_stream << fixtureName << "." << testName;
        m_stream << Console::TextDefault << " (" << setprecision(6) << (stats.m_total / 1000000.0) << " ms)" << endl;

        m_stream << Console::TextBlue << "[   RUNS   ] " << Console::TextDefault
                 << "       Average time: " << setprecision(3) << stats.m_average / 1000.0 << " us "
                 << "(" << Console::TextBlue << "~" << stats.m_std_dev / 1000.0 << " us" << Console::TextDefault << ")"
                 << endl;
        {
                double       mn    = (stats.m_min / 1000.0);
                double       av    = (stats.m_average / 1000.0);
                const string unit  = "us";
                double       delta = mn - av;

                m_stream << setw(34) << "Fastest time: " << (stats.m_min / 1000.0) << " " << unit << " ("
                         << (mn > av ? Console::TextRed : Console::TextGreen) << (mn > av ? "+" : "") << delta << " "
                         << unit << " / " << (mn > av ? "+" : "") << (delta * 100.0 / av) << " %"
                         << Console::TextDefault << ")" << endl;
        }
        {
                double       mx    = (stats.m_max / 1000.0);
                double       av    = (stats.m_average / 1000.0);
                const string unit  = "us";
                double       delta = double(mx) - double(av);

                m_stream << setw(34) << "Slowest time: " << mx << " " << unit << " ("
                         << (mx > av ? Console::TextRed : Console::TextGreen) << (mx > av ? "+" : "") << delta << " "
                         << unit << " / " << (mx > av ? "+" : "") << (delta * 100.0 / av) << " %"
                         << Console::TextDefault << ")" << endl;
        }

        m_stream << setw(34) << "Median time: " << stats.m_median / 1000.0 << " us (" << Console::TextCyan
                 << "1st quartile: " << stats.m_quartile1 / 1000.0
                 << " us | 3rd quartile: " << stats.m_quartile3 / 1000.0 << " us" << Console::TextDefault << ")"
                 << endl;
}

} // namespace myhayai
