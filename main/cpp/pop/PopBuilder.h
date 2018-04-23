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
#include <spdlog/spdlog.h>

namespace stride {

class Population;

/**
 * Initializes Population objects.
 */
class PopBuilder
{
public:
        /// @param configPt      Property_tree with general configuration settings.
        explicit PopBuilder(const boost::property_tree::ptree& configPt, std::shared_ptr<spdlog::logger> logger);

        /// Initializes a Population.
        /// @return              Pointer to the population.
        std::shared_ptr<Population> Build();

private:
        ///
        void MakePoolSys();

        ///
        void MakePersons();

        ///
        void Preliminaries();

private:
        boost::property_tree::ptree m_config_pt;   ///< Configuration property tree
        unsigned int                m_num_threads; ///< The number of (OpenMP) threads.
        std::shared_ptr<Population> m_pop;

        util::RNManager m_rn_manager; ///< Random numbere generation management.

        std::shared_ptr<spdlog::logger> m_stride_logger; ///< Stride run logger.
};

} // namespace stride
