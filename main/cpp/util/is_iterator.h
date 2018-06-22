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
 *  Author bistromatics/Robin Jadoul.
 */

/**
 * @file
 * Interface/Implementation of is_iterator.
 */

#include <iterator>
#include <type_traits>

namespace stride {
namespace util {

// This ought to be redundant, but ApplClang does not have the std::void_t yet so ...
namespace ii_detail {
template <typename... Ts>
struct make_void
{
        typedef void type;
};
template <typename... Ts>
using void_t = typename make_void<Ts...>::type;
} // namespace ii_detail

/// When it 's not an iterator.
template <class T, class = void>
struct is_iterator : std::false_type
{
};

/// When it is an iterator.
template <class T>
struct is_iterator<T, ii_detail::void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type
{
};

} // namespace util
} // namespace stride
