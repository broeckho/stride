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

#include "GeoGridJSONReader.h"

#include "ThreadException.h"
#include "gengeopop/College.h"
#include "gengeopop/Community.h"
#include "gengeopop/Household.h"
#include "gengeopop/K12School.h"
#include "gengeopop/PrimaryCommunity.h"
#include "gengeopop/SecondaryCommunity.h"
#include "gengeopop/Workplace.h"
#include "util/Exception.h"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <memory>

namespace gengeopop {

using namespace std;

GeoGridJSONReader::GeoGridJSONReader(unique_ptr<istream> inputStream, stride::Population* pop)
    : GeoGridReader(move(inputStream), pop), m_geoGrid(nullptr)
{
}

shared_ptr<GeoGrid> GeoGridJSONReader::Read()
{
        boost::property_tree::ptree root;
        try {
                boost::property_tree::read_json(*m_inputStream, root);
        } catch (runtime_error&) {
                throw stride::util::Exception(
                    "There was a problem parsing the JSON file, please check if it is not empty and it is valid JSON.");
        }
        m_geoGrid   = make_shared<GeoGrid>(m_population);
        auto people = root.get_child("persons");
#pragma omp parallel
#pragma omp single
        {
                for (auto it = people.begin(); it != people.end(); it++) {
#pragma omp task firstprivate(it)
                        {
#pragma omp critical
                                {
                                        stride::Person* person    = ParsePerson(it->second.get_child(""));
                                        m_people[person->GetId()] = person;
                                }
                        }
                }
#pragma omp taskwait
        }
        auto locations = root.get_child("locations");
        auto e         = make_shared<ThreadException>();
#pragma omp parallel
#pragma omp single
        {
                for (auto it = locations.begin(); it != locations.end(); it++) {
                        shared_ptr<Location> loc;
#pragma omp task firstprivate(it, loc)
                        {
                                e->Run([&loc, this, &it] { loc = ParseLocation(it->second.get_child("")); });
                                if (!e->HasError())
#pragma omp critical
                                        m_geoGrid->AddLocation(move(loc));
                        }
                }
#pragma omp taskwait
        }
        e->Rethrow();
        AddCommutes(m_geoGrid);
        m_commutes.clear();
        m_people.clear();
        return m_geoGrid;
} // namespace gengeopop

shared_ptr<Location> GeoGridJSONReader::ParseLocation(boost::property_tree::ptree& location)
{
        auto       id         = boost::lexical_cast<unsigned int>(location.get<string>("id"));
        auto       name       = location.get<string>("name");
        auto       province   = boost::lexical_cast<unsigned int>(location.get<string>("province"));
        auto       population = boost::lexical_cast<unsigned int>(location.get<string>("population"));
        Coordinate coordinate = ParseCoordinate(location.get_child("coordinate"));

        auto result         = make_shared<Location>(id, province, population, coordinate, name);
        auto contactCenters = location.get_child("contactCenters");
        auto e              = make_shared<ThreadException>();

#pragma omp parallel
#pragma omp single
        {
                for (auto it = contactCenters.begin(); it != contactCenters.end(); it++) {
                        shared_ptr<ContactCenter> center;
#pragma omp task firstprivate(it, center)
                        {
                                e->Run([&it, this, &center] { center = ParseContactCenter(it->second.get_child("")); });
                                if (!e->HasError())
#pragma omp critical
                                        result->AddContactCenter(center);
                        }
                }
#pragma omp taskwait
        }
        e->Rethrow();

        if (location.count("commutes")) {
                boost::property_tree::ptree commutes = location.get_child("commutes");
                for (auto it = commutes.begin(); it != commutes.end(); it++) {
                        auto to     = boost::lexical_cast<unsigned int>(it->first);
                        auto amount = boost::lexical_cast<double>(it->second.data());
#pragma omp critical
                        m_commutes.emplace_back(id, to, amount);
                }
        }

        return result;
}

Coordinate GeoGridJSONReader::ParseCoordinate(boost::property_tree::ptree& coordinate)
{
        auto longitude = boost::lexical_cast<double>(coordinate.get<string>("longitude"));
        auto latitude  = boost::lexical_cast<double>(coordinate.get<string>("latitude"));
        return {longitude, latitude};
}

shared_ptr<ContactCenter> GeoGridJSONReader::ParseContactCenter(boost::property_tree::ptree& contactCenter)
{
        auto                        type = contactCenter.get<string>("type");
        shared_ptr<ContactCenter>   result;
        auto                        id = boost::lexical_cast<unsigned int>(contactCenter.get<string>("id"));
        stride::ContactPoolType::Id typeId;
        if (type == "K12School") {
                result = make_shared<K12School>(id);
                typeId = stride::ContactPoolType::Id::K12School;
        } else if (type == "College") {
                result = make_shared<College>(id);
                typeId = stride::ContactPoolType::Id::College;
        } else if (type == "Household") {
                result = make_shared<Household>(id);
                typeId = stride::ContactPoolType::Id::Household;
        } else if (type == "Primary Community") {
                result = make_shared<PrimaryCommunity>(id);
                typeId = stride::ContactPoolType::Id::PrimaryCommunity;
        } else if (type == "Secondary Community") {
                result = make_shared<SecondaryCommunity>(id);
                typeId = stride::ContactPoolType::Id::SecondaryCommunity;
        } else if (type == "Workplace") {
                result = make_shared<Workplace>(id);
                typeId = stride::ContactPoolType::Id::Work;
        } else {
                throw stride::util::Exception("No such ContactCenter type: " + type);
        }

        auto contactPools = contactCenter.get_child("pools");
        auto e            = make_shared<ThreadException>();

#pragma omp parallel
#pragma omp single
        {
                for (auto it = contactPools.begin(); it != contactPools.end(); it++) {
#pragma omp task firstprivate(it)
                        {
                                stride::ContactPool* pool = nullptr;
                                e->Run([&it, &pool, this, typeId] {
                                        pool = ParseContactPool(it->second.get_child(""), typeId);
                                });
                                if (!e->HasError()) {
#pragma omp critical
                                        result->AddPool(pool);
                                }
                        }
                }
#pragma omp taskwait
        }
        e->Rethrow();
        return result;
}

stride::ContactPool* GeoGridJSONReader::ParseContactPool(boost::property_tree::ptree& contactPool,
                                                         stride::ContactPoolType::Id  typeId)
{
        // Don't use the id of the ContactPool but the let the Population create an id.
        auto result = m_geoGrid->CreateContactPool(typeId);
        auto people = contactPool.get_child("people");

        for (auto it = people.begin(); it != people.end(); it++) {
                auto person_id = boost::lexical_cast<unsigned int>(it->second.get<string>(""));
                if (m_people.count(person_id) == 0) {
                        throw stride::util::Exception("No such person: " + to_string(person_id));
                }
#pragma omp critical
                result->AddMember(m_people[person_id]);
        }

        return result;
}

stride::Person* GeoGridJSONReader::ParsePerson(boost::property_tree::ptree& person)
{
        auto id                   = boost::lexical_cast<unsigned int>(person.get<string>("id"));
        auto age                  = boost::lexical_cast<unsigned int>(person.get<string>("age"));
        auto gender               = person.get<string>("gender");
        auto schoolId             = boost::lexical_cast<unsigned int>(person.get<string>("K12School"));
        auto collegeId            = boost::lexical_cast<unsigned int>(person.get<string>("College"));
        auto householdId          = boost::lexical_cast<unsigned int>(person.get<string>("Household"));
        auto workplaceId          = boost::lexical_cast<unsigned int>(person.get<string>("Workplace"));
        auto primaryCommunityId   = boost::lexical_cast<unsigned int>(person.get<string>("PrimaryCommunity"));
        auto secondaryCommunityId = boost::lexical_cast<unsigned int>(person.get<string>("SecondaryCommunity"));

        return m_geoGrid->CreatePerson(id, age, householdId, schoolId, collegeId, workplaceId, primaryCommunityId,
                                       secondaryCommunityId);
}

} // namespace gengeopop
