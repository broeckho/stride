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
 *  Copyright 2018, 2019Jan Broeckhove and Bistromatics group.
 */

#include "GeoGridProtoReader.h"

#include "geogrid.pb.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"
#include "pop/Population.h"

#include <iostream>
#include <stdexcept>

namespace geopop {

using namespace std;
using namespace stride::ContactType;

GeoGridProtoReader::GeoGridProtoReader(unique_ptr<istream> inputStream, stride::Population* pop)
    : GeoGridReader(move(inputStream), pop)
{
}

void GeoGridProtoReader::Read()
{
        proto::GeoGrid protoGrid;
        if (!protoGrid.ParseFromIstream(m_inputStream.get())) {
                throw runtime_error("Failed to parse Proto file");
        }
        auto& geoGrid = m_population->RefGeoGrid();

        for (int idx = 0; idx < protoGrid.persons_size(); idx++) {
                const proto::GeoGrid_Person& protoPerson = protoGrid.persons(idx);
                const auto                   person      = ParsePerson(protoPerson);
                m_people[person->GetId()]                = person;
        }

        for (int idx = 0; idx < protoGrid.locations_size(); idx++) {
                const proto::GeoGrid_Location& protoLocation = protoGrid.locations(idx);
                auto                           loc           = ParseLocation(protoLocation);
                geoGrid.AddLocation(move(loc));
        }

        AddCommutes(geoGrid);
        m_people.clear();
        m_commutes.clear();
}

void GeoGridProtoReader::ParseContactPools(shared_ptr<Location>                        loc,
                                           const proto::GeoGrid_Location_ContactPools& protoContactPools)
{
        const auto protoType = protoContactPools.type();

        static const map<proto::GeoGrid_Location_ContactPools_Type, Id> types = {
            {proto::GeoGrid_Location_ContactPools_Type_K12School, Id::K12School},
            {proto::GeoGrid_Location_ContactPools_Type_PrimaryCommunity, Id::PrimaryCommunity},
            {proto::GeoGrid_Location_ContactPools_Type_SecondaryCommunity, Id::SecondaryCommunity},
            {proto::GeoGrid_Location_ContactPools_Type_College, Id::College},
            {proto::GeoGrid_Location_ContactPools_Type_Household, Id::Household},
            {proto::GeoGrid_Location_ContactPools_Type_Workplace, Id::Workplace}};

        const auto typeId = types.at(protoType);

        for (int idx = 0; idx < protoContactPools.pools_size(); idx++) {
                const proto::GeoGrid_Location_ContactPools_ContactPool& protoContactPool = protoContactPools.pools(idx);
                ParseContactPool(loc, protoContactPool, typeId);
        }
}

Coordinate GeoGridProtoReader::ParseCoordinate(const proto::GeoGrid_Location_Coordinate& protoCoordinate)
{
        return {protoCoordinate.longitude(), protoCoordinate.latitude()};
}

void GeoGridProtoReader::ParseContactPool(shared_ptr<Location>                                    loc,
                                          const proto::GeoGrid_Location_ContactPools_ContactPool& protoContactPool,
                                          Id                                                      type)
{
        // Don't use the id of the ContactPool but the let the Population create an id
        auto result = m_population->RefPoolSys().CreateContactPool(type);
        loc->RefPools(type).emplace_back(result);

        for (int idx = 0; idx < protoContactPool.people_size(); idx++) {
                const auto person_id = static_cast<unsigned int>(protoContactPool.people(idx));
                const auto person    = m_people.at(person_id);
                result->AddMember(person);
                // Update original pool id with new pool id used in the population
                person->SetPoolId(type, static_cast<unsigned int>(result->GetId()));
        }
}

shared_ptr<Location> GeoGridProtoReader::ParseLocation(const proto::GeoGrid_Location& protoLocation)
{
        const auto  id         = protoLocation.id();
        const auto& name       = protoLocation.name();
        const auto  province   = protoLocation.province();
        const auto  population = protoLocation.population();
        const auto& coordinate = ParseCoordinate(protoLocation.coordinate());

        auto loc = make_shared<Location>(id, province, coordinate, name, population);

        for (int idx = 0; idx < protoLocation.contactpools_size(); idx++) {
                const proto::GeoGrid_Location_ContactPools& protoPools = protoLocation.contactpools(idx);
                ParseContactPools(loc, protoPools);
        }

        for (int idx = 0; idx < protoLocation.commutes_size(); idx++) {
                const proto::GeoGrid_Location_Commute& commute = protoLocation.commutes(idx);
                m_commutes.emplace_back(make_tuple(id, commute.to(), commute.proportion()));
        }

        return loc;
}

stride::Person* GeoGridProtoReader::ParsePerson(const proto::GeoGrid_Person& protoPerson)
{
        const auto id = static_cast<unsigned int>(protoPerson.id());
        return m_population->CreatePerson(id, protoPerson.age(), 0, 0, 0, 0, 0, 0);
}

} // namespace geopop
