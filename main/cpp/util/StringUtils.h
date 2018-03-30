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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file.
 * Miscellaneous string utilities.
 */

#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace stride {
namespace util {

/// Builds a value of type T representation from a string.
template <typename T>
inline T FromString(const std::string& s)
{
        std::stringstream ss(s);
        T                 t;
        ss >> t;
        return t;
}

/// Split a string (in order of occurence) by splitting it on the given
/// delimiters.
inline std::vector<std::string> Split(const std::string& str, const std::string& delimiters)
{
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, str, boost::is_any_of(delimiters));
        return tokens;
}

/// Tokenize a string (in order of occurence) with the given delimiters.
/// Multiple consecutive delimiters do NOT define "empty" tokens; they are skipped.
inline std::vector<std::string> Tokenize(const std::string& str, const std::string& delimiters)
{
        std::vector<std::string> tokens;

        // Skip delimiters at beginning.
        std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // Find first non-delimiter.
        std::string::size_type pos = str.find_first_of(delimiters, lastPos);

        while (std::string::npos != pos || std::string::npos != lastPos) {
                // Found a token, add it to the vector.
                tokens.push_back(str.substr(lastPos, pos - lastPos));
                // Skip delimiters.
                lastPos = str.find_first_not_of(delimiters, pos);
                // Find next non-delimiter.
                pos = str.find_first_of(delimiters, lastPos);
        }

        return tokens;
}

/// Builds a string representation of a value of type T.
template <typename T>
inline std::string ToString(const T& value)
{
        std::stringstream ss;
        ss << value;
        return ss.str();
}

/// Builds a string representation with minimum width of a value of type T.
template <typename T>
inline std::string ToString(const T& value, int width, char fill = ' ')
{
        std::stringstream ss;
        ss << std::setw(width) << std::setfill(fill) << value;
        return ss.str();
}

/// Builds a string with lower case characters only.
inline std::string ToLower(const std::string& source)
{
        auto        lower = [](int c) -> int { return std::tolower(c); };
        std::string copy;
        std::transform(source.begin(), source.end(), std::back_inserter(copy), lower);
        return copy;
}

/// Builds a string with upper case characters only.
inline std::string ToUpper(const std::string& source)
{
        auto        upper = [](int c) -> int { return std::toupper(c); };
        std::string copy;
        std::transform(source.begin(), source.end(), std::back_inserter(copy), upper);
        return copy;
}

/// Trim characters at right end of string.
inline std::string TrimRight(const std::string& source, const std::string& t = " ")
{
        std::string str = source;
        return str.erase(str.find_last_not_of(t) + 1);
}

/// Trim characters at left end of string.
inline std::string TrimLeft(const std::string& source, const std::string& t = " ")
{
        std::string str = source;
        return str.erase(0, source.find_first_not_of(t));
}

/// Trim characters at both ends of string.
inline std::string Trim(const std::string& source, const std::string& t = " ")
{
        return TrimLeft(TrimRight(source, t), t);
}

} // namespace util
} // namespace stride
