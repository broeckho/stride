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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Tobia De Koninck, Robin Jadoul
 */

/**
 * @file
 * Base Class for PopBuilders
 */

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <spdlog/logger.h>
#include <util/RnMan.h>

namespace stride {

class Population;
namespace util {
}

/**
 * Base Class for PopBuilders
 */
class AbstractPopBuilder
{
public:
        /// Initializing constructor.
        /// \param configPt    Property_tree with general configuration settings.
        /// \param rnManager   Random number manager for pop build process.
        AbstractPopBuilder(const boost::property_tree::ptree& configPt, util::RnMan& rnManager);

        /// Build Population and return it afterwards.
        virtual std::shared_ptr<Population> Build(std::shared_ptr<Population> pop) = 0;

        virtual ~AbstractPopBuilder() = default;

protected:
        const boost::property_tree::ptree& m_config_pt;  ///< Configuration property tree.
        util::RnMan&                       m_rn_manager; ///< Random number generation management.
};

} // namespace stride
