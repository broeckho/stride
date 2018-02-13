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
 * Interface of Random Engine Type
 */

#include <string>

namespace stride {
namespace RNEngineType {

/// Random number engine type ids. We use a subset of engines provided by the trng library.
enum class Id
{
        lcg64,
        lcg64_shift,
        mrg2,
        mrg3,
        yarn2,
        yarn3,
};

/// Check whether type with name s exists.
bool IsType(std::string s);

/// Cast to size_t for indexing.
inline std::size_t ToSizeT(Id id) { return static_cast<std::size_t>(id); }

/// Convert a type id to corresponding name.
std::string ToString(Id b);

/// Converts a string with name to Id.
Id ToType(const std::string& s);

} // namespace RNEngineType
} // namespace stride
