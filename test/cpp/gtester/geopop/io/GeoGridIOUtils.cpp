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

#include "GeoGridIOUtils.h"

#include "contact/ContactType.h"
#include "geogrid.pb.h"
#include "geopop/CollegeCenter.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/K12SchoolCenter.h"
#include "geopop/PrimaryCommunityCenter.h"
#include "geopop/SecondaryCommunityCenter.h"
#include "geopop/WorkplaceCenter.h"
#include "geopop/io/GeoGridProtoReader.h"
#include "geopop/io/GeoGridProtoWriter.h"
#include "pop/Population.h"

#include <gtest/gtest.h>
#include <map>

using namespace std;
using namespace geopop;
using namespace stride;
using namespace stride::ContactType;
using namespace util;

map<int, Person*>       persons_found;
map<pair<int, Id>, int> persons_pools;

namespace {

// for internal use only
void compareGeoGrid(const GeoGrid& geoGrid, proto::GeoGrid& protoGrid)
{
        ASSERT_EQ(geoGrid.size(), protoGrid.locations_size());
        for (int idx = 0; idx < protoGrid.locations_size(); idx++) {
                const auto& protoLocation = protoGrid.locations(idx);
                auto        location      = geoGrid.GetById(static_cast<unsigned int>(protoLocation.id()));
                CompareLocation(*location, protoLocation);
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

void CompareContactCenter(const ContactCenter&                         contactCenter,
                          const proto::GeoGrid_Location_ContactCenter& protoContactCenter)
{

        map<Id, proto::GeoGrid_Location_ContactCenter_Type> types = {
            {Id::K12School, proto::GeoGrid_Location_ContactCenter_Type_K12School},
            {Id::PrimaryCommunity, proto::GeoGrid_Location_ContactCenter_Type_PrimaryCommunity},
            {Id::SecondaryCommunity, proto::GeoGrid_Location_ContactCenter_Type_SecondaryCommunity},
            {Id::College, proto::GeoGrid_Location_ContactCenter_Type_College},
            {Id::Household, proto::GeoGrid_Location_ContactCenter_Type_Household},
            {Id::Workplace, proto::GeoGrid_Location_ContactCenter_Type_Workplace}};

        EXPECT_EQ(contactCenter.GetId(), protoContactCenter.id());
        EXPECT_EQ(types[contactCenter.GetContactPoolType()], protoContactCenter.type());
        ASSERT_EQ(protoContactCenter.pools_size(), contactCenter.size());

        // Currently no tests with more than one contactpool
        if (contactCenter.size() == 1) {
                const auto& protoContactPool = protoContactCenter.pools(0);
                auto        contactPool      = contactCenter[0];
                CompareContactPool(contactPool, protoContactPool);
        }
}

void CompareCoordinate(const Coordinate& coordinate, const proto::GeoGrid_Location_Coordinate& protoCoordinate)
{
        using boost::geometry::get;
        EXPECT_EQ(get<0>(coordinate), protoCoordinate.longitude());
        EXPECT_EQ(get<1>(coordinate), protoCoordinate.latitude());
}

void CompareLocation(const Location& location, const proto::GeoGrid_Location& protoLocation)
{
        EXPECT_EQ(location.GetName(), protoLocation.name());
        EXPECT_EQ(location.GetProvince(), protoLocation.province());
        EXPECT_EQ(location.GetPopCount(), protoLocation.population());
        EXPECT_EQ(location.GetPopCount(), protoLocation.population());
        CompareCoordinate(location.GetCoordinate(), protoLocation.coordinate());

        // ASSERT_EQ(protoLocation.contactcenters_size(), location->GetContactCenters().size());

        map<int, ContactCenter*>                        idToCenter;
        map<int, proto::GeoGrid_Location_ContactCenter> idToProtoCenter;

        vector<ContactCenter*> centers;
        for (Id typ : IdList) {
                for (const auto& p : location.CRefCenters(typ)) {
                        centers.emplace_back(p.get());
                }
        }

        for (int idx = 0; idx < protoLocation.contactcenters_size(); idx++) {
                auto protoContactCenter                  = protoLocation.contactcenters(idx);
                auto contactCenter                       = centers[idx];
                idToCenter[contactCenter->GetId()]       = contactCenter;
                idToProtoCenter[protoContactCenter.id()] = move(protoContactCenter);
        }

        for (auto& contactCenterPair : idToCenter) {
                CompareContactCenter(*contactCenterPair.second, idToProtoCenter[contactCenterPair.first]);
        }

        ASSERT_EQ(protoLocation.commutes_size(), location.CRefOutgoingCommutes().size());
        for (int idx = 0; idx < protoLocation.commutes_size(); idx++) {
                const auto& protoCommute = protoLocation.commutes(idx);
                auto        commute_pair = location.CRefOutgoingCommutes()[idx];
                EXPECT_EQ(protoCommute.to(), commute_pair.first->GetID());
                EXPECT_EQ(protoCommute.proportion(), commute_pair.second);
        }
}

void ComparePerson(const proto::GeoGrid_Person& protoPerson)
{
        using namespace ContactType;

        const auto person = persons_found[protoPerson.id()];
        EXPECT_EQ(person->GetAge(), protoPerson.age());
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::College)], person->GetPoolId(Id::College));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::K12School)], person->GetPoolId(Id::K12School));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::Household)], person->GetPoolId(Id::Household));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::Workplace)], person->GetPoolId(Id::Workplace));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::PrimaryCommunity)],
                  person->GetPoolId(Id::PrimaryCommunity));
        EXPECT_EQ(persons_pools[make_pair(protoPerson.id(), Id::SecondaryCommunity)],
                  person->GetPoolId(Id::SecondaryCommunity));
}

