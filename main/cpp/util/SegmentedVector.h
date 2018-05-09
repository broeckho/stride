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
 * Interface and implementation for SegmentedVector class
 */

#include "SVIterator.h"

#include <array>
#include <cassert>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace stride {
namespace util {

/**
 * Container that stores objects "almost contiguously" and guarantees that
 * pointers/iterators are not invalidated when the container grows, either
 * through push_back/emplace_back of elements or resevation of capacity.
 * It combines vector properties (high data locality) with queue properties
 * (can increase capacity without pointer/iterator invalidation). Actually,
 * its implementation is much like a queue but with a limited interface
 * e.g. no insertions. The reason for the SegmentedVector is that one cannot
 * control the block size for std:queue (where it is small) and we need that
 * control to make the block size flexible and rather large.
 *
 * Template parameters:
 * 	T   type of elements stored in the container
 * 	N   block size i.e. number of elements per block
 */
template <typename T, size_t N = 512>
class SegmentedVector
{
public:
        // ==================================================================
        // Member types
        // ==================================================================
        using value_type     = T;
        using size_type      = std::size_t;
        using self_type      = SegmentedVector<T, N>;
        using iterator       = SVIterator<T, N, T*, T&, false>;
        using const_iterator = SVIterator<T, N>;

        // ==================================================================
        // Construction / Copy / Move / Destruction
        // ==================================================================

        /// Construct empty SegmentedVector.
        explicit SegmentedVector() : m_blocks(), m_size(0) {}

        /// Copy constructor (copies elements & capacity). If excess capacity
        /// not needed, do shrink_to_fit.
        SegmentedVector(const self_type& other) : m_blocks(), m_size(0)
        {
                reserve(other.capacity());
                for (const auto& elem : other) {
                        push_back(elem);
                }
                assert(m_size == other.m_size);
                assert(m_blocks.size() == other.m_blocks.size());
                assert(this->capacity() == other.capacity());
        }

        /// Move constructor (moves elements & capacity). If excess capacity
        /// not needed, do shrink_to_fit.
        SegmentedVector(self_type&& other) noexcept : m_blocks(std::move(other.m_blocks)), m_size(other.m_size)
        {
                other.m_size = 0;
        }

        /// Copy assignment (copies elements & capacity). If excess capacity
        /// not needed, do shrink_to_fit.
        SegmentedVector& operator=(const self_type& other)
        {
                if (this != &other) {
                        clear();
                        reserve(other.capacity());
                        for (const auto& elem : other) {
                                push_back(elem);
                        }
                        assert(m_size == other.m_size);
                        assert(m_blocks.size() == other.m_blocks.size());
                        assert(this->capacity() == other.capacity());
                }
                return *this;
        }

        /// Move assignment (copies elements & capacity). If excess capacity
        /// not needed, do shrink_to_fit.
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
                const size_t b = pos / N;
                const size_t i = pos % N;
                return *static_cast<T*>(static_cast<void*>(&(m_blocks[b][i])));
        }

        /// Access specified element with bounds checking.
        const T& at(std::size_t pos) const
        {
                if (pos >= m_size) {
                        throw std::out_of_range("CompactStorage: index out of range.");
                }
                const size_t b = pos / N;
                const size_t i = pos % N;
                return *static_cast<const T*>(static_cast<const void*>(&(m_blocks[b][i])));
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
        iterator end() { return iterator(iterator::m_end, this); }

        /// Returns a const_iterator to the end of the container.
        const_iterator end() const { return const_iterator(const_iterator::m_end, this); }

        /// Returns a const_iterator to the end.
        const_iterator cend() const { return const_iterator(const_iterator::m_end, this); }

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

        /// Allocates aditional blocks to achieve requested capacity.
        void reserve(size_type new_capacity)
        {
                while (new_capacity > capacity()) {
                        m_blocks.push_back(new Chunk[N]);
                }
        }

        /// Deallocates (empty) blocks to schrink capacity to fit current size.
        void shrink_to_fit()
        {
                size_type req_blocks = 1 + (size() - 1) / N;
                while (req_blocks < m_blocks.size()) {
                        delete[] m_blocks.back();
                        m_blocks.pop_back();
                }
        }
        /// Clears the content.
        void clear()
        {
                for (auto& i : *this) {
                        i.~T();
                }
                for (auto p : m_blocks) {
                        delete[] p;
                }
                m_blocks.clear();
                m_size = 0;
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
        friend class SVIterator<T, N>;
        friend class SVIterator<T, N, T*, T&, false>;

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

} // namespace util
} // namespace stride
