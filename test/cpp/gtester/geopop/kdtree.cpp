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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "geopop/geo/KdTree_def.h"
#include "util/FileSys.h"

#include <cmath>
#include <fstream>
#include <gtest/gtest.h>
#include <set>
#include <string>

using namespace geopop;

namespace {

class Pt2D
{
public:
        constexpr static std::size_t dim = 2;

        int x, y;

        bool operator==(const Pt2D& o) const { return o.x == x && o.y == y; }
        bool operator<(const Pt2D& o) const { return std::make_pair(x, y) < std::make_pair(o.x, o.y); }

        template <std::size_t D>
        int Get() const
        {
                static_assert(0 <= D && D <= 1, "Dimension should be in range");
                if (D == 0) {
                        return x;
                } else {
                        return y;
                }
        }

        bool InBox(const AABBox<Pt2D>& box) const
        {
                return box.lower.x <= x && x <= box.upper.x && box.lower.y <= y && y <= box.upper.y;
        }

        template <std::size_t D>
        struct dimension_type
        {
                using type = int;
        };
};

class Pt4D
{
public:
        constexpr static std::size_t dim = 4;

        int x, y, z, w;

        bool operator==(const Pt4D& o) const
        {
                return std::make_tuple(x, y, z, w) == std::make_tuple(o.x, o.y, o.z, o.w);
        }

        bool operator<(const Pt4D& o) const
        {
                return std::make_tuple(x, y, z, w) < std::make_tuple(o.x, o.y, o.z, o.w);
        }

        template <std::size_t D>
        int Get() const
        {
                static_assert(0 <= D && D <= 3, "Dimension should be in range");
                switch (D) {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                case 3: return w;
                }
        }

        template <std::size_t D>
        struct dimension_type
        {
                using type = int;
        };
};

class PtIntStr
{
public:
        constexpr static std::size_t dim = 2;
        int                          x;
        std::string                  y;

        bool operator==(const PtIntStr& o) const { return o.x == x && o.y == y; }

        bool operator<(const PtIntStr& o) const { return make_pair(x, y) < make_pair(o.x, o.y); }

        template <std::size_t D>
        struct dimension_type;

        template <std::size_t D>
        typename dimension_type<D>::type Get() const
        {
                static_assert(D != D, "This cannot be instanciated");
        }
};

template <>
struct PtIntStr::dimension_type<0>
{
        using type = int;
};

template <>
int PtIntStr::Get<0>() const
{
        return x;
}

template <>
struct PtIntStr::dimension_type<1>
{
        using type = std::string;
};

template <>
std::string PtIntStr::Get<1>() const
{
        return y;
}

TEST(KdTreeTest, BuildHasLimitedHeight)
{
        std::vector<Pt2D> points;
        std::ifstream     in(stride::util::FileSys::GetTestsDir().string() +
                         "/testdata/KdTree/BuildHasLimitedHeightPoints");
        int               x, y;
        while (in >> x >> y) {
                points.push_back({x, y});
        }

        auto tree = KdTree<Pt2D>::Build(points);
        EXPECT_LE(tree.GetHeight(), std::log2(points.size() + 1) + 1);
        EXPECT_EQ(points.size(), tree.Size());
}

TEST(KdTreeTest, RangeQuery)
{
        std::vector<Pt2D> points;
        for (int i = -50; i <= 50; i++) {
                for (int j = -50; j <= 50; j++) {
                        points.push_back({i, j});
                }
        }

        auto              tree = KdTree<Pt2D>::Build(points);
        std::vector<Pt2D> tmp  = tree.Query({{-2, 42}, {19, 48}});
        std::set<Pt2D>    result(tmp.begin(), tmp.end());
        ASSERT_EQ(22 * 7, result.size());
        for (int i = -2; i <= 19; i++) {
                for (int j = 42; j <= 48; j++) {
                        EXPECT_NE(result.find({i, j}), result.end());
                }
        }
}

TEST(KdTreeTest, Insert)
{
        KdTree<Pt2D> tree;
        tree.Insert({0, 0});
        tree.Insert({1, 0});
        tree.Insert({-1, 0});
        tree.Insert({1, 1});
        tree.Insert({-1, -1});
        EXPECT_TRUE(tree.Contains({0, 0}));
        EXPECT_TRUE(tree.Contains({1, 0}));
        EXPECT_TRUE(tree.Contains({-1, 0}));
        EXPECT_TRUE(tree.Contains({1, 1}));
        EXPECT_TRUE(tree.Contains({-1, -1}));
        EXPECT_FALSE(tree.Contains({-785960024, 314069115}));
        EXPECT_EQ(5, tree.Size());
        EXPECT_EQ(3, tree.GetHeight());
}

TEST(KdTreeTest, HigherDimensional)
{
        KdTree<Pt4D> tree;
        tree.Insert({0, 0, 0, 0});
        tree.Insert({2, 0, 1, 8});
        tree.Insert({-1, -4, -9, -2});
        std::set<Pt4D> expected;
        expected.insert({0, 0, 0, 0});
        expected.insert({2, 0, 1, 8});
        expected.insert({-1, -4, -9, -2});
        std::set<Pt4D> found;
        tree.Apply([&found](const Pt4D& pt) -> bool {
                found.insert(pt);
                return true;
        });
        EXPECT_EQ(expected, found);
}

TEST(KdTreeTest, NonArithmeticDimension)
{
        KdTree<PtIntStr> tree;
        tree.Insert({0, "Hello, World!"});
        tree.Insert({2, "War of the worlds"});
        tree.Insert({-1, "2+2=5"});
        std::set<PtIntStr> expected;
        expected.insert({0, "Hello, World!"});
        expected.insert({2, "War of the worlds"});
        expected.insert({-1, "2+2=5"});
        std::set<PtIntStr> found;
        tree.Apply([&found](const PtIntStr& pt) -> bool {
                found.insert(pt);
                return true;
        });
        EXPECT_EQ(expected, found);
}

TEST(KdTreeTest, EarlyExit)
{
        std::vector<Pt2D> points;
        for (int i = -50; i <= 50; i++) {
                for (int j = -50; j <= 50; j++) {
                        points.push_back({i, j});
                }
        }

        auto tree  = KdTree<Pt2D>::Build(points);
        int  count = 0;
        auto f     = [&count](const Pt2D&) -> bool {
                count++;
                return false;
        };
        tree.Apply(f);
        EXPECT_EQ(1, count);
        count = 0;
        tree.Apply(f, AABBox<Pt2D>{{-10, -10}, {10, 10}});
        EXPECT_EQ(1, count);
}

} // namespace
