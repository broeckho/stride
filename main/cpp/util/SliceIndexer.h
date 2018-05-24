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
 * Interface/Implementation for RangeIndexer.
 */

#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

namespace stride {
namespace util {

/**
 * Datastructure to index a container that supports RandomAccessIterators with slices.
 * @tparam T    Type of the container in whom the slices are indexed
 */
template <typename T>
class SliceIndexer
{
public:
        using range_type = boost::sliced_range<T>;

public:
        /// SliceIndexer holds a reference to the conatiner that it indexes.
        explicit SliceIndexer(T& t) : m_t(t) {}

        /// Set a range. Warning: range is [ibegin, iend) i.e. half-open, iend not included!
        range_type& SetSlice(std::size_t ibegin, std::size_t iend, const std::string& name)
        {
                if (m_map.find(name) != m_map.end()) {
                        throw std::range_error("SliceIndexer::SetSlice> Name is a duplicate: " + name);
                } else {
                        m_ranges.emplace_back(range_type(m_t, ibegin, iend));
                        m_map[name] = m_ranges.size() - 1;
                }
                return m_ranges.back();
        }

        /// Set a range, where the end is the end of the container.
        range_type& SetSlice(std::size_t ibegin, const std::string& name)
        {
                if (m_map.find(name) != m_map.end()) {
                        throw std::range_error("SliceIndexer::SetSlice> Name is a duplicate: " + name);
                } else {
                        m_ranges.emplace_back(range_type(m_t, ibegin, m_t.size()));
                        m_map[name] = m_ranges.size() - 1;
                }
                return m_ranges.back();
        }

        /// Retrieve reference to a slice by its subscipt in the indexer.
        range_type& GetSlice(std::size_t i) { return m_ranges.at(i); }

        /// Retrieve reference to a slice by its name.
        range_type& GetSlice(const std::string& s)
        {
                auto i = m_map.at(s);
                return m_ranges.at(i);
        }

private:
        std::map<std::string, std::size_t> m_map;
        std::vector<range_type>            m_ranges;
        T&                                 m_t;
};

//-----------------------
// Helpers
//-----------------------

template <typename T>
SliceIndexer<T> make_slice_indexer(T& t)
{
        return SliceIndexer<T>(t);
}

} // namespace util
} // namespace stride
