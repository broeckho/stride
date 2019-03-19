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

#include "geopop/io/ReaderFactory.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/io/CommutesCSVReader.h"
#include "geopop/io/HouseholdCSVReader.h"
#include "geopop/io/LocationsCSVReader.h"
#include "pop/Population.h"
#include "util/FileSys.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace util;

namespace {

TEST(ReaderFactoryTest, TestCommutes)
{
        ReaderFactory readerFactory;

        const shared_ptr<CommutesReader>& res1 = ReaderFactory::CreateCommutesReader(string("flanders_cities.csv"));

        EXPECT_NE(dynamic_pointer_cast<CommutesCSVReader>(res1), nullptr);
        EXPECT_THROW(readerFactory.CreateCommutesReader(FileSys::GetTestsDir() / "testdata/io/empty.txt"),
                     runtime_error);
}

TEST(ReaderFactoryTest, TestCommutesFromFile)
{
        const shared_ptr<CommutesReader>& res2 =
            ReaderFactory::CreateCommutesReader(FileSys::GetTestsDir() / "testdata/io/commutes.csv");

        auto  pop     = Population::Create();
        auto& geoGrid = pop->RefGeoGrid();
        geoGrid.AddLocation(make_shared<Location>(21, 0, Coordinate(0.0, 0.0), "", 1000));
        geoGrid.AddLocation(make_shared<Location>(22, 0, Coordinate(0.0, 0.0), "", 1000));

        res2->FillGeoGrid(geoGrid);

        EXPECT_EQ(geoGrid.GetById(21)->GetIncomingCommuteCount(1.0), 500);
        EXPECT_EQ(geoGrid.GetById(22)->GetOutgoingCommuteCount(1.0), 500);
        EXPECT_EQ(geoGrid.GetById(21)->GetIncomingCommuteCount(1.0), 500);
        EXPECT_EQ(geoGrid.GetById(22)->GetOutgoingCommuteCount(1.0), 500);
}

TEST(ReaderFactoryTest, TestCities)
{
        const shared_ptr<LocationsReader>& res1 = ReaderFactory::CreateCitiesReader(string("flanders_cities.csv"));

        EXPECT_NE(dynamic_pointer_cast<LocationsCSVReader>(res1), nullptr);

        EXPECT_THROW(ReaderFactory::CreateCitiesReader(FileSys::GetTestsDir() / "testdata/io/empty.txt"),
                     runtime_error);
        EXPECT_THROW(ReaderFactory::CreateCitiesReader(FileSys::GetTestsDir() / "testdata/io/random.txt"),
                     runtime_error);
}

TEST(ReaderFactoryTest, TestHouseHolds)
{
        const shared_ptr<HouseholdReader>& res1 = ReaderFactory::CreateHouseholdReader(string("flanders_cities.csv"));

        EXPECT_NE(dynamic_pointer_cast<HouseholdCSVReader>(res1), nullptr);

        EXPECT_THROW(ReaderFactory::CreateHouseholdReader(FileSys::GetTestsDir() / "testdata/io/empty.txt"),
                     runtime_error);
}

} // namespace
