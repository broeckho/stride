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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "AABBox.h"
#include "KdNode.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace geopop {

/**
 * A k-d tree: a k-dimensional generalization of binary search trees
 * This data structure allows for efficient lookup of points and range queries with
 * an Axis-Aligned Bounding Box (when balanced).
 *
 * The template parameter `P` should have the following attributes and operations:
 *  - A `static constexpr std::size_t dim`: the number of dimensions of the point type.
 *  - A `template <std::size_d D> get() const` returns the coordinate of the `D`th dimension of the point.
 *  - A nested `template <std::size_t D> struct dimension_type` has a member type `type` with return type for `Get<D>`.
 *  - A default constructor and a copy constructor
 *  - The individual dimensions should each have a total order and equality
 *  - A method `bool InBox(const AABB<P>& box) const` indicates if a point falls withing the bounding box (only for
 *    range queries)
 */
template <typename P>
class KdTree
{
public:
        static_assert(P::dim > 0, "Cannot have points in 0 dimensions");

        /**
         * Constructor: builds an empty tree
         */
        KdTree();

        /**
         * Build a balanced tree from the given set of points efficiently.
         * @param points The points to insert in the resulting tree.
         * @returns A balanced KdTree containing the given points.
         */
        static KdTree Build(const std::vector<P>& points);

        /**
         * Calls a function with each of the points in the tree
         * @param f A function that will be called with each point, if f returns false, traversal stops.
         */
        void Apply(std::function<bool(const P&)> f) const;

        /**
         * Calls a function with every point contained in `box`
         * @param f A function that will be called with each point within `box`, if f returns false, traversal stops.
         * @param box The containing Axis-Aligned Bounding Box to search for points
         */
        void Apply(std::function<bool(const P&)> f, const AABBox<P>& box) const;

        /**
         * Test wether a point is contained in the tree.
         * @param point The point to test. P should support `bool operator==(const P&) const`.
         * @returns Whether the point is found in the tree.
         */
        bool Contains(const P& point) const;

        /// Is the tree empty.
        bool Empty() const;

        /// Get the height of the tree (mostly for testing purposes).
        std::size_t GetHeight() const;

        /**
         * Insert a new point into the tree, using this often may result in an unbalanced tree.
         * @param point The point to insert into the tree.
         */
        void Insert(P point);

        /**
         * Get all points in the tree that lie within `box`.
         * @param box The limiting AABB to search for points.
         * @returns A collection of points found within `box`.
         */
        std::vector<P> Query(const AABBox<P>& box) const;

        /// Get the size of the tree.
        std::size_t Size() const;

private:
        template <std::size_t D>
        static std::unique_ptr<kd::Node<P, D>> Construct(const std::vector<P>& points);

private:
        std::size_t                     m_size; ///< The number of points in the tree
        std::unique_ptr<kd::Node<P, 0>> m_root; ///< The root node of the tree
};

} // namespace geopop
