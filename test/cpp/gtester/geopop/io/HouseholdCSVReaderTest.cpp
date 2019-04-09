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

#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace std;
using namespace geopop;
using namespace stride;

namespace {

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

        reader.SetReferenceHouseholds(geoConfig.refHH.person_count, geoConfig.refHH.ages);

        EXPECT_EQ(geoConfig.refHH.person_count, 23U);

        const vector<vector<unsigned int>>& HHages = geoConfig.refHH.ages;

        EXPECT_EQ(HHages.size(), 8U);
        EXPECT_EQ(HHages[0].size(), 3U);
        EXPECT_EQ(HHages[1].size(), 2U);
        EXPECT_EQ(HHages[2].size(), 4U);
        EXPECT_EQ(HHages[3].size(), 2U);
        EXPECT_EQ(HHages[4].size(), 2U);
        EXPECT_EQ(HHages[5].size(), 4U);
        EXPECT_EQ(HHages[6].size(), 4U);
        EXPECT_EQ(HHages[7].size(), 2U);

        EXPECT_EQ(HHages[0][0], 42U);
        EXPECT_EQ(HHages[0][1], 38U);
        EXPECT_EQ(HHages[0][2], 15U);

        EXPECT_EQ(HHages[1][0], 70U);
        EXPECT_EQ(HHages[1][1], 68U);

        EXPECT_EQ(HHages[6][0], 35U);
        EXPECT_EQ(HHages[6][1], 32U);
        EXPECT_EQ(HHages[6][2], 6U);
        EXPECT_EQ(HHages[6][3], 3U);

        EXPECT_EQ(HHages[7][0], 78U);
        EXPECT_EQ(HHages[7][1], 75U);
}

} // namespace
