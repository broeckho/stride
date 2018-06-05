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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file of base class for config that needs to be read from a file.
 */

#include "CSVRow.h"

#include "util/StringUtils.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <vector>

namespace stride {
namespace util {

/**
 * A collection of CSVRow elements. Iterate with begin and end like STL containers.
 */
class CSV : protected std::vector<CSVRow>
{
public:
        /// Initialize from file.
        explicit CSV(const boost::filesystem::path& path);

        /// Initialize from inputstream.
        explicit CSV(std::istream& inputStream);

        /// Initialize with columnCount only; labels default to sequence numbers.
        explicit CSV(size_t columnCount);

        /// Initialize with header only.
        CSV(const std::initializer_list<std::string>& labels);

        /// Initialize with header only.
        explicit CSV(const std::vector<std::string>& labels);

        /// Default constructor, used for swig.
        CSV() = default;

        /// Comparison operator.
        bool operator==(const CSV& other) const;

        /// iterators
        using std::vector<CSVRow>::begin;
        using std::vector<CSVRow>::end;
        using std::vector<CSVRow>::size;

        /// Add row of values.
        template <typename... T>
        void AddRow(const T&... values);

        /// Add row of string values.
        void AddRow(std::vector<std::string> values);

        /// Number of columns in the CSV.
        size_t GetColumnCount() const { return m_column_count; }

        /// Convert label to index for more user friendly and robust implementation.
        /// This level of indirection does introduce a perfomance tradeoff.
        size_t GetIndexForLabel(const std::string& label) const;

        /// Write CSV to file.
        virtual void Write(const boost::filesystem::path& path) const;

private:
        friend boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const CSV& csv);

private:
        /// Read data from input stream.
        void ReadFromStream(std::istream& inputStream);

protected:
        /// Write header with labels.
        virtual void WriteLabels(boost::filesystem::ofstream& file) const;

        /// Write the body of rows.
        virtual void WriteRows(boost::filesystem::ofstream& file) const;

protected:
        std::vector<std::string> m_labels;
        size_t                   m_column_count = 0;
};

template <typename... T>
inline void CSV::AddRow(const T&... values)
{
        AddRow({ToString(values)...});
}

inline boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const CSV& csv)
{
        csv.WriteLabels(ofs);
        csv.WriteRows(ofs);
        return ofs;
}

} // namespace util
} // namespace stride
