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

#include <gengeopop/io/CitiesCSVReader.h>
#include <gengeopop/io/CommutesCSVReader.h>
#include <gengeopop/io/HouseholdCSVReader.h>
#include <gengeopop/io/ReaderFactory.h>
#include <gtest/gtest.h>
#include <memory>
#include <util/FileSys.h>

using namespace gengeopop;

namespace {

TEST(ReaderFactoryTest, TestCommutes)
{
        ReaderFactory readerFactory;

        const std::shared_ptr<CommutesReader>& res1 =
            readerFactory.CreateCommutesReader(std::string("flanders_cities.csv"));

        EXPECT_NE(std::dynamic_pointer_cast<CommutesCSVReader>(res1), nullptr);
        EXPECT_THROW(readerFactory.CreateCommutesReader(stride::util::FileSys::GetTestsDir() / "testdata/io/empty.txt"),
                     std::runtime_error);
}

TEST(ReaderFactoryTest, TestCommutesFromFile)
{
        ReaderFactory readerFactory;

        const std::shared_ptr<CommutesReader>& res2 =
            readerFactory.CreateCommutesReader(stride::util::FileSys::GetTestsDir() / "testdata/io/commutes.csv");

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());
        geoGrid->AddLocation(std::make_shared<Location>(21, 0, 1000));
        geoGrid->AddLocation(std::make_shared<Location>(22, 0, 1000));

        res2->FillGeoGrid(geoGrid);

        EXPECT_EQ(geoGrid->GetById(21)->IncomingCommutingPeople(1.0), 500);
        EXPECT_EQ(geoGrid->GetById(22)->OutGoingCommutingPeople(1.0), 500);
        EXPECT_EQ(geoGrid->GetById(21)->IncomingCommutingPeople(1.0), 500);
        EXPECT_EQ(geoGrid->GetById(22)->OutGoingCommutingPeople(1.0), 500);
}

TEST(ReaderFactoryTest, TestCities)
{
        ReaderFactory readerFactory;

        const std::shared_ptr<CitiesReader>& res1 =
            readerFactory.CreateCitiesReader(std::string("flanders_cities.csv"));

        EXPECT_NE(std::dynamic_pointer_cast<CitiesCSVReader>(res1), nullptr);

        EXPECT_THROW(readerFactory.CreateCitiesReader(stride::util::FileSys::GetTestsDir() / "testdata/io/empty.txt"),
                     std::runtime_error);
        EXPECT_THROW(readerFactory.CreateCitiesReader(stride::util::FileSys::GetTestsDir() / "testdata/io/random.txt"),
                     std::runtime_error);
}

TEST(ReaderFactoryTest, TestHouseHolds)
{
        ReaderFactory readerFactory;

        const std::shared_ptr<HouseholdReader>& res1 =
            readerFactory.CreateHouseholdReader(std::string("flanders_cities.csv"));

        EXPECT_NE(std::dynamic_pointer_cast<HouseholdCSVReader>(res1), nullptr);

        EXPECT_THROW(
            readerFactory.CreateHouseholdReader(stride::util::FileSys::GetTestsDir() / "testdata/io/empty.txt"),
            std::runtime_error);
}

} // namespace
