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

#include "util/FileUtils.h"
#include "util/Misc.h"

#include <boost/filesystem/fstream.hpp>

namespace stride {
namespace util {

CSV::CSV(const boost::filesystem::path& path, std::initializer_list<std::string> optLabels) : columnCount(0)
{
        try {
                boost::filesystem::path     full_path = util::checkFile(path);
                boost::filesystem::ifstream file;
                file.open(full_path.string());
                if (!file.is_open()) {
                        throw std::runtime_error("Error opening csv file: " + full_path.string());
                }

                std::string line;

                // header
                getline(file, line);
                line = Trim(line);
                std::vector<std::string> headerLabels =
                    Split(line, ","); // Split is bad! There is no option to escape ",".
                for (const std::string& label : headerLabels) {
                        labels.push_back(Trim(label, "\""));
                }
                columnCount = labels.size();

                // body
                while (getline(file, line)) {
                        line = Trim(line);
                        if (!line.empty()) {
                                std::vector<std::string> values =
                                    Split(line, ","); // Split is bad! There is no option to escape ",".
                                addRow(values);
                        }
                }
        } catch (std::runtime_error& error) {
                // thrown by util::checkFile
                if (optLabels.size() == 0) {
                        throw error;
                } else {
                        labels      = optLabels;
                        columnCount = labels.size();
                }
        }
}

CSV::CSV(std::initializer_list<std::string> labels) : labels(labels), columnCount(labels.size()) {}

void CSV::addRow(std::vector<std::string> values)
{
        CSVRow csvRow(this, values);
        this->push_back(csvRow);
}

void stride::util::CSV::addRows(std::vector<std::vector<std::string>>& rows)
{
        for (const std::vector<std::string>& row : rows) {
                addRow(row);
        }
}

size_t CSV::getIndexForLabel(const std::string& label) const
{
        for (unsigned int index = 0; index < labels.size(); ++index) {
                if (labels.at(index) == label)
                        return index;
        }
        throw std::runtime_error("Label: " + label + " not found in CSV");
}

void stride::util::CSV::write(const boost::filesystem::path& path) const
{
        boost::filesystem::ofstream file;
        file.open(path.string());
        if (!file.is_open()) {
                throw std::runtime_error("Error opening csv file: " + path.string());
        }

        for (unsigned int i = 0; i < labels.size(); ++i) {
                const std::string& label = labels.at(i);
                file << "\"" << label << "\"";
                if (i != labels.size() - 1) {
                        file << ",";
                } else {
                        file << std::endl;
                }
        }

        for (const CSVRow& row : *this) {
                file << row << std::endl;
        }
        file.close();
}

bool CSV::operator==(const CSV& other) const
{
        return labels == other.labels && (const std::vector<CSVRow>&)*this == (const std::vector<CSVRow>&)other;
}

} // namespace util
} // namespace stride
