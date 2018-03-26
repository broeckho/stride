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
 * Unit tests of SegmentedVector.
 */

#include "util/SegmentedVector.h"

#include <gtest/gtest.h>
#include <ostream>
#include <stdexcept>

using namespace std;
using namespace stride::util;

namespace SimPT_Sim {
namespace Container {
namespace Tests {

namespace {
	class TraceMemory
	{
	public:
		enum EventType
		{
			ALLOCATED, DEALLOCATED, COPIED, MOVED
		};

		using EventList = std::vector<EventType>;

		TraceMemory() : m_counter(0) {}

		void Allocated() { m_counter++; m_event_list.push_back(ALLOCATED); }
		void Copied() { m_counter++; m_event_list.push_back(COPIED); }
		void Deallocated() { m_counter--; m_event_list.push_back(DEALLOCATED); }
		void Moved() { m_event_list.push_back(MOVED); }

		int GetCounter() const { return m_counter; }
		const EventList& GetEventList() const { return m_event_list; }

	private:
		int         m_counter;
		EventList   m_event_list;
	};

	class TestType
	{
	public:

		TestType(int _i, const std::string& _str, TraceMemory& _t)
			: i(_i), str(_str), t(_t)
		{
			t.Allocated();
			array = new int[10];
		}

		TestType(const TestType& other)
			: i(other.i), str(other.str), t(other.t)
		{
			t.Copied();
			array = new int[10];
		}

		TestType(TestType&& other)
			: i(other.i), str(std::move(other.str)), array(other.array), t(other.t)
		{
			t.Moved();
			other.array = nullptr;
		}

		~TestType() {
			if (array) {
				t.Deallocated();
				delete[] array;
			}
		}

