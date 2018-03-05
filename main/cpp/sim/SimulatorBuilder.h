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
 * Header for the SimulatorBuilder class.
 */

#include "sim/Simulator.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace stride {

/**
 * Class to build the simulator.
 */
class SimulatorBuilder
{
public:
        /// Initializing SimulatorBuilder.
        SimulatorBuilder(const boost::property_tree::ptree& config_pt);

        /// Build the simulator.
        std::shared_ptr<Simulator> Build();

private:
        /// Build the simulator.
        std::shared_ptr<Simulator> Build(const boost::property_tree::ptree& pt_disease,
                                         const boost::property_tree::ptree& pt_contact);

        /// Initialize the contactpoolss.
        static void InitializeContactPools(std::shared_ptr<Simulator> sim);

        /// Get the contact configuration data.
        boost::property_tree::ptree ReadContactPtree();

        /// Get the disease configuration data.
        boost::property_tree::ptree ReadDiseasePtree();

private:
        std::shared_ptr<spdlog::logger> m_logger;
        boost::property_tree::ptree     m_pt_config;
};

} // namespace stride
