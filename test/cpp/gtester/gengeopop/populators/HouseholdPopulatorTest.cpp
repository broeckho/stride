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

#include <gengeopop/Household.h>
#include <gengeopop/populators/HouseholdPopulator.h>
#include <gtest/gtest.h>
#include <util/LogUtils.h>
#include <util/RnMan.h>

using namespace gengeopop;

class HouseholdPopulatorTest : public testing::Test
{
public:
        HouseholdPopulatorTest() : householdPopulator(), rnManager(), config() {}

protected:
        virtual void SetUp()
        {
                stride::util::RnMan::Info rnInfo;
                rnInfo.m_seed_seq_init = "1,2,3,4";
                rnManager              = std::make_shared<stride::util::RnMan>(rnInfo);
                std::shared_ptr<spdlog::logger> logger =
                    stride::util::LogUtils::CreateCliLogger("stride_logger", "stride_log.txt");
                logger->set_level(spdlog::level::off);
                householdPopulator = std::make_shared<HouseholdPopulator>(*rnManager.get(), logger);
        }

        std::shared_ptr<HouseholdPopulator>  householdPopulator;
        std::shared_ptr<stride::util::RnMan> rnManager;
        GeoGridConfig                        config;
};

TEST_F(HouseholdPopulatorTest, OneHouseholdTest)
{
        auto householdType = std::make_shared<Household>();
        auto poolType      = new stride::ContactPool(0, stride::ContactPoolType::Id::Household);
        auto personType    = std::make_shared<stride::Person>();
        personType->SetAge(18);
        poolType->AddMember(personType.get());
        householdType->AddPool(poolType);
        config.generated.household_types.push_back(householdType);

        auto pop       = stride::Population::Create();
        auto geoGrid   = std::make_shared<GeoGrid>(pop.get());
        auto loc1      = std::make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Antwerpen");
        auto household = std::make_shared<Household>();
        household->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household);
        geoGrid->AddLocation(loc1);

        householdPopulator->Apply(geoGrid, config);

        const auto& pools = household->GetPools();
        ASSERT_EQ(pools.size(), 1);
        EXPECT_EQ(pools[0]->GetSize(), 1);
}

TEST_F(HouseholdPopulatorTest, ZeroHouseholdsTest)
{
        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());

        EXPECT_NO_THROW(householdPopulator->Apply(geoGrid, config));
}

TEST_F(HouseholdPopulatorTest, FiveHouseholdsTest)
{
        auto householdType = std::make_shared<Household>();
        auto poolType      = new stride::ContactPool(0, stride::ContactPoolType::Id::Household);
        auto personType    = std::make_shared<stride::Person>();
        personType->SetAge(18);
        poolType->AddMember(personType.get());
        householdType->AddPool(poolType);
        config.generated.household_types.push_back(householdType);

        auto pop     = stride::Population::Create();
        auto geoGrid = std::make_shared<GeoGrid>(pop.get());
        auto loc1    = std::make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Antwerpen");

        auto household1 = std::make_shared<Household>();
        household1->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household1);
        auto household2 = std::make_shared<Household>();
        household2->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household2);
        auto household3 = std::make_shared<Household>();
        household3->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household3);
        auto household4 = std::make_shared<Household>();
        household4->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household4);
        auto household5 = std::make_shared<Household>();
        household5->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household5);

        geoGrid->AddLocation(loc1);

        householdPopulator->Apply(geoGrid, config);

        for (auto household : *loc1) {
                ASSERT_EQ(household->GetPools().size(), 1);
                ASSERT_EQ(household->GetPools()[0]->GetSize(), 1);
                EXPECT_EQ((*household->GetPools()[0]->begin())->GetAge(), 18);
        }
}
TEST_F(HouseholdPopulatorTest, MultipleHouseholdTypesTest)
{
        std::shared_ptr<stride::Person> personType1;
        std::shared_ptr<stride::Person> personType2;
        std::shared_ptr<stride::Person> personType3;

        {
                auto householdType = std::make_shared<Household>();
                auto poolType      = new stride::ContactPool(0, stride::ContactPoolType::Id::Household);
                personType1        = std::make_shared<stride::Person>();
                personType1->SetAge(18);
                poolType->AddMember(personType1.get());
                householdType->AddPool(poolType);
                config.generated.household_types.push_back(householdType);
        }
        {
                auto householdType = std::make_shared<Household>();
                auto poolType      = new stride::ContactPool(0, stride::ContactPoolType::Id::Household);
                personType2        = std::make_shared<stride::Person>();
                personType2->SetAge(12);
                poolType->AddMember(personType2.get());
                householdType->AddPool(poolType);
                personType3 = std::make_shared<stride::Person>();
                personType3->SetAge(56);
                poolType->AddMember(personType3.get());
                config.generated.household_types.push_back(householdType);
        }

        auto pop       = stride::Population::Create();
        auto geoGrid   = std::make_shared<GeoGrid>(pop.get());
        auto loc1      = std::make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Antwerpen");
        auto household = std::make_shared<Household>();
        household->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household);
        geoGrid->AddLocation(loc1);
        auto household2 = std::make_shared<Household>();
        household2->AddPool(new stride::ContactPool(0, stride::ContactPoolType::Id::Household));
        loc1->AddContactCenter(household2);
        householdPopulator->Apply(geoGrid, config);

        std::map<int, std::vector<stride::ContactPool*>> pools_map;
        pools_map[household->GetPools()[0]->GetSize()]  = household->GetPools();
        pools_map[household2->GetPools()[0]->GetSize()] = household2->GetPools();
        {
                const auto& pools = pools_map[1];
                ASSERT_EQ(pools.size(), 1);
                EXPECT_EQ(pools[0]->GetSize(), 1);
                EXPECT_EQ((*pools[0]->begin())->GetAge(), 18);
        }
        {
                const auto& pools = pools_map[2];
                ASSERT_EQ(pools.size(), 1);
                EXPECT_EQ(pools[0]->GetSize(), 2);
                EXPECT_EQ((*pools[0]->begin())->GetAge(), 12);
                EXPECT_EQ((*(pools[0]->begin() + 1))->GetAge(), 56);
        }
}
