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
 * Source file of CSVRow.
 */

#include "CSVRow.h"

#include "CSV.h"
#include "util/Misc.h"

namespace stride {
namespace util {

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

} // namespace util
} // namespace stride
