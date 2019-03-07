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

#include "geopop/io/CommutesCSVReader.h"
#include "geopop/GeoGrid.h"
#include "geopop/Location.h"
#include "geopop/io/HouseholdCSVReader.h"
#include "pop/Population.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace geopop;
using namespace stride;

namespace {

shared_ptr<GeoGrid> getExpectedGeoGrid()
{
        auto                pop     = Population::Create();
        shared_ptr<GeoGrid> geoGrid = make_shared<GeoGrid>(pop.get());
        geoGrid->AddLocation(make_shared<Location>(21, 0, 1000));
        geoGrid->AddLocation(make_shared<Location>(22, 0, 800));
        geoGrid->AddLocation(make_shared<Location>(23, 0, 900));
        geoGrid->AddLocation(make_shared<Location>(24, 0, 1300));

        // to 21
        geoGrid->GetById(21)->AddIncomingCommutingLocation(geoGrid->GetById(22), 0.15012305168170631);
        geoGrid->GetById(22)->AddOutgoingCommutingLocation(geoGrid->GetById(21), 0.15012305168170631);

        geoGrid->GetById(21)->AddIncomingCommutingLocation(geoGrid->GetById(23), 0.59115044247787607);
        geoGrid->GetById(23)->AddOutgoingCommutingLocation(geoGrid->GetById(21), 0.59115044247787607);

        geoGrid->GetById(21)->AddIncomingCommutingLocation(geoGrid->GetById(24), 0.37610619469026546);
        geoGrid->GetById(24)->AddOutgoingCommutingLocation(geoGrid->GetById(21), 0.37610619469026546);

        // to 22
        geoGrid->GetById(22)->AddIncomingCommutingLocation(geoGrid->GetById(21), 0.11969439728353141);
        geoGrid->GetById(21)->AddOutgoingCommutingLocation(geoGrid->GetById(22), 0.11969439728353141);

        geoGrid->GetById(22)->AddIncomingCommutingLocation(geoGrid->GetById(24), 0.62389380530973448);
        geoGrid->GetById(24)->AddOutgoingCommutingLocation(geoGrid->GetById(22), 0.62389380530973448);

        // to 23
        geoGrid->GetById(23)->AddIncomingCommutingLocation(geoGrid->GetById(21), 0.41341256366723261);
        geoGrid->GetById(21)->AddOutgoingCommutingLocation(geoGrid->GetById(23), 0.41341256366723261);

        geoGrid->GetById(23)->AddIncomingCommutingLocation(geoGrid->GetById(22), 0.28712059064807222);
        geoGrid->GetById(22)->AddOutgoingCommutingLocation(geoGrid->GetById(23), 0.28712059064807222);

        // to 24
        geoGrid->GetById(24)->AddIncomingCommutingLocation(geoGrid->GetById(22), 0.2506152584085316);
        geoGrid->GetById(22)->AddOutgoingCommutingLocation(geoGrid->GetById(24), 0.2506152584085316);

        return geoGrid;
}

TEST(CommutesCSVReaderTest, test1)
{
        string csvString = "id_21,id_22,id_23,id_24\n"
                           "550,366,668,425\n" // to 21
                           "141,761,0,705\n"   // to 22
                           "487,700,462,0\n"   // to 23
                           "0,611,0,0\n";      // to 24

        const auto expectedGeoGrid = getExpectedGeoGrid();
        const auto pop             = Population::Create();
        const auto geoGrid         = make_shared<GeoGrid>(pop.get());
        geoGrid->AddLocation(make_shared<Location>(21, 0, 1000));
        geoGrid->AddLocation(make_shared<Location>(22, 0, 800));
        geoGrid->AddLocation(make_shared<Location>(23, 0, 900));
        geoGrid->AddLocation(make_shared<Location>(24, 0, 1300));
        auto instream = make_unique<istringstream>(csvString);

        CommutesCSVReader reader(move(instream));
        reader.FillGeoGrid(geoGrid);

        for (const auto& loc : *geoGrid) {
                int         i                = 0;
                const auto& expectedLoc      = expectedGeoGrid->GetById(loc->GetID());
                const auto& outGoingExpected = expectedLoc->GetOutgoingCommutingCities();
                for (const auto& commute : loc->GetOutgoingCommutingCities()) {
                        EXPECT_DOUBLE_EQ(outGoingExpected[i].first->GetID(), commute.first->GetID());
                        EXPECT_DOUBLE_EQ(outGoingExpected[i].second, commute.second);
                        i++;
                }
                i                            = 0;
                const auto& incomingExpected = expectedLoc->GetIncomingCommuningCities();
                for (const auto& commute : loc->GetIncomingCommuningCities()) {
                        EXPECT_DOUBLE_EQ(incomingExpected[i].first->GetID(), commute.first->GetID());
                        EXPECT_DOUBLE_EQ(incomingExpected[i].second, commute.second);
                        i++;
                }

                EXPECT_DOUBLE_EQ(expectedLoc->GetOutgoingCommuterCount(1), loc->GetOutgoingCommuterCount(1));
                EXPECT_DOUBLE_EQ(expectedLoc->GetIncomingCommuterCount(1), loc->GetIncomingCommuterCount(1));
        }
}

} // namespace
