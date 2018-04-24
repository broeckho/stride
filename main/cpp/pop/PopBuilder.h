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
class PopBuilder
{
public:
        /// Initializing constructor.
        /// \param configPt    Property_tree with general configuration settings.
        explicit PopBuilder(const boost::property_tree::ptree& configPt);

        /// Builds a Population. The steps are:
        /// - Preliminaries (check input data, rnManager, contactLogger).
        /// - Read persons from file and instatiate them.
        /// - Fill up the various type of contactpools.
        /// - Seed the population with contact survey participants.
        /// @return              Pointer to the population.
        std::shared_ptr<Population> Build();

private:
        /// Fills up the contact pool system.
        void MakePoolSys();

        /// generates persons.
        void MakePersons();

        /// Preliminary setup.
        void Preliminaries();

private:
        boost::property_tree::ptree m_config_pt;   ///< Configuration property tree
        std::shared_ptr<Population> m_pop;         ///< The population.
        util::RNManager             m_rn_manager;  ///< Random numbere generation management.
};

} // namespace stride
