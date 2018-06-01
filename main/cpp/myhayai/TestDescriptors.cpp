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
 * Implementation file for TestDescriptors container.
 */

#include "TestDescriptors.h"

#include <algorithm>

namespace myhayai {

size_t TestDescriptors::CountDisabled()
{
        return static_cast<size_t>(
            count_if(this->begin(), this->end(), [](const auto& item) { return item.second.m_is_disabled; }));
}

} // namespace myhayai
