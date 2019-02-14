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

#include "GeoGridIOUtils.h"

#include "gengeopop/College.h"
#include "gengeopop/Community.h"
#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/Household.h"
#include "gengeopop/K12School.h"
#include "gengeopop/PrimaryCommunity.h"
#include "gengeopop/SecondaryCommunity.h"
#include "gengeopop/Workplace.h"
#include "gengeopop/io/GeoGridProtoReader.h"
#include "gengeopop/io/GeoGridProtoWriter.h"
#include "pool/ContactPoolType.h"

#include <gtest/gtest.h>
#include <map>

using namespace std;
using namespace gengeopop;
using namespace stride;
using namespace stride::ContactPoolType;
using namespace util;

map<int, Person*>       persons_found;
map<pair<int, Id>, int> persons_pools;

namespace {

// for internal use only
void compareGeoGrid(const shared_ptr<GeoGrid>& geoGrid, proto::GeoGrid& protoGrid)
{
        ASSERT_EQ(geoGrid->size(), protoGrid.locations_size());
        for (int idx = 0; idx < protoGrid.locations_size(); idx++) {
                const auto& protoLocation = protoGrid.locations(idx);
                auto        location      = geoGrid->GetById(static_cast<unsigned int>(protoLocation.id()));
                CompareLocation(location, protoLocation);
        }
        ASSERT_EQ(persons_found.size(), protoGrid.persons_size());
        for (int idx = 0; idx < protoGrid.persons_size(); idx++) {
                const auto& protoPerson = protoGrid.persons(idx);
                ComparePerson(protoPerson);
        }
        persons_found.clear();
        persons_pools.clear();
}

} // namespace

void CompareContactPool(ContactPool*                                             contactPool,
                        const proto::GeoGrid_Location_ContactCenter_ContactPool& protoContactPool)
{
        ASSERT_EQ(protoContactPool.people_size(), (contactPool->end() - contactPool->begin()));
        for (int idx = 0; idx < protoContactPool.people_size(); idx++) {
                auto    personId = protoContactPool.people(idx);
                Person* person   = contactPool->begin()[idx];
                EXPECT_EQ(person->GetId(), personId);
                persons_found[personId]                                    = person;
                persons_pools[make_pair(personId, contactPool->GetType())] = static_cast<int>(contactPool->GetId());
        }
}

void CompareContactCenter(shared_ptr<ContactCenter>                    contactCenter,
                          const proto::GeoGrid_Location_ContactCenter& protoContactCenter)
{
        map<string, proto::GeoGrid_Location_ContactCenter_Type> types = {
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
                const auto& protoContactPool = protoContactCenter.pools(0);
                auto        contactPool      = contactCenter->GetPools()[0];
                CompareContactPool(contactPool, protoContactPool);
        }
}

void CompareCoordinate(const Coordinate& coordinate, const proto::GeoGrid_Location_Coordinate& protoCoordinate)
{
        using boost::geometry::get;
        EXPECT_EQ(get<0>(coordinate), protoCoordinate.longitude());
        EXPECT_EQ(get<1>(coordinate), protoCoordinate.latitude());
}

void CompareLocation(shared_ptr<Location> location, const proto::GeoGrid_Location& protoLocation)
{
        EXPECT_EQ(location->GetName(), protoLocation.name());
        EXPECT_EQ(location->GetProvince(), protoLocation.province());
        EXPECT_EQ(location->GetPopCount(), protoLocation.population());
        EXPECT_EQ(location->GetPopCount(), protoLocation.population());
        CompareCoordinate(location->GetCoordinate(), protoLocation.coordinate());
        ASSERT_EQ(protoLocation.contactcenters_size(), location->GetContactCenters().size());

        map<int, shared_ptr<ContactCenter>>             idToCenter;
        map<int, proto::GeoGrid_Location_ContactCenter> idToProtoCenter;

        for (int idx = 0; idx < protoLocation.contactcenters_size(); idx++) {
                auto protoContactCenter                  = protoLocation.contactcenters(idx);
                auto contactCenter                       = location->GetContactCenters()[idx];
                idToCenter[contactCenter->GetId()]       = contactCenter;
                idToProtoCenter[protoContactCenter.id()] = move(protoContactCenter);
        }
        for (auto& contactCenterPair : idToCenter) {
                CompareContactCenter(contactCenterPair.second, idToProtoCenter[contactCenterPair.first]);
        }

        ASSERT_EQ(protoLocation.commutes_size(), location->GetOutgoingCommutingCities().size());
        for (int idx = 0; idx < protoLocation.commutes_size(); idx++) {
                const auto& protoCommute = protoLocation.commutes(idx);
                auto        commute_pair = location->GetOutgoingCommutingCities()[idx];
                EXPECT_EQ(protoCommute.to(), commute_pair.first->GetID());
                EXPECT_EQ(protoCommute.proportion(), commute_pair.second);
        }
}

