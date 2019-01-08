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
 * Implementation of myhayai::event::Id.
 */

#include "Id.hpp"

#include <boost/algorithm/string.hpp>
#include <map>

namespace myhayai {
namespace event {

using namespace std;
using boost::to_upper;

bool IsType(const string& s)
{
        static map<string, Id> ids{make_pair("BEGINBENCH", Id::BeginBench), make_pair("ENDBENCH", Id::EndBench),
                                   make_pair("SKIPTEST", Id::SkipTest),     make_pair("ABORTTEST", Id::AbortTest),
                                   make_pair("BEGINTEST", Id::BeginTest),   make_pair("ENDTEST", Id::EndTest)};
        string                 t{s};
        to_upper(t);
        return (ids.count(t) == 1);
}

string ToString(Id c)
{
        static map<Id, string> names{make_pair(Id::BeginBench, "BeginBench"), make_pair(Id::EndBench, "EndBench"),
                                     make_pair(Id::SkipTest, "AbortTest"),    make_pair(Id::SkipTest, "AbortTest"),
                                     make_pair(Id::BeginTest, "BeginTest"),   make_pair(Id::EndTest, "EndTest")};
        return (names.count(c) == 1) ? names[c] : "null";
}

Id ToType(const string& s)
{
        static map<string, Id> ids{make_pair("BEGINBENCH", Id::BeginBench), make_pair("ENDBENCH", Id::EndBench),
                                   make_pair("SKIPTEST", Id::SkipTest),     make_pair("ABORTTEST", Id::AbortTest),
                                   make_pair("BEGINTEST", Id::BeginTest),   make_pair("ENDTEST", Id::EndTest)};
        string                 t{s};
        to_upper(t);
        return (ids.count(t) == 1) ? ids[t] : throw runtime_error("myhayai::event::Id::ToString> Not available:" + t);
}

} // namespace event
} // namespace myhayai
