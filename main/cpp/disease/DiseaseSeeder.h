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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the DiseaseSeeder class.
 */

#include "pool/ContactPool.h"
#include "pop/Population.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace stride {

class Sim;

/**
 * Seeds population w.r.t immunity (natural immunity, vaccination, ...) and infection.
 */
class DiseaseSeeder
{
public:
        /// Initializing DiseaseSeeder.
        DiseaseSeeder(const boost::property_tree::ptree& configPt, util::RNManager& rnManager);

        /// Build the simulator.
        void Seed(std::shared_ptr<Population> pop);

private:
        /// Seed for vaccination/natural immunity.
        void Vaccinate(const std::string& immunityType, const std::string& immunizationProfile,
                       std::vector<ContactPool>& immunityPools);

private:
        const boost::property_tree::ptree& m_config_pt;  ///< Run config.
        util::RNManager&                   m_rn_manager; ///< Random number manager.
};

} // namespace stride
