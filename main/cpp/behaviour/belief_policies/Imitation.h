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
 * Header file for Imitation belief policy.
 */

#include "behaviour/belief_policies/Belief.h"

namespace stride {

class Imitation : public Belief
{
public:
        /// Initializing constructor, for now with ptree.
        Imitation(const boost::property_tree::ptree& pt)
            : Belief(pt), m_accept_threshold(pt.get<double>("accept_threshold")),
              m_no_accept_threshold(pt.get<double>("no_accept_threshold")), m_accept(false)
        {
                // TODO belief strength
                // TODO stickiness
        }

        bool HasAdopted() const { return m_accept; }

        // TODO update

private:
        double m_accept_threshold;
        double m_no_accept_threshold;

        bool m_accept;

        // double m_belief_strength; ///< The 'strength' of someone's belief, varying from -1 (very negative attitude to
        ///< behaviour) to 1 (very positive attitude to behaviour).
        // double m_stickiness;      ///< How likely the belief is to change
        // TODO influence of belonging to same household?
        // TODO awareness?
};

} // namespace stride
