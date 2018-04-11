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

#include <string>
#include <vector>

namespace myhayai {

/// Default main executable runner.
class MainRunner
{
public:
        ///
        MainRunner() : m_filter("*"), m_list_mode(true), m_shuffle(false), m_xml_path("") {}

        ///
        ~MainRunner() = default;

        /// Parse arguments and initializes the MainRunner.
        /// @param argc             Argument count including the executable name.
        /// @param argv             Arguments.
        /// @returns EXIT_SUCCESS / EXIT_FAILURE.
        int ParseArgs(int argc, char** argv);

        /// Run the selected execution mode.
        /// @returns the exit status code to be returned from the executable.
        int Execute();

private:
        /// List benchmarks.
        /// @returns the exit status code to be returned from the executable.
        int ListBenchmarks();

        /// Run benchmarks.
        /// @returns the exit status code to be returned from the executable.
        int ExecuteBenchmarks();

        /// Shuffle test names.
        /// \param names  vector to be shuffled.
        static void Shuffle(std::vector<std::string>& names);

private:
        std::string m_filter;    ///< Filter the names of tests to be executed.
        bool        m_list_mode; ///< Execution mode.
        bool        m_shuffle;   ///< Shuffle benchmarks.
        std::string m_xml_path;  ///< If not empty, produce xml in file at path.
};

} // namespace myhayai
