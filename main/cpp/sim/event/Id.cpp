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
 * Implementation of EventId.
 */

#include "Id.h"

#include <boost/algorithm/string.hpp>
#include <map>

namespace stride {
namespace sim_event {

using namespace std;
using boost::to_upper;

bool IsType(const string& s)
{
        static map<string, Id> ids{make_pair("ATSTART", Id::AtStart), make_pair("STEPPED", Id::Stepped),
                                   make_pair("FINISHED", Id::Finished), make_pair("SETUPBEGIN", Id::SetupBegin),
                                   make_pair("SETUPEND", Id::SetupEnd)};
        string                 t{s};
        to_upper(t);
        return (ids.count(t) == 1);
}

string ToString(Id c)
{
        static map<Id, string> names{make_pair(Id::AtStart, "atstart"), make_pair(Id::Stepped, "stepped"),
                                     make_pair(Id::Finished, "finished"), make_pair(Id::SetupBegin, "setupbegin"),
                                     make_pair(Id::SetupEnd, "setupend")};
        return (names.count(c) == 1) ? names[c] : "null";
}

Id ToType(const string& s)
{
        static map<string, Id> ids{make_pair("ATSTART", Id::AtStart), make_pair("STEPPED", Id::Stepped),
                                   make_pair("FINISHED", Id::Finished), make_pair("SETUPBEGIN", Id::SetupBegin),
                                   make_pair("SETUPEND", Id::SetupEnd)};
        string                 t{s};
        to_upper(t);
        return (ids.count(t) == 1) ? ids[t] : throw runtime_error("EventId::ToString> not available:" + t);
}

} // namespace sim_event
} // namespace stride