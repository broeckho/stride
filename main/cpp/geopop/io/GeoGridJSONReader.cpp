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

#include "ThreadException.h"
#include "geopop/GeoGrid.h"
#include "geopop/ContactCenter.h"
#include "pop/Population.h"
#include "util/Exception.h"

#include <memory>
#include <omp.h>

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
        std::cout << "<<<<<<<<<<< In GeoGridJSONReader >>>>>>>>>>>>" << std::endl;
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
        std::cout << "<<<<<<<<<<< Out GeoGridJSONReader >>>>>>>>>>>>" << std::endl;

}

shared_ptr<Location> GeoGridJSONReader::ParseLocation(json& location)
{
        std::cout << "<<<<<<<<<<< In ParseLocation  >>>>>>>>>>>>" << std::endl;

        const auto id         = location["id"].get<unsigned int>();
        const auto name       = location["name"].get<string>();
        const auto province   = location["province"].get<unsigned int>();
        const auto population = location["population"].get<unsigned int>();
        const auto coordinate = ParseCoordinate(location["coordinate"]);

        auto result         = make_shared<Location>(id, province, coordinate, name, population);
        auto contactCenters = location["contactCenters"];
        auto e              = make_shared<ThreadException>();
        {
                for (auto it = contactCenters.begin(); it != contactCenters.end(); it++) {
                        shared_ptr<ContactCenter> center;
                        {
                                e->Run([&it, this, &center] { center = ParseContactCenter(*it); });
                                if (!e->HasError())
                                        result->AddCenter(center);
                        }
                }
        }
        e->Rethrow();

        if (location.find("commutes") != location.end()) {
                auto commutes = location["commutes"];
                for (auto it = commutes.begin(); it != commutes.end(); it++) {
                        json temp_commute = *it;
                        const auto to     = temp_commute["to"].get<unsigned int>();
                        const auto amount = temp_commute["proportion"].get<double>();
                        m_commutes.emplace_back(id, to, amount);
                }
        }
        std::cout << "<<<<<<<<<<< Out ParseLocation  >>>>>>>>>>>>" << std::endl;

        return result;
}

Coordinate GeoGridJSONReader::ParseCoordinate(json& coordinate)
{
        std::cout << "<<<<<<<<<<< In ParseCoordinate >>>>>>>>>>>>" << std::endl;

        const auto longitude = coordinate["longitude"].get<double>();
        const auto latitude  = coordinate["latitude"].get<double>();
        std::cout << "<<<<<<<<<<< Out ParseCoordinate >>>>>>>>>>>>" << std::endl;

        return {longitude, latitude};
}

shared_ptr<ContactCenter> GeoGridJSONReader::ParseContactCenter(json& contactCenter)
{
        std::cout << "<<<<<<<<<<< In ParseContactCenter >>>>>>>>>>>>" << std::endl;

        const auto type = contactCenter["class"].get<string>();
        const auto id   = contactCenter["id"].get<unsigned int>();

        ContactType::Id           typeId;

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
                throw Exception("No such ContactCenter type: " + type);
        }

        auto result = make_shared<ContactCenter>(id, typeId);

        auto contactPools = contactCenter["pools"];
        for (auto it = contactPools.begin(); it != contactPools.end(); it++) {
                const auto pool = ParseContactPool(*it, typeId);
                result->RegisterPool(pool);
        }
        std::cout << "<<<<<<<<<<< Out ParseContactCenter >>>>>>>>>>>>" << std::endl;

        return result;
}

ContactPool* GeoGridJSONReader::ParseContactPool(json& contactPool, ContactType::Id typeId)
{
        std::cout << "<<<<<<<<<<< In ParseContactPool >>>>>>>>>>>>" << std::endl;

        // Don't use the id of the ContactPool but the let the Population create an id.
        auto result = m_population->RefPoolSys().CreateContactPool(typeId);
        auto people = contactPool["people"];

        for (auto it = people.begin(); it != people.end(); it++) {
                auto person_id = *it;
                if (m_people.count(person_id) == 0) {
                        throw Exception("No such person: " );//+ to_string(person_id));
                }
                result->AddMember(m_people[person_id]);
        }
        std::cout << "<<<<<<<<<<< Out ParseContactPool >>>>>>>>>>>>" << std::endl;

        return result;
}

Person* GeoGridJSONReader::ParsePerson(json& person)
{
        std::cout << "<<<<<<<<<<< In ParsePerson >>>>>>>>>>>>" << std::endl;

        const auto id = person["id"].get<unsigned int>();
        const auto age = person["age"].get<double>();
        const auto hhId = person["household"].get<unsigned int>();
        const auto ksId = person["k12School"].get<unsigned int>();
        const auto coId = person["college"].get<unsigned int>();
        const auto wpId = person["workplace"].get<unsigned int>();
        const auto pcId = person["primaryCommunity"].get<unsigned int>();
        const auto scId = person["secondaryCommunity"].get<unsigned int>();
        std::cout << "<<<<<<<<<<< Out ParsePerson >>>>>>>>>>>>" << std::endl;

        return m_population->CreatePerson(id, age, hhId, ksId, coId, wpId, pcId, scId);
}

} // namespace geopop
