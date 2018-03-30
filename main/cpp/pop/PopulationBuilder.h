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
        /// Initializes a Population: add persons, set immunity, seed infection.
        /// @param config_pt     Property_tree with general configuration settings.
        /// @param disease_pt    Property_tree with disease configuration settings.
        /// @return              Pointer to the initialized population.
        static std::shared_ptr<Population> Build(const boost::property_tree::ptree& config_pt,
                                                 util::RNManager&                   rn_manager);
};

} // namespace stride