void CompareGeoGrid(GeoGrid& geoGrid)
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
        reader.Read();
        compareGeoGrid(pop->RefGeoGrid(), protoGrid);
}

shared_ptr<GeoGrid> GetPopulatedGeoGrid(Population* pop)
{
        const auto geoGrid  = make_shared<GeoGrid>(pop);
        const auto location = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        const auto school = make_shared<K12SchoolCenter>(0);
        location->AddCenter(school);
        const auto schoolPool = new ContactPool(2, Id::K12School);
        school->RegisterPool(schoolPool);

        const auto community = make_shared<PrimaryCommunityCenter>(1);
        location->AddCenter(community);
        const auto communityPool = new ContactPool(3, Id::PrimaryCommunity);
        community->RegisterPool(communityPool);

        const auto secondaryCommunity = make_shared<SecondaryCommunityCenter>(2);
        location->AddCenter(secondaryCommunity);
        const auto secondaryCommunityPool = new ContactPool(7, Id::SecondaryCommunity);
        secondaryCommunity->RegisterPool(secondaryCommunityPool);

        const auto college = make_shared<CollegeCenter>(3);
        location->AddCenter(college);
        const auto collegePool = new ContactPool(4, Id::College);
        college->RegisterPool(collegePool);

        const auto household = make_shared<HouseholdCenter>(4);
        location->AddCenter(household);
        const auto householdPool = new ContactPool(5, Id::Household);
        household->RegisterPool(householdPool);

        const auto workplace = make_shared<WorkplaceCenter>(5);
        location->AddCenter(workplace);
        const auto workplacePool = new ContactPool(6, Id::Workplace);
        workplace->RegisterPool(workplacePool);

        geoGrid->AddLocation(location);
        const auto person = geoGrid->GetPopulation()->CreatePerson(1, 18, 5, 2, 4, 6, 3, 7);
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
        const auto bavikhove = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);
        const auto gent      = make_shared<Location>(2, 4, Coordinate(0, 0), "Gent", 2500);
        const auto mons      = make_shared<Location>(3, 4, Coordinate(0, 0), "Mons", 2500);

        bavikhove->AddOutgoingCommute(gent, 0.5);
        gent->AddIncomingCommute(bavikhove, 0.5);

        bavikhove->AddOutgoingCommute(mons, 0.25);
        mons->AddIncomingCommute(bavikhove, 0.25);

        gent->AddOutgoingCommute(bavikhove, 0.75);
        bavikhove->AddIncomingCommute(gent, 0.75);

        gent->AddOutgoingCommute(mons, 0.5);
        mons->AddIncomingCommute(gent, 0.5);

        geoGrid->AddLocation(bavikhove);
        geoGrid->AddLocation(gent);
        geoGrid->AddLocation(mons);
        return geoGrid;
}
