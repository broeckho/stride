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
 * Unit tests of Range &SegmentedVector.
 */

#include "util/Any.h"
#include "util/SegmentedVector.h"

#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/irange.hpp>
#include <gtest/gtest.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <map>

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
        /// Warning: range is [ibegin, iend) i.e. half-open, iend not included!
        /// \param t
        /// \param ibegin
        /// \param iend
        /// \param name
        /// \return
        boost::sliced_range<T>& Add(T& t, std::size_t ibegin, std::size_t iend, const std::string& name)
        {
                if (m_map.find(name) != m_map.end()) {
                        throw std::range_error("RangeIndexer::Add> Name is a duplicate: " + name);
                } else {
                        m_ranges.emplace_back(boost::adaptors::slice(t, ibegin, iend));
                        m_map[name] = m_ranges.size() - 1;
                }
                return m_ranges.back();
        }

        ///
        /// \param i
        /// \return
        boost::sliced_range<T>& Get(std::size_t i) {return m_ranges.at(i);}

        ///
        /// \param s
        /// \return
        boost::sliced_range<T>& Get(std::string s) {return m_ranges.at(m_map[s]);}

private:
        std::map<std::string, std::size_t> m_map;
        std::vector<boost::sliced_range<T>> m_ranges;
};

TEST(UnitRange, Setup)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());
        EXPECT_EQ(0, c[0]);
        EXPECT_EQ(1, c[1]);
        EXPECT_EQ(50, c[50]);
        EXPECT_EQ(100, c[100]);
}

TEST(UnitRange, Add1)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        RangeIndexer<SegmentedVector<int, 4, false>> ri;
        ri.Add(c, 0, 10, "first_10");
        ri.Add(c, 10, 30, "next_20");
        ri.Add(c, 30, 100, "last_71");
        EXPECT_EQ(ri.Get(0), ri.Get("first_10"));
        EXPECT_EQ(ri.Get(1), ri.Get("next_20"));
        EXPECT_EQ(ri.Get(2), ri.Get("last_71"));
}

TEST(UnitRange, Size1)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        RangeIndexer<SegmentedVector<int, 4, false>> ri;
        ri.Add(c, 0, 10, "first_10");
        ri.Add(c, 10, 30, "next_20");
        ri.Add(c, 30, 101, "last_71");
        EXPECT_EQ(10, ri.Get(0).size());
        EXPECT_EQ(20, ri.Get(1).size());
        EXPECT_EQ(71, ri.Get(2).size());
}

TEST(UnitRange, Content1)
{
        SegmentedVector<int, 4, false> c(101);
        boost::range::copy(boost::irange(0, 101), c.begin());

        RangeIndexer<SegmentedVector<int, 4, false>> ri;
        ri.Add(c, 0, 10, "first_10");
        ri.Add(c, 10, 30, "next_20");
        ri.Add(c, 30, 100, "last_71");
        EXPECT_EQ(0, ri.Get(0)[0]);
        EXPECT_EQ(9, ri.Get(0)[9]);
        EXPECT_EQ(10, ri.Get(1)[0]);
        EXPECT_EQ(29, ri.Get(1)[19]);
        EXPECT_EQ(30, ri.Get(2)[0]);
        EXPECT_EQ(100, ri.Get(2)[100]);
}

} // namespace
} // namespace
} // namespace
