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
#include "sim/Simulator.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>

namespace stride {

enum class ImmunizationProfile
{
        None   = 0U,
        Random = 1U,
        Cocoon = 2U,
        Null
};

/**
 * Apply the natural immunity and/or vaccination strategy specified in the configuration file.
 */
class Vaccinator
{
public:
        Vaccinator(const boost::property_tree::ptree& pt_config, util::RNManager& rn_manager);

        /// Apply the strategies specified in the configuration file
        void Apply(std::shared_ptr<Simulator> sim);

private:
        ///
        void Administer(std::string immunity_type, std::string immunization_profile, std::shared_ptr<Simulator> sim);

private:
        const boost::property_tree::ptree& m_pt_config;
        util::RNManager&                   m_rn_manager;
};

} // namespace stride
