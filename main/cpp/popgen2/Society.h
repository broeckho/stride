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
 */

/**
 * @file
 * The header for the Society class.
 */

#include "Community.h"
#include "GeneratorPerson.h"
#include "Household.h"
#include "geo/City.h"
#include "pop/Population.h"

#include <iostream>
#include <vector>

namespace stride {
namespace generator {

class Society
{
public:
        ///
        Society() = default;

        ///
        Society(std::vector<GeneratorPerson> persons, std::vector<City> cities, std::vector<Community> communities,
                std::vector<Household> households)
            : m_persons(persons), m_cities(cities), m_communities(communities), m_households(households)
        {
        }

        ///
        bool operator==(const Society& other) const
        {
                return m_persons == other.GetPersons() && m_cities == other.GetCities();
        }

        ///
        void SetPersons(std::vector<GeneratorPerson> persons) { m_persons = persons; }

        ///
        void SetCities(std::vector<City> cities) { m_cities = cities; }

        ///
        void SetCommunities(std::vector<Community> communities) { m_communities = communities; }

        ///
        void SetHouseholds(std::vector<Household> households) { m_households = households; }

        ///
        const std::vector<GeneratorPerson>& GetPersons() const { return m_persons; }

        ///
        stride::Population GetStridePopulation() const
        {
                stride::Population population;
                population.reserve(m_persons.size());
                for (auto& p : m_persons) {
                        population.push_back(p);
                }
                return population;
        }

        ///
        const std::vector<City>& GetCities() const { return m_cities; }

        ///
        const std::vector<Community>& GetCommunities() const { return m_communities; }

        ///
        const std::vector<Household>& GetHouseholds() const { return m_households; }

private:
        std::vector<GeneratorPerson> m_persons; ///< The persons in the population
        std::vector<City> m_cities;             ///< The cities in the population
        std::vector<Community> m_communities;   ///< The communities in the population
        std::vector<Household> m_households;    ///< The households in the population
};

} // namespace generator
} // namespace stride
