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

#include <gengeopop/College.h>
#include <gengeopop/generators/CollegeGenerator.h>
#include <gengeopop/generators/K12SchoolGenerator.h>
#include <gtest/gtest.h>
#include <util/RnMan.h>

#include "../../createlogger.h"

using namespace gengeopop;

namespace {

TEST(CollegeGeneratorTest, OneLocationTest)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        CollegeGenerator collegeGenerator(rnManager, CreateLogger());
        GeoGridConfig    config{};
        config.input.populationSize               = 45000;
        config.calculated._1826_years_and_student = 9000;

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());
        auto loc1    = std::make_shared<Location>(1, 4, 45000, Coordinate(0, 0), "Antwerpen");

        geoGrid->AddLocation(loc1);

        collegeGenerator.Apply(geoGrid, config);

        const auto& centersOfLoc1 = loc1->GetContactCenters();
        EXPECT_EQ(centersOfLoc1.size(), 3);
}

TEST(CollegeGeneratorTest, ZeroLocationTest)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        CollegeGenerator collegeGenerator(rnManager, CreateLogger());
        GeoGridConfig    config{};
        config.input.populationSize               = 10000;
        config.calculated._1826_years_and_student = 2000;

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());
        collegeGenerator.Apply(geoGrid, config);

        EXPECT_EQ(geoGrid->size(), 0);
}

TEST(CollegeGeneratorTest, FiveLocationsTest)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        CollegeGenerator collegeGenerator(rnManager, CreateLogger());
        GeoGridConfig    config{};
        config.input.populationSize               = 399992;
        config.calculated._1826_years_and_student = 79998;

        auto             pop     = stride::Population::Create();
        auto             geoGrid = std::make_shared<GeoGrid>(pop.get());
        std::vector<int> sizes{28559, 33319, 39323, 37755, 35050, 10060, 13468, 8384,
                               9033,  31426, 33860, 4110,  50412, 25098, 40135};
        for (int size : sizes) {
                geoGrid->AddLocation(
                    std::make_shared<Location>(1, 4, size, Coordinate(0, 0), "Size: " + std::to_string(size)));
        }
        collegeGenerator.Apply(geoGrid, config);

        std::vector<int> expectedSchoolCount{2, 2, 5, 2, 3, 0, 0, 0, 0, 2, 2, 0, 3, 3, 3};
        for (size_t i = 0; i < sizes.size(); i++) {
                EXPECT_EQ(expectedSchoolCount[i], geoGrid->Get(i)->GetContactCenters().size());
        }
}

} // namespace
