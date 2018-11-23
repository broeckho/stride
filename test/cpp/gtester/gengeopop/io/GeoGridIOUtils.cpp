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

#include "GeoGridIOUtils.h"
#include <gengeopop/College.h>
#include <gengeopop/Community.h>
#include <gengeopop/GeoGridConfig.h>
#include <gengeopop/K12School.h>
#include <gengeopop/PrimaryCommunity.h>
#include <gengeopop/SecondaryCommunity.h>
#include <gengeopop/Workplace.h>
#include <gengeopop/generators/GeoGridGenerator.h>
#include <gengeopop/io/GeoGridProtoReader.h>
#include <gengeopop/io/GeoGridProtoWriter.h>
#include <gtest/gtest.h>
#include <pool/ContactPoolType.h>

std::map<int, stride::Person*>                             persons_found;
std::map<std::pair<int, stride::ContactPoolType::Id>, int> persons_pools;
using namespace gengeopop;

void CompareContactPool(stride::ContactPool*                                     contactPool,
                        const proto::GeoGrid_Location_ContactCenter_ContactPool& protoContactPool)
{
        ASSERT_EQ(protoContactPool.people_size(), (contactPool->end() - contactPool->begin()));
        for (int idx = 0; idx < protoContactPool.people_size(); idx++) {
                auto            personId = protoContactPool.people(idx);
                stride::Person* person   = contactPool->begin()[idx];
                EXPECT_EQ(person->GetId(), personId);
                persons_found[personId]                                         = person;
                persons_pools[std::make_pair(personId, contactPool->GetType())] = contactPool->GetId();
        }
}

void CompareContactCenter(std::shared_ptr<ContactCenter>               contactCenter,
                          const proto::GeoGrid_Location_ContactCenter& protoContactCenter)
{
        std::map<std::string, proto::GeoGrid_Location_ContactCenter_Type> types = {
            {"K12School", proto::GeoGrid_Location_ContactCenter_Type_K12School},
            {"Community", proto::GeoGrid_Location_ContactCenter_Type_Community},
            {"Primary Community", proto::GeoGrid_Location_ContactCenter_Type_PrimaryCommunity},
            {"Secondary Community", proto::GeoGrid_Location_ContactCenter_Type_SecondaryCommunity},
            {"College", proto::GeoGrid_Location_ContactCenter_Type_College},
            {"Household", proto::GeoGrid_Location_ContactCenter_Type_Household},
            {"Workplace", proto::GeoGrid_Location_ContactCenter_Type_Workplace}};

        EXPECT_EQ(contactCenter->GetId(), protoContactCenter.id());
        EXPECT_EQ(types[contactCenter->GetType()], protoContactCenter.type());
        ASSERT_EQ(protoContactCenter.pools_size(), contactCenter->GetPools().size());

        // Currently no tests with more than one contactpool
        if (contactCenter->GetPools().size() == 1) {
                auto protoContactPool = protoContactCenter.pools(0);
                auto contactPool      = contactCenter->GetPools()[0];
                CompareContactPool(contactPool, protoContactPool);
        }
}

void CompareCoordinate(const Coordinate& coordinate, const proto::GeoGrid_Location_Coordinate& protoCoordinate)
{
        using boost::geometry::get;
        EXPECT_EQ(get<0>(coordinate), protoCoordinate.longitude());
        EXPECT_EQ(get<1>(coordinate), protoCoordinate.latitude());
}

