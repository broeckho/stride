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
 * Interface and implementation for SegmentedVector class
 */

#pragma once

#include "SVIterator.h"

#include <array>
#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace stride {
namespace util {

/**
 * Container that stores objects "almost contiguously" (in a chain of blocks)
 * and guarantees that pointers/iterators are not invalidated when the container
 * grows. Elements are assigned to the container either sequentially through
 * push_back and emplace_back or through direct adressing with emplace, at or
 * the subscript operator.
 * It supports most familiar operators except reserve (no need for advance
 * reservation of capacity to avoid re-allocation that invalidates pointers as
 * in std::vector) and insertion/deletion.
 *
 * Template parameters:
 * 	T   type of elements stored in the container
 * 	N   block size i.e. number of elements per block
 */
template <typename T, size_t N = 512, bool Safe = true>
class SegmentedVector
{
public:
        // ==================================================================
        // Member types
        // ==================================================================
        using value_type     = T;
        using size_type      = std::size_t;
        using self_type      = SegmentedVector<T, N>;
        using iterator       = SVIterator<T, N, Safe, T*, T&, false>;
        using const_iterator = SVIterator<T, N, Safe>;

        // ==================================================================
        // Construction / Copy / Move / Destruction
        // ==================================================================

        /// Construct empty SegmentedVector.
        /// CAVEAT: if you resize itbut do not subsequently initialize all
        /// elements, the SegmentedVector destructor or a call to clear will cause a segmentation
        /// fault because of the destructor call on unitilialezed elements.
        explicit SegmentedVector() : m_blocks(), m_size(0) {}

        /// Construct with given number of elements but DO NOT INITIALIZE them.
        /// CAVEAT: if you resize (as you do here) but do not subsequently initialize all
        /// elements, the SegmentedVector destructor or a call to clear will cause a segmentation
        /// fault because of the destructor call on unitilialezed elements.
        explicit SegmentedVector(size_type i) : m_blocks(), m_size(0) { resize(i); }

        /// Construct with given number of elements and INITIALIZE them with value.
        /// CAVEAT: if you resize (as you do here) but do not subsequently initialize all
        /// elements, the SegmentedVector destructor or a call to clear will cause a segmentation
        /// fault because of the destructor call on unitilialezed elements.
        explicit SegmentedVector(size_type i, const value_type& value) : m_blocks(), m_size(0) { resize(i, value); }

        /// Copy constructor.
        explicit SegmentedVector(const self_type& other) : m_blocks(), m_size(0)
        {
                for (const auto& elem : other) {
                        push_back(elem);
                }
                assert(m_size == other.m_size);
                assert(m_blocks.size() == other.m_blocks.size());
        }

        /// Move constructor.
        explicit SegmentedVector(self_type&& other) noexcept : m_blocks(std::move(other.m_blocks)), m_size(other.m_size)
        {
                other.m_size = 0;
        }

        /// Copy assignment.
        SegmentedVector& operator=(const self_type& other)
        {
                if (this != &other) {
                        clear();
                        for (const auto& elem : other) {
                                push_back(elem);
                        }
                        assert(m_size == other.m_size);
                        assert(m_blocks.size() == other.m_blocks.size());
                }
                return *this;
        }

        /// Move assignment.
        SegmentedVector& operator=(self_type&& other) noexcept
        {
                if (this != &other) {
                        clear();
                        m_blocks = std::move(other.m_blocks);
                        std::swap(m_size, other.m_size);
                }
                return *this;
        }

        /// Destructor
        ~SegmentedVector() { clear(); }

        // ==================================================================
        // Element access
        // ==================================================================

        /// Access specified element with bounds checking.
        T& at(std::size_t pos)
        {
                if (pos >= m_size) {
                        throw std::out_of_range("CompactStorage: index out of range.");
                }
                return *static_cast<T*>(static_cast<void*>(&(m_blocks[pos / N][pos % N])));
        }

        /// Access specified element with bounds checking.
        const T& at(std::size_t pos) const
        {
                if (pos >= m_size) {
                        throw std::out_of_range("CompactStorage: index out of range.");
                }
                return *static_cast<const T*>(static_cast<const void*>(&(m_blocks[pos / N][pos % N])));
        }

        /// Access the last element.
        T& back() { return *static_cast<T*>(static_cast<void*>(&m_blocks[(m_size - 1) / N][(m_size - 1) % N])); }

        /// Access the last element.
        const T& back() const
        {
                return *static_cast<const T*>(static_cast<const void*>(&m_blocks[(m_size - 1) / N][(m_size - 1) % N]));
        }

        /// Access specified element (no bounds checking).
        T& operator[](size_t pos) { return *static_cast<T*>(static_cast<void*>(&(m_blocks[pos / N][pos % N]))); }

        /// Access specified element (no bounds checking).
        const T& operator[](size_t pos) const
        {
                return *static_cast<const T*>(static_cast<const void*>(&(m_blocks[pos / N][pos % N])));
        }

        // ==================================================================
        // Iterators
        // ==================================================================

        /// Returns an iterator to the beginning of the container.
        iterator begin() { return (m_size == 0) ? end() : iterator(0, this); }

        /// Returns a const_iterator to the beginning of the container.
        const_iterator begin() const { return (m_size == 0) ? end() : const_iterator(0, this); }

        /// Returns a const_iterator to the beginning of the container.
        const_iterator cbegin() const { return (m_size == 0) ? end() : const_iterator(0, this); }

        /// Returns an iterator to the end of the container.
        iterator end() { return iterator(size(), this); }

        /// Returns a const_iterator to the end of the container.
        const_iterator end() const { return const_iterator(size(), this); }

