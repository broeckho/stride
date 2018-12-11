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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#include "HouseholdCSVReader.h"

#include "util/CSV.h"

gengeopop::HouseholdCSVReader::HouseholdCSVReader(std::unique_ptr<std::istream> inputStream)
    : m_persons(), m_contactPools()
{
        stride::util::CSV reader(*(inputStream.get()));

        unsigned int id = 1;

        for (const stride::util::CSVRow& row : reader) {
                std::shared_ptr<gengeopop::Household> household = std::make_shared<gengeopop::Household>();

                // Create contactpool of the household
                m_contactPools.emplace_back(id++, stride::ContactPoolType::Id::Household);
                stride::ContactPool* newCP = &m_contactPools.back();

                for (std::size_t i = 0; i < 12; i++) {
                        unsigned int age;
                        try {
                                age = row.GetValue<unsigned int>(i);
                        } catch (const std::bad_cast& e) {
                                // NA
                                break;
                        }

                        if (age < 18 && age >= 6) {
                                m_totalCompulsory++;
                        }

                        if (age >= 18 && age < 26) {
                                m_total1826Years++;
                        }

                        if (age >= 18 && age < 65) {
                                m_total1865Years++;
                        }

                        m_total++;

                        stride::Person p;
                        m_persons.push_back(p);

                        stride::Person* p_ptr = &m_persons.back();
                        p_ptr->SetAge(age);
                        newCP->AddMember(p_ptr);
                }
                household->AddPool(newCP);
                m_households.push_back(household);
        }
}
