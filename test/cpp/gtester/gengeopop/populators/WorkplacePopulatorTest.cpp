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

#include "createGeogrid.h"
#include <gengeopop/College.h>
#include <gengeopop/GeoGridConfig.h>
#include <gengeopop/K12School.h>
#include <gengeopop/Workplace.h>
#include <gengeopop/populators/CollegePopulator.h>
#include <gengeopop/populators/WorkplacePopulator.h>
#include <gtest/gtest.h>
#include <util/LogUtils.h>
#include <util/RnMan.h>

using namespace gengeopop;
using namespace stride;

namespace {

TEST(WorkplacePopulatorTest, NoPopulation)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());

        geoGrid->AddLocation(std::make_shared<Location>(0, 0, 0));

        WorkplacePopulator workplacePopulator(rnManager, stride::util::LogUtils::CreateNullLogger("nullLogger"));

        GeoGridConfig config{};

        geoGrid->Finalize();

        EXPECT_NO_THROW(workplacePopulator.Apply(geoGrid, config));
}

TEST(WorkplacePopulatorTest, NoActive)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplacePopulator workplacePopulator(rnManager, stride::util::LogUtils::CreateNullLogger("nullLogger"));
        GeoGridConfig      config{};
        config.input.fraction_1865_years_active           = 0;
        config.input.fraction_1826_years_WhichAreStudents = 1;

        auto pop     = stride::Population::Create();
        auto geoGrid = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());

        auto location = *geoGrid->begin();

        // Brasschaat and Schoten are close to each other
        // There is no commuting, but since they will still receive students from each other
        // Kortrijk will only receive students from Kortrijik
        auto brasschaat = *geoGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));

        auto schoten = *(geoGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));

        auto kortrijk = *(geoGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        geoGrid->Finalize();

        workplacePopulator.Apply(geoGrid, config);

        for (const stride::Person& person : *geoGrid->GetPopulation()) {
                EXPECT_EQ(0, person.GetWorkId());
        }
}

TEST(WorkplacePopulatorTest, NoCommuting)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplacePopulator workplacePopulator(rnManager, stride::util::LogUtils::CreateNullLogger("nullLogger"));
        GeoGridConfig      config{};
        config.input.fraction_active_commutingPeople      = 0;
        config.input.fraction_1865_years_active           = 1;
        config.input.fraction_1826_years_WhichAreStudents = 0.5;

        auto pop     = stride::Population::Create();
        auto geoGrid = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());

        // Brasschaat and Schoten are close to each other
        // There is no commuting, but since they will still receive students from each other
        // Kortrijk will only receive students from Kortrijik
        auto brasschaat = *geoGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));

        auto workBra1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workBra1->Fill(geoGrid);
        brasschaat->AddContactCenter(workBra1);

        auto workBra2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workBra2->Fill(geoGrid);
        brasschaat->AddContactCenter(workBra2);

        auto schoten = *(geoGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));

        auto workScho1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workScho1->Fill(geoGrid);
        schoten->AddContactCenter(workScho1);

        auto workScho2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workScho2->Fill(geoGrid);
        schoten->AddContactCenter(workScho2);

        auto kortrijk = *(geoGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        auto workKor1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workKor1->Fill(geoGrid);
        kortrijk->AddContactCenter(workKor1);

        auto workKor2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workKor2->Fill(geoGrid);
        kortrijk->AddContactCenter(workKor2);

        geoGrid->Finalize();

        workplacePopulator.Apply(geoGrid, config);

        // Assert that persons of Schoten only go to Schoten or Brasschaat
        for (const auto& household : schoten->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 325 && person->GetWorkId() <= 328);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 325 && person->GetWorkId() <= 328) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }

        // Assert that persons of Schoten only go to Schoten or Brasschaat
        for (const auto& household : brasschaat->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 325 && person->GetWorkId() <= 328);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 325 && person->GetWorkId() <= 328) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Kortijk
        for (const auto& household : kortrijk->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 329 && person->GetWorkId() <= 330);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 329 && person->GetWorkId() <= 330) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }
}

