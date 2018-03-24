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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Definition of ContactPoolType.
 */

#include <cstddef>
#include <string>

namespace stride {
namespace ContactPoolType {

/// Enumerates the ContactPool types.
enum class Id
{
        Household,
        School,
        Work,
        PrimaryCommunity,
        SecondaryCommunity,
};

/// Number of ContactPool types.
inline constexpr unsigned int NumOfTypes() { return 5U; }

/// Check whether string is name of a ContactPoolType::Id.
bool IsType(const std::string& s);

/// Cast to size_t for indexing.
inline std::size_t ToSizeT(Id id) { return static_cast<std::size_t>(id); }

/// Converts a ContactPoolType::Id value to corresponding name.
std::string ToString(Id w);

/// Converts a string with name to Id.
Id ToType(const std::string& s);

} // namespace ContactPoolType
} // namespace stride
