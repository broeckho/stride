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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation of ClusterType.
 */

#include "ClusterType.h"

#include <boost/algorithm/string.hpp>
#include <map>

namespace stride {
namespace ClusterType {

using namespace std;
using boost::to_upper;

string ToString(Id c)
{

        map<Id, string> cluster_names{
            make_pair(Id::Household, "household"),
            make_pair(Id::School, "school"),
            make_pair(Id::Work, "work"),
            make_pair(Id::PrimaryCommunity, "primary_community"),
            make_pair(Id::SecondaryCommunity, "secondary_community"),
        };
        return (cluster_names.count(c) == 1) ? cluster_names[c] : "null";
}

bool IsType(const string& s) { return (ToType(s) != Id::Null); }

Id ToType(const string& s)
{

        static map<string, Id> cluster_ids{
            make_pair("HOUSEHOLD", Id::Household),
            make_pair("SCHOOL", Id::School),
            make_pair("WORK", Id::Work),
            make_pair("PRIMARY_COMMUNITY", Id::PrimaryCommunity),
            make_pair("SECONDARY_COMMUNITY", Id::SecondaryCommunity),
        };

        std::string t{s};
        to_upper(t);
        return (cluster_ids.count(t) == 1) ? cluster_ids[t] : Id::Null;
}

} // namespace ClusterType
} // namespace stride