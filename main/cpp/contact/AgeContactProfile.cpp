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
 * Contact profile.
 */

#include "AgeContactProfile.h"

namespace stride {

using namespace std;
using namespace stride::ContactPoolType;
using namespace boost::property_tree;

AgeContactProfile::AgeContactProfile(Id poolType, const ptree& contactPt)
    : std::array<double, MaximumAge() + 1>()
{
        string typeKey = ContactPoolType::ToString(poolType);
        if (poolType == Id::K12School || poolType == Id::College) {
                typeKey = "school";
        }
        const string key{string("matrices.").append(typeKey)};
        unsigned int i = 0U;
        for (const auto& participant : contactPt.get_child(key)) {
                double totalContacts = 0;
                for (const auto& contact : participant.second.get_child("contacts")) {
                        totalContacts += contact.second.get<double>("rate");
                }
                (*this)[i++] = totalContacts;
        }
}

} // namespace stride
