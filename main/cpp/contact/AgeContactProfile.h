#pragma once
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

#include "pool/ContactPoolType.h"
#include "pop/Age.h"

#include <boost/property_tree/ptree.hpp>
#include <array>

namespace stride {

class AgeContactProfile : public std::array<double, MaximumAge() + 1>
{
public:
        /// Need to keep the default constructor available.
        AgeContactProfile() = default;

        /// Explicitly initialize
        AgeContactProfile(ContactPoolType::Id poolType, const boost::property_tree::ptree& contactPt);
};

} // namespace stride
