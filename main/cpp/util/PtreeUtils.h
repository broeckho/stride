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
 *  Copyright 2017, 2018 Willem L, Kuylen E, Stijven S & Broeckhove J
 *
 *  This code is adapted for this project from code by
 *  Renato Florentino Garcia. His copyright also applies.
 *  Copyright (C) 2011 Renato Florentino Garcia
 *
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file BOOST_LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *  For more information, see http://www.boost.org
 */

#pragma once

//#include <boost/foreach.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/property_tree/ptree.hpp>
#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>

namespace stride {
namespace util {

namespace detail {

template <class T, class Ptree>
struct pair2data : public std::unary_function<const typename Ptree::value_type, T>
{
        T operator()(const typename Ptree::value_type& pair) const { return pair.second.template get_value<T>(); }
};

} // namespace detail

template <class Sequence>
Sequence ToSequence(const boost::property_tree::ptree& ptree)
{
        namespace bpt            = boost::property_tree;
        using Concrete_pair2data = detail::pair2data<typename Sequence::value_type, bpt::ptree>;
        using Iterator           = boost::iterators::transform_iterator<Concrete_pair2data, bpt::ptree::const_iterator>;

        const Iterator begin(ptree.begin(), Concrete_pair2data());
        const Iterator end(ptree.end(), Concrete_pair2data());
        return Sequence(begin, end);
}

template <class T, std::size_t N>
std::array<T, N> ToArray(const boost::property_tree::ptree& ptree)
{
        namespace bpt            = boost::property_tree;
        using Concrete_pair2data = detail::pair2data<T, bpt::ptree>;
        using Iterator           = boost::iterators::transform_iterator<Concrete_pair2data, bpt::ptree::const_iterator>;

        if (ptree.size() != N) {
                throw std::range_error("Array size error.");
        }
        const Iterator   begin(ptree.begin(), Concrete_pair2data());
        const Iterator   end(ptree.end(), Concrete_pair2data());
        std::array<T, N> tmpArray;
        std::copy(begin, end, tmpArray.begin());
        return tmpArray;
}

template <class Ptree>
Ptree Merge(const Ptree& pt1, const Ptree& pt2)
{
        Ptree result;
        for (const typename Ptree::value_type& v : pt1) {
                result.add_child(v.first, v.second);
        }
        for (const typename Ptree::value_type& v : pt2) {
                result.add_child(v.first, v.second);
        }
        return result;
}

} // namespace util
} // namespace stride
