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
 * Interface/Implementation for SVIterator.
 */

#pragma once

#include <cassert>
#include <cmath>
#include <iterator>
#include <type_traits>

namespace stride {
namespace util {

template <typename T, std::size_t N, bool Safe>
class SegmentedVector;

/**
 * Implementation of iterator for SegmentedVector. It will provide
 * both const and non-const iterators.
 *
 * Possible states for the iterator are:
 * (a) Default constructed: m_c == nullptr && m_p == 0. This is
 * the singular state in which the iterator can be assigned, but not
 * incremented or compared.
 * (b) Past-the-end: m_c != nullptr && m_p == m_c->size(). The iterator
 * cannot be dereferenced.
 * (c) Dereferencable: m_c != nullptr && m_p < mc->size(). Notice that
 * m_p is of type size_t and hence always non-negative. Thus the above
 * reuires !m_c->empty().
 *
 * Template parameters:
 * T                    value type of iterator and of its container.
 * N                    block size of its container
 * P                    pointer-to-T type (can be const qualified).
 * R                    reference-to-T type (can be const qualified).
 * is_const_iterator	to make it a const_iterator
 */
template <typename T, std::size_t N, bool Safe, typename P = const T*, typename R = const T&,
          bool is_const_iterator = true>
class SVIterator
{
public:
        // C++17 deprecates std::iterator so we need to define these traits ourselves.
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = P;
        using reference         = R;

public:
        // ==================================================================
        // Member types (in addition to those introduced by the std::iterator
        // base class (i.e. value_type, difference_type, pointer, reference,
        // iterator_category).
        // ==================================================================
        using self_type = SVIterator<T, N, Safe, P, R, is_const_iterator>;

        // ==================================================================
        // Construction / Copy / Move / Destruction
        // ==================================================================
        /// Default constructor
        SVIterator() : m_p(0), m_c(nullptr) {}

        /// Copy constructor
        SVIterator(const self_type& other) : m_p(other.m_p), m_c(other.m_c) {}

        // ==================================================================
        // Bidirectional iterator methods
        // ==================================================================

        /// Element access.
        R operator*() const
        {
                // assert(m_c != nullptr && 0 <= m_p && m_p < m_c->m_size);
                assert(m_c != nullptr && 0 <= m_p && m_p < m_c->m_size);
                return *static_cast<T*>(static_cast<void*>(&(m_c->m_blocks[m_p / N][m_p % N])));
        }

        /// Member of element access.
        P operator->() const
        {
                assert(m_c != nullptr && 0 <= m_p && m_p < m_c->m_size);
                return static_cast<T*>(static_cast<void*>(&(m_c->m_blocks[m_p / N][m_p % N])));
        }

        /// Pre-increment (returns position after increment)
        self_type& operator++()
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                // defensive: m_p = std::max(++m_p, mc->m_size);
                ++m_p;
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                return *this;
        }

        /// Post-increment (returns position prior to increment)
        const self_type operator++(int)
        {
                self_type tmp(*this);
                          operator++();
                return tmp;
        }

        /// Pre-decrement (returns position after decrement)
        self_type& operator--()
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                // defensive: m_p = std::max(--m_p, 0);
                --m_p;
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                return *this;
        }

        /// Pre-increment (returns position after decrement)
        const self_type operator--(int)
        {
                self_type tmp(*this);
                          operator--();
                return tmp;
        }

        /// Iterator equality.
        bool operator==(const self_type& other) const { return (m_p == other.m_p) && (m_c == other.m_c); }

        /// Iterator inequality.
        bool operator!=(const self_type& other) const { return (m_p != other.m_p) || (m_c != other.m_c); }

        // ==================================================================
        // Random-Access iterator methods
        // ==================================================================

        /// Direct access to n-th element
        R operator[](std::size_t n) const
        {
                assert(m_c != nullptr && 0 <= m_p + n && m_p + n < m_c->m_size);
                return *static_cast<T*>(static_cast<void*>(&(m_c->m_blocks[(m_p + n) / N][(m_p + n) % N])));
        }

        /// Set iterator to n-th next element.
        self_type& operator+=(std::ptrdiff_t n)
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                // defensive: m_p = std::max((m_p+=n), mc->m_size);
                m_p += n;
                return *this;
        }

        /// Set iterator to n-th previous element.
        self_type& operator-=(std::ptrdiff_t n)
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                // defensive: m_p = std::max((m_p -= n), 0);
                m_p -= n;
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                return *this;
        }

        /// Return iterator pointing to n-th next element.
        self_type operator+(std::ptrdiff_t n)
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                assert(m_c != nullptr && 0 <= m_p + n && m_p + n <= m_c->m_size);
                return self_type(m_p + n, m_c);
        }

        /// Return iterator pointing to n-th previous element.
        self_type operator-(std::ptrdiff_t n)
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                assert(m_c != nullptr && 0 <= m_p - n && m_p - n <= m_c->m_size);
                return self_type(m_p - n, m_c);
        };

        /// Return distance between iterators.
        long int operator-(const self_type& other) const
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                assert(other.m_c != nullptr && 0 <= other.m_p && other.m_p <= other.m_c->m_size);
                return m_p - other.m_p;
        }

        /// Returns whether iterator is before other.
        bool operator<(const self_type& other) const
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                assert(other.m_c != nullptr && 0 <= other.m_p && other.m_p <= other.m_c->m_size);
                return m_p < other.m_p;
        }

        /// Returns whether iterator is not after other.
        bool operator<=(const self_type& other) const
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                assert(other.m_c != nullptr && 0 <= other.m_p && other.m_p <= other.m_c->m_size);
                return m_p <= other.m_p;
        }

        /// Returns whether iterator is after other.
        bool operator>(const self_type& other) const
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                assert(other.m_c != nullptr && 0 <= other.m_p && other.m_p <= other.m_c->m_size);
                return m_p > other.m_p;
        }

        /// Returns whether iterator is not after other.
        bool operator>=(const self_type& other) const
        {
                assert(m_c != nullptr && 0 <= m_p && m_p <= m_c->m_size);
                assert(other.m_c != nullptr && 0 <= other.m_p && other.m_p <= other.m_c->m_size);
                return m_p >= other.m_p;
        }

private:
        friend class SegmentedVector<T, N, Safe>;

private:
        /// Type of pointer-to-container (i.e. its const qualification).
        using container_pointer_type = typename std::conditional<is_const_iterator, const SegmentedVector<T, N, Safe>*,
                                                                 SegmentedVector<T, N, Safe>*>::type;

private:
        /// Private constructor, currently only container itself can create iterators.
        SVIterator(std::size_t p, container_pointer_type c) : m_p(p), m_c(c) {}

private:
        std::size_t            m_p; ///< Current iterator position in the container.
        container_pointer_type m_c; ///< Container that the iterator points into.
};

//------------------------------------
// Helpers
//------------------------------------
template <typename T, std::size_t N, bool Safe, typename P = const T*, typename R = const T&,
          bool is_const_iterator = true>
SVIterator<T, N, Safe, P, R, is_const_iterator> operator+(std::ptrdiff_t                                  i,
                                                          SVIterator<T, N, Safe, P, R, is_const_iterator> p)
{
        return p.operator+(i);
}

template <typename T, std::size_t N, bool Safe, typename P = const T*, typename R = const T&,
          bool is_const_iterator = true>
SVIterator<T, N, Safe, P, R, is_const_iterator> operator-(std::ptrdiff_t                                  i,
                                                          SVIterator<T, N, Safe, P, R, is_const_iterator> p)
{
        return p.operator-(i);
}

} // namespace util
} // namespace stride
