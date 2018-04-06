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

#include <iomanip>

namespace myhayai {

void ConsoleOutputter::Begin(const std::size_t& enabledCount, const std::size_t& disabledCount)
{
        _stream << std::fixed;
        _stream << Console::TextGreen << "[==========]" << Console::TextDefault << " Running " << enabledCount
                << (enabledCount == 1 ? " benchmark." : " benchmarks");

        if (disabledCount) {
                _stream << ", skipping " << disabledCount << (disabledCount == 1 ? " benchmark." : " benchmarks");
        } else {
                _stream << ".";
        }
        _stream << std::endl;
}

void ConsoleOutputter::End(const std::size_t& executedCount, const std::size_t& disabledCount)
{
        _stream << Console::TextGreen << "[==========]" << Console::TextDefault << " Ran " << executedCount
                << (executedCount == 1 ? " benchmark." : " benchmarks");

        if (disabledCount) {
                _stream << ", skipped " << disabledCount << (disabledCount == 1 ? " benchmark." : " benchmarks");
        } else {
                _stream << ".";
        }
        _stream << std::endl;
}

void ConsoleOutputter::BeginOrSkipTest(const std::string& fixtureName, const std::string& testName,
                                       const TestParametersDescriptor& parameters, const std::size_t& runsCount,
                                       const bool skip)
{
        if (skip) {
                _stream << Console::TextCyan << "[ DISABLED ]";
        } else {
                _stream << Console::TextGreen << "[ RUN      ]";
        }
        _stream << Console::TextYellow << " ";
        WriteTestNameToStream(_stream, fixtureName, testName, parameters);
        _stream << Console::TextDefault << " (" << runsCount << (runsCount == 1 ? " run, " : " runs, ") << std::endl;
}

void ConsoleOutputter::BeginTest(const std::string& fixtureName, const std::string& testName,
                                 const TestParametersDescriptor& parameters, const std::size_t& runsCount)
{
        BeginOrSkipTest(fixtureName, testName, parameters, runsCount, false);
}

void ConsoleOutputter::SkipDisabledTest(const std::string& fixtureName, const std::string& testName,
                                        const TestParametersDescriptor& parameters, const std::size_t& runsCount)
{
        BeginOrSkipTest(fixtureName, testName, parameters, runsCount, true);
}

void ConsoleOutputter::EndTest(const std::string& fixtureName, const std::string& testName,
                               const TestParametersDescriptor& parameters, const TestResult& result)
{
        _stream << Console::TextGreen << "[     DONE ]" << Console::TextYellow << " ";
        WriteTestNameToStream(_stream, fixtureName, testName, parameters);
        _stream << Console::TextDefault << " (" << std::setprecision(6) << (result.TimeTotal() / 1000000.0) << " ms)"
                << std::endl;

        _stream << Console::TextBlue << "[   RUNS   ] " << Console::TextDefault
                << "       Average time: " << std::setprecision(3) << result.RunTimeAverage() / 1000.0 << " us "
                << "(" << Console::TextBlue << "~" << result.RunTimeStdDev() / 1000.0 << " us" << Console::TextDefault
                << ")" << std::endl;

        {
                double            deviated = (result.RunTimeMinimum() / 1000.0);
                double            average  = (result.RunTimeAverage() / 1000.0);
                const std::string unit     = "us";
                double            _d_      = deviated - average;

                _stream << std::setw(34) << "Fastest time: " << (result.RunTimeMinimum() / 1000.0) << " " << unit
                        << " (" << (deviated > average ? Console::TextRed : Console::TextGreen)
                        << (deviated > average ? "+" : "") << _d_ << " " << unit << " / "
                        << (deviated > average ? "+" : "") << (_d_ * 100.0 / average) << " %" << Console::TextDefault
                        << ")" << std::endl;
        }
        {
                double            deviated = (result.RunTimeMaximum() / 1000.0);
                double            average  = (result.RunTimeAverage() / 1000.0);
                const std::string unit     = "us";
                double            _d_      = double(deviated) - double(average);

                _stream << std::setw(34) << "Slowest time: " << deviated << " " << unit << " ("
                        << (deviated > average ? Console::TextRed : Console::TextGreen)
                        << (deviated > average ? "+" : "") << _d_ << " " << unit << " / "
                        << (deviated > average ? "+" : "") << (_d_ * 100.0 / average) << " %" << Console::TextDefault
                        << ")" << std::endl;
        }

        _stream << std::setw(34) << "Median time: " << result.RunTimeMedian() / 1000.0 << " us (" << Console::TextCyan
                << "1st quartile: " << result.RunTimeQuartile1() / 1000.0
                << " us | 3rd quartile: " << result.RunTimeQuartile3() / 1000.0 << " us" << Console::TextDefault << ")"
                << std::endl;
}

} // namespace myhayai
