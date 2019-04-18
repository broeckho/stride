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
 */

/**
 * @file
 * Unit tests of SegmentedVector.
 */

#include "util/SegmentedVector.h"

#include <gtest/gtest.h>
#include <ostream>
#include <stdexcept>

using namespace std;
using namespace stride::util;

namespace stride {
namespace tests {

namespace {
class TraceMemory
{
public:
        enum EventType
        {
                ALLOCATED,
                DEALLOCATED,
                COPIED,
                MOVED
        };

        using EventList = std::vector<EventType>;

        TraceMemory() : m_counter(0), m_event_list() {}

        void Allocated()
        {
                m_counter++;
                m_event_list.push_back(ALLOCATED);
        }
        void Copied()
        {
                m_counter++;
                m_event_list.push_back(COPIED);
        }
        void Deallocated()
        {
                m_counter--;
                m_event_list.push_back(DEALLOCATED);
        }
        void Moved() { m_event_list.push_back(MOVED); }

        int              GetCounter() const { return m_counter; }
        const EventList& GetEventList() const { return m_event_list; }

private:
        int       m_counter;
        EventList m_event_list;
};

class Base
{
public:
        virtual size_t Get1() const { return 0; }
        virtual ~Base() = default;
};

class Derived : public Base
{
public:
        size_t         Get1() const override { return 1; }
        size_t         Get2() const { return 2; }
        virtual size_t Get3() const { return 3; }
};

class TestType
{
public:
        TestType(int i, std::string str, TraceMemory& t) : m_array{nullptr}, m_i(i), m_str(std::move(str)), m_t(t)
        {
                m_t.Allocated();
                m_array = new int[10];
        }

        TestType(const TestType& other) : m_array{nullptr}, m_i(other.m_i), m_str(other.m_str), m_t(other.m_t)
        {
                m_t.Copied();
                m_array = new int[10];
        }

        TestType(TestType&& other) noexcept
            : m_array(other.m_array), m_i(other.m_i), m_str(std::move(other.m_str)), m_t(other.m_t)
        {
                m_t.Moved();
                other.m_array = nullptr;
        }

        TestType& operator=(const TestType& rhs) = delete;

        ~TestType()
        {
                if (m_array) {
                        m_t.Deallocated();
                        delete[] m_array;
                }
        }

        int*         m_array;
        int          m_i;
        std::string  m_str;
        TraceMemory& m_t;
};
} // namespace

template <class SegmentedVector>
void RunBasicOperationsTest(SegmentedVector& c, size_t size)
{
        const size_t block_size = c.get_elements_per_block();

        // Allocation of blocks when filling container.
        for (size_t i = 0; i < size; i++) {
                c.push_back(i);
                EXPECT_EQ(1 + i / block_size, c.get_block_count());
        }

        // De-allocation of blocks when emptying container.
        for (size_t i = 0; i < size; i++) {
                c.pop_back();
                auto num = c.size() / block_size;
                if (!c.empty() && (c.size() % block_size > 0))
                        ++num;
                EXPECT_EQ(num, c.get_block_count());
        }

        // Size function.
        for (size_t i = 0; i < size; i++) {
                c.push_back(i);
        }
        EXPECT_EQ(c.size(), size);

        // Iterator: sequential access.
        auto i = 0UL;
        for (auto j : c) {
                EXPECT_EQ(i, j);
                i++;
        }

        // Element access.
        for (size_t k = 0; k < size; k++) {
                EXPECT_EQ(c.at(k), k);
        }

        // Element access: bounds check.
        EXPECT_THROW(c.at(size), out_of_range);

        // Check if correct elements are in the same block */
        size_t how_long_ago_since_new_block = 0UL;
        for (size_t l = 0; l < size; l++) {
                if (&c[l] != &c[l - how_long_ago_since_new_block] + how_long_ago_since_new_block) {
                        EXPECT_EQ(how_long_ago_since_new_block % block_size, 0UL);
                        how_long_ago_since_new_block = 0;
                }
                how_long_ago_since_new_block++;
        }
}

TEST(UnitSegmentedVector, BasicOperations_128_0)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 128);
}

TEST(UnitSegmentedVector, BasicOperations_128_99)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 128);
}

