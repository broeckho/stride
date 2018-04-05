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
 * Header file for MainRunner.
 */

#include "Benchmarker.hpp"
#include "ConsoleOutputter.hpp"
#include "myhayai/ConsoleFileOutputter.hpp"
#include "myhayai/JUnitXmlFileOutputter.hpp"
#include "myhayai/JsonFileOutputter.hpp"
#include "myhayai/JsonOutputter.hpp"
#include "myhayai/JunitXmlOutputter.hpp"
#include "myhayai/Fixture.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <set>
#include <vector>

namespace myhayai {

/// Execution mode.
enum MainExecutionMode
{
        MainRunBenchmarks,
        MainListBenchmarks
};

/// Default main executable runner for Hayai.
class MainRunner
{
public:
        ///
        MainRunner() : ExecutionMode(MainRunBenchmarks), ShuffleBenchmarks(false), StdoutOutputter(nullptr) {}

        ///
        ~MainRunner();

        /// Parse arguments and initializes the MainRunner.
        /// @param argc             Argument count including the executable name.
        /// @param argv             Arguments.
        /// @param residualArgs     Pointer to vector to hold residual arguments
        /// after parsing. If not NULL, the parser will not fail upon
        /// encounting an unknown argument but will instead add it to the list
        /// of residual arguments and return a success code. Note: the parser
        /// will still fail if an invalid value is provided to a known
        /// argument.
        /// @returns 0 on success, otherwise the exit status code to be
        /// returned from the executable.
        int ParseArgs(int argc, char** argv, std::vector<char*>* residualArgs = nullptr);

        /// Run the selected execution mode.
        /// @returns the exit status code to be returned from the executable.
        int Run();

private:
        /// Run benchmarks.
        /// @returns the exit status code to be returned from the executable.
        int RunBenchmarks();

        /// List benchmarks.
        /// @returns the exit status code to be returned from the executable.
        int ListBenchmarks();

        /// Show usage.
        /// @param execName Executable name.
        void ShowUsage(const char* execName);

private:
        MainExecutionMode ExecutionMode; ///< Execution mode.
        bool ShuffleBenchmarks; ///< Shuffle benchmarks.
        std::vector<FileOutputter*> FileOutputters; ///< File outputters (freed by the class on destruction).
        Outputter* StdoutOutputter; /// Standard outputter (freed by the class on destruction).
};

} // namespace myhayai