		int i;
		std::string str;
		int* array;
		TraceMemory& t;
	};
}


template <class SegmentedVector>
void RunBasicOperationsTest(SegmentedVector& c, size_t size)
{
	const size_t block_size = c.get_elements_per_block();

	// Allocation of blocks when filling container.
	for (size_t i = 0; i < size; i++) {
		c.push_back(i);
		EXPECT_EQ( 1 + i / block_size, c.get_block_count());
	}

	// De-allocation of blocks when emptying container.
	for (size_t i = 0; i < size; i++) {
		c.pop_back();
		auto num = c.size() / block_size;
		if (c.size() > 0 && (c.size() % block_size > 0)) ++num;
		EXPECT_EQ( num, c.get_block_count());
	}

	// Size function.
	for (size_t i = 0; i < size; i++) {
		c.push_back(i);
	}
	EXPECT_EQ( c.size(), size );

	// Iterator: sequential access.
	auto i = 0UL;
	for (auto j : c) {
		EXPECT_EQ(i, j);
		i++;
	}

	// Element access.
	for (size_t i = 0; i < size; i++) {
		EXPECT_EQ(c.at(i), i);
	}

	// Element access: bounds check.
	EXPECT_THROW( c.at(size), out_of_range );

	// Check if correct elements are in the same block */
	size_t how_long_ago_since_new_block = 0UL;
	for (size_t i = 0; i < size; i++) {
		if (&c[i] != &c[i-how_long_ago_since_new_block]+how_long_ago_since_new_block) {
			EXPECT_EQ(how_long_ago_since_new_block % block_size, 0UL);
			how_long_ago_since_new_block = 0;
		}
		how_long_ago_since_new_block++;
	}
}

TEST( UnitSegmentedVector, BasicOperations128 )
{
	SegmentedVector<size_t, 128> c;
	RunBasicOperationsTest(c, 128);
}

TEST( UnitSegmentedVector, BasicOperations260 )
{
	SegmentedVector<size_t, 128> c;
	RunBasicOperationsTest(c, 260);
}

TEST( UnitSegmentedVector, BasicOperations12345 )
{
	SegmentedVector<size_t, 128> c;
	RunBasicOperationsTest(c, 12345);
}

TEST( UnitSegmentedVector, CopyPushBack )
{
	TraceMemory trace;
	{
		SegmentedVector<TestType, 4> c;
		for (int i=0; i<10; i++) {
			TestType t(i, "hello", trace);
			c.push_back(t);
		}
		for (int i=0; i<10; i++) {
			EXPECT_EQ(c[i].i, i);
			EXPECT_EQ(c[i].str, "hello");
		}
	}

	EXPECT_EQ(0, trace.GetCounter());
	EXPECT_EQ(40UL, trace.GetEventList().size()); // 10 allocations, 10 copies, 20 deallocations
}

TEST( UnitSegmentedVector, MovePushBack )
{
	TraceMemory trace;
	{
		SegmentedVector<TestType, 4> c;
		for (int i=0; i<10; i++) {
			TestType t(i, "hello", trace);
			c.push_back(std::move(t));
		}

		for (int i=0; i<10; i++) {
			EXPECT_EQ(c[i].i, i);
			EXPECT_EQ(c[i].str, "hello");
		}
	}

	EXPECT_EQ(0, trace.GetCounter());
	EXPECT_EQ(30UL, trace.GetEventList().size()); // 10 allocations, 10 moves, 10 deallocations
}

TEST( UnitSegmentedVector, EmplaceBack )
{
	TraceMemory trace;
	{
		SegmentedVector<TestType, 4> c;
		for (int i = 0; i < 10; i++) {
			c.emplace_back(i, "hello", trace);
		}

		for (int i=0; i<10; i++) {
			EXPECT_EQ(c[i].i, i);
			EXPECT_EQ(c[i].str, "hello");
		}
	}

	EXPECT_EQ(0, trace.GetCounter());
	EXPECT_EQ(20UL, trace.GetEventList().size()); // 10 allocations, 10 deallocations
}

TEST( UnitSegmentedVector, PushPop )
{
	SegmentedVector<char, 4> c;
	for (int i=0; i<100; i++) {
		c.push_back('a');
	}
	EXPECT_EQ(100UL, c.size());

	for (int i=0; i<100; i++) {
		c.pop_back();
	}
	EXPECT_EQ(0UL, c.size());

	for (int i=0; i<10; i++) {
		c.push_back('a' + i);
	}
	EXPECT_EQ(10UL, c.size());

	for (int i=9; i>=0; i--) {
		EXPECT_EQ('a' + i, c.back());
		c.pop_back();
	}
	EXPECT_EQ(0UL, c.size());
}

TEST( UnitSegmentedVector, CopyAssignment )
{
	TraceMemory trace_c;
	TraceMemory trace_d;
	{
		SegmentedVector<TestType, 4> c;
		for (int i=0; i<10; i++) {
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
			for (int i=0; i<10; i++) {
				EXPECT_EQ(i, d[i].i);
			}

			// remove all elements from d
			d.clear();
			EXPECT_EQ(0UL, d.size());
		} // will now destroy d and all of its elements

		// c should not be affected when d is changed
		EXPECT_EQ(10UL, c.size());
		for (int i=0; i<10; i++) {
			EXPECT_EQ(i, c[i].i);
		}
	} // will now destroy c and all of its elements

	EXPECT_EQ(0, trace_c.GetCounter());
	EXPECT_EQ(0, trace_d.GetCounter());
}

TEST( UnitSegmentedVector, CopyConstruct )
{
	SegmentedVector<int, 4> c;
	for (int i=0; i<100; i++) {
		c.push_back(i);
	}

	// copy construct
	SegmentedVector<int, 4> d(c);

	// d must contain c's contents
	EXPECT_EQ(100UL, d.size());
	for (int i=0; i<100; i++) {
		EXPECT_EQ(i, d[i]);
	}

	// remove all elements from d
	d.clear();
	EXPECT_EQ(0UL, d.size());

	// c should not be affected when d is changed
	EXPECT_EQ(100UL, c.size());
	for (int i=0; i<100; i++) {
		EXPECT_EQ(i, c[i]);
	}
}

TEST( UnitSegmentedVector, MoveConstruct )
{
	SegmentedVector<int, 4> c;
	for (int i = 0; i < 100; i++) {
		c.push_back(i);
	}

	SegmentedVector<int, 4> d(std::move(c));
	EXPECT_EQ(100UL, d.size());
	for (int i=0; i<100; i++) {
		EXPECT_EQ(i, d[i]);
	}
}

TEST( UnitSegmentedVector, MoveAssignment )
{
	SegmentedVector<int, 4> c;
	for (int i = 0; i < 100; i++) {
		c.push_back(i);
	}

	SegmentedVector<int, 4> d;
	for (int i = 0; i < 20; i++) {
		d.push_back(5);
	}

	d = std::move(c);
	EXPECT_EQ(100UL, d.size());
	for (int i=0; i<100; i++) {
		EXPECT_EQ(i, d[i]);
	}
	EXPECT_EQ(0UL, c.size());
}

TEST( UnitSegmentedVector, IndexOperator )
{
	SegmentedVector<int, 4> c;
	for (int i = 0; i < 100; i++) {
		c.push_back(i);
	}
	for (int i = 0; i < 100; i++) {
		EXPECT_EQ( i, c[i]);
	}
}

TEST( UnitSegmentedVector, IteratorForEmptyMBV )
{
	SegmentedVector<int, 4> c;
	auto it1 = c.begin();
	auto it2 = c.end();
	EXPECT_EQ(true, it1 == it2);
}

TEST( UnitSegmentedVector, Dereferencing )
{
	SegmentedVector<int, 4> c;
	for (int i = 0; i < 100; i++) {
		c.push_back(i);
	}
	for(auto it = c.begin(); it != c.end(); ++it) {
		*it += 1;
	}
	auto i = 1;
	for(auto it = c.cbegin(); it != c.cend(); ++it, ++i) {
		EXPECT_EQ(i, *it);
	}
}

TEST( UnitSegmentedVector, RangeBasedLoop )
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

} // namespace
} // namespace
} // namespace
