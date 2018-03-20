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
 * Header for the Vaccinator class.
 */

#include "core/ContactPool.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>

namespace stride {

enum class ImmunizationProfile
{
        None   = 0U,
        Random = 1U,
        Cocoon = 2U,
};

/// Primary immunization profile: do nothing
template <ImmunizationProfile profile>
class Immunizer
{
public:
        static void Administer(const std::vector<ContactPool>&, std::vector<double>&, double, util::RNManager&) {}
};

/// Random immunization profile
template <>
class Immunizer<ImmunizationProfile::Random>
{
public:
        static void Administer(const std::vector<ContactPool>& pools, std::vector<double>& immunity_distribution,
                               double immunity_link_probability, util::RNManager& rn_manager);
};

/// Profile for cocoon vaccination strategy
template <>
class Immunizer<ImmunizationProfile::Cocoon>
{
public:
        static void Administer(const std::vector<ContactPool>& pools, std::vector<double>& immunity_distribution,
                               double immunity_link_probability, util::RNManager& rn_manager);
};

extern template class Immunizer<ImmunizationProfile::Random>;
extern template class Immunizer<ImmunizationProfile::Cocoon>;

} // namespace stride
