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
 * Source file of csv.
 */

#include "CSV.h"

#include <boost/filesystem/fstream.hpp>

using namespace boost;
using namespace std;

namespace {

/// Checks if there is a file with "filename" relative to the exectution path.
/// @param filename    filename to check.
/// @param root       root of the path.
/// @return the full path to the file if it exists
/// @throws runtime error if file doesn't exist
const filesystem::path check(const filesystem::path& filename,
                             const filesystem::path& root = boost::filesystem::current_path())
{
        const filesystem::path file_path = canonical(complete(filename, root));
        if (!is_regular_file(file_path)) {
                throw runtime_error(string(__func__) + ">File " + file_path.string() + " not present. Aborting.");
        }
        return file_path;
}

} // namespace

namespace stride {
namespace util {

CSV::CSV(const boost::filesystem::path& path) : m_labels(), m_column_count(0)
{
        boost::filesystem::path     full_path = check(path);
        boost::filesystem::ifstream file;
        file.open(full_path.string());
        if (!file.is_open()) {
                throw runtime_error("Error opening csv file: " + full_path.string());
        }
        ReadFromStream(file);
}

CSV::CSV(istream& inputStream) : m_labels(), m_column_count(0) { ReadFromStream(inputStream); }

CSV::CSV(const initializer_list<string>& labels) : m_labels(labels), m_column_count(labels.size()) {}

CSV::CSV(const vector<string>& labels) : m_labels(labels), m_column_count(labels.size()) {}

CSV::CSV(size_t columnCount) : m_labels(), m_column_count(columnCount)
{
        for (unsigned i = 1U; i < columnCount + 1; ++i) {
                m_labels.emplace_back(ToString(i));
        }
}

bool CSV::operator==(const CSV& other) const
{
        return m_labels == other.m_labels && (const vector<CSVRow>&)*this == (const vector<CSVRow>&)other;
}

size_t CSV::GetIndexForLabel(const string& label) const
{
        for (unsigned int index = 0; index < m_labels.size(); ++index) {
                if (m_labels.at(index) == label)
                        return index;
        }
        throw runtime_error("Label: " + label + " not found in CSV");
}

void CSV::ReadFromStream(istream& inputStream)
{
        string line;

        // process header, get labels and columnCount
        getline(inputStream, line);
        line                        = Trim(line);
        vector<string> headerLabels = Split(line, ",");
        for (const string& label : headerLabels) {
                m_labels.push_back(Trim(label, "\""));
        }
        m_column_count = m_labels.size();

        // body
        while (getline(inputStream, line)) {
                line = Trim(line);
                if (!line.empty()) {
                        vector<string> values = Split(line, ",");
                        AddRow(values.begin(), values.end());
                }
        }
}

void CSV::Write(const boost::filesystem::path& path) const
{
        boost::filesystem::ofstream file;
        file.open(path.string());
        if (!file.is_open()) {
                throw runtime_error("Error opening csv file: " + path.string());
        }
        file << *this;
        file.close();
}

void CSV::WriteLabels(boost::filesystem::ofstream& file) const
{
        for (unsigned int i = 0; i < m_labels.size(); ++i) {
                const string& label = m_labels.at(i);
                file << "\"" << label << "\"";
                if (i != m_labels.size() - 1) {
                        file << ",";
                } else {
                        file << endl;
                }
        }
}

void CSV::WriteRows(boost::filesystem::ofstream& file) const
{
        for (const CSVRow& row : *this) {
                file << row << endl;
        }
}

} // namespace util
} // namespace stride
