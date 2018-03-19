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
 * Implementation of LogMode.
 */

#include "ContactLogMode.h"

#include <boost/algorithm/string.hpp>
#include <map>

namespace stride {
namespace ContactLogMode {

using namespace std;
using boost::to_upper;

string ToString(Id l)
{
        static map<Id, string> names{make_pair(Id::None, "None"), make_pair(Id::Transmissions, "Transmissions"),
                                     make_pair(Id::Contacts, "Contacts"),
                                     make_pair(Id::SusceptibleContacts, "SusceptibleContacts")};
        return (names.count(l) == 1) ? names[l] : "Null";
}

bool IsMode(const string& s) { return (ToMode(s) != Id::Null); }

Id ToMode(const string& s)
{
        static map<string, Id> modes{make_pair("NONE", Id::None), make_pair("TRANSMISSIONS", Id::Transmissions),
                                     make_pair("CONTACTS", Id::Contacts),
                                     make_pair("SUSCEPTIBLECONTACTS", Id::SusceptibleContacts)};
        std::string            t{s};
        to_upper(t);
        return (modes.count(t) == 1) ? modes[t] : Id::Null;
}

} // namespace ContactLogMode
} // namespace stride
