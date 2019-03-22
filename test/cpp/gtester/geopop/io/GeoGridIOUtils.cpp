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

#include "geogrid.pb.h"
#include "geopop/GeoGridConfig.h"
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

void CompareContactPool(ContactPool*                                            contactPool,
                        const proto::GeoGrid_Location_ContactPools_ContactPool& protoContactPool)
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

void CompareContactPools(Id typeId, const stride::util::SegmentedVector<stride::ContactPool*>& contactPools,
                         const proto::GeoGrid_Location_ContactPools& protoContactPools)
{
        static const map<Id, proto::GeoGrid_Location_ContactPools_Type> types = {
            {Id::K12School, proto::GeoGrid_Location_ContactPools_Type_K12School},
            {Id::PrimaryCommunity, proto::GeoGrid_Location_ContactPools_Type_PrimaryCommunity},
            {Id::SecondaryCommunity, proto::GeoGrid_Location_ContactPools_Type_SecondaryCommunity},
            {Id::College, proto::GeoGrid_Location_ContactPools_Type_College},
            {Id::Household, proto::GeoGrid_Location_ContactPools_Type_Household},
            {Id::Workplace, proto::GeoGrid_Location_ContactPools_Type_Workplace}};

        EXPECT_EQ(types.at(typeId), protoContactPools.type());
        ASSERT_EQ(protoContactPools.pools_size(), contactPools.size());

        // Currently no tests with more than one contactpool
        if (contactPools.size() == 1) {
                const auto& protoContactPool = protoContactPools.pools(0);
                auto        contactPool      = contactPools[0];
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

        static const map<proto::GeoGrid_Location_ContactPools_Type, Id> types = {
            {proto::GeoGrid_Location_ContactPools_Type_K12School, Id::K12School},
            {proto::GeoGrid_Location_ContactPools_Type_PrimaryCommunity, Id::PrimaryCommunity},
            {proto::GeoGrid_Location_ContactPools_Type_SecondaryCommunity, Id::SecondaryCommunity},
            {proto::GeoGrid_Location_ContactPools_Type_College, Id::College},
            {proto::GeoGrid_Location_ContactPools_Type_Household, Id::Household},
            {proto::GeoGrid_Location_ContactPools_Type_Workplace, Id::Workplace}};

        for (int idx = 0; idx < protoLocation.contactpools_size(); idx++) {
                const auto& protoContactPools     = protoLocation.contactpools(idx);
                auto        protoContactPoolsType = protoContactPools.type();
                auto        typeId                = types.at(protoContactPoolsType);
                CompareContactPools(typeId, location.CRefPools(typeId), protoContactPools);
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
        const auto geoGrid = make_shared<GeoGrid>(pop);
        const auto loc     = make_shared<Location>(1, 4, Coordinate(0, 0), "Bavikhove", 2500);

        auto k12Pool = pop->RefPoolSys().CreateContactPool(Id::K12School);
        loc->RefPools(Id::K12School).emplace_back(k12Pool);
        auto pcPool = pop->RefPoolSys().CreateContactPool(Id::PrimaryCommunity);
        loc->RefPools(Id::PrimaryCommunity).emplace_back(pcPool);
        auto scPool = pop->RefPoolSys().CreateContactPool(Id::SecondaryCommunity);
        loc->RefPools(Id::SecondaryCommunity).emplace_back(scPool);
        auto cPool = pop->RefPoolSys().CreateContactPool(Id::College);
        loc->RefPools(Id::College).emplace_back(cPool);
        auto hPool = pop->RefPoolSys().CreateContactPool(Id::Household);
        loc->RefPools(Id::Household).emplace_back(hPool);
        auto wPool = pop->RefPoolSys().CreateContactPool(Id::Workplace);
        loc->RefPools(Id::Workplace).emplace_back(wPool);

        geoGrid->AddLocation(loc);
        const auto person = geoGrid->GetPopulation()->CreatePerson(
            0, 18, hPool->GetId(), k12Pool->GetId(), cPool->GetId(), wPool->GetId(), pcPool->GetId(), scPool->GetId());
        k12Pool->AddMember(person);
        pcPool->AddMember(person);
        scPool->AddMember(person);
        cPool->AddMember(person);
        hPool->AddMember(person);
        wPool->AddMember(person);
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
