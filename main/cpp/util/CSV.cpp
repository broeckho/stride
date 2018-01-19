/**
 * @file
 * Source file of csv.
 */

#include "CSV.h"

#include <boost/filesystem/fstream.hpp>

#include "util/File.h"
#include "util/Misc.h"

namespace stride {
namespace config {
using namespace stride::util;

CSVRow::CSVRow(const CSV* parent, const std::vector<std::string>& values) : parent(parent)
{
        if (values.size() != parent->getColumnCount()) {
                throw std::runtime_error("Tried adding row with " + ToString(values.size()) + " value(s) to CSV with " +
                                         ToString(parent->getColumnCount()) + " columns.");
        }
        for (const std::string& value : values) {
                this->values.push_back(Trim(value, "\""));
        }
}

/// specialization for string
template <>
std::string CSVRow::getValue<std::string>(size_t index) const
{
        if (index >= parent->getColumnCount()) {
                throw std::runtime_error("Index out of range for CSV: " + ToString(index));
        }
        return values.at(index);
}

/// specialization for string
template <>
std::string CSVRow::getValue<std::string>(const std::string& label) const
{
        size_t index = parent->getIndexForLabel(label);
        return getValue(index);
}

bool CSVRow::operator==(const CSVRow& other) const { return values == other.values; }

std::ostream& operator<<(std::ostream& os, const CSVRow& row)
{
        for (unsigned int i = 0; i < row.values.size(); ++i) {
                const std::string& value = row.values.at(i);
                if (!util::IsFloat(value)) {
                        os << "\"";
                }
                os << value;
                if (!util::IsFloat(value)) {
                        os << "\"";
                }
                if (i != row.values.size() - 1) {
                        os << ",";
                }
        }
        return os;
}

CSV::CSV(const boost::filesystem::path& path, std::initializer_list<std::string> optLabels) : columnCount(0)
{
        try {
                boost::filesystem::path full_path = util::checkFile(path);
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
                        labels = optLabels;
                        columnCount = labels.size();
                }
        }
}

CSV::CSV(std::initializer_list<std::string> labels) : labels(labels), columnCount(labels.size()) {}

CSV::CSV() {}

void CSV::addRow(std::vector<std::string> values)
{
        CSVRow csvRow(this, values);
        this->push_back(csvRow);
}

void stride::config::CSV::addRows(std::vector<std::vector<std::string>>& rows)
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

void stride::config::CSV::write(const boost::filesystem::path& path) const
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

} // namespace config
} // namespace stride
