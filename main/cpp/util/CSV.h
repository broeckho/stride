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

#pragma once

#include "CSVRow.h"

#include "util/StringUtils.h"
#include "util/is_iterator.h"

#include <iosfwd>
#include <type_traits>
#include <vector>

#ifdef BOOST_FOUND
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace filesys = boost::filesystem;
#else
#include <filesystem>
namespace filesys = std::filesystem;
#endif
namespace stride {
namespace util {

/**
 * A collection of CSVRow elements. Iterate with begin and end like STL containers.
 */
class CSV : protected std::vector<CSVRow>
{
public:
        /// Initialize from file. If optLabels not specifed, the file is required.
        explicit CSV(const filesys::path& path, std::initializer_list<std::string> optLabels = {});

        /// Initialize from inputstream.
        explicit CSV(std::istream& inputStream);

        /// Initialize with columnCount only; labels default to sequence numbers.
        explicit CSV(size_t columnCount);

        /// Initialize with header only.
        explicit CSV(const std::vector<std::string>& labels);

        /// Initialize with header labels only.
        template <typename It>
        explicit CSV(It labelsBegin, It labelsEnd)
            : m_labels(ToString(labelsBegin, labelsEnd)), m_column_count(m_labels.size())
        {
        }

        /// Default constructor, used for swig.
        CSV() = default;

        /// iterators
        using std::vector<CSVRow>::begin;
        using std::vector<CSVRow>::end;
        using std::vector<CSVRow>::size;

        /// Comparison operator.
        bool operator==(const CSV& other) const;

        /// Add row of values.
        template <typename... T>
        void AddRow(const T&... values);

        /// Add multiple rows of strings at the same time
        void AddRows(const std::vector<std::vector<std::string>>& rows);

        /// Add a row of strings
        void AddRow(const std::vector<std::string>& row);

        /// Add row of values (ToString handles the case when the iterator points to strings).
        /// \tparam It      Iterator type pointing to the values.
        /// \param first    Start of the range to be included
        /// \param last     Past-the-end of the range.
        template <typename It>
        void AddRow(typename std::enable_if<is_iterator<It>::value, It>::type first, It last)
        {
                CSVRow csvRow(this, ToString(first, last));
                this->push_back(csvRow);
        }

        /// Number of columns in the CSV.
        size_t GetColumnCount() const { return m_column_count; }

        /// Convert label to index for more user friendly and robust implementation.
        /// This level of indirection does introduce a perfomance tradeoff.
        size_t GetIndexForLabel(const std::string& label) const;

        /// Write CSV to file.
        void Write(const filesys::path& path) const;

        const std::vector<std::string>& GetLabels() const;

private:
        friend std::ofstream& operator<<(std::ofstream& ofs, const CSV& csv);

        /// Read data from input stream.
        void ReadFromStream(std::istream& inputStream);

        /// Write header with labels.
        void WriteLabels(std::ofstream& file) const;

        /// Write the body of rows.
        void WriteRows(std::ofstream& file) const;

        std::vector<std::string> m_labels;
        size_t                   m_column_count = 0;
};

template <typename... T>
inline void CSV::AddRow(const T&... values)
{
        AddRow({ToString(values)...});
}

inline std::ofstream& operator<<(std::ofstream& ofs, const CSV& csv)
{
        csv.WriteLabels(ofs);
        csv.WriteRows(ofs);
        return ofs;
}

} // namespace util
} // namespace stride
