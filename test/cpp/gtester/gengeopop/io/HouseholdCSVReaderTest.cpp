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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "geopop/io/HouseholdCSVReader.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Household.h"

#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace std;
using namespace geopop;
using namespace stride;

namespace {

shared_ptr<geopop::Household> createCP(const vector<unsigned int>& ages)
{
        auto cp = new ContactPool(0, ContactType::Id::Household);
        for (unsigned int age : ages) {
                auto p = new Person();
                p->SetAge(age);
                cp->AddMember(p);
        }

        auto hh = make_shared<geopop::Household>();
        hh->RegisterPool(cp);
        return hh;
}

vector<shared_ptr<geopop::Household>> getExpectedHouseHolds()
{
        vector<shared_ptr<geopop::Household>> households;

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
        string csvString =
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

        GeoGridConfig      geoConfig{};
        auto               instream = make_unique<istringstream>(csvString);
        HouseholdCSVReader reader(move(instream));

        reader.SetReferenceHouseholds(geoConfig.refHH.households, geoConfig.refHH.persons, geoConfig.refHH.pools);

        const vector<shared_ptr<geopop::Household>>& HHs         = geoConfig.refHH.households;
        const vector<shared_ptr<geopop::Household>>& expectedHHS = getExpectedHouseHolds();

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