TEST(UnitSegmentedVector, BasicOperations_128_128)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 128);
}

TEST(UnitSegmentedVector, BasicOperations_128_143)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 128);
}

TEST(UnitSegmentedVector, BasicOperations_260_0)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 260);
}

TEST(UnitSegmentedVector, BasicOperations_260_128)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 260);
}

TEST(UnitSegmentedVector, BasicOperations_260_143)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 260);
}

TEST(UnitSegmentedVector, BasicOperations_260_288)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 260);
}

TEST(UnitSegmentedVector, BasicOperations_12345_0)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 12345);
}

TEST(UnitSegmentedVector, BasicOperations_12345_1001)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 12345);
}

TEST(UnitSegmentedVector, BasicOperations_12345_14001)
{
        SegmentedVector<size_t, 128> c;
        RunBasicOperationsTest(c, 12345);
}

TEST(UnitSegmentedVector, CopyPushBack)
{
        TraceMemory trace;
        {
                SegmentedVector<TestType, 4> c;
                for (int i = 0; i < 10; i++) {
                        TestType t(i, "hello", trace);
                        c.push_back(t);
                }
                for (int i = 0; i < 10; i++) {
                        EXPECT_EQ(c[i].m_i, i);
                        EXPECT_EQ(c[i].m_str, "hello");
                }
        }

        EXPECT_EQ(0, trace.GetCounter());
        EXPECT_EQ(40UL, trace.GetEventList().size()); // 10 allocations, 10 copies, 20 deallocations
}

TEST(UnitSegmentedVector, MovePushBack)
{
        TraceMemory trace;
        {
                SegmentedVector<TestType, 4> c;
                for (int i = 0; i < 10; i++) {
                        TestType t(i, "hello", trace);
                        c.push_back(std::move(t));
                }

                for (int i = 0; i < 10; i++) {
                        EXPECT_EQ(c[i].m_i, i);
                        EXPECT_EQ(c[i].m_str, "hello");
                }
        }

        EXPECT_EQ(0, trace.GetCounter());
        EXPECT_EQ(30UL, trace.GetEventList().size()); // 10 allocations, 10 moves, 10 deallocations
}

TEST(UnitSegmentedVector, EmplaceBack)
{
        TraceMemory trace;
        {
                SegmentedVector<TestType, 4> c;
                for (int i = 0; i < 10; i++) {
                        c.emplace_back(i, "hello", trace);
                }

                for (int i = 0; i < 10; i++) {
                        EXPECT_EQ(c[i].m_i, i);
                        EXPECT_EQ(c[i].m_str, "hello");
                }
        }

        EXPECT_EQ(0, trace.GetCounter());
        EXPECT_EQ(20UL, trace.GetEventList().size()); // 10 allocations, 10 deallocations
}

TEST(UnitSegmentedVector, PushPop)
{
        SegmentedVector<char, 4> c;
        for (int i = 0; i < 100; i++) {
                c.push_back('a');
        }
        EXPECT_EQ(100UL, c.size());

        for (int i = 0; i < 100; i++) {
                c.pop_back();
        }
        EXPECT_EQ(0UL, c.size());

        for (int i = 0; i < 10; i++) {
                c.push_back(static_cast<char>('a' + i));
        }
        EXPECT_EQ(10UL, c.size());

        for (int i = 9; i >= 0; i--) {
                EXPECT_EQ('a' + i, c.back());
                c.pop_back();
        }
        EXPECT_EQ(0UL, c.size());
}

TEST(UnitSegmentedVector, CopyAssignment)
{
        TraceMemory trace_c;
        TraceMemory trace_d;
        {
                SegmentedVector<TestType, 4> c;
                for (int i = 0; i < 10; i++) {
                        c.push_back(TestType(i, "c", trace_c));
                }
                {
                        SegmentedVector<TestType, 4> d;
                        d.emplace_back(123, "d", trace_d);
                        d.emplace_back(1234, "d", trace_d);

                        // copy
                        d = c;

                        // d must now contain c's contents
                        EXPECT_EQ(10UL, d.size());
                        for (int i = 0; i < 10; i++) {
                                EXPECT_EQ(i, d[i].m_i);
                        }

                        // remove all elements from d
                        d.clear();
                        EXPECT_EQ(0UL, d.size());
                } // will now destroy d and all of its elements

                // c should not be affected when d is changed
                EXPECT_EQ(10UL, c.size());
                for (int i = 0; i < 10; i++) {
                        EXPECT_EQ(i, c[i].m_i);
                }
        } // will now destroy c and all of its elements

        EXPECT_EQ(0, trace_c.GetCounter());
        EXPECT_EQ(0, trace_d.GetCounter());
}

