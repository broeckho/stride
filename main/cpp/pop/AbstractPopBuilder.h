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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Base Class for PopBuilders
 */

#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <spdlog/logger.h>

namespace stride {

class Population;
namespace util {
class RnMan;
}

/**
 * Base Class for PopBuilders.
 */
class AbstractPopBuilder
{
public:
        /// Initializing constructor.
        /// \param configPt    Property_tree with general configuration settings.
        /// \param rnManager   Random number manager for pop build process.
        AbstractPopBuilder(const boost::property_tree::ptree& configPt, util::RnMan& rnManager,
                           std::shared_ptr<spdlog::logger> stride_logger = nullptr)
            : m_config_pt(configPt), m_rn_manager(rnManager), m_stride_logger(std::move(stride_logger))
        {
        }

        /// Build Population and return it afterwards.
        virtual std::shared_ptr<Population> Build(std::shared_ptr<Population> pop) = 0;

        virtual ~AbstractPopBuilder() = default;

protected:
        const boost::property_tree::ptree& m_config_pt;     ///< Configuration property tree.
        util::RnMan&                       m_rn_manager;    ///< Random number generation management.
        std::shared_ptr<spdlog::logger>    m_stride_logger; /// Logger for build process.
};

} // namespace stride
