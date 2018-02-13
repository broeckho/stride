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
 *  Copyright 20178, Kuylen E, Willem L, Broeckhove J
 *
 */
/**
 * @file
 * Implementation of RandomEngineType.
 */

#include "RNEngineType.h"

#include <boost/algorithm/string.hpp>
#include <map>

using namespace std;
using boost::to_lower;

namespace stride {
namespace RNEngineType {

bool IsType(const string& s)
{
        static map<string, Id> ids{make_pair("lcg64", Id::lcg64), make_pair("lcg64_shift", Id::lcg64_shift),
                                   make_pair("mrg2", Id::mrg2),   make_pair("mrg3", Id::mrg3),
                                   make_pair("yarn2", Id::yarn2), make_pair("yarn3", Id::yarn3)};
        std::string            t{s};
        to_lower(t);
        return (ids.count(t) == 1);
}

string ToString(Id c)
{
        static map<Id, string> names{make_pair(Id::lcg64, "lcg64"), make_pair(Id::lcg64_shift, "lcg64_shift"),
                                     make_pair(Id::mrg2, "mrg2"),   make_pair(Id::mrg3, "mrg3"),
                                     make_pair(Id::yarn2, "yarn2"), make_pair(Id::yarn3, "yarn3")};
        return (names.count(c) == 1) ? names[c] : "null";
}

Id ToType(const string& s)
{
        static map<string, Id> ids{make_pair("lcg64", Id::lcg64), make_pair("lcg64_shift", Id::lcg64_shift),
                                   make_pair("mrg2", Id::mrg2),   make_pair("mrg3", Id::mrg3),
                                   make_pair("yarn2", Id::yarn2), make_pair("yarn3", Id::yarn3)};
        std::string            t{s};
        to_lower(t);
        return (ids.count(t) == 1) ? ids[t] : throw runtime_error("RNEngineType::ToString> not available:" + t);
}

} // namespace RNEngineType
} // namespace stride
