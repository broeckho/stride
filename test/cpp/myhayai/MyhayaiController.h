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
 */

/**
 * @file
 * Header file for myhayai::MyhayaiController.
 */

#pragma once

#include "BenchmarkRunner.h"

#include <regex>
#include <string>
#include <vector>

namespace myhayai {

/**
 * Controls the benchmark test execution based on command line arguments.
 * Its two functions are to list the benchmark that are registered or to
 * execute them.
 */
class MyhayaiController
{
public:
        /// Defaults.
        MyhayaiController()
            : m_list_mode(true), m_no_color(true), m_shuffle(false), m_info_path(), m_json_path(), m_xml_path(),
              m_negative(), m_positive()
        {
        }

        ///
        ~MyhayaiController() = default;

        /// Parse the command line arguments.
        /// @param argc  Argument count including the executable name.
        /// @param argv  Arguments.
        void ParseArgs(int argc, char** argv);

        /// Execute the selected (List/Run) function.
        void Control();

        /// List benchmarks.
        void ListBenchmarks() const;

        /// Run benchmarks.
        void RunBenchmarks();

private:
        bool m_list_mode; ///< Execution mode (list or run benchmarks).
        bool m_no_color;  ///< Do not use color on console output.
        bool m_shuffle;   ///< Shuffle benchmarks.

        std::string m_info_path; ///< If not empty, produce output in info format in file at path.
        std::string m_json_path; ///< If not empty, produce output in json format in file at path.
        std::string m_xml_path;  ///< If not empty, produce output in xml format in file at path.

        std::vector<std::string> m_negative; ///< Negative for excluding tests.
        std::vector<std::string> m_positive; ///< Positive for including tests.
};

} // namespace myhayai