void ComparePerson(const proto::GeoGrid_Person& protoPerson)
{
        using namespace ContactPoolType;

        const auto person = persons_found[protoPerson.id()];
        EXPECT_EQ(person->GetAge(), protoPerson.age());
        EXPECT_EQ(string(1, person->GetGender()), protoPerson.gender());
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::College)], person->GetPoolId(Id::College));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::K12School)], person->GetPoolId(Id::K12School));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::Household)], person->GetPoolId(Id::Household));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::Work)], person->GetPoolId(Id::Work));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::PrimaryCommunity)],
                  person->GetPoolId(Id::PrimaryCommunity));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::SecondaryCommunity)],
                  person->GetPoolId(Id::SecondaryCommunity));
}

void CompareGeoGrid(shared_ptr<GeoGrid> geoGrid)
{
        GeoGridProtoWriter writer;
        stringstream       ss;
        writer.Write(geoGrid, ss);
        proto::GeoGrid protoGrid;
        protoGrid.ParseFromIstream(&ss);
        compareGeoGrid(geoGrid, protoGrid);
}

void CompareGeoGrid(proto::GeoGrid& protoGrid)
{
        auto ss = make_unique<stringstream>();
        protoGrid.SerializeToOstream(ss.get());
        unique_ptr<istream> is(move(ss));
        const auto          pop = Population::Create();
        GeoGridProtoReader  reader(move(is), pop.get());
        const auto          geogrid = reader.Read();
        compareGeoGrid(geogrid, protoGrid);
}

shared_ptr<GeoGrid> GetPopulatedGeoGrid(Population* pop)
{
        const auto geoGrid  = make_shared<GeoGrid>(pop);
        const auto location = make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove");

        const auto school = make_shared<K12School>(0);
        location->AddContactCenter(school);
        const auto schoolPool = new ContactPool(2, Id::K12School);
        school->AddPool(schoolPool);

        const auto community = make_shared<PrimaryCommunity>(1);
        location->AddContactCenter(community);
        const auto communityPool = new ContactPool(3, Id::PrimaryCommunity);
        community->AddPool(communityPool);

        const auto secondaryCommunity = make_shared<SecondaryCommunity>(2);
        location->AddContactCenter(secondaryCommunity);
        const auto secondaryCommunityPool = new ContactPool(7, Id::SecondaryCommunity);
        secondaryCommunity->AddPool(secondaryCommunityPool);

        const auto college = make_shared<College>(3);
        location->AddContactCenter(college);
        const auto collegePool = new ContactPool(4, Id::College);
        college->AddPool(collegePool);

        const auto household = make_shared<Household>(4);
        location->AddContactCenter(household);
        const auto householdPool = new ContactPool(5, Id::Household);
        household->AddPool(householdPool);

        const auto workplace = make_shared<Workplace>(5);
        location->AddContactCenter(workplace);
        const auto workplacePool = new ContactPool(6, Id::Work);
        workplace->AddPool(workplacePool);

        geoGrid->AddLocation(location);
        const auto person = geoGrid->CreatePerson(1, 18, 5, 2, 4, 6, 3, 7);
        communityPool->AddMember(person);
        schoolPool->AddMember(person);
        secondaryCommunityPool->AddMember(person);
        collegePool->AddMember(person);
        householdPool->AddMember(person);
        workplacePool->AddMember(person);
        return geoGrid;
}

shared_ptr<GeoGrid> GetCommutesGeoGrid(Population* pop)
{
        const auto geoGrid   = make_shared<GeoGrid>(pop);
        const auto bavikhove = make_shared<Location>(1, 4, 2500, Coordinate(0, 0), "Bavikhove");
        const auto gent      = make_shared<Location>(2, 4, 2500, Coordinate(0, 0), "Gent");
        const auto mons      = make_shared<Location>(3, 4, 2500, Coordinate(0, 0), "Mons");

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