TEST(UnitSegmentedVector, CopyConstruct)
{
        SegmentedVector<int, 4> c;
        for (int i = 0; i < 100; i++) {
                c.push_back(i);
        }

        // copy construct
        SegmentedVector<int, 4> d(c);

        // d must contain c's contents
        EXPECT_EQ(100UL, d.size());
        for (int i = 0; i < 100; i++) {
                EXPECT_EQ(i, d[i]);
        }

        // remove all elements from d
        d.clear();
        EXPECT_EQ(0UL, d.size());

        // c should not be affected when d is changed
        EXPECT_EQ(100UL, c.size());
        for (int i = 0; i < 100; i++) {
                EXPECT_EQ(i, c[i]);
        }
}

TEST(UnitSegmentedVector, MoveConstruct)
{
        SegmentedVector<int, 4> c;
        for (int i = 0; i < 100; i++) {
                c.push_back(i);
        }
        const size_t cap = c.capacity();

        SegmentedVector<int, 4> d(std::move(c));
        EXPECT_EQ(0UL, c.size());
        EXPECT_EQ(100UL, d.size());
        EXPECT_EQ(0UL, c.capacity());
        EXPECT_EQ(cap, d.capacity());
        for (int i = 0; i < 100; i++) {
                EXPECT_EQ(i, d[i]);
        }
}

TEST(UnitSegmentedVector, MoveAssignment)
{
        SegmentedVector<int, 4> c;
        for (int i = 0; i < 100; i++) {
                c.push_back(i);
        }
        const size_t cap = c.capacity();

        SegmentedVector<int, 4> d;
        for (int i = 0; i < 20; i++) {
                d.push_back(5);
        }

        d = std::move(c);
        EXPECT_EQ(0UL, c.size());
        EXPECT_EQ(100UL, d.size());
        EXPECT_EQ(0UL, c.capacity());
        EXPECT_EQ(cap, d.capacity());
        for (int i = 0; i < 100; i++) {
                EXPECT_EQ(i, d[i]);
        }
}

TEST(UnitSegmentedVector, IndexOperator)
{
        SegmentedVector<int, 4> c;
        for (int i = 0; i < 100; i++) {
                c.push_back(i);
        }
        for (int i = 0; i < 100; i++) {
                EXPECT_EQ(i, c[i]);
        }
}

TEST(UnitSegmentedVector, IteratorForEmptyMBV)
{
        SegmentedVector<int, 4> c;
        auto                    it1 = c.begin();
        auto                    it2 = c.end();
        EXPECT_EQ(true, it1 == it2);
}

TEST(UnitSegmentedVector, Dereferencing)
{
        SegmentedVector<int, 4> c;
        for (int i = 0; i < 100; i++) {
                c.push_back(i);
        }
        for (auto it = c.begin(); it != c.end(); ++it) {
                *it += 1;
        }
        auto i = 1;
        for (auto it = c.cbegin(); it != c.cend(); ++it, ++i) {
                EXPECT_EQ(i, *it);
        }
}

TEST(UnitSegmentedVector, RangeBasedLoop)
{
        SegmentedVector<int, 4> c;
        for (int i = 0; i < 100; i++) {
                c.push_back(i);
        }
        auto i = 0;
        for (const auto& e : c) {
                EXPECT_EQ(i, e);
                ++i;
        }
}

TEST(UnitSegmentedVector, PolyNoPoly)
{
        SegmentedVector<Derived, 3> c;
        c.resize(4);
        for (int i = 0; i < 4; i++) {
                new (&c[i]) Derived();
        }
        for (auto& p : c) {
                EXPECT_EQ(2, p.Get2());
        }
}

