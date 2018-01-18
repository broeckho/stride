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
 * Population generator uilities implementations.
 */

#include "popgen/utils.h"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace stride;
using namespace popgen;

uniform_real_distribution<double> real01;

std::ostream& operator<<(std::ostream& os, const SimplePerson& p)
{
        assert(p.m_household_id != 0); // everyone is part of a family!

        // "age","household_id","school_id","work_id","primary_community","secondary_community"
        os << p.m_age << "," << p.m_household_id << "," << p.m_school_id << "," << p.m_work_id << ","
           << p.m_primary_community << "," << p.m_secondary_community << "\n";

        return os;
}

SimplePerson::SimplePerson(uint age, uint family_id) : m_age(age), m_household_id(family_id) {}
