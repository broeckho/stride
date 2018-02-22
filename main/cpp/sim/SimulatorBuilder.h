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

namespace stride {

/**
 * Class to build the simulator.
 */
class SimulatorBuilder
{
public:
        ///
        static std::shared_ptr<Simulator> Build(const std::string& config_file_name, unsigned int num_threads,
                                                bool track_index_case = false);

        /// Build the simulator.
        static std::shared_ptr<Simulator> Build(const boost::property_tree::ptree& pt_config, unsigned int num_threads,
                                                bool track_index_case = false);

        /// Build the simulator.
        static std::shared_ptr<Simulator> Build(const boost::property_tree::ptree& pt_config,
                                                const boost::property_tree::ptree& pt_disease,
                                                const boost::property_tree::ptree& pt_contact,
                                                unsigned int num_threads = 1U, bool track_index_case = false);

private:
        /// Initialize the contactpoolss.
        static void InitializeContactPools(std::shared_ptr<Simulator> sim);
};

} // namespace stride
