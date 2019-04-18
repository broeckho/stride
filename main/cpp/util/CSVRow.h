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
 * @file
 * Header file of base class for config that needs to be read from a file.
 */

#pragma once

#include "util/StringUtils.h"

#include <boost/lexical_cast.hpp>
#include <iosfwd>

namespace stride {
namespace util {

class CSV;

/**
 * Row in CSV file.
 */
class CSVRow
{
public:
        /// CSVRow initialized with values. Should no be called by user code. CSV has convenience functions.
        CSVRow(const CSV* parent, const std::vector<std::string>& values);

        CSVRow(const CSVRow&) = default;

        CSVRow& operator=(const CSVRow&) = default;

        /// Get value at index. When T is specified, StringUtils are used to try to convert the value to type T.
        template <typename T = std::string>
        T GetValue(size_t index) const;

        /// Get value based on label. Note this is slower than using the index.
        template <typename T = std::string>
        T GetValue(const std::string& label) const;

        /// Compare operator.
        bool operator==(const CSVRow& other) const;

        /// Print to stream.
        friend std::ostream& operator<<(std::ostream& os, const CSVRow& row);

protected:
        const CSV*               m_parent;
        std::vector<std::string> m_values;
};

/**
 * Converts a string to an arithmetic type, in a safe manner.
 * @throws bad_lexical_cast if \p val can't be converted to a double or int
 * @throws bad_numeric_cast if \p val can't be converted to T
 * @tparam T the type to safe cast to
 * @param val the value to cast
 * @return
 */
template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
inline T safe_cast(const std::string& val)
{
        if (std::is_floating_point<T>::value) {
                return boost::numeric_cast<T>(boost::lexical_cast<double>(val));
        }
        return boost::numeric_cast<T>(boost::lexical_cast<long long>(val));
}

/**
 * Converts a string to a type.
 * @throws bad_lexical_cast if \p val can't be converted to T
 * @tparam T the type to safe cast to
 * @param val the value to cast
 * @return
 */
template <typename T, std::enable_if_t<!std::is_arithmetic<T>::value, int> = 0>
inline T safe_cast(const std::string& val)
{
        return boost::lexical_cast<T>(val);
}

/// Declaration of specialization
template <>
std::string CSVRow::GetValue<std::string>(size_t index) const;

/// Declaration of specialization
template <>
std::string CSVRow::GetValue<std::string>(const std::string& label) const;

///
template <typename T>
inline T CSVRow::GetValue(size_t index) const
{
        return safe_cast<T>(GetValue<std::string>(index));
}

///
template <typename T>
inline T CSVRow::GetValue(const std::string& label) const
{
        return safe_cast<T>(GetValue<std::string>(label));
}

} // namespace util
} // namespace stride
