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
 * Header file for TestParametersDescriptor.
 */

#include "Fixture.hpp"
#include "TestFactory.hpp"
#include "TestParameterDescriptor.hpp"

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

namespace myhayai {

/// Test parameters descriptor.
class TestParametersDescriptor
{
public:
        ///
        TestParametersDescriptor() = default;

        ///
        TestParametersDescriptor(const char* rawDeclarations, const char* rawValues);

        ///
        const std::vector<TestParameterDescriptor>& Parameters() const { return _parameters; }

private:
        /// Quoting state.
        enum QuotingState
        {
                Unquoted,
                SingleQuoted,
                DoubleQuoted
        };

        /// Trimmed string.
        /// @param start    Start character.
        /// @param end      Character one position beyond end.
        inline static std::string TrimmedString(const char* start, const char* end)
        {
                while (start < end) {
                        if ((*start == ' ') || (*start == '\r') || (*start == '\n') || (*start == '\t'))
                                ++start;
                        else
                                break;
                }
                while (end > start) {
                        const char c = *(end - 1);
                        if ((c != ' ') && (c != '\r') && (c != '\n') && (c != '\t'))
                                break;
                        --end;
                }
                return std::string(start, std::string::size_type(end - start));
        }

        /// Parse comma separated parentherized value.
        /// @param separated Separated values as "(..[, ..])".
        /// @returns the individual values with white space trimmed.
        static std::vector<std::string> ParseCommaSeparated(const char* separated);

        /// Parse parameter declaration.
        /// @param raw Raw declaration.
        TestParameterDescriptor ParseDescriptor(const std::string& raw);

private:
        std::vector<TestParameterDescriptor> _parameters;
};

} // namespace myhayai
