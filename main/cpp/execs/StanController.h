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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the St(ochastic)an(alysis)Controller.
 */

#include "ControlHelper.h"

#include <boost/property_tree/ptree_fwd.hpp>

namespace stride {

/**
 * Runs a simulation multiple times, with different random engine seeds.
 * The infection count at each day is captured a a text file is produced
 * with the infection counts over time and the associated boxplot data.
 * The runs are in parallel (iff OpenMP available), individuals runs not.
 *
 * StanController setup functions include (@see ControlHelper):
 * \li checks the OpenMP environment
 * \li checks the file system environment
 * \li interprets and executes the ouput prefix
 * \li intalls a stride logger
 *
 * The StanController execution:
 * \li produces a random seed for each run
 * \li creates a population (@see Population) for each run
 * \li creates a simulation runner (@see SimRunner) for each run
 * \li registers the appropriate viewer for each runner
 * \li runs the simulations
 * \li produces the output.
 *
 * The procedure implies that the parameters "run.rng_seed" and also
 * "run.num_threads" in the configuration are overriden for the
 * individual runs.
 *
 * The output is textual and consists of multiple CSV blocks separated
 * by blank lines. The first block has the time evolution of the infection
 * count in rows, the second block has the boxplot data in columns. The
 * format is amenable to the gnuplot plotting tool (so in each CSV block
 * the header row with column labels starts with a '#').
 */
class StanController : protected ControlHelper
{
public:
        /// Straight initialization.
        explicit StanController(const boost::property_tree::ptree& config);

        /// Actual run of the simulator.
        void Control();
};

} // namespace stride
