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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Definition of ContactPool Id Type.
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace stride {
namespace ContactType {

/// Enumerates the ContactPool types.
enum class Id : unsigned int
{
        Household = static_cast<unsigned int>(0), // Needs to be zero.
        K12School,
        College,
        Workplace,
        PrimaryCommunity,
        SecondaryCommunity,
};

/// Number of ContactPool types.
inline constexpr unsigned int NumOfTypes() { return 6U; }

/// Check whether string is name of a ContactPoolType::Id.
bool IsId(const std::string& s);

/// Converts a string with name to Id.
Id ToId(const std::string& s);

/// Cast to size_t for indexing.
inline constexpr std::size_t ToSizeT(Id id) { return static_cast<std::size_t>(id); }

/// Converts a ContactPoolType::Id value to corresponding name.
std::string ToString(Id w);

/// Keeps all available Ids in a template pack and exposes it as a std::initialize_list
/// Useful when initializing a IdSubscriptArray with a type that is not default constructible
template <Id... ids>
struct IDPack
{
        constexpr static std::initializer_list<Id> AsInitializerList = {ids...}; ///< Exposed as std::initializer_list
};

/// Placed separately to please swig and avoid syntax errors there
using IdPack_t =
    IDPack<Id::Household, Id::K12School, Id::College, Id::Workplace, Id::PrimaryCommunity, Id::SecondaryCommunity>;

/// A constexpr global variable that gives access to the available Ids
constexpr IdPack_t IdPack;

/// To allow iteration over the type ids.
constexpr std::initializer_list<Id> IdList = IdPack_t::AsInitializerList;

} // namespace ContactType
} // namespace stride
