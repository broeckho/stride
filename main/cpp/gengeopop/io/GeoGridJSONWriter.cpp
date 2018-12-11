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

#include "GeoGridJSONWriter.h"

#include "gengeopop/ContactCenter.h"

#include <iostream>
#include <omp.h>

namespace gengeopop {

using namespace std;

GeoGridJSONWriter::GeoGridJSONWriter() : m_persons_found() {}

void GeoGridJSONWriter::Write(shared_ptr<gengeopop::GeoGrid> geoGrid, ostream& stream)
{
        boost::property_tree::ptree root;
        boost::property_tree::ptree locations;

#pragma omp parallel
#pragma omp single
        {
                for (const auto& location : *geoGrid) {
                        pair<string, boost::property_tree::ptree> child;
#pragma omp task firstprivate(location)
                        {
                                child = make_pair("", WriteLocation(location));
#pragma omp critical

                                locations.push_back(move(child));
                        }
                }
#pragma omp taskwait
        }

        root.add_child("locations", locations);

        boost::property_tree::ptree persons;
#pragma omp parallel
#pragma omp single
        {
                for (const auto& person : m_persons_found) {
                        pair<string, boost::property_tree::ptree> child;
#pragma omp task firstprivate(person)
                        {
                                child = make_pair("", WritePerson(person));
#pragma omp critical
                                persons.push_back(move(child));
                        }
                }
#pragma omp taskwait
        }
        root.add_child("persons", persons);

        m_persons_found.clear();
        boost::property_tree::write_json(stream, root);
}

boost::property_tree::ptree GeoGridJSONWriter::WriteLocation(shared_ptr<Location> location)
{
        boost::property_tree::ptree location_root;
        location_root.put("id", location->GetID());
        location_root.put("name", location->GetName());
        location_root.put("province", location->GetProvince());
        location_root.put("population", location->GetPopulation());
        location_root.add_child("coordinate", WriteCoordinate(location->GetCoordinate()));

        auto commutes = location->GetOutgoingCommuningCities();
        if (!commutes.empty()) {
                boost::property_tree::ptree commutes_root;
                for (auto commute_pair : commutes) {
                        commutes_root.put(to_string(commute_pair.first->GetID()), commute_pair.second);
                }
                location_root.add_child("commutes", commutes_root);
        }

        boost::property_tree::ptree contactCenters;
#pragma omp parallel
#pragma omp single
        {
                for (const auto& contactCenter : *location) {
                        pair<string, boost::property_tree::ptree> child;
#pragma omp task firstprivate(contactCenter)
                        {
                                child = make_pair("", WriteContactCenter(contactCenter));
#pragma omp critical
                                contactCenters.push_back(move(child));
                        }
                }
#pragma omp taskwait
        }
        location_root.add_child("contactCenters", contactCenters);

        return location_root;
}

boost::property_tree::ptree GeoGridJSONWriter::WriteCoordinate(const Coordinate& coordinate)
{
        using boost::geometry::get;
        boost::property_tree::ptree coordinate_root;
        coordinate_root.put("longitude", get<0>(coordinate));
        coordinate_root.put("latitude", get<1>(coordinate));
        return coordinate_root;
}

boost::property_tree::ptree GeoGridJSONWriter::WriteContactCenter(shared_ptr<ContactCenter> contactCenter)
{
        boost::property_tree::ptree contactCenter_root;
        contactCenter_root.put("id", contactCenter->GetId());
        contactCenter_root.put("type", contactCenter->GetType());
        boost::property_tree::ptree pools;
#pragma omp parallel
#pragma omp single
        {
                for (const auto& pool : *contactCenter) {
                        pair<string, boost::property_tree::ptree> child;
#pragma omp task firstprivate(pool)
                        {
                                child = make_pair("", WriteContactPool(pool));
#pragma omp critical
                                pools.push_back(move(child));
                        }
                }
#pragma omp taskwait
        }
        contactCenter_root.add_child("pools", pools);
        return contactCenter_root;
}

boost::property_tree::ptree GeoGridJSONWriter::WriteContactPool(stride::ContactPool* contactPool)
{
        boost::property_tree::ptree contactPool_root;
        contactPool_root.put("id", contactPool->GetId());
        boost::property_tree::ptree people;
        for (stride::Person* person : *contactPool) {
                boost::property_tree::ptree person_root;
#pragma omp critical
                m_persons_found.insert(person);
                person_root.put("", person->GetId());
                people.push_back(make_pair("", person_root));
        }
        contactPool_root.add_child("people", people);
        return contactPool_root;
}

boost::property_tree::ptree GeoGridJSONWriter::WritePerson(stride::Person* person)
{
        boost::property_tree::ptree person_root;
        person_root.put("id", person->GetId());
        person_root.put("age", person->GetAge());
        person_root.put("gender", person->GetGender());
        person_root.put("K12School", person->GetK12SchoolId());
        person_root.put("College", person->GetCollegeId());
        person_root.put("Household", person->GetHouseholdId());
        person_root.put("Workplace", person->GetWorkId());
        person_root.put("PrimaryCommunity", person->GetPrimaryCommunityId());
        person_root.put("SecondaryCommunity", person->GetSecondaryCommunityId());
        return person_root;
}

} // namespace gengeopop
