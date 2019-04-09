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

#include <fstream>

using namespace std;

namespace {

/// Checks if there is a file with "filename" relative to the execution path.
/// @param filename filename to check.
/// @param root root of the path.
/// @return the full path to the file if it exists
/// @throws runtime error if file doesn't exist
const filesys::path check(const filesys::path& filename, const filesys::path& root = filesys::current_path())
{
        const filesys::path file_path = canonical(absolute(root / filename));
        if (!is_regular_file(file_path)) {
                throw runtime_error(string(__func__) + ">File " + file_path.string() + " not present. Aborting.");
        }
        return file_path;
}

} // namespace

namespace stride {
namespace util {

CSV::CSV(const filesys::path& path, std::initializer_list<std::string> optLabels) : m_labels(), m_column_count(0)
{
        try {
                filesys::path full_path = check(path);
                std::ifstream file;
                file.open(full_path.string());
                if (!file.is_open()) {
                        throw runtime_error("Error opening csv file: " + full_path.string());
                }

                ReadFromStream(file);
        } catch (std::runtime_error& error) {
                // thrown by util::checkFile
                if (optLabels.size() == 0) {
                        throw error;
                } else {
                        m_labels       = optLabels;
                        m_column_count = m_labels.size();
                }
        }
}

CSV::CSV(std::istream& inputStream) : m_labels(), m_column_count(0) { ReadFromStream(inputStream); }

CSV::CSV(const vector<string>& labels) : m_labels(labels), m_column_count(labels.size()) {}

CSV::CSV(size_t columnCount) : m_labels(), m_column_count(columnCount)
{
        for (unsigned i = 1U; i < columnCount + 1; ++i) {
                m_labels.emplace_back(ToString(i));
        }
}

void CSV::AddRows(const vector<vector<string>>& rows)
{
        for (const vector<string>& row : rows) {
                AddRow(row);
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

void CSV::Write(const filesys::path& path) const
{
        std::ofstream file;
        file.open(path.string());
        if (!file.is_open()) {
                throw runtime_error("Error opening csv file: " + path.string());
        }
        file << *this;
        file.close();
}

void CSV::WriteLabels(std::ofstream& file) const
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

void CSV::WriteRows(std::ofstream& file) const
{
        for (const CSVRow& row : *this) {
                file << row << endl;
        }
}

void CSV::ReadFromStream(std::istream& inputStream)
{
        std::string line;

        // header
        getline(inputStream, line);
        line                                  = Trim(line);
        std::vector<std::string> headerLabels = Split(line, ","); // Split is bad! There is no option to escape ",".
        for (const std::string& label : headerLabels) {
                m_labels.push_back(Trim(label, "\""));
        }
        m_column_count = m_labels.size();

        // body
        while (getline(inputStream, line)) {
                line = Trim(line);
                if (!line.empty()) {
                        std::vector<std::string> values =
                            Split(line, ","); // Split is bad! There is no option to escape ",".
                        AddRow(values);
                }
        }
}

void CSV::AddRow(const vector<string>& values)
{
        CSVRow csvRow(this, values);
        this->push_back(csvRow);
}

const std::vector<std::string>& CSV::GetLabels() const { return m_labels; }

} // namespace util
} // namespace stride
