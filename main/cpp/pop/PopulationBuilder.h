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
 *  Copyright 2017, 2018 Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Initialize populations.
 */

#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>

namespace stride {

class Population;

/**
 * Initializes Population objects.
 */
class PopulationBuilder
{
public:
        /// @param configPt      Property_tree with general configuration settings.
        PopulationBuilder(const boost::property_tree::ptree& configPt);

        /// Initializes a Population.
        /// @return              Pointer to the population.
        std::shared_ptr<Population> Build();

private:
        boost::property_tree::ptree m_config_pt;   ///< Configuration property tree
        unsigned int                m_num_threads; ///< The number of (OpenMP) threads.

        util::RNManager m_rn_manager; ///< Random numbere generation management.
};

} // namespace stride