TEST(WorkplacePopulatorTest, OnlyCommuting)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplacePopulator workplacePopulator(rnManager, stride::util::LogUtils::CreateNullLogger("nullLogger"));
        GeoGridConfig      config{};
        config.input.fraction_active_commutingPeople      = 0;
        config.input.fraction_active_commutingPeople      = 1;
        config.input.fraction_student_commutingPeople     = 0;
        config.calculated.popcount_1865_and_years_active          = 1;
        config.input.fraction_1865_years_active           = 1;
        config.input.fraction_1826_years_WhichAreStudents = 0.5;

        auto pop     = stride::Population::Create();
        auto geoGrid = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());

        // only commuting
        auto schoten = *(geoGrid->begin());
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));

        auto workScho1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workScho1->Fill(geoGrid);
        schoten->AddContactCenter(workScho1);

        auto workScho2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workScho2->Fill(geoGrid);
        schoten->AddContactCenter(workScho2);

        auto kortrijk = *(geoGrid->begin() + 1);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        auto workKor1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workKor1->Fill(geoGrid);
        kortrijk->AddContactCenter(workKor1);

        auto workKor2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workKor2->Fill(geoGrid);
        kortrijk->AddContactCenter(workKor2);

        schoten->AddOutgoingCommutingLocation(kortrijk, 0.5);
        kortrijk->AddIncomingCommutingLocation(schoten, 0.5);
        kortrijk->AddOutgoingCommutingLocation(schoten, 0.5);
        schoten->AddIncomingCommutingLocation(kortrijk, 0.5);

        geoGrid->Finalize();

        workplacePopulator.Apply(geoGrid, config);

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& household : schoten->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 327 && person->GetWorkId() <= 328);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 327 && person->GetWorkId() <= 328) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Schoten
        for (const auto& household : kortrijk->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 325 && person->GetWorkId() <= 326);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 325 && person->GetWorkId() <= 326) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }
}

TEST(WorkplacePopulatorTest, OnlyCommutingButNoCommutingAvaiable)
{
        stride::util::RnMan::Info rnInfo;
        rnInfo.m_seed_seq_init = "1,2,3,4";
        stride::util::RnMan rnManager(rnInfo);

        WorkplacePopulator workplacePopulator(rnManager, stride::util::LogUtils::CreateNullLogger("nullLogger"));
        GeoGridConfig      config{};
        config.input.fraction_active_commutingPeople      = 0;
        config.input.fraction_active_commutingPeople      = 1;
        config.input.fraction_student_commutingPeople     = 0;
        config.calculated.popcount_1865_and_years_active          = 1;
        config.input.fraction_1865_years_active           = 1;
        config.input.fraction_1826_years_WhichAreStudents = 0.5;

        auto pop     = stride::Population::Create();
        auto geoGrid = CreateGeoGrid(3, 100, 3, 33, 3, pop.get());

        auto brasschaat = *geoGrid->begin();
        brasschaat->SetCoordinate(Coordinate(51.29227, 4.49419));

        auto workBra1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workBra1->Fill(geoGrid);
        brasschaat->AddContactCenter(workBra1);

        auto workBra2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workBra2->Fill(geoGrid);
        brasschaat->AddContactCenter(workBra2);

        auto schoten = *(geoGrid->begin() + 1);
        schoten->SetCoordinate(Coordinate(51.2497532, 4.4977063));

        auto workScho1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workScho1->Fill(geoGrid);
        schoten->AddContactCenter(workScho1);

        auto workScho2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workScho2->Fill(geoGrid);
        schoten->AddContactCenter(workScho2);

        auto kortrijk = *(geoGrid->begin() + 2);
        kortrijk->SetCoordinate(Coordinate(50.82900246, 3.264406009));

        auto workKor1 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workKor1->Fill(geoGrid);
        kortrijk->AddContactCenter(workKor1);

        auto workKor2 = std::make_shared<Workplace>(config.generated.contactCenters++);
        workKor2->Fill(geoGrid);
        kortrijk->AddContactCenter(workKor2);

        // test case is only commuting but between nobody is commuting from or to Brasschaat
        schoten->AddOutgoingCommutingLocation(kortrijk, 0.5);
        kortrijk->AddIncomingCommutingLocation(schoten, 0.5);

        kortrijk->AddOutgoingCommutingLocation(schoten, 0.5);
        schoten->AddIncomingCommutingLocation(kortrijk, 0.5);

        geoGrid->Finalize();

        workplacePopulator.Apply(geoGrid, config);

        // Assert that persons of Schoten only go to Kortrijk
        for (const auto& household : schoten->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 329 && person->GetWorkId() <= 330);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 329 && person->GetWorkId() <= 330) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }

        // Assert that persons of Brasschaat only go to Brasschaat or Schoten
        for (const auto& household : brasschaat->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 325 && person->GetWorkId() <= 328);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 325 && person->GetWorkId() <= 328) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }

        // Assert that persons of Kortrijk only go to Schoten
        for (const auto& household : kortrijk->GetContactCentersOfType<Household>()) {
                for (auto person : *household->GetPools()[0]) {
                        if (person->IsWorkableCandidate() && !person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE(person->GetWorkId() >= 327 && person->GetWorkId() <= 328);
                        } else if (person->IsCollegeStudentCandidate()) {
                                EXPECT_TRUE((person->GetWorkId() >= 327 && person->GetWorkId() <= 328) ||
                                            person->GetWorkId() == 0);
                        } else {
                                EXPECT_EQ(0, person->GetWorkId());
                        }
                }
        }
}

} // namespace
