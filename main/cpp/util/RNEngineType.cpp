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
 * Implementation of Random Engine Type.
 */

#include "RNEngineType.h"

#include <boost/algorithm/string.hpp>
#include <map>

using namespace std;
using boost::to_lower_copy;

namespace stride {
namespace RNEngineType {

bool IsType(const string& s)
{
        static map<string, Id> ids{make_pair("lcg64", Id::lcg64), make_pair("lcg64_shift", Id::lcg64_shift),
                                   make_pair("mrg2", Id::mrg2),   make_pair("mrg3", Id::mrg3),
                                   make_pair("yarn2", Id::yarn2), make_pair("yarn3", Id::yarn3)};
        return (ids.find(to_lower_copy(s)) != ids.end());
}

string ToString(Id c)
{
        static map<Id, string> names{make_pair(Id::lcg64, "lcg64"), make_pair(Id::lcg64_shift, "lcg64_shift"),
                                     make_pair(Id::mrg2, "mrg2"),   make_pair(Id::mrg3, "mrg3"),
                                     make_pair(Id::yarn2, "yarn2"), make_pair(Id::yarn3, "yarn3")};
        const auto             it = names.find(c);
        return (it != names.end()) ? it->second : "null";
}

Id ToType(const string& s)
{
        static map<string, Id> ids{make_pair("lcg64", Id::lcg64), make_pair("lcg64_shift", Id::lcg64_shift),
                                   make_pair("mrg2", Id::mrg2),   make_pair("mrg3", Id::mrg3),
                                   make_pair("yarn2", Id::yarn2), make_pair("yarn3", Id::yarn3)};
        const auto             it = ids.find(to_lower_copy(s));
        return (it != ids.end()) ? it->second : throw runtime_error("RNEngineType::ToString> not available:" + s);
}

} // namespace RNEngineType
} // namespace stride
