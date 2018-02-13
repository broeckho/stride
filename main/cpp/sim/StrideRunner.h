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
 * Header for the StrideRunner class.
 */

#include "sim/Simulator.h"
#include "sim/python/SimulatorObserver.h"
#include "util/Stopwatch.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <string>
#include <vector>

namespace stride {

/**
 * Class for setting up & managing a simulation.
 */
class StrideRunner
{
public:
        /// Constructor
        StrideRunner();

        /// Destructor
        virtual ~StrideRunner() = default;

        /// Register observer (method used by the python environment).
        void RegisterObserver(std::shared_ptr<python::SimulatorObserver>& observer);

        ///
        void Setup(bool track_index_case, const std::string& config_file_name, bool use_install_dirs = false);

        /// Run the simulator with config information provided.
        void Run();

        ///
        void Stop();

        /// Get the simulator (method used by the python environment).
        std::shared_ptr<Simulator> GetSimulator() { return m_sim; }

private:
        /// Generate output files (at the end of the simulation).
        void GenerateOutputFiles(const std::string& output_prefix, const std::vector<unsigned int>& cases,
                                 const std::vector<unsigned int>& adopted, const boost::property_tree::ptree& pt_config,
                                 unsigned int run_time, unsigned int total_time);

private:
        bool                        m_is_running;
        bool                        m_operational;
        std::string                 m_output_prefix;
        boost::property_tree::ptree m_pt_config;
        util::Stopwatch<>           m_clock;
        std::shared_ptr<Simulator>  m_sim;
};

} // namespace stride
