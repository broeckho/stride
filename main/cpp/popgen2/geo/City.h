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

#include "GeoGrid.h"

#include <string>

namespace stride {
namespace generator {

class City
{
public:
        static std::size_t g_id_generator;

        City(std::size_t id = 0, double population = 0.0, std::string name = "", std::size_t province = 0,
             Point2D location = Point2D(), Point2D dummy = Point2D());

        City(double population, std::string name, std::size_t province, Point2D location, Point2D dummy);

        bool operator==(const City& other) const;

        std::size_t GetID() const { return m_id; }
        double      GetPopulation() const { return m_population; }
        std::string GetName() const { return m_name; }
        std::size_t GetProvince() const { return m_province; }
        Point2D     GetDummyLocation() const { return m_dummy_location; }
        Point2D     GetLocation() const { return m_location; }

private:
        std::size_t m_id;             ///< ID of the city
        double      m_population;     ///< Fraction of the population living in this city
        std::string m_name;           ///<
        std::size_t m_province;       ///<
        Point2D     m_location;       ///< The Location of the city in longitude and latitude
        Point2D     m_dummy_location; ///< The location of the city in x and y coordinates
};

} // namespace generator
} // namespace stride
