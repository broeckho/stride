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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Implementation of ContactPoolType.
 */

#include "ContactPoolType.h"

#include <boost/algorithm/string.hpp>
#include <map>

namespace stride {
namespace ContactPoolType {

using namespace std;
using boost::to_upper;

bool IsId(const string& s)
{
        static map<string, Id> ids{
            make_pair("HOUSEHOLD", Id::Household),
            make_pair("SCHOOL", Id::K12School),
            make_pair("SCHOOL", Id::College),
            make_pair("WORK", Id::Work),
            make_pair("PRIMARY_COMMUNITY", Id::PrimaryCommunity),
            make_pair("SECONDARY_COMMUNITY", Id::SecondaryCommunity),
        };
        string t{s};
        to_upper(t);
        return (ids.count(t) == 1);
}

string ToString(Id c)
{
        static map<Id, string> names{
            make_pair(Id::Household, "household"),
            make_pair(Id::K12School, "k12school"),
            make_pair(Id::College, "college"),
            make_pair(Id::Work, "work"),
            make_pair(Id::PrimaryCommunity, "primary_community"),
            make_pair(Id::SecondaryCommunity, "secondary_community"),
        };
        return (names.count(c) == 1) ? names[c] : throw runtime_error("ContactPoolType::ToString> not available:");
}

Id ToId(const string& s)
{
        static map<string, Id> ids{
            make_pair("HOUSEHOLD", Id::Household),
            make_pair("K12SCHOOL", Id::K12School),
            make_pair("COLLEGE", Id::College),
            make_pair("WORK", Id::Work),
            make_pair("PRIMARY_COMMUNITY", Id::PrimaryCommunity),
            make_pair("SECONDARY_COMMUNITY", Id::SecondaryCommunity),
        };
        string t{s};
        to_upper(t);
        return (ids.count(t) == 1) ? ids[t] : throw runtime_error("ContactPoolType::ToId> not available:" + t);
}

} // namespace ContactPoolType
} // namespace stride
