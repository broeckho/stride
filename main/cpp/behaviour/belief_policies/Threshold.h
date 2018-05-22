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
 *  Copyright 2017, 2018, Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Header file for Threshold belief policy.
 */

#include "behaviour/belief_policies/Belief.h"

namespace stride {

class Threshold : public Belief
{
public:
        Threshold()
            : Belief(), m_is_aware(false), m_num_contacts(0), m_num_contacts_infected(0), m_num_contacts_adopted(0),
              m_threshold_infected(1), m_threshold_adopted(1)
        {
        }

private:
        bool         m_is_aware;              ///< Whether the person is aware of the current epidemic.
        unsigned int m_num_contacts;          ///< The total number of contacts the person has had.
        unsigned int m_num_contacts_infected; ///< Number of contacts that is symptomatic.
        unsigned int m_num_contacts_adopted;  ///< Number of contacts that have adopted the behaviour.
        double       m_threshold_infected;    ///< Fraction of contacts that needs to be infected to adopt behaviour.
        double       m_threshold_adopted;     ///< Fraction of contacts that needs to have adopted to adopt behaviour.
};

} // namespace stride
