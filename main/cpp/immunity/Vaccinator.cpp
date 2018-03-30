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
 * Implementation for the Vaccinator class.
 */

#include "Vaccinator.h"

#include "Immunizer.h"
#include "core/ContactPoolSys.h"
#include "util/StringUtils.h"
#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>

namespace stride {

using namespace util;
using namespace stride::ContactPoolType;

Vaccinator::Vaccinator(const boost::property_tree::ptree& config_pt, util::RNManager& rn_manager)
    : m_config_pt(config_pt), m_rn_manager(rn_manager)
{
}

void Vaccinator::Administer(const std::string& immunity_type, const std::string& immunization_profile,
                            std::vector<ContactPool>& immunity_pools)
{
        std::vector<double> immunity_distribution;
        const double        immunity_link_probability = 0;

        if (immunization_profile == "Random") {
                const auto immunity_rate = m_config_pt.get<double>("run." + ToLower(immunity_type) + "_rate");
                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        immunity_distribution.push_back(immunity_rate);
                }
                Immunizer<ImmunizationProfile::Random>::Administer(immunity_pools, immunity_distribution,
                                                                   immunity_link_probability, m_rn_manager);
        } else if (immunization_profile == "Cocoon") {
                Immunizer<ImmunizationProfile::Cocoon>::Administer(immunity_pools, immunity_distribution,
                                                                   immunity_link_probability, m_rn_manager);
        } else {
                Immunizer<ImmunizationProfile::None>::Administer(immunity_pools, immunity_distribution,
                                                                 immunity_link_probability, m_rn_manager);
        }
}

} // namespace stride
