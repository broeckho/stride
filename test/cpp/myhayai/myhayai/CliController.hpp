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
 */

/**
 * @file
 * Header file for myhayai::CliController.
 */

#include <regex>
#include <string>
#include <vector>

namespace myhayai {

/// Default main executable runner.
class CliController
{
public:
        /// Defaults.
        CliController()
            : m_list_mode(true), m_no_color(true), m_shuffle(false), m_info_path(), m_json_path(), m_xml_path(),
              m_negative(), m_positive()
        {
        }

        ///
        ~CliController() = default;

        /// Parse arguments and initializes the MainRunner.
        /// @param argc             Argument count including the executable name.
        /// @param argv             Arguments.
        void ParseArgs(int argc, char** argv);

        /// Run the selected execution mode.
        void Control();

private:
        /// Return the canonical names obtained after applying
        /// the positive and negative regex filters.
        std::vector<std::string> GetIncludedNames() const;

        /// Checks whether a name gets to be included after applying
        /// the positive and exclude negative filters.
        bool IsIncluded(const std::string& name) const;

        /// List benchmarks.
        void ListBenchmarks() const;

        /// Run benchmarks.
        void RunBenchmarks();

        /// Shuffle test names.
        /// \param names  vector to be shuffled.
        void Shuffle(std::vector<std::string>& names) const;

private:
        bool m_list_mode; ///< Execution mode (list or run benchmarks).
        bool m_no_color;  ///< Do not use color on console output.
        bool m_shuffle;   ///< Shuffle benchmarks.

        std::string m_info_path; ///< If not empty, produce output in info format in file at path.
        std::string m_json_path; ///< If not empty, produce output in json format in file at path.
        std::string m_xml_path;  ///< If not empty, produce output in xml format in file at path.

        std::vector<std::regex> m_negative; ///< Negative regexes for excluding tests.
        std::vector<std::regex> m_positive; ///< Positive regexes for including tests.
};

} // namespace myhayai
