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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "HouseholdCSVReader.h"

#include "contact/ContactPool.h"
#include "geopop/Household.h"
#include "pop/Person.h"
#include "util/CSV.h"

namespace geopop {

using namespace std;
using namespace stride::util;

HouseholdCSVReader::HouseholdCSVReader(std::unique_ptr<std::istream> inputStream)
    : m_input_stream(std::move(inputStream))
{
}

void HouseholdCSVReader::SetReferenceHouseholds(std::vector<std::shared_ptr<Household>>& ref_households,
                                                SegmentedVector<stride::Person>&         ref_persons,
                                                SegmentedVector<stride::ContactPool>&    ref_pools)
{
        CSV reader(*(m_input_stream.get()));

        auto id = 1U;
        for (const CSVRow& row : reader) {
                auto household = std::make_shared<Household>();

                // Create contactpool of the household.
                ref_pools.emplace_back(id++, stride::ContactType::Id::Household);
                stride::ContactPool* newCP = &ref_pools.back();

                for (std::size_t i = 0; i < 12; i++) {
                        unsigned int age;
                        try {
                                age = row.GetValue<unsigned int>(i);
                        } catch (const std::bad_cast& e) {
                                // NA
                                break;
                        }

                        stride::Person p;
                        ref_persons.push_back(p);

                        stride::Person* p_ptr = &ref_persons.back();
                        p_ptr->SetAge(age);
                        newCP->AddMember(p_ptr);
                }
                household->RegisterPool(newCP);
                ref_households.push_back(household);
        }
}

} // namespace geopop
