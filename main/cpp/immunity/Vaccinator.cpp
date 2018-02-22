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
//#include "util/PtreeUtils.h"
#include "util/StringUtils.h"
#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>

namespace stride {

using namespace util;

Vaccinator::Vaccinator(const boost::property_tree::ptree& pt_config, util::RNManager& rn_manager)
    : m_pt_config(pt_config), m_rn_manager(rn_manager)
{
}

void Vaccinator::Administer(std::string immunity_type, std::string immunization_profile, std::shared_ptr<Simulator> sim)
{
        std::vector<double> immunity_distribution;
        const double        immunity_link_probability = 0;
        // const double immunity_link_probability = ((immunization_profile == "Cocoon") ? 1 :
        // m_pt_config.get<double>("run." + ToLower(immunity_type) + "_link_probability"));
        std::vector<ContactPool>* immunity_pools = &sim->m_households; ///< The default case.

        /*
         *                 if (immunity_link_probability > 0) {
                        using namespace ContactPoolType;
                        Id c_type = ToType(m_config.get<string>("run." + ToLower(s) + "_link_pooltype"));
                        switch (c_type) {
                        case Id::Household: immunity_pools = &m_sim->m_households; break;
                        case Id::School: immunity_pools = &m_sim->m_school_pools; break;
                        case Id::Work: immunity_pools = &m_sim->m_work_pools; break;
                        case Id::PrimaryCommunity: immunity_pools = &m_sim->m_primary_community; break;
                        case Id::SecondaryCommunity: immunity_pools = &m_sim->m_secondary_community; break;
                        }
                }
         */
        if (immunization_profile == "Random") {
                const auto immunity_rate = m_pt_config.get<double>("run." + ToLower(immunity_type) + "_rate");
                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        immunity_distribution.push_back(immunity_rate);
                }
                Immunizer<ImmunizationProfile::Random>::Administer(*immunity_pools, immunity_distribution,
                                                                   immunity_link_probability, m_rn_manager);
        } else if (immunization_profile == "Cocoon") {
                Immunizer<ImmunizationProfile::Cocoon>::Administer(*immunity_pools, immunity_distribution,
                                                                   immunity_link_probability, m_rn_manager);
        } else {
                Immunizer<ImmunizationProfile::None>::Administer(*immunity_pools, immunity_distribution,
                                                                 immunity_link_probability, m_rn_manager);
        }
}

} // namespace stride
