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
 *  Copyright 2017, Draulans S, Van Leeuwen L
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

namespace stride {
namespace generator {

class Village
{
public:
        ///
        Village(double pop, unsigned int number) : m_population(pop), m_number(number) {}

        ///
        double GetPopulation() const { return m_population; }

        ///
        unsigned int GetNumber() { return m_number; }

public:
        double m_population;   ///< Population living in a village
        unsigned int m_number; ///< Number of villages of size m_population
};

} // namespace generator
} // namespace stride
