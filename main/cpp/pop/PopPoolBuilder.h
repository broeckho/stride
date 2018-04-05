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
 * Header for the ContactPoolBuilder class.
 */

#include "pool/ContactPoolSys.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace stride {

class Population;
class Simulator;

/**
 * Builds the contact pool system and adds members from the population.
 * The population members should have their pool ids for the various
 * pools already assigned to them.
 */
class PopPoolBuilder
{
public:
        /// Initializing ContactPoolBuilder.
        explicit PopPoolBuilder(std::shared_ptr<spdlog::logger> logger);

        /// Build the contact pool system.
        void Build(ContactPoolSys& pool_sys, const Population& population);

private:
        std::shared_ptr<spdlog::logger> m_stride_logger; ///< Stride run logger.
};

} // namespace stride
