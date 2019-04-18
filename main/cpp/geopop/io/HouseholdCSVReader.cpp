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
 *  Copyright 2019, Jan Broeckhove.
 */

#include "HouseholdCSVReader.h"

#include "util/CSV.h"

namespace geopop {

using namespace std;
using namespace stride::util;

HouseholdCSVReader::HouseholdCSVReader(std::unique_ptr<std::istream> inputStream)
    : m_input_stream(std::move(inputStream))
{
}

void HouseholdCSVReader::SetReferenceHouseholds(unsigned int&                           ref_person_count,
                                                std::vector<std::vector<unsigned int>>& ref_ages)
{
        CSV reader(*(m_input_stream.get()));

        unsigned int p_count = 0U;
        for (const CSVRow& row : reader) {

                vector<unsigned int> temp;
                for (unsigned int i = 0; i < 12; i++) {
                        unsigned int age;
                        try {
                                age = row.GetValue<unsigned int>(i);
                        } catch (const std::bad_cast& e) {
                                // NA
                                break;
                        }
                        temp.emplace_back(age);
                }
                p_count += temp.size();
                ref_ages.emplace_back(temp);
        }
        ref_person_count = p_count;
}

} // namespace geopop
