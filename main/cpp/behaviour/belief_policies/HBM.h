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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for HBM belief.
 */

#include "behaviour/belief_policies/Belief.h"

namespace {
constexpr double odds_ratio_0              = -1;
constexpr double odds_ratio_severity       = 0.626;
constexpr double odds_ratio_susceptibility = 1.07;
constexpr double odds_ratio_benefit        = 0;
constexpr double odds_ratio_barriers       = -0.4;
} // namespace

namespace stride {

// p(behaviour) = OR0 * (OR1^x1 * OR2^x2 * OR3^x3 * OR4^x4)/ (1 + OR0 * (prod ORi^xi))

enum class VariableState
{
        Low  = 0U,
        High = 1U
};

class HBM : public Belief
{
public:
        HBM()
            : Belief(), m_num_contacts(0), m_num_contacts_infected(0), m_num_contacts_adopted(0),
              m_perceived_severity(VariableState::Low), m_perceived_susceptibility(VariableState::Low),
              m_perceived_benefit(VariableState::High), m_perceived_barriers(VariableState::High)
        {
        }

private:
        unsigned int  m_num_contacts;
        unsigned int  m_num_contacts_infected;
        unsigned int  m_num_contacts_adopted;
        VariableState m_perceived_severity;
        VariableState m_perceived_susceptibility;
        VariableState m_perceived_benefit;
        VariableState m_perceived_barriers;
};
} // namespace stride
