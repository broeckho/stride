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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "Generator.h"

#include "util/LogUtils.h"

namespace geopop {

Generator::Generator(stride::util::RnMan rnMan, std::shared_ptr<spdlog::logger> logger)
    : m_rn_man(std::move(rnMan)), m_logger(std::move(logger))
{
        if (!m_logger)
                m_logger = stride::util::LogUtils::CreateNullLogger();
}

} // namespace geopop
