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

#include "GeoGridProtoWriter.h"

#include "contact/ContactType.h"
#include "geogrid.pb.h"
#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "util/Exception.h"

#include <iostream>
#include <omp.h>

namespace geopop {

using namespace std;

GeoGridProtoWriter::GeoGridProtoWriter() : m_persons_found() {}

void GeoGridProtoWriter::Write(shared_ptr<geopop::GeoGrid> geoGrid, ostream& stream)
{
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        proto::GeoGrid protoGrid;
        for (const auto& location : *geoGrid) {
                WriteLocation(location, protoGrid.add_locations());
        }
        for (const auto& person : m_persons_found) {
                WritePerson(person, protoGrid.add_persons());
        }

        m_persons_found.clear();
        if (!protoGrid.SerializeToOstream(&stream)) {
                throw stride::util::Exception("There was an error writing the GeoGrid to the file.");
        }
        google::protobuf::ShutdownProtobufLibrary();
        stream.flush();
}

void GeoGridProtoWriter::WriteContactCenter(shared_ptr<ContactCenter>              contactCenter,
                                            proto::GeoGrid_Location_ContactCenter* protoContactCenter)
{
        using namespace stride::ContactType;

        map<Id, proto::GeoGrid_Location_ContactCenter_Type> types = {
            {Id::K12School, proto::GeoGrid_Location_ContactCenter_Type_K12School},
            {Id::PrimaryCommunity, proto::GeoGrid_Location_ContactCenter_Type_PrimaryCommunity},
            {Id::SecondaryCommunity, proto::GeoGrid_Location_ContactCenter_Type_SecondaryCommunity},
            {Id::College, proto::GeoGrid_Location_ContactCenter_Type_College},
            {Id::Household, proto::GeoGrid_Location_ContactCenter_Type_Household},
            {Id::Workplace, proto::GeoGrid_Location_ContactCenter_Type_Workplace}};

        protoContactCenter->set_id(contactCenter->GetId());
        protoContactCenter->set_type(types[contactCenter->GetContactPoolType()]);
        for (stride::ContactPool* pool : *contactCenter) {
                WriteContactPool(pool, protoContactCenter->add_pools());
        }
}

void GeoGridProtoWriter::WriteContactPool(stride::ContactPool*                               contactPool,
                                          proto::GeoGrid_Location_ContactCenter_ContactPool* protoContactPool)
{
        protoContactPool->set_id(static_cast<google::protobuf::int64>(contactPool->GetId()));
        for (const auto& person : *contactPool) {
                protoContactPool->add_people(person->GetId());
                m_persons_found.insert(person);
        }
}

void GeoGridProtoWriter::WriteCoordinate(const Coordinate&                   coordinate,
                                         proto::GeoGrid_Location_Coordinate* protoCoordinate)
{
        protoCoordinate->set_longitude(boost::geometry::get<0>(coordinate));
        protoCoordinate->set_latitude(boost::geometry::get<1>(coordinate));
}

void GeoGridProtoWriter::WriteLocation(shared_ptr<Location> location, proto::GeoGrid_Location* protoLocation)
{
        protoLocation->set_id(location->GetID());
        protoLocation->set_name(location->GetName());
        protoLocation->set_province(location->GetProvince());
        protoLocation->set_population(location->GetPopCount());
        auto coordinate = new proto::GeoGrid_Location_Coordinate();
        WriteCoordinate(location->GetCoordinate(), coordinate);
        protoLocation->set_allocated_coordinate(coordinate);

        auto commutes = location->GetOutgoingCommutingCities();
        for (auto commute_pair : commutes) {
                auto commute = protoLocation->add_commutes();
                commute->set_to(commute_pair.first->GetID());
                commute->set_proportion(commute_pair.second);
        }

        for (const auto& contactCenter : *location) {
                WriteContactCenter(contactCenter, protoLocation->add_contactcenters());
        }
}

void GeoGridProtoWriter::WritePerson(stride::Person* person, proto::GeoGrid_Person* protoPerson)
{
        protoPerson->set_id(person->GetId());
        protoPerson->set_age(static_cast<google::protobuf::int64>(person->GetAge()));
}

} // namespace geopop
