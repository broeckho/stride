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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "contact/ContactType.h"


namespace geopop {

/**
 * Config params for ContactPools (constants for now at least):
 * <people> is the number of Persons per unit (college, workplace, etc.),
 * while <pools> is the number of ContactPools per unit.
 */
template<stride::ContactType::Id ID>
struct PoolParams;

template<>
struct PoolParams<stride::ContactType::Id::Household>
{
        static constexpr unsigned int pools = 1U;
};

template<>
struct PoolParams<stride::ContactType::Id::K12School>
{
        static constexpr unsigned int people = 500U;
        static constexpr unsigned int pools = 25U;
};

template<>
struct PoolParams<stride::ContactType::Id::College> {
        static constexpr unsigned int people = 3000U;
        static constexpr unsigned int pools = 20U;
};

template<>
struct PoolParams<stride::ContactType::Id::PrimaryCommunity> {
        static constexpr unsigned int people = 2000U;
        static constexpr unsigned int pools = 1U;
};

template<>
struct PoolParams<stride::ContactType::Id::SecondaryCommunity> {
        static constexpr unsigned int people = 2000U;
        static constexpr unsigned int pools = 1U;
};

template<>
struct PoolParams<stride::ContactType::Id::Workplace> {
        static constexpr unsigned int people = 20U;
        static constexpr unsigned int pools = 1U;
};

} // namespace geopop