void CompareLocation(std::shared_ptr<Location> location, const proto::GeoGrid_Location& protoLocation)
{
        EXPECT_EQ(location->GetName(), protoLocation.name());
        EXPECT_EQ(location->GetProvince(), protoLocation.province());
        EXPECT_EQ(location->GetPopulation(), protoLocation.population());
        EXPECT_EQ(location->GetPopulation(), protoLocation.population());
        CompareCoordinate(location->GetCoordinate(), protoLocation.coordinate());
        ASSERT_EQ(protoLocation.contactcenters_size(), location->GetContactCenters().size());

        std::map<int, std::shared_ptr<ContactCenter>>        idToCenter;
        std::map<int, proto::GeoGrid_Location_ContactCenter> idToProtoCenter;

        for (int idx = 0; idx < protoLocation.contactcenters_size(); idx++) {
                auto protoContactCenter                  = protoLocation.contactcenters(idx);
                auto contactCenter                       = location->GetContactCenters()[idx];
                idToCenter[contactCenter->GetId()]       = contactCenter;
                idToProtoCenter[protoContactCenter.id()] = std::move(protoContactCenter);
        }
        for (auto& contactCenterPair : idToCenter) {
                CompareContactCenter(contactCenterPair.second, idToProtoCenter[contactCenterPair.first]);
        }

        ASSERT_EQ(protoLocation.commutes_size(), location->GetOutgoingCommuningCities().size());
        for (int idx = 0; idx < protoLocation.commutes_size(); idx++) {
                auto protoCommute = protoLocation.commutes(idx);
                auto commute_pair = location->GetOutgoingCommuningCities()[idx];
                EXPECT_EQ(protoCommute.to(), commute_pair.first->GetID());
                EXPECT_EQ(protoCommute.proportion(), commute_pair.second);
        }
}
void ComparePerson(const proto::GeoGrid_Person& protoPerson)
{
        auto person = persons_found[protoPerson.id()];
        EXPECT_EQ(person->GetAge(), protoPerson.age());
        EXPECT_EQ(std::string(1, person->GetGender()), protoPerson.gender());
        EXPECT_EQ(persons_pools[std::make_pair(protoPerson.id(), stride::ContactPoolType::Id::College)],
                  person->GetCollegeId());
        EXPECT_EQ(persons_pools[std::make_pair(protoPerson.id(), stride::ContactPoolType::Id::K12School)],
                  person->GetK12SchoolId());
        EXPECT_EQ(persons_pools[std::make_pair(protoPerson.id(), stride::ContactPoolType::Id::Household)],
                  person->GetHouseholdId());
        EXPECT_EQ(persons_pools[std::make_pair(protoPerson.id(), stride::ContactPoolType::Id::Work)],
                  person->GetWorkId());
        EXPECT_EQ(persons_pools[std::make_pair(protoPerson.id(), stride::ContactPoolType::Id::PrimaryCommunity)],
                  person->GetPrimaryCommunityId());
        EXPECT_EQ(persons_pools[std::make_pair(protoPerson.id(), stride::ContactPoolType::Id::SecondaryCommunity)],
                  person->GetSecondaryCommunityId());
}

void compareGeoGrid(std::shared_ptr<GeoGrid> geoGrid, proto::GeoGrid& protoGrid)
{
        ASSERT_EQ(geoGrid->size(), protoGrid.locations_size());
        for (int idx = 0; idx < protoGrid.locations_size(); idx++) {
                auto protoLocation = protoGrid.locations(idx);
                auto location      = geoGrid->GetById(protoLocation.id());
                CompareLocation(location, protoLocation);
        }
        ASSERT_EQ(persons_found.size(), protoGrid.persons_size());
        for (int idx = 0; idx < protoGrid.persons_size(); idx++) {
                auto protoPerson = protoGrid.persons(idx);
                ComparePerson(protoPerson);
        }
        persons_found.clear();
        persons_pools.clear();
}

void CompareGeoGrid(std::shared_ptr<GeoGrid> geoGrid)
{
        GeoGridProtoWriter writer;
        std::stringstream  ss;
        writer.Write(geoGrid, ss);
        proto::GeoGrid protoGrid;
        protoGrid.ParseFromIstream(&ss);
        compareGeoGrid(geoGrid, protoGrid);
}

