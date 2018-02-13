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

#include "AliasSampler.h"

using namespace stride;
using namespace util;

AliasSampler::AliasSampler(const vector<double>& probabilities)
    : m_blocks(probabilities.size()), m_diceroll(0, static_cast<unsigned int>(probabilities.size() - 1))
{
        const double factor = 1.0 / std::accumulate(probabilities.begin(), probabilities.end(), 0.0);

        const auto     n = probabilities.size();
        vector<double> probs(n);
        for (unsigned int i = 0; i < n; i++)
                probs[i] = probabilities[i] * factor * n;

        deque<unsigned int> small;
        deque<unsigned int> large;
        for (unsigned int i = 0; i < n; i++) {
                (probs[i] < 1.0 ? small : large).push_back(i);
        }

        while ((not small.empty()) and (not large.empty())) {
                const unsigned int l = *small.begin();
                small.pop_front();
                const unsigned int g = *large.begin();
                large.pop_front();

                m_blocks[l].prob  = probs[l];
                m_blocks[l].alias = g;
                probs[g]          = (probs[g] + probs[l]) - 1;
                (probs[g] < 1.0 ? small : large).push_back(g);
        }

        for (unsigned int i : large) {
                m_blocks[i].prob = 1.0;
        }
        // If small is not empty, this may be sign of numerical instability
        for (unsigned int i : small) {
                m_blocks[i].prob = 1.0;
        }
}
