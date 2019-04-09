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

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace boost::property_tree;

GeoGridJSONWriter::GeoGridJSONWriter() : m_persons_found() {}

void GeoGridJSONWriter::Write(GeoGrid& geoGrid, ostream& stream)
{
        ptree root;
        ptree locations;
        ptree persons;

        for (const auto& location : geoGrid) {
                pair<string, ptree> child;
                child = make_pair("", WriteLocation(*location));
                locations.push_back(move(child));
        }
        root.add_child("locations", locations);

        for (const auto& person : m_persons_found) {
                pair<string, ptree> child;
                child = make_pair("", WritePerson(person));
                persons.push_back(move(child));
        }
        root.add_child("persons", persons);

        m_persons_found.clear();
        write_json(stream, root);
}

ptree GeoGridJSONWriter::WriteContactCenter(shared_ptr<ContactCenter> contactCenter)
{
        ptree contactCenter_root;
        contactCenter_root.put("id", contactCenter->GetId());
        contactCenter_root.put("type", ToString(contactCenter->GetContactPoolType()));
        ptree pools;

        for (const auto& pool : *contactCenter) {
                pair<string, ptree> child;
                child = make_pair("", WriteContactPool(pool));
                pools.push_back(move(child));
        }

        contactCenter_root.add_child("pools", pools);
        return contactCenter_root;
}

ptree GeoGridJSONWriter::WriteContactPool(ContactPool* contactPool)
{
        ptree contactPool_root;
        contactPool_root.put("id", contactPool->GetId());
        ptree people;
        for (auto person : *contactPool) {
                ptree person_root;
                m_persons_found.insert(person);
                person_root.put("", person->GetId());
                people.push_back(make_pair("", person_root));
        }
        contactPool_root.add_child("people", people);
        return contactPool_root;
}

ptree GeoGridJSONWriter::WriteCoordinate(const Coordinate& coordinate)
{
        ptree coordinate_root;
        coordinate_root.put("longitude", boost::geometry::get<0>(coordinate));
        coordinate_root.put("latitude", boost::geometry::get<1>(coordinate));
        return coordinate_root;
}

ptree GeoGridJSONWriter::WriteLocation(const Location& location)
{
        ptree location_root;
        location_root.put("id", location.GetID());
        location_root.put("name", location.GetName());
        location_root.put("province", location.GetProvince());
        location_root.put("population", location.GetPopCount());
        location_root.add_child("coordinate", WriteCoordinate(location.GetCoordinate()));

        auto commutes = location.CRefOutgoingCommutes();
        if (!commutes.empty()) {
                ptree commutes_root;
                for (auto commute_pair : commutes) {
                        commutes_root.put(to_string(commute_pair.first->GetID()), commute_pair.second);
                }
                location_root.add_child("commutes", commutes_root);
        }

        ptree contactCenters;
        for (Id typ : IdList) {
                for (const auto& c : location.CRefCenters(typ)) {
                        pair<string, ptree> child;
                        child = make_pair("", WriteContactCenter(c));
                        contactCenters.push_back(move(child));
                }
        }
        location_root.add_child("contactCenters", contactCenters);

        return location_root;
}

ptree GeoGridJSONWriter::WritePerson(Person* person)
{
        ptree person_root;
        person_root.put("id", person->GetId());
        person_root.put("age", person->GetAge());
        person_root.put("K12School", person->GetPoolId(Id::K12School));
        person_root.put("College", person->GetPoolId(Id::College));
        person_root.put("Household", person->GetPoolId(Id::Household));
        person_root.put("Workplace", person->GetPoolId(Id::Workplace));
        person_root.put("PrimaryCommunity", person->GetPoolId(Id::PrimaryCommunity));
        person_root.put("SecondaryCommunity", person->GetPoolId(Id::SecondaryCommunity));
        return person_root;
}

} // namespace geopop
