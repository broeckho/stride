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
 * Contact profiles stored in simulator.
 */

#include "core/ContactPool.h"
#include "core/ContactPoolType.h"

namespace stride {

using ContactPoolSys = std::array<std::vector<ContactPool>, ContactPoolType::NumOfTypes()>;

/*
struct ContactPoolSys
{
        std::vector<ContactPool> m_households;
        std::vector<ContactPool> m_school_pools;
        std::vector<ContactPool> m_work_pools;
        std::vector<ContactPool> m_primary_community;
        std::vector<ContactPool> m_secondary_community;
};
*/

} // namespace stride
