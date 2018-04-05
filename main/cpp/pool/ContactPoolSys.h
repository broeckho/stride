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
 * Container for the contact pools of various type (household, work, ...)..
 */

#include "pool/ContactPool.h"
#include "pool/ContactPoolType.h"
#include "pool/IdSubscriptArray.h"

namespace stride {

/// ContactPoolSys contains for each of the type of pools (household, school, ...)
/// a vector of with all of the contcatpools of the type.
/// The ContactPoolSys container is an std::array extended to be subscriptable
/// with the enum class of the pool types.
using ContactPoolSys = ContactPoolType::IdSubscriptArray<std::vector<ContactPool>>;

} // namespace stride
