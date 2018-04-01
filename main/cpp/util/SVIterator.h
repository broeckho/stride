#pragma once
/*
 * Copyright 2011-2016 Universiteit Antwerpen
 *
 * Licensed under the EUPL, Version 1.1 or  as soon they will be approved by
 * the European Commission - subsequent versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl5
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the Licence is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
/**
 * @file
 * Interface/Implementation for SVIterator.
 */

#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <type_traits>

namespace stride {
namespace util {

template <typename T, size_t N>
class SegmentedVector;

/**
 * Implementation of iterator for SegmentedVector. It will provide
 * both const and non-const iterators.
 *
 * Possible states for the iterator are:
 * (a) Default constructed: m_c == nullptr && m_p == m_end. This is
 * the singular state in which the iterator can be assigned, but not
 * incremented or compared.
 * (b) Past-the-end: m_c != nullptr && m_p == m_end. The iterator
 * cannot be dereferenced.
 * (c) Dereferencable: m_c != nullptr && m_p < mc->size(). Notice that
 * m_p is of type size_t and hence always non-negative. Thus the above
 * reuires !m_c->empty().
 *
 * Template parameters:
 * 	T	value type of iterator and of its container.
 * 	N       block size of its container
 * 	P	pointer-to-T type (can be const qualified).
 * 	R	reference-to-T type (can be const qualified).
 * 	is_const_iterator	to make it a const_iterator
 */
template <typename T, std::size_t N, typename P = const T*, typename R = const T&, bool is_const_iterator = true>
class SVIterator : public std::iterator<std::random_access_iterator_tag, T, std::ptrdiff_t, P, R>
{
public:
        // ==================================================================
        // Member types (in addition to those introduced by the std::iterator
        // base class (i.e. value_type, difference_type, pointer, reference,
        // iterator_category).
        // ==================================================================
        using self_type = SVIterator<T, N, P, R, is_const_iterator>;

        // ==================================================================
        // Construction / Copy / Move / Destruction
        // ==================================================================
        /// Default constructor
        SVIterator() : m_p(m_end), m_c(nullptr) {}

        /// Copy constructor
        SVIterator(const self_type& other) : m_p(other.m_p), m_c(other.m_c) {}

        // ==================================================================
        // Bidirectional iterator methods
        // ==================================================================

        /// Element access.
        R operator*() const
        {
                assert(m_c != nullptr && m_p < m_c->size());
                size_t b = m_p / N; // index of buffer
                size_t i = m_p % N; // index in buffer b
                return *static_cast<T*>(static_cast<void*>(&(m_c->m_blocks[b][i])));
        }

        /// Member of element access.
        P operator->() const
        {
                assert(m_c != nullptr && m_p < m_c->size());
                size_t b = m_p / N; // index of buffer
                size_t i = m_p % N; // index in buffer b
                return static_cast<T*>(static_cast<void*>(&(m_c->m_blocks[b][i])));
        }

        /// Pre-increment (returns position after increment)
        self_type& operator++()
        {
                if (m_c != nullptr) { // This is a nullptr only when default constructed
                        if (m_p < m_c->m_size - 1)
                                ++m_p;
                        else
                                m_p = m_end;
                }
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
                if (m_c != nullptr) { // This is a nullptr only when default constructed
                        if (m_p > 0 && m_p != m_end)
                                --m_p;
                        else if (m_p == m_end)
                                m_p = m_c->m_size - 1;
                }
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
                assert(m_p + n != m_end);
                size_t b = (m_p + n) / N; // index of buffer
                size_t i = (m_p + n) % N; // index in buffer b
                return *static_cast<T*>(static_cast<void*>(&(m_c->m_blocks[b][i])));
        }

        /// Set iterator to n-th next element.
        self_type& operator+=(std::ptrdiff_t n)
        {
                m_p += n;
                if (m_p > m_c->m_size)
                        m_p = m_end;
                return *this;
        }

        /// Set iterator to n-th previous element.
        self_type& operator-=(std::ptrdiff_t n)
        {
                m_p -= n;
                if (m_p < 0)
                        m_p = m_end;
                return *this;
        }

        /// Return iterator pointing to n-th next element.
        self_type operator+(std::ptrdiff_t n) { return self_type(m_p + n, m_c); }

        /// Return iterator pointing to n-th previous element.
        //  self_type operator-(std::ptrdiff_t);

        /// Return distance between iterators.
        long int operator-(const self_type& other) const { return m_p - other.m_p; }

        /// Returns whether iterator is before other.
        bool operator<(const self_type& other) const { return m_p < other.m_p; }

        /// Returns whether iterator is not after other.
        bool operator<=(const self_type& other) const { return m_p <= other.m_p; }

        /// Returns whether iterator is after other.
        bool operator>(const self_type& other) const { return m_p > other.m_p; }

        /// Returns whether iterator is not after other.
        bool operator>=(const self_type& other) const { return m_p >= other.m_p; }

private:
        friend class SegmentedVector<T, N>;

private:
        /// Current iterator position in the container.
        std::size_t m_p;

        /// One past the last element iterator position.
        constexpr static std::size_t m_end = std::numeric_limits<size_t>::max();

private:
        /// Type of pointer-to-container (i.e. its const qualification).
        using container_pointer_type =
            typename std::conditional<is_const_iterator, const SegmentedVector<T, N>*, SegmentedVector<T, N>*>::type;

        // Container that the iterator points into.
        container_pointer_type m_c;

private:
        /// Private constructor, currently only container itself can create iterators.
        SVIterator(std::size_t p, container_pointer_type c) : m_p(p), m_c(c) {}

        /// See class description.
        bool IsDefaultContructed() { return m_c == nullptr && m_p == m_end; }

        /// See class description.
        bool IsPastTheEnd() { return m_c != nullptr && m_p == m_end; }

        /// See class description.
        bool IsDereferencable() { return m_c != nullptr && m_p < m_c->size(); }
};

} // namespace util
} // namespace stride
