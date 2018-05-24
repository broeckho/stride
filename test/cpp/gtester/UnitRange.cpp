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
 * Unit tests of Range &SegmentedVector.
 */

#include "util/SegmentedVector.h"

#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>
#include <gtest/gtest.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <map>
#include <iostream>

using namespace std;
using namespace stride::util;

namespace SimPT_Sim {
namespace Container {
namespace Tests {

class Base
{
public:
        virtual size_t Get1() const {return 0;}
        virtual ~Base() {}
};

class Derived : public Base
{
public:
        size_t Get1() const override {return 1;}
        size_t Get2() const {return 2;}
        virtual size_t Get3() const {return 3;}
};

template <typename T>
class RangeIndexer
{
public:
        using range_type = boost::sub_range<T>;

public:
        ///
        /// \param t
        explicit RangeIndexer(T& t) : m_t(t) {}

        /// Warning: range is [ibegin, iend) i.e. half-open, iend not included!
        /// \param t
        /// \param ibegin
        /// \param iend
        /// \param name
        /// \return
        range_type& SetRange(std::size_t ibegin, std::size_t iend, const std::string &name)
        {
                if (m_map.find(name) != m_map.end()) {
                        throw std::range_error("RangeIndexer::Add> Name is a duplicate: " + name);
                } else {
                        m_ranges.emplace_back(range_type(m_t.begin() + ibegin, m_t.begin() + iend));
                        m_map[name] = m_ranges.size() - 1;
                }
                return m_ranges.back();
        }

        range_type& SetRange(std::size_t ibegin, const std::string &name)
        {
                if (m_map.find(name) != m_map.end()) {
                        throw std::range_error("RangeIndexer::Add> Name is a duplicate: " + name);
                } else {
                        m_ranges.emplace_back(range_type(m_t.begin() + ibegin, m_t.end()));
                        m_map[name] = m_ranges.size() - 1;
                }
                return m_ranges.back();
        }
        ///
        /// \param i
        /// \return
        range_type& GetRange(std::size_t i) {return m_ranges.at(i);}

        ///
        /// \param s
        /// \return
        range_type& GetRange(const std::string &s)
        {
                auto i = m_map.at(s);
                return m_ranges.at(i);
        }

private:
        std::map<std::string, std::size_t>  m_map;
        std::vector<range_type>             m_ranges;
        T&                                  m_t;
};

template<typename T>
RangeIndexer<T> make_range_indexer(T &t)
{
        return RangeIndexer<T>(t);
}

TEST(UnitRange, Setup)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());
        EXPECT_EQ(0, c[0]);
        EXPECT_EQ(1, c[1]);
        EXPECT_EQ(50, c[50]);
        EXPECT_EQ(100, c[100]);
}

TEST(UnitRange, Size1)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        RangeIndexer<SegmentedVector<int, 4, false>> si(c);
        si.SetRange(0, 10, "first_10");
        si.SetRange(10, 30, "next_20");
        si.SetRange(30, 101, "last_71");

        EXPECT_EQ(10, si.GetRange(0).size());
        EXPECT_EQ(20, si.GetRange(1).size());
        EXPECT_EQ(71, si.GetRange(2).size());
}

TEST(UnitRange, Size2)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_range_indexer(c);
        si.SetRange(0, 10, "first_10");
        si.SetRange(10, 30, "next_20");
        si.SetRange(30, 101, "last_71");

        EXPECT_EQ(10, si.GetRange("first_10").size());
        EXPECT_EQ(20, si.GetRange("next_20").size());
        EXPECT_EQ(71, si.GetRange("last_71").size());
}


TEST(UnitRange, Content1)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_range_indexer(c);
        si.SetRange(0, 10, "first_10");
        si.SetRange(10, 30, "next_20");
        si.SetRange(30, 101, "last_71");

        EXPECT_EQ(0, si.GetRange(0)[0]);
        EXPECT_EQ(9, si.GetRange(0)[9]);

        EXPECT_EQ(10, si.GetRange(1)[0]);
        EXPECT_EQ(29, si.GetRange(1)[19]);

        EXPECT_EQ(30, si.GetRange(2)[0]);
        EXPECT_EQ(100, si.GetRange(2)[70]);
}

TEST(UnitRange, Content2)
{
        SegmentedVector<int, 4> c(101,0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_range_indexer(c);
        si.SetRange(0, 10, "first_10");
        si.SetRange(10, 30, "next_20");
        si.SetRange(30, "last_71");

        int sub1 = 30;
        for (auto e : si.GetRange(2)) {
                EXPECT_EQ(sub1++ , e);
        }

        int accum = 0;
        for (auto e : si.GetRange(0)) {
                accum += e;
        }
        EXPECT_EQ(45, accum);

        auto s1 = si.GetRange("next_20");
        for (size_t i = 0; i < s1.size(); ++i) {
                EXPECT_EQ(i + 10, s1[i]);
        }

        auto s2 = si.GetRange("last_71");
        int sub2 = 30;
        for (auto it = s2.begin() ; it < s2.end(); ++it) {
                EXPECT_EQ(sub2++ , *it);
        }
}

TEST(UnitRange, Resize1)
{
        SegmentedVector<int, 4> c(101,0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_range_indexer(c);
        si.SetRange(0, 10, "first_10");
        si.SetRange(10, 30, "next_20");
        si.SetRange(30, "last_71");

        c.resize(201);

        int accum = 0;
        for (auto e : si.GetRange(0)) {
                accum += e;
        }
        EXPECT_EQ(45, accum);

        auto s1 = si.GetRange("next_20");
        for (size_t i = 0; i < s1.size(); ++i) {
                EXPECT_EQ(i + 10, s1[i]);
        }

        auto s2 = si.GetRange("last_71");
        int sub2 = 30;
        for (auto it = s2.begin() ; it < s2.end(); ++it) {
                EXPECT_EQ(sub2++ , *it);
        }
}


TEST(UnitRange, Resize2)
{
        SegmentedVector<int, 4> c(101,0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_range_indexer(c);
        si.SetRange(0, 10, "first_10");
        si.SetRange(10, 30, "next_20");
        si.SetRange(30, "last_71");

        c.resize(201);
        si.SetRange(101, "new_last");
        boost::range::copy(boost::irange(101, 201), si.GetRange("new_last").begin());
}

TEST(UnitRange, Resize3)
{
        SegmentedVector<int, 4> c(101,0);
        boost::range::copy(boost::irange(0, 101), c.begin());

        auto si = make_range_indexer(c);
        si.SetRange(0, 10, "first_10");
        si.SetRange(10, 30, "next_20");
        si.SetRange(30, "last_71");

        c.resize(201);
        si.SetRange(101, "new_last");
        boost::range::copy(boost::irange(101, 201), si.GetRange("new_last").begin());

        int accum = 0;
        for (auto e : si.GetRange(0)) {
                accum += e;
        }
        EXPECT_EQ(45, accum);

        auto s1 = si.GetRange("next_20");
        for (size_t i = 0; i < s1.size(); ++i) {
                EXPECT_EQ(i + 10, s1[i]);
        }

        auto s2 = si.GetRange("last_71");
        int sub2 = 30;
        for (auto it = s2.begin() ; it < s2.end(); ++it) {
                EXPECT_EQ(sub2++ , *it);
        }

        int sub3 = 101;
        for (auto e : si.GetRange("new_last")) {
                EXPECT_EQ(sub3++ , e);
        }
}

} // namespace
} // namespace
} // namespace
