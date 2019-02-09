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

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include <iostream>

namespace gengeopop {

namespace kd {

template <typename P>
class BaseNode;

template <typename P, std::size_t D>
class Node;

template <typename P, std::size_t D>
std::size_t Median(const std::vector<P>& points);

} // namespace kd

/**********************************
 *  Public interface starts here  *
 **********************************/

/**
 * Axis Aligned Bounding Box
 *
 * @brief A hyper rectangle defined by 2 points: the lower bound for every dimension and the upper bound
 */
template <typename P>
struct AABB
{
        AABB() : lower(), upper(){};
        AABB(P l, P u) : lower(l), upper(u){};
        P lower; ///< The lower bound for every dimension
        P upper; ///< The upper bound for every dimension
};

/**
 * A k-d tree: a k-dimensional generalization of binary search trees
 * This data structure allows for efficient lookup of points and range queries with an Axis-Aligned Bounding Box (when
 * balanced).
 *
 * The template parameter `P` should have the following attributes and operations:
 *  - A `static constexpr std::size_t dim`: the number of dimensions of the point type.
 *  - A `template <std::size_d D> get() const` method that returns the coordinate of the `D`th dimension of the point
 *  - A nested `template <std::size_t D> struct dimension_type` that has a member type `type` giving the return type for
 *    `Get<D>`
 *  - A default constructor and a copy constructor
 *  - The individual dimensions should each have a total order and equality
 *  - A method `bool InBox(const AABB<P>& box) const` that indicates if a point falls withing the bounding box (only for
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
        KdTree() : m_size(0), m_root(nullptr) {}

        /**
         * Build a balanced tree from the given set of points efficiently
         *
         * @param points The points to insert in the resulting tree
         * @returns A balanced KdTree containing the given points
         */
        static KdTree Build(const std::vector<P>& points)
        {
                KdTree result;
                result.m_size = points.size();
                result.m_root = Construct<0>(points);
                return result;
        }

        /**
         * Insert a new point into the tree, using this often may result in an unbalanced tree
         *
         * @param point The point to insert into the tree
         */
        void Insert(P point)
        {
                m_size++;
                if (!m_root) {
                        m_root = std::make_unique<kd::Node<P, 0>>(point);
                        return;
                }
                kd::BaseNode<P>* current = m_root.get();
                while (true) {
                        kd::BaseNode<P>* next = current->BorrowSplitChild(point);
                        if (!next) {
                                current->AddChild(point);
                                return;
                        }
                        current = next;
                }
        }

        /**
         * Test wether a point is contained in the tree
         *
         * @param point The point to test. P should support `bool operator==(const P&) const`
         * @returns Whether the point is found in the tree
         */
        bool Contains(const P& point) const
        {
                bool result = false;
                Apply([&result, &point](const P& pt) -> bool {
                        if (pt == point) {
                                result = true;
                                return false;
                        }
                        return true;
                });
                return result;
        }

        /**
         * Get all points in the tree that lie within `box`
         *
         * @param box The limiting AABB to search for points
         * @returns A collection of points found within `box`
         */
        std::vector<P> Query(const AABB<P>& box) const
        {
                std::vector<P> result;
                Apply(
                    [&result](const P& pt) -> bool {
                            result.push_back(pt);
                            return true;
                    },
                    box);
                return result;
        }

        /**
         * Calls a function with each of the points in the tree
         *
         * @param f A function that will be called with each point, if f returns false, the traversal stops
         */
        void Apply(std::function<bool(const P&)> f) const
        {
                if (m_root == nullptr)
                        return;

                std::queue<kd::BaseNode<P>*> todo;
                todo.push(m_root.get());

                while (!todo.empty()) {
                        kd::BaseNode<P>* current = todo.front();
                        todo.pop();

                        if (!f(current->GetPoint())) {
                                break;
                        }

                        kd::BaseNode<P>* left = current->BorrowLeft();
                        if (left != nullptr)
                                todo.push(left);

                        kd::BaseNode<P>* right = current->BorrowRight();
                        if (right != nullptr)
                                todo.push(right);
                }
        }

        /**
         * Calls a function with every point contained in `box`
         *
         * @param f A function that will be called with each point within `box`, if f returns false, the traversal stops
         * @param box The containing Axis-Aligned Bounding Box to search for points
         */
        void Apply(std::function<bool(const P&)> f, const AABB<P>& box) const
        {
                std::queue<kd::BaseNode<P>*> q;
                q.push(m_root.get());
                while (!q.empty()) {
                        kd::BaseNode<P>* current = q.front();
                        q.pop();
                        if (current == nullptr)
                                continue;

                        if (current->GetPoint().InBox(box)) {
                                if (!f(current->GetPoint())) {
                                        break;
                                }
                        }
                        kd::BaseNode<P>* a = current->BorrowSplitChild(box.lower);
                        kd::BaseNode<P>* b = current->BorrowSplitChild(box.upper);
                        q.push(a);
                        if (a != b)
                                q.push(b);
                }
        }

