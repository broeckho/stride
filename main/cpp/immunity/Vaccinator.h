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
 * Header for the Vaccinator class.
 */

#include "core/Cluster.h"
#include "sim/Simulator.h"
#include "util/Random.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <string>
#include <vector>

namespace stride {

/**
 * Apply the immunization strategy in the configuration file to the Simulator object.
 */
class Vaccinator
{
public:
        Vaccinator(std::shared_ptr<Simulator> sim, const boost::property_tree::ptree& pt_config,
                   const boost::property_tree::ptree& pt_disease, util::Random& rng);

        /// Apply the immunization strategy in the configuration file to the Simulator object.
        void Apply(const std::string& s);

private:
        /// Initiate the given immunity distribution in the population, according the given link probability.
        void Administer(const std::vector<Cluster>& clusters, std::vector<double>& immunity_distribution,
                        double immunity_link_probability);

        /// Administer cocoon immunization for the given rate and target ages [min-max] to protect connected
        /// individuals of the given age class [min-max].
        void AdministerCocoon(const std::vector<Cluster>& clusters, double immunity_rate, double adult_age_min,
                              double adult_age_max, double child_age_min, double child_age_max);

private:
        const boost::property_tree::ptree& m_config;
        const boost::property_tree::ptree& m_disease;
        std::shared_ptr<Simulator> m_sim;
        util::Random& m_rng;
};

} // namespace stride
