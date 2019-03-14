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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Initialize populations: implementation.
 */

#include "AbstractPopBuilder.h"

#include "util/LogUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <spdlog/common.h>

namespace stride {

using namespace std;
using namespace boost::property_tree;

AbstractPopBuilder::AbstractPopBuilder(const boost::property_tree::ptree& config, util::RnMan& rnMan,
                                       std::shared_ptr<spdlog::logger> strideLogger)
    : m_config(config), m_rn_man(rnMan), m_stride_logger(std::move(strideLogger))
{
        if (!m_stride_logger) {
                m_stride_logger = util::LogUtils::CreateNullLogger("PopBuilder_logger");
        }
}

} // namespace stride
