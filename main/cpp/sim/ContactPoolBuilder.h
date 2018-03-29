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

#include "core/ContactPoolSys.h"

#include <boost/property_tree/ptree.hpp>
#include <memory>

namespace spdlog {
class logger;
}

namespace stride {

class Simulator;

/**
 * Builds the contact pool system  for the simulator.
 */
class ContactPoolBuilder
{
public:
        /// Initializing ContactPoolBuilder.
        explicit ContactPoolBuilder(std::shared_ptr<spdlog::logger> logger);

        /// Build the contact pool system.
        void Build(const boost::property_tree::ptree& pt_contact, std::shared_ptr<Simulator> sim);

private:
        /// Initialize the contactpoolss.
        static void InitializeContactPools(ContactPoolSys& pool_sys, std::shared_ptr<Simulator> sim);

private:
        std::shared_ptr<spdlog::logger> m_stride_logger; ///< Stride run logger (!= contact_logger).
};

} // namespace stride