        /// Returns a const_iterator to the end.
        const_iterator cend() const { return const_iterator(size(), this); }

        // ==================================================================
        // Capacity
        // ==================================================================

        /// Returns number of elements that can be stored without allocating additional blocks.
        std::size_t capacity() const { return N * m_blocks.size(); }

        /// Checks whether container is empty.
        bool empty() const { return m_size == 0; }

        /// Returns number of currently allocated blocks.
        std::size_t get_block_count() const { return m_blocks.size(); }

        /// Returns number of elements block (template parameter 'N').
        std::size_t get_elements_per_block() const { return N; }

        /// Returns the number of elements.
        std::size_t size() const { return m_size; }

        // ==================================================================
        // Modifiers
        // ==================================================================

        /// Increases the number of elements (but DOES NOT INITIALIZE the additional
        /// elements) or pops elements (and DOES RUN those element's destructor).
        /// CAVEAT: if you resize (as you do here) but do not subsequently initialize all
        /// elements, the SegmentedVector destructor or a call to clear will cause a segmentation
        /// fault because of the destructor call on unitilialezed elements.
        void resize(size_type new_size)
        {
                if (new_size < size()) {
                        if (Safe) {
                                for (size_type i = m_size - 1; new_size - 1 < i; --i) {
                                        pop_back();
                                }
                        } else {
                                const size_type new_block_count = 1 + (new_size - 1) / N;
                                while (new_block_count < get_block_count()) {
                                        delete[] m_blocks[m_blocks.size() - 1];
                                        m_blocks.pop_back();
                                }
                                m_size = new_size;
                        }
                } else if (new_size > size()) {
                        if (Safe) {
                                for (size_type i = size(); i < new_size; ++i) {
                                        push_back(std::move(T()));
                                }
                        } else {
                                while (new_size > capacity()) {
                                        m_blocks.push_back(new Chunk[N]);
                                }
                                m_size = new_size;
                        }
                }
                assert((size() == new_size));
                assert((size() <= capacity()));
        }

        void resize(size_type new_size, const value_type& value)
        {
                if (new_size < size()) {
                        resize(new_size);
                } else if (new_size > size()) {
                        for (size_type i = size(); i < new_size; ++i) {
                                push_back(value);
                        }
                }
                assert((size() == new_size));
                assert((size() <= capacity()));
        }

        /// Clears the content.
        void clear()
        {
                if (Safe) {
                        for (auto& i : *this) {
                                i.~T();
                        }
                }
                for (auto p : m_blocks) {
                        delete[] p;
                }
                m_blocks.clear();
                m_size = 0;
                assert(size() == 0);
                assert(m_blocks.size() == 0);
        }

        /// Constructs element in-place at position pos.
        template <class... Args>
        T* emplace(size_type pos, Args&&... args)
        {
                assert(0 <= pos && pos < m_size);
                T* memory = static_cast<T*>(static_cast<void*>(&(m_blocks[pos / N][pos % N])));
                return new (memory) T(std::forward<Args>(args)...); // construct new object
        }

        /// Constructs element in-place at the end.
        template <class... Args>
        T* emplace_back(Args&&... args)
        {
                T* memory = this->get_chunk();
                return new (memory) T(std::forward<Args>(args)...); // construct new object
        }

        /// Removes the last element.
        void pop_back()
        {
                // No pop on empty container.
                if (m_size <= 0) {
                        throw std::logic_error("CompactStorage::pop_back called on empty object.");
                }

                // Element destruction.
                at(m_size - 1).~T();
                --m_size;

                // If tail block vacated, release it.
                const size_t last_block_index = m_blocks.size() - 1;
                if (m_size <= last_block_index * N) {
                        delete[] m_blocks[last_block_index];
                        m_blocks.pop_back();
                }
        }

        /// Adds element to end.
        T* push_back(const T& obj)
        {
                T* memory = get_chunk();
                return new (memory) T(obj); // copy-construct new object
        }

        /// Adds element to end.
        T* push_back(T&& obj)
        {
                T* memory = get_chunk();
                return new (memory) T(std::move(obj)); // move-construct new object
        }

private:
        /// POD type with same alignment requirement as for T's.
        using Chunk = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;

private:
        friend class SVIterator<T, N, Safe>;
        friend class SVIterator<T, N, Safe, T*, T&, false>;

private:
        /// Get next available chunk for element construction with placement new.
        T* get_chunk()
        {
                const size_t b = m_size / N; // Index of block in vector m_blocks
                const size_t i = m_size % N; // Offset of chunk within its block

                if (b == m_blocks.size()) { // Out of buffers, last buffer is full
                        auto chunk = new Chunk[N];
                        m_blocks.push_back(chunk);
                }
                ++m_size;
                return static_cast<T*>(static_cast<void*>(&((m_blocks[b])[i])));
        }

private:
        std::vector<Chunk*> m_blocks; ///< Vector registers pointers to blocks of chunks.
        size_t              m_size;   ///< Index of first free chunk when indexed contiguously.
};

/**
 * Helper function for equality test (equal size and all elements equal).
 */
template <typename T, size_t N = 512, bool Safe = true>
inline bool operator==(const SegmentedVector<T, N, Safe>& lhs, const SegmentedVector<T, N, Safe>& rhs)
{
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

/**
 * Helper function for inequality test (unequal size or not all elements equal).
 */
template <typename T, size_t N = 512, bool Safe = true>
inline bool operator!=(const SegmentedVector<T, N, Safe>& lhs, const SegmentedVector<T, N, Safe>& rhs)
{
        return !operator==(lhs, rhs);
}

} // namespace util
} // namespace stride
