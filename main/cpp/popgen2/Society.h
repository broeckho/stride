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

/// Society differs from Population clas in that it also has geographical info.
class Society
{
public:
        ///
        Society() = default;

        ///
        Society(std::vector<GeneratorPerson> persons, std::vector<City> cities, std::vector<Community> communities,
                std::vector<Household> households)
            : m_persons(std::move(persons)), m_cities(std::move(cities)), m_communities(std::move(communities)),
              m_households(std::move(households))
        {
        }

        /*
        ///
        Society(std::vector<GeneratorPerson>&& persons, std::vector<City>&& cities,
                std::vector<Community>&& communities, std::vector<Household>&& households)
            : m_persons(persons), m_cities(cities), m_communities(communities), m_households(households)
        {
        }
        */

        ///
        bool operator==(const Society& other) const;

        ///
        const std::vector<GeneratorPerson>& GetPersons() const { return m_persons; }

        ///
        const std::vector<City>& GetCities() const { return m_cities; }

        ///
        const std::vector<Community>& GetCommunities() const { return m_communities; }

        ///
        const std::vector<Household>& GetHouseholds() const { return m_households; }

private:
        std::vector<GeneratorPerson> m_persons;     ///< The persons in the population
        std::vector<City>            m_cities;      ///< The cities in the population
        std::vector<Community>       m_communities; ///< The communities in the population
        std::vector<Household>       m_households;  ///< The households in the population
};

} // namespace generator
} // namespace stride
