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

#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "geogrid.pb.h"
#include "geopop/GeoGrid.h"
#include "pop/Person.h"
#include "util/Exception.h"
#include "util/SegmentedVector.h"

#include <iostream>
#include <map>
#include <omp.h>

namespace geopop {

using namespace std;
using namespace stride::util;
using namespace stride::ContactType;

GeoGridProtoWriter::GeoGridProtoWriter() : m_persons_found() {}

void GeoGridProtoWriter::Write(GeoGrid& geoGrid, ostream& stream)
{
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        proto::GeoGrid protoGrid;
        for (const auto& location : geoGrid) {
                WriteLocation(*location, protoGrid.add_locations());
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

void GeoGridProtoWriter::WriteContactPools(Id typeId, SegmentedVector<stride::ContactPool*>& contactPools,
                                           proto::GeoGrid_Location_ContactPools* protoContactPools)
{
        static const map<Id, proto::GeoGrid_Location_ContactPools_Type> types = {
            {Id::K12School, proto::GeoGrid_Location_ContactPools_Type_K12School},
            {Id::PrimaryCommunity, proto::GeoGrid_Location_ContactPools_Type_PrimaryCommunity},
            {Id::SecondaryCommunity, proto::GeoGrid_Location_ContactPools_Type_SecondaryCommunity},
            {Id::College, proto::GeoGrid_Location_ContactPools_Type_College},
            {Id::Household, proto::GeoGrid_Location_ContactPools_Type_Household},
            {Id::Workplace, proto::GeoGrid_Location_ContactPools_Type_Workplace}};

        protoContactPools->set_type(types.at(typeId));
        for (stride::ContactPool* pool : contactPools) {
                WriteContactPool(pool, protoContactPools->add_pools());
        }
}

void GeoGridProtoWriter::WriteContactPool(stride::ContactPool*                              contactPool,
                                          proto::GeoGrid_Location_ContactPools_ContactPool* protoContactPool)
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

void GeoGridProtoWriter::WriteLocation(Location& location, proto::GeoGrid_Location* protoLocation)
{
        protoLocation->set_id(location.GetID());
        protoLocation->set_name(location.GetName());
        protoLocation->set_province(location.GetProvince());
        protoLocation->set_population(location.GetPopCount());
        auto coordinate = new proto::GeoGrid_Location_Coordinate();
        WriteCoordinate(location.GetCoordinate(), coordinate);
        protoLocation->set_allocated_coordinate(coordinate);

        auto commutes = location.CRefOutgoingCommutes();
        for (auto commute_pair : commutes) {
                auto commute = protoLocation->add_commutes();
                commute->set_to(commute_pair.first->GetID());
                commute->set_proportion(commute_pair.second);
        }

        for (Id typ : IdList) {
                WriteContactPools(typ, location.RefPools(typ), protoLocation->add_contactpools());
        }
}

void GeoGridProtoWriter::WritePerson(stride::Person* person, proto::GeoGrid_Person* protoPerson)
{
        protoPerson->set_id(person->GetId());
        protoPerson->set_age(static_cast<google::protobuf::int64>(person->GetAge()));
}

} // namespace geopop
