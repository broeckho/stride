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
 * Datastructure to index a container that supports RandomAccessIterators with ranges.
 * We store those ranges in order in a vector and expose the vector because processing
 * effiency may dictate that we retrieve the in that order. They can also be retrieved
 * by identifier.
 * @tparam T    Type of the container in whom the ranges are indexed.
 * @tparam Id   Type of Id that can be useld to retrieve the ranges.
 */
template <typename T, typename Id = std::string>
class SliceIndexer
{
public:
        using range_type = boost::sliced_range<T>;

public:
        /// SliceIndexer holds a reference to the conatiner that it indexes.
        explicit SliceIndexer(T& t) : m_t(t) {}

        /// Retrieve reference to a range by its Id.
        range_type& Get(const Id& s) { return m_ranges.at(m_map.at(s)); }

        /// Retrieve const reference to a range by its Id.
        const range_type& Get(const Id& s) const { return m_ranges.at(m_map.at(s)); }

        /// Retrieve all the ranges,
        const std::vector<range_type>& Get() { return m_ranges; }

        /// Set a range. Warning: range is [ibegin, iend) i.e. half-open, iend not included!
        range_type& Set(std::size_t ibegin, std::size_t iend, const Id& name)
        {
                check(name);
                m_ranges.emplace_back(std::move(make_range(ibegin, iend)));
                m_map[name] = m_ranges.size() - 1;
                return m_ranges.back();
        }

        /// Set a range, where the end is the end of the container.
        range_type& Set(std::size_t ibegin, const Id& name)
        {
                check(name);
                m_ranges.emplace_back(std::move(make_range(ibegin, m_t.size())));
                m_map[name] = m_ranges.size() - 1;
                return m_ranges.back();
        }
private:
        /// Check Id map for duplicate; throw iff duplicate.
        void check(const Id& name)
        {
                if (m_map.find(name) != m_map.end()) {
                        throw std::range_error("Indexer::Set> Name is a duplicate: " + name);
                }
        }

        ///
        range_type make_range(std::size_t ibegin, std::size_t iend)
        {
                return range_type(m_t, ibegin, iend);
        }

private:
        std::map<Id, std::size_t> m_map;     ///< Maps Id values to subscripts.
        std::vector<range_type>  m_ranges;   ///< Contains the ranges.
        T&                       m_t;        ///< Refernec to container that gets sliced into ranges.
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
