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

#include "GeoGridProtoWriter.h"

#include "gengeopop/ContactCenter.h"
#include "gengeopop/io/proto/geogrid.pb.h"
#include "util/Exception.h"

#include <iostream>
#include <omp.h>


namespace gengeopop {

GeoGridProtoWriter::GeoGridProtoWriter() : m_persons_found() {}

void GeoGridProtoWriter::Write(std::shared_ptr<gengeopop::GeoGrid> geoGrid, std::ostream& stream)
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

void GeoGridProtoWriter::WriteLocation(std::shared_ptr<Location> location, proto::GeoGrid_Location* protoLocation)
{
        protoLocation->set_id(location->GetID());
        protoLocation->set_name(location->GetName());
        protoLocation->set_province(location->GetProvince());
        protoLocation->set_population(location->GetPopulation());
        auto coordinate = new proto::GeoGrid_Location_Coordinate();
        WriteCoordinate(location->GetCoordinate(), coordinate);
        protoLocation->set_allocated_coordinate(coordinate);

        auto commutes = location->GetOutgoingCommuningCities();
        for (auto commute_pair : commutes) {
                auto commute = protoLocation->add_commutes();
                commute->set_to(commute_pair.first->GetID());
                commute->set_proportion(commute_pair.second);
        }

        for (const auto& contactCenter : *location) {
                WriteContactCenter(contactCenter, protoLocation->add_contactcenters());
        }
}

void GeoGridProtoWriter::WriteCoordinate(const Coordinate&                   coordinate,
                                         proto::GeoGrid_Location_Coordinate* protoCoordinate)
{
        using boost::geometry::get;
        protoCoordinate->set_longitude(get<0>(coordinate));
        protoCoordinate->set_latitude(get<1>(coordinate));
}

void GeoGridProtoWriter::WriteContactCenter(std::shared_ptr<ContactCenter>         contactCenter,
                                            proto::GeoGrid_Location_ContactCenter* protoContactCenter)
{
        std::map<std::string, proto::GeoGrid_Location_ContactCenter_Type> types = {
            {"K12School", proto::GeoGrid_Location_ContactCenter_Type_K12School},
            {"Community", proto::GeoGrid_Location_ContactCenter_Type_Community},
            {"Primary Community", proto::GeoGrid_Location_ContactCenter_Type_PrimaryCommunity},
            {"Secondary Community", proto::GeoGrid_Location_ContactCenter_Type_SecondaryCommunity},
            {"College", proto::GeoGrid_Location_ContactCenter_Type_College},
            {"Household", proto::GeoGrid_Location_ContactCenter_Type_Household},
            {"Workplace", proto::GeoGrid_Location_ContactCenter_Type_Workplace}};

        protoContactCenter->set_id(contactCenter->GetId());
        protoContactCenter->set_type(types[contactCenter->GetType()]);
        for (stride::ContactPool* pool : *contactCenter) {
                WriteContactPool(pool, protoContactCenter->add_pools());
        }
}

void GeoGridProtoWriter::WriteContactPool(stride::ContactPool*                               contactPool,
                                          proto::GeoGrid_Location_ContactCenter_ContactPool* protoContactPool)
{
        protoContactPool->set_id(contactPool->GetId());
        for (const auto& person : *contactPool) {
                protoContactPool->add_people(person->GetId());
                m_persons_found.insert(person);
        }
}

void GeoGridProtoWriter::WritePerson(stride::Person* person, proto::GeoGrid_Person* protoPerson)
{
        protoPerson->set_id(person->GetId());
        protoPerson->set_age(static_cast<google::protobuf::int64>(person->GetAge()));
        protoPerson->set_gender(std::string(1, person->GetGender()));
}
} // namespace gengeopop
