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

#include "GeoGridJSONWriter.h"

#include "contact/ContactPool.h"
#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <omp.h>

using json = nlohmann::json;

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace boost::property_tree;

GeoGridJSONWriter::GeoGridJSONWriter() : m_persons_found() {}

void GeoGridJSONWriter::Write(GeoGrid& geoGrid, ostream& stream)
{
        std::cout << "<<<<<<<<<<<< IN WRITE >>>>>>>>>>>>>>" << std::endl;
        json jsonfile = json::object();

        json locations_array = json::array();

        for (const auto& location : geoGrid) {
                json location_json = json::object();
                location_json = WriteLocation(location);
                locations_array.push_back(location_json);
        }

        jsonfile["locations"] = locations_array;

        json persons_array = json::array();

        for (const auto& person : m_persons_found) {
                json person_json = json::object();
                person_json = WritePerson(person);
                persons_array.push_back(person_json);
        }

        jsonfile["persons"] = persons_array;

        m_persons_found.clear();
        stream << jsonfile;
}

json GeoGridJSONWriter::WriteContactCenter(shared_ptr<ContactCenter> contactCenter)
{
        json contactCenter_json = json::object();
        contactCenter_json["id"] = contactCenter->GetId();
        contactCenter_json["class"] = ToString(contactCenter->GetContactPoolType());

        json pools_array = json::array();
        for (const auto& pool : *contactCenter) {
                json temp_pool;
                temp_pool = WriteContactPool(pool);
                pools_array.push_back(temp_pool);
        }

        contactCenter_json["pools"] = pools_array;
        return contactCenter_json;
}

json GeoGridJSONWriter::WriteContactPool(ContactPool* contactPool)
{
        json pool = json::object();
        pool["id"] = contactPool->GetId();

        json people_array = json::array();
        for (auto person : *contactPool) {
                m_persons_found.insert(person);
                people_array.push_back(person->GetId());
        }
        pool["people"] = people_array;

        return pool;
}

json GeoGridJSONWriter::WriteCoordinate(const Coordinate& coordinate)
{
        json coordinate_object = json::object();

        coordinate_object["longitude"] = boost::geometry::get<0>(coordinate);
        coordinate_object["latitude"] = boost::geometry::get<1>(coordinate);

        return coordinate_object;
}

json GeoGridJSONWriter::WriteLocation(shared_ptr<Location> location)
{
        json location_object = json::object();

        location_object["id"] = location->GetID();
        location_object["name"] = location->GetName();
        location_object["province"] = location->GetProvince();
        location_object["population"] = location->GetPopCount();
        location_object["coordinate"] = WriteCoordinate(location->GetCoordinate());


        auto commutes = location->CRefOutgoingCommutes();
        if (!commutes.empty()) {
                json commutes_list = json::array();
                for (auto commute_pair : commutes) {
                        json com_obj = json::object();
                        com_obj["to"] = commute_pair.first->GetID();
                        com_obj["proportion"] = commute_pair.second;
                        commutes_list.push_back(com_obj);
                }
                location_object["commute"] = commutes_list;
        }
        json contactCenters_array = json::array();

        vector<shared_ptr<ContactCenter>> centers;
        for (Id typ : IdList) {
                for (const auto& c : location->RefCenters(typ)) {
                        contactCenters_array.push_back(WriteContactCenter(c));
                }
        }

        location_object["contactCenters"] = contactCenters_array;

        return location_object;
}

json GeoGridJSONWriter::WritePerson(Person* person)
{
        using namespace ContactType;

        json person_json = json::object();

        person_json["id"] = person->GetId();
        person_json["age"] = person->GetAge();
        person_json["k12School"] = person->GetPoolId(Id::K12School);
        person_json["college"] = person->GetPoolId(Id::College);
        person_json["household"] = person->GetPoolId(Id::Household);
        person_json["workplace"] = person->GetPoolId(Id::Workplace);
        person_json["primaryCommunity"] = person->GetPoolId(Id::PrimaryCommunity);
        person_json["secondaryCommunity"] = person->GetPoolId(Id::SecondaryCommunity);

        return person_json;

} // namespace geopop
