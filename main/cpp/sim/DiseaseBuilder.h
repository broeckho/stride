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
 * Header for the DiseaseBuilder class.
 */

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace stride {

class Simulator;

/**
 * Initializes status of population members w.r.t disease (natural immunity, vaccination, ...).
 */
class DiseaseBuilder
{
public:
        /// Initializing DiseaseBuilder.
        DiseaseBuilder(const boost::property_tree::ptree& config_pt);

        /// Build the simulator.
        void Build(std::shared_ptr<Simulator> sim);

private:
        boost::property_tree::ptree m_config_pt; /// Run config.
};

} // namespace stride
