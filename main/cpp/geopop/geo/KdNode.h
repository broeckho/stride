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

#include <cstddef>
#include <memory>
#include <utility>

namespace geopop {

template <typename P>
class KdTree;

namespace kd {

/**
 * A base class for all instantiations of a Node with D.
 */
template <typename P>
class BaseNode
{
public:
        virtual ~BaseNode() = default;

        /// Get a non-owning pointer to the left child (nullptr if no such child).
        virtual BaseNode<P>* BorrowLeft() const = 0;

        /// Get a non-owning pointer to the right child (nullptr if no such child).
        virtual BaseNode<P>* BorrowRight() const = 0;

        /// Get a non-owning pointer to the child corresponding to the correct split for point.
        virtual BaseNode<P>* BorrowSplitChild(const P& point) const = 0;

        /// Add a new child in the right place, according to split.
        virtual void AddChild(P point) = 0;

        /// Gets the point for this node.
        virtual P GetPoint() const = 0;
};

/**
 * A node in the KdTree (parameter P: the type of point, parameter D: dimension this node splits on).
 */
template <typename P, std::size_t D>
class Node : public BaseNode<P>
{
public:
        explicit Node(P pt) : m_point(pt), m_left(nullptr), m_right(nullptr) {}

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

} // namespace kd
} // namespace geopop
