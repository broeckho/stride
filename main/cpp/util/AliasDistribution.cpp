#pragma once
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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Alias method implementation.
 */

#include "AliasDistribution.h"

#include <cassert>

using namespace stride;
using namespace util;

AliasDistribution::AliasDistribution(const vector<double>& _probs)
    : m_blocks(_probs.size()), m_diceroll(0, _probs.size() - 1)
{
        double factor = 1.0 / std::accumulate(_probs.begin(), _probs.end(), 0.0);

        unsigned int n = _probs.size();
        assert(n > 0);
        vector<double> probs(n);
        for (unsigned int i = 0; i < n; i++)
                probs[i] = _probs[i] * factor * n;

        deque<unsigned int> small, large;
        for (unsigned int i = 0; i < n; i++) {
                (probs[i] < 1.0 ? small : large).push_back(i);
        }

        while ((not small.empty()) and (not large.empty())) {
                unsigned int l = *small.begin();
                small.pop_front();
                unsigned int g = *large.begin();
                large.pop_front();

                m_blocks[l].prob = probs[l];
                m_blocks[l].alias = g;
                probs[g] = (probs[g] + probs[l]) - 1;
                (probs[g] < 1.0 ? small : large).push_back(g);
        }

        for (unsigned int i : large)
                m_blocks[i].prob = 1.0;
        // If small is not empty, this may be sign of numerical instability
        for (unsigned int i : small)
                m_blocks[i].prob = 1.0;
}

uniform_real_distribution<double> AliasDistribution::g_coinflip = uniform_real_distribution<double>(0, 1);

template <typename K, typename V>
vector<V> map_values(const map<K, V>& m)
{
        vector<V> v;
        v.reserve(m.size());
        for (const auto& it : m) {
                v.push_back(it.second);
        }
        return v;
};

MappedAliasDistribution::MappedAliasDistribution(const map<unsigned int, double>& m) : AliasDistribution(map_values(m))
{
        unsigned int k = 0;
        for (const auto& it : m) {
                m_translation[k] = it.first;
                k++;
        }
}
