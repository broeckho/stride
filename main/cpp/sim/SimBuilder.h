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
 * Header for the SimBuilder class.
 */

#pragma once

#include <boost/property_tree/ptree.hpp>
#include <memory>

namespace stride {

class Sim;
class Population;

/**
 * Builds a simulator based a configuration property tree.
 * It
 * \li reads any additional configuration files (disease, contact, ...)
 * \li initializes calendar and random number manager for the simulator
 * \li builds a population and contact pool system (@see PopulationBuilder)
 * \li deals with health and immunity (@see HealthSeeder, @see DiseaseBuilder)
 */
class SimBuilder
{
public:
        /// Initializing SimBuilder.
        explicit SimBuilder(const boost::property_tree::ptree& configPt);

        /// Build the simulator and return it afterwards.
        std::shared_ptr<Sim> Build(std::shared_ptr<Sim> sim, std::shared_ptr<Population> pop);

private:
        /// Get the contact configuration data.
        boost::property_tree::ptree ReadAgeContactPtree();

        /// Get the disease configuration data.
        boost::property_tree::ptree ReadDiseasePtree();

private:
        boost::property_tree::ptree m_config; ///< Run config in ptree.
};

} // namespace stride