        /**
         * Get the height of the tree
         *
         * Mostly for testing purposes
         */
        std::size_t Height() const
        {
                int                                          h = 0;
                std::queue<std::pair<int, kd::BaseNode<P>*>> q;
                q.emplace(1, m_root.get());
                while (!q.empty()) {
                        auto tmp = q.front();
                        q.pop();
                        kd::BaseNode<P>* n = tmp.second;
                        if (!n)
                                continue;
                        h = tmp.first;
                        q.emplace(h + 1, n->BorrowLeft());
                        q.emplace(h + 1, n->BorrowRight());
                }
                return static_cast<size_t>(h);
        }

        /**
         * Is the tree empty
         */
        bool Empty() const { return Size() == 0; }

        /**
         * Get the size of the tree
         */
        std::size_t Size() const { return m_size; }

private:
        template <std::size_t D>
        static std::unique_ptr<kd::Node<P, D>> Construct(const std::vector<P>& points)
        {
                if (points.empty())
                        return nullptr;

                std::size_t med        = kd::Median<P, D>(points);
                auto        median_val = points[med].template Get<D>();
                P           root_pt    = P();

                std::vector<P> left, right;
                for (std::size_t i = 0; i < points.size(); i++) {
                        const auto& p = points[i];
                        if (i == med) {
                                root_pt = p;
                        } else if (p.template Get<D>() <= median_val) {
                                left.push_back(p);
                        } else {
                                right.push_back(p);
                        }
                }

                auto root     = std::make_unique<kd::Node<P, D>>(root_pt);
                root->m_left  = Construct<(D + 1) % P::dim>(left);
                root->m_right = Construct<(D + 1) % P::dim>(right);

                return root;
        }

        std::size_t                     m_size; ///< The number of points in the tree
        std::unique_ptr<kd::Node<P, 0>> m_root; ///< The root node of the tree
};

/***************************************
 *  Implementation of kd starts here   *
 ***************************************/
namespace kd {

/// A base class for all instanciations of a Node with D
template <typename P>
class BaseNode
{
public:
        virtual ~BaseNode() = default;

        /**
         * Get a non-owning pointer to the left child
         * nullptr if there's no such child
         */
        virtual BaseNode<P>* BorrowLeft() const = 0;

        /**
         * Get a non-owning pointer to the right child
         * nullptr if there's no such child
         */
        virtual BaseNode<P>* BorrowRight() const = 0;

        /**
         * Get a non-owning pointer to the child corresponding to the correct split for point
         */
        virtual BaseNode<P>* BorrowSplitChild(const P& point) const = 0;

        /**
         * Add a new child in the right place, according to split
         */
        virtual void AddChild(P point) = 0;

        /**
         * Gets the point for this node
         */
        virtual P GetPoint() const = 0;
};

/**
 * A node in the KdTree
 *
 * Template parameter P: the type of point
 * Template parameter D: The dimension this node splits on
 */
template <typename P, std::size_t D>
class Node : public BaseNode<P>
{
public:
        Node(P pt) : m_point(pt), m_left(nullptr), m_right(nullptr) {}

        BaseNode<P>* BorrowLeft() const override { return m_left.get(); }

        BaseNode<P>* BorrowRight() const override { return m_right.get(); }

        BaseNode<P>* BorrowSplitChild(const P& point) const override
        {
                auto refval  = m_point.template Get<D>();
                auto testval = point.template Get<D>();
                if (testval <= refval) {
                        return m_left.get();
                } else {
                        return m_right.get();
                }
        }

        void AddChild(P point) override
        {
                auto refval  = m_point.template Get<D>();
                auto testval = point.template Get<D>();
                if (testval <= refval) {
                        m_left = std::make_unique<Child>(point);
                } else {
                        m_right = std::make_unique<Child>(point);
                }
        }

        P GetPoint() const override { return m_point; }

private:
        using Child = Node<P, (D + 1) % P::dim>;

        P                      m_point;
        std::unique_ptr<Child> m_left, m_right;

        friend class KdTree<P>;
};

template <typename P, std::size_t D>
std::size_t Median(const std::vector<P>& points)
{
        if (points.empty())
                return 0;

        using C = std::pair<decltype(points[0].template Get<D>()), std::size_t>;
        std::vector<C> sorting;
        for (std::size_t i = 0; i < points.size(); i++) {
                sorting.emplace_back(points[i].template Get<D>(), i);
        }
        std::sort(sorting.begin(), sorting.end());
        return sorting[sorting.size() / 2].second;
}

} // namespace kd
} // namespace gengeopop
