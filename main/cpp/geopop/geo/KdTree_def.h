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

#include "KdTree.h"

#include "AABBox.h"
#include "KdNode.h"
#include "Median.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

namespace geopop {

template <typename P>
KdTree<P>::KdTree() : m_size(0), m_root(nullptr)
{
}

template <typename P>
KdTree<P> KdTree<P>::Build(const std::vector<P>& points)
{
        KdTree result;
        result.m_size = points.size();
        result.m_root = Construct<0>(points);
        return result;
}

template <typename P>
void KdTree<P>::Apply(std::function<bool(const P&)> f) const
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

template <typename P>
void KdTree<P>::Apply(std::function<bool(const P&)> f, const AABBox<P>& box) const
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

template <typename P>
bool KdTree<P>::Contains(const P& point) const
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

template <typename P>
bool KdTree<P>::Empty() const
{
        return Size() == 0;
}

template <typename P>
std::size_t KdTree<P>::GetHeight() const
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

template <typename P>
void KdTree<P>::Insert(P point)
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

template <typename P>
std::vector<P> KdTree<P>::Query(const AABBox<P>& box) const
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

template <typename P>
std::size_t KdTree<P>::Size() const
{
        return m_size;
}

template <typename P>
template <std::size_t D>
std::unique_ptr<kd::Node<P, D>> KdTree<P>::Construct(const std::vector<P>& points)
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

} // namespace geopop
