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
 * population generator utilities.
 */

#include "geo/GeoCoordinate.h"

#include <random>
#include <string>

namespace stride {
namespace popgen {

using namespace std;
using namespace util;

#define PI 3.14159265

extern uniform_real_distribution<double> real01;

using uint = unsigned int;

template <class T>
class PopulationGenerator;

class SimplePerson
{
public:
        SimplePerson(uint age = 0, uint family_id = 0);

        friend std::ostream& operator<<(std::ostream& os, const SimplePerson& p);

        template <class U>
        friend class PopulationGenerator;

        uint                m_age                 = 0;
        uint                m_household_id        = 0;
        uint                m_school_id           = 0;
        uint                m_work_id             = 0;
        uint                m_primary_community   = 0;
        uint                m_secondary_community = 0;
        util::GeoCoordinate m_coord;
};

struct SimpleHousehold
{
        uint              m_id = 0;
        std::vector<uint> m_indices;
};

struct SimpleCluster
{
        uint                m_current_size = 0;
        uint                m_max_size     = 0;
        uint                m_id           = 0;
        util::GeoCoordinate m_coord;
};

struct SimpleCity
{

        SimpleCity(uint current_size, uint max_size, uint id, std::string name, util::GeoCoordinate coordinate)
        {
                m_current_size = current_size;
                m_max_size     = max_size;
                m_id           = id;
                m_name         = name;
                m_coord        = coordinate;
        }

        uint                m_current_size = 0;
        uint                m_max_size     = 0;
        uint                m_id           = 0;
        std::string         m_name         = "";
        util::GeoCoordinate m_coord;
};

std::ostream& operator<<(std::ostream& os, const SimplePerson& p);

struct MinMax
{
        MinMax(uint _min = 0, uint _max = 0) : min(_min), max(_max) {}

        uint min;
        uint max;
};

struct MinMaxAvg : public MinMax
{
        MinMaxAvg(uint _min = 0, uint _max = 0, uint _avg = 0) : MinMax(_min, _max), avg(_avg) {}

        uint avg;
};

} // namespace popgen
} // namespace stride
