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
#include <iterator>

namespace stride {
namespace util {

/**
 * Datastructure to index a container (supporting RandomAccessIterators) with slices.
 * We store those slices in order in a vector and expose the vector because processing
 * effiency may dictate that we retrieve the in that order. They can also be retrieved
 * by ket (key type defaults to std::string).
 * @tparam T     Type of the container in whom the ranges are indexed.
 * @tparam Key   Type of Id that can be useld to retrieve the ranges.
 */
template <typename T, typename Key = std::string>
class SliceIndexer
{
public:
        using range_type = boost::sliced_range<T>;

public:
        /// SliceIndexer holds a reference to the conatiner that it indexes.
        explicit SliceIndexer(T& t) : m_map(), m_slices(), m_t(t) {}

        /// Retrieve reference to a range by its key.
        range_type& Get(const Key& s) { return m_slices.at(m_map.at(s)); }

        /// Retrieve const reference to a range by its Id.
        const range_type& Get(const Key& s) const { return m_slices.at(m_map.at(s)); }

        /// Retrieve all the ranges,
        const std::vector<range_type>& Get() { return m_slices; }

        /// Set a range. Warning: range is [ibegin, iend) i.e. half-open, iend not included!
        range_type& Set(std::size_t ibegin, std::size_t iend, const Key& name)
        {
                check(name);
                assert((0 <= ibegin && iend <= iend && iend <= boost::size(m_t)) && "Bad subscript.");
                m_slices.emplace_back(range_type(m_t, ibegin, iend));
                m_map[name] = m_slices.size() - 1;
                return m_slices.back();
        }

        /// Set a range, where the end is the end of the container.
        range_type& Set(std::size_t ibegin, const Key& name)
        {
                check(name);
                assert(0 <= ibegin && ibegin <= boost::size(m_t) && "Bad subscript.");
                m_slices.emplace_back(range_type(m_t, ibegin, boost::size(m_t)));
                m_map[name] = m_slices.size() - 1;
                return m_slices.back();
        }

private:
        /// Check key map for duplicate; throw iff duplicate.
        void check(const Key& name)
        {
                if (m_map.find(name) != m_map.end()) {
                        throw std::range_error("Name is a duplicate: ");
                }
        }

private:
        std::map<Key, std::size_t> m_map;    ///< Maps key values to subscripts.
        std::vector<range_type>    m_slices; ///< Contains the slices.
        T&                         m_t;      ///< Refernec to container that gets sliced into ranges.
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
