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
        /// \param config        Property_tree with general configuration settings.
        /// \param rnMan         Random number manager for pop build process.
        /// \param strideLogger  Logging.
        AbstractPopBuilder(const boost::property_tree::ptree& config, util::RnMan& rnMan,
                           std::shared_ptr<spdlog::logger> strideLogger = nullptr);

        /// Has to be virtual.
        virtual ~AbstractPopBuilder() = default;

        /// Build Population and return it afterwards.
        virtual std::shared_ptr<Population> Build(std::shared_ptr<Population> pop) = 0;

protected:
        const boost::property_tree::ptree& m_config;        ///< Configuration property tree.
        util::RnMan&                       m_rn_man;        ///< Random number generation management.
        std::shared_ptr<spdlog::logger>    m_stride_logger; /// Logger for build process.
};

} // namespace stride