TEST(UnitSegmentedVector, Poly)
{
        SegmentedVector<Derived, 3> c;
        c.resize(4);
        for (int i = 0; i < 4; i++) {
                c.emplace(i, Derived());
        }

        for (auto& p : c) {
                EXPECT_EQ(1, p.Get1());
        }
        for (auto& p : c) {
                EXPECT_EQ(2, p.Get2());
        }
        for (auto& p : c) {
                EXPECT_EQ(3, p.Get3());
        }

        vector<Base*> v{&c[0], &c[1], &c[2], &c[3]};
        for (auto& p : v) {
                EXPECT_EQ(1, p->Get1());
        }
}

TEST(UnitSegmentedVector, Poly2)
{
        SegmentedVector<Derived, 3> c(4);
        for (int i = 0; i < 4; i++) {
                c.emplace(i, Derived());
        }

        for (int i = 0; i < 4; i++) {
                EXPECT_EQ(1, c[i].Get1());
        }
        for (int i = 0; i < 4; i++) {
                EXPECT_EQ(2, c[i].Get2());
        }
        for (int i = 0; i < 4; i++) {
                EXPECT_EQ(3, c[i].Get3());
        }

        vector<Base*> v{&c[0], &c[1], &c[2], &c[3]};
        for (auto& p : v) {
                EXPECT_EQ(1, p->Get1());
        }
}

TEST(UnitSegmentedVector, Ctor1)
{
        SegmentedVector<size_t, 7> c;
        EXPECT_EQ(0, c.size());
        EXPECT_EQ(0, c.get_block_count());

        SegmentedVector<size_t, 7> d(4);
        EXPECT_EQ(4, d.size());
        EXPECT_EQ(1, d.get_block_count());

        SegmentedVector<size_t, 7> e(9);
        EXPECT_EQ(9, e.size());
        EXPECT_EQ(2, e.get_block_count());
}

TEST(UnitSegmentedVector, Ctor2)
{
        SegmentedVector<size_t, 7, false> c;
        EXPECT_EQ(0, c.size());
        EXPECT_EQ(0, c.get_block_count());

        SegmentedVector<size_t, 7, false> d(4);
        EXPECT_EQ(4, d.size());
        EXPECT_EQ(1, d.get_block_count());

        SegmentedVector<size_t, 7, false> e(9);
        EXPECT_EQ(9, e.size());
        EXPECT_EQ(2, e.get_block_count());
}

TEST(UnitSegmentedVector, Resize1)
{
        SegmentedVector<size_t, 7, false> c;
        EXPECT_EQ(0, c.size());
        EXPECT_EQ(0, c.get_block_count());

        c.resize(7);
        EXPECT_EQ(7, c.size());
        EXPECT_EQ(1, c.get_block_count());

        c.resize(8);
        EXPECT_EQ(8, c.size());
        EXPECT_EQ(2, c.get_block_count());

        c.resize(32);
        EXPECT_EQ(32, c.size());
        EXPECT_EQ(5, c.get_block_count());

        c.resize(19);
        EXPECT_EQ(19, c.size());
        EXPECT_EQ(3, c.get_block_count());
}

TEST(UnitSegmentedVector, Resize2)
{
        SegmentedVector<Derived, 7, false> c;
        c.resize(17, Derived());
        for (const auto& e : c) {
                EXPECT_EQ(3, e.Get3());
        }
        c.resize(6, Derived());
        for (const auto& e : c) {
                EXPECT_EQ(3, e.Get3());
        }
}

TEST(UnitSegmentedVector, Resize3)
{
        SegmentedVector<Derived, 4, false> c;
        EXPECT_EQ(0, c.size());
        EXPECT_EQ(0, c.get_block_count());
        c.resize(5);
        EXPECT_EQ(5, c.size());
        EXPECT_EQ(2, c.get_block_count());
        for (size_t i = 0; i < c.size(); ++i) {
                c.emplace(i, Derived());
        }
        c.resize(3);
        EXPECT_EQ(3, c.size());
        EXPECT_EQ(1, c.get_block_count());
        for (const auto& e : c) {
                EXPECT_EQ(3, e.Get3());
        }
}

} // namespace tests
} // namespace stride
