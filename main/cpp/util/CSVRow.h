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
 * @file
 * Header file of base class for config that needs to be read from a file.
 */

#include "util/StringUtils.h"

#include <boost/filesystem/path.hpp>
#include <ostream>

namespace stride {
namespace util {

// forward declaration
class CSV;

/**
 * @class Row in CSV file.
 */
class CSVRow
{
private:
protected:
        const CSV*               parent;
        std::vector<std::string> values;

public:
        /// CSVRow initialized with values. Should no be called by user code. CSV has convenience functions.
        CSVRow(const CSV* parent, const std::vector<std::string>& values);

        /// Get value at index. When T is specified, StringUtils are used to try to convert the value to type T.
        template <typename T = std::string>
        T GetValue(size_t index) const;

        /// Get value based on label. Note this is slower than using the index.
        template <typename T = std::string>
        T GetValue(const std::string &label) const;

        /// Compare operator.
        bool operator==(const CSVRow& other) const;

        /// Print to stream.
        friend std::ostream& operator<<(std::ostream& os, const CSVRow& row);
};

/// Declaration of specialization
template <>
std::string CSVRow::GetValue<std::string>(size_t index) const;

/// Declaration of specialization
template <>
std::string CSVRow::GetValue<std::string>(const std::string &label) const;

///
template <typename T>
inline T CSVRow::GetValue(size_t index) const
{
        return FromString<T>(GetValue<std::string>(index));
}

///
template <typename T>
inline T CSVRow::GetValue(const std::string &label) const
{
        return FromString<T>(GetValue<std::string>(label));
}

} // namespace util
} // namespace stride
