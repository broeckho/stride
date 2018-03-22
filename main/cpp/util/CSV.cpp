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

namespace stride {
namespace util {

using namespace boost;
using namespace std;

CSV::CSV(const boost::filesystem::path& path, initializer_list<string> optLabels) : columnCount(0)
{
        try {
                boost::filesystem::path     full_path = check(path);
                boost::filesystem::ifstream file;
                file.open(full_path.string());
                if (!file.is_open()) {
                        throw runtime_error("Error opening csv file: " + full_path.string());
                }
                string line;

                // header
                getline(file, line);
                line = Trim(line);
                // Split is bad! There is no option to escape ",".
                vector<string> headerLabels = Split(line, ",");
                for (const string& label : headerLabels) {
                        labels.push_back(Trim(label, "\""));
                }
                columnCount = labels.size();

                // body
                while (getline(file, line)) {
                        line = Trim(line);
                        if (!line.empty()) {
                                // Split is bad! There is no option to escape ",".
                                vector<string> values = Split(line, ",");
                                addRow(values);
                        }
                }
        } catch (runtime_error& error) {
                if (optLabels.size() == 0) {
                        throw error;
                } else {
                        labels      = optLabels;
                        columnCount = labels.size();
                }
        }
}

CSV::CSV(initializer_list<string> labels) : labels(labels), columnCount(labels.size()) {}

void CSV::addRow(vector<string> values)
{
        CSVRow csvRow(this, values);
        this->push_back(csvRow);
}

void CSV::addRows(vector<vector<string>>& rows)
{
        for (const vector<string>& row : rows) {
                addRow(row);
        }
}
const filesystem::path check(const filesystem::path& filename, const filesystem::path& root)
{
        const filesystem::path file_path = canonical(complete(filename, root));
        if (!is_regular_file(file_path)) {
                throw std::runtime_error(std::string(__func__) + ">File " + file_path.string() +
                                         " not present. Aborting.");
        }
        return file_path;
}

size_t CSV::getIndexForLabel(const string& label) const
{
        for (unsigned int index = 0; index < labels.size(); ++index) {
                if (labels.at(index) == label)
                        return index;
        }
        throw runtime_error("Label: " + label + " not found in CSV");
}

void CSV::write(const boost::filesystem::path& path) const
{
        boost::filesystem::ofstream file;
        file.open(path.string());
        if (!file.is_open()) {
                throw runtime_error("Error opening csv file: " + path.string());
        }

        for (unsigned int i = 0; i < labels.size(); ++i) {
                const string& label = labels.at(i);
                file << "\"" << label << "\"";
                if (i != labels.size() - 1) {
                        file << ",";
                } else {
                        file << endl;
                }
        }

        for (const CSVRow& row : *this) {
                file << row << endl;
        }
        file.close();
}

bool CSV::operator==(const CSV& other) const
{
        return labels == other.labels && (const vector<CSVRow>&)*this == (const vector<CSVRow>&)other;
}

} // namespace util
} // namespace stride