void CompareGeoGrid(proto::GeoGrid& protoGrid)
{
        std::unique_ptr<std::stringstream> ss = std::make_unique<std::stringstream>();
        protoGrid.SerializeToOstream(ss.get());
        std::unique_ptr<std::istream> is(std::move(ss));
        auto                          pop = stride::Population::Create();
        GeoGridProtoReader            reader(std::move(is), pop.get());
        std::shared_ptr<GeoGrid>      geogrid = reader.Read();
        compareGeoGrid(geogrid, protoGrid);
}

std::shared_ptr<GeoGrid> GetGeoGrid(stride::Population* pop)
{
        GeoGridConfig config{};
        config.input.populationSize        = 10000;
        config.calculated.compulsoryPupils = static_cast<unsigned int>(0.20 * 1000);

        GeoGridGenerator geoGridGenerator(config, std::make_shared<GeoGrid>(pop));
        return geoGridGenerator.GetGeoGrid();
}

std::shared_ptr<GeoGrid> GetPopulatedGeoGrid(stride::Population* pop)
{
        auto geoGrid  = GetGeoGrid(pop);
        auto location = std::make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove");

        auto school = std::make_shared<K12School>(0);
        location->AddContactCenter(school);
        auto schoolPool = new stride::ContactPool(2, stride::ContactPoolType::Id::K12School);
        school->AddPool(schoolPool);

        auto community = std::make_shared<PrimaryCommunity>(1);
        location->AddContactCenter(community);
        auto communityPool = new stride::ContactPool(3, stride::ContactPoolType::Id::PrimaryCommunity);
        community->AddPool(communityPool);

        auto secondaryCommunity = std::make_shared<SecondaryCommunity>(2);
        location->AddContactCenter(secondaryCommunity);
        auto secondaryCommunityPool = new stride::ContactPool(7, stride::ContactPoolType::Id::SecondaryCommunity);
        secondaryCommunity->AddPool(secondaryCommunityPool);

        auto college = std::make_shared<College>(3);
        location->AddContactCenter(college);
        auto collegePool = new stride::ContactPool(4, stride::ContactPoolType::Id::College);
        college->AddPool(collegePool);

        auto household = std::make_shared<Household>(4);
        location->AddContactCenter(household);
        auto householdPool = new stride::ContactPool(5, stride::ContactPoolType::Id::Household);
        household->AddPool(householdPool);

        auto workplace = std::make_shared<Workplace>(5);
        location->AddContactCenter(workplace);
        auto workplacePool = new stride::ContactPool(6, stride::ContactPoolType::Id::Work);
        workplace->AddPool(workplacePool);

        geoGrid->AddLocation(location);
        stride::Person* person = geoGrid->CreatePerson(1, 18, 5, 2, 4, 6, 3, 7);
        communityPool->AddMember(person);
        schoolPool->AddMember(person);
        secondaryCommunityPool->AddMember(person);
        collegePool->AddMember(person);
        householdPool->AddMember(person);
        workplacePool->AddMember(person);
        return geoGrid;
}

std::shared_ptr<GeoGrid> GetCommutesGeoGrid(stride::Population* pop)
{
        auto geoGrid   = GetGeoGrid(pop);
        auto bavikhove = std::make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove");
        auto gent      = std::make_shared<Location>(2, 4, 2500, Coordinate(0, 0), "Gent");
        auto mons      = std::make_shared<Location>(3, 4, 2500, Coordinate(0, 0), "Mons");

        bavikhove->AddOutgoingCommutingLocation(gent, 0.5);
        gent->AddIncomingCommutingLocation(bavikhove, 0.5);

        bavikhove->AddOutgoingCommutingLocation(mons, 0.25);
        mons->AddIncomingCommutingLocation(bavikhove, 0.25);

        gent->AddOutgoingCommutingLocation(bavikhove, 0.75);
        bavikhove->AddIncomingCommutingLocation(gent, 0.75);

        gent->AddOutgoingCommutingLocation(mons, 0.5);
        mons->AddIncomingCommutingLocation(gent, 0.5);

        geoGrid->AddLocation(bavikhove);
        geoGrid->AddLocation(gent);
        geoGrid->AddLocation(mons);
        return geoGrid;
}
