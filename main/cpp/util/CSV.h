#ifndef MAIN_CPP_CONFIG_CSV_H_
#define MAIN_CPP_CONFIG_CSV_H_

/**
 * @file
 * Header file of base class for config that needs to be read from a file.
 */

#include <fstream>
#include <vector>

#include <boost/filesystem/path.hpp>

#include "util/StringUtils.h"

using namespace stride::util;

namespace stride {
namespace config {
using namespace stride::util;

// forward declaration
class CSV;

/**
 * @class Row in CSV file.
 */
class CSVRow
{
private:
protected:
        const CSV* parent;
        std::vector<std::string> values;

public:
        /// CSVRow initialized with values. Should no be called by user code. CSV has convenience functions.
        CSVRow(const CSV* parent, const std::vector<std::string>& values);

        /// Get value at specific index. When T is specified, StringUtils are used to try to convert the value to type
        /// T.
        template <typename T = std::string>
        T getValue(size_t index) const;

        /// Get value based on label. Note this is slower than using the index.
        template <typename T = std::string>
        T getValue(const std::string& label) const;

        /// Compare operator.
        bool operator==(const CSVRow& other) const;

        /// Print to stream.
        friend std::ostream& operator<<(std::ostream& os, const CSVRow& row);
};

/**
 * @class Collection of CSVRow's. Iterate with begin and end like STL containers.
 */
class CSV : protected std::vector<CSVRow>
{
protected:
        std::vector<std::string> labels;
        size_t columnCount;

public:
        /// Initialize from file. If optLabels not specied, the file is required. Otherwise initialize like second
        /// constructor.
        CSV(const boost::filesystem::path& path, std::initializer_list<std::string> optLabels = {});

        /// Initialize with header.
        CSV(std::initializer_list<std::string> labels);

        /// Default constructor. Mainly used for swig.
        CSV();

        /// iterators
        using std::vector<CSVRow>::begin;
        using std::vector<CSVRow>::end;
        using std::vector<CSVRow>::size;

        /// Amount of columns in the CSV.
        size_t getColumnCount() const { return columnCount; }

        /// Convert label to index for more user friendly and robuust implementation. This level of indirection does
        /// introduce a perfomance tradeoff.
        size_t getIndexForLabel(const std::string& label) const;

        /// Add row of values. Will all be converted to string with StringUtils::ToString
        template <typename... T>
        void addRow(const T&... values);

        /// Add rom of string values.
        void addRow(std::vector<std::string> values);

        /// Add a collection of rows.
        void addRows(std::vector<std::vector<std::string>>& rows);

        /// Write CSV to file.
        void write(const boost::filesystem::path& path) const;

        /// Compare operator.
        bool operator==(const CSV& other) const;

        /// Returns header of CSV.
        std::vector<std::string> getLabels() const { return labels; }
};

/// Declaration of specialization
template <>
std::string CSVRow::getValue<std::string>(size_t index) const;

/// Declaration of specialization
template <>
std::string CSVRow::getValue<std::string>(const std::string& label) const;

template <typename T>
inline T CSVRow::getValue(size_t index) const
{
        return FromString<T>(getValue<std::string>(index));
}

template <typename T>
inline T CSVRow::getValue(const std::string& label) const
{
        return FromString<T>(getValue<std::string>(label));
}

template <typename... T>
inline void CSV::addRow(const T&... values)
{
        addRow({ToString(values)...});
}

} // namespace config
} // namespace stride

#endif /* MAIN_CPP_CONFIG_CSV_H_ */
