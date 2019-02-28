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

#include <boost/property_tree/ptree.hpp>

namespace stride {

using namespace std;
using namespace stride::ContactType;
using namespace boost::property_tree;

AgeContactProfile::AgeContactProfile(Id poolType, const ptree& contactPt) : std::array<double, MaximumAge() + 1>()
{
        string typeKey = ContactType::ToString(poolType);
        // TODO ELiminate this hack by fixing the data file
        if (poolType == Id::K12School || poolType == Id::College) {
                typeKey = "school";
        } else if (poolType == Id::Household) {
                typeKey = "household";
        } else if (poolType == Id::Workplace) {
                typeKey = "work";
        } else if (poolType == Id::PrimaryCommunity) {
                typeKey = "primary_community";
        } else if (poolType == Id::SecondaryCommunity) {
                typeKey = "secondary_community";
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
