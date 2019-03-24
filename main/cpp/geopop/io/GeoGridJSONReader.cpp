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

#include "GeoGridJSONReader.h"

#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <memory>

using json = nlohmann::json;

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace stride::util;

GeoGridJSONReader::GeoGridJSONReader(unique_ptr<istream> inputStream, Population* pop)
    : GeoGridReader(move(inputStream), pop)
{
}

void GeoGridJSONReader::Read()
{
        json json_file;

        try {
                json_file = json::parse(*m_inputStream);

        } catch (runtime_error&) {
                throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        }

        auto& geoGrid = m_population->RefGeoGrid();
        auto people = json_file["persons"];

        for (auto it = people.begin(); it != people.end(); it++) {
                auto person = ParsePerson(*it);
                m_people[person->GetId()] = person;
        }

        auto locations = json_file["locations"];

        for (auto it = locations.begin(); it != locations.end(); it++) {
                shared_ptr<Location> loc;
                loc = ParseLocation(*it);
                geoGrid.AddLocation(move(loc));
        }

        AddCommutes(geoGrid);
        m_commutes.clear();
        m_people.clear();

}

shared_ptr<Location> GeoGridJSONReader::ParseLocation(json& location)
{
        const auto id         = location["id"].get<unsigned int>();
        const auto name       = location["name"].get<string>();
        const auto province   = location["province"].get<unsigned int>();
        const auto population = location["population"].get<unsigned int>();
        const auto coordinate = ParseCoordinate(location["coordinate"]);

        auto result         = make_shared<Location>(id, province, coordinate, name, population);
        auto contactCenters = location["contactCenters"];
        for (auto it = contactCenters.begin(); it != contactCenters.end(); it++) {
                ParseContactPools(result, *it);
        }

        if (location.find("commutes") != location.end()) {
                auto commutes = location["commutes"];
                for (auto it = commutes.begin(); it != commutes.end(); it++) {
                        json temp_commute = *it;
                        const auto to     = temp_commute["to"].get<unsigned int>();
                        const auto amount = temp_commute["proportion"].get<double>();
                        m_commutes.emplace_back(id, to, amount);
                }
        }

        return result;
}

Coordinate GeoGridJSONReader::ParseCoordinate(json& coordinate)
{

        const auto longitude = coordinate["longitude"].get<double>();
        const auto latitude  = coordinate["latitude"].get<double>();

        return {longitude, latitude};
}

void GeoGridJSONReader::ParseContactPools(std::shared_ptr<geopop::Location> loc, nlohmann::json &contactCenter) {
        const auto type = contactCenter["class"].get<string>();

        ContactType::Id typeId;

        if (type == ToString(Id::K12School)) {
                typeId = Id::K12School;
        } else if (type == ToString(Id::College)) {
                typeId = Id::College;
        } else if (type == ToString(Id::Household)) {
                typeId = Id::Household;
        } else if (type == ToString(Id::PrimaryCommunity)) {
                typeId = Id::PrimaryCommunity;
        } else if (type == ToString(Id::SecondaryCommunity)) {
                typeId = Id::SecondaryCommunity;
        } else if (type == ToString(Id::Workplace)) {
                typeId = Id::Workplace;
        } else {
                throw Exception("No such ContactPool type: " + type);
        }

        auto contactPools = contactCenter["pools"];

        for(auto it = contactPools.begin(); it != contactPools.end(); it++){
                ParseContactPool(loc, *it, typeId);
        }
}

void GeoGridJSONReader::ParseContactPool(std::shared_ptr<Location> loc, json& contactPool, ContactType::Id typeId)
{

        // Don't use the id of the ContactPool but the let the Population create an id.
        auto result = m_population->RefPoolSys().CreateContactPool(typeId);
        loc->RefPools(typeId).emplace_back(result);
        auto people = contactPool["people"];

        for (auto it = people.begin(); it != people.end(); it++) {
                auto person_id = *it;
                result->AddMember(m_people[person_id]);
                m_people[person_id]->SetPoolId(typeId, static_cast<unsigned int>(result->GetId()));
        }
}

Person* GeoGridJSONReader::ParsePerson(json& person)
{

        const auto id = person["id"].get<unsigned int>();
        const auto age = person["age"].get<double>();
        const auto hhId = person["household"].get<unsigned int>();
        const auto ksId = person["k12School"].get<unsigned int>();
        const auto coId = person["college"].get<unsigned int>();
        const auto wpId = person["workplace"].get<unsigned int>();
        const auto pcId = person["primaryCommunity"].get<unsigned int>();
        const auto scId = person["secondaryCommunity"].get<unsigned int>();


        return m_population->CreatePerson(id, age, hhId, ksId, coId, wpId, pcId, scId);
}

} // namespace geopop
