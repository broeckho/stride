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

#include <gengeopop/io/HouseholdCSVReader.h>
#include <gtest/gtest.h>
#include <memory>

using namespace gengeopop;

namespace {

std::shared_ptr<Household> createCP(const std::vector<unsigned int>& ages)
{
        auto cp = new stride::ContactPool(0, stride::ContactPoolType::Id::Household);
        for (unsigned int age : ages) {
                //                auto p = std::make_shared<stride::Person>();
                stride::Person* p = new stride::Person();
                p->SetAge(age);
                cp->AddMember(p);
        }

        auto hh = std::make_shared<Household>();
        hh->AddPool(cp);
        return hh;
}

std::vector<std::shared_ptr<Household>> getExpectedHouseHolds()
{
        std::vector<std::shared_ptr<Household>> households;

        households.push_back(createCP({42, 38, 15}));
        households.push_back(createCP({70, 68}));
        households.push_back(createCP({40, 39, 9, 6}));
        households.push_back(createCP({43, 42}));
        households.push_back(createCP({55, 54}));
        households.push_back(createCP({40, 40, 3, 3}));
        households.push_back(createCP({35, 32, 6, 3}));
        households.push_back(createCP({78, 75}));

        return households;
}

TEST(HouseholdCSVReader, test1)
{
        std::string csvString =
            R"(hh_age1,hh_age2,hh_age3,hh_age4,hh_age5,hh_age6,hh_age7,hh_age8,hh_age9,hh_age10,hh_age11,hh_age12
 42,38,15,NA,NA,NA,NA,NA,NA,NA,NA,NA
 70,68,NA,NA,NA,NA,NA,NA,NA,NA,NA,NA
 40,39,9,6,NA,NA,NA,NA,NA,NA,NA,NA
 43,42,NA,NA,NA,NA,NA,NA,NA,NA,NA,NA
 55,54,NA,NA,NA,NA,NA,NA,NA,NA,NA,NA
 40,40,3,3,NA,NA,NA,NA,NA,NA,NA,NA
 35,32,6,3,NA,NA,NA,NA,NA,NA,NA,NA
 78,75,NA,NA,NA,NA,NA,NA,NA,NA,NA,NA
)";

        auto instream = std::make_unique<std::istringstream>(csvString);

        HouseholdCSVReader                             reader(std::move(instream));
        const std::vector<std::shared_ptr<Household>>& HHs         = reader.GetHouseHolds();
        const std::vector<std::shared_ptr<Household>>& expectedHHS = getExpectedHouseHolds();

        EXPECT_EQ(HHs.size(), (unsigned int)8);

        int i = 0;
        for (const auto& hh : expectedHHS) {
                const auto& expectedCP = hh->GetPools()[0];
                const auto& actualCP   = HHs[i]->GetPools()[0];

                auto actualCPI = actualCP->begin();
                for (const auto& person : *expectedCP) {
                        EXPECT_EQ(person->GetAge(), (*actualCPI)->GetAge());
                        actualCPI++;
                }

                i++;
        }
        for (const auto& hh : expectedHHS) {
                for (const auto& person : *hh->GetPools()[0]) {
                        delete person;
                }
        }
}

} // namespace
