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

/**
 * @file
 * Header file for the SocietyGenerator class.
 */

#include "Community.h"
#include "GeneratorPerson.h"
#include "Household.h"
#include "Society.h"
#include "geo/City.h"
#include "geo/Geometry.h"
#include "geo/Village.h"
#include "util/RNG.h"

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace stride {
namespace generator {

/**
 * Generate a society
 */
class SocietyGenerator
{
public:
        /// Create generator from file.
        explicit SocietyGenerator(const boost::filesystem::path& config_path, unsigned int num_threads = 1U);

        /// Generate a society.
        Society Generate();

        /// Return property tree with config info.
        boost::property_tree::ptree GetConfig() { return m_config; }

private:
        /// Generate households based on the household profile and the population size.
        void GenerateHouseholds(std::vector<std::vector<unsigned int>>& hh_profile, unsigned int pop_size);

        /// Generate villages based on the simulation area and the loaded villages.
        void GenerateVillages(const std::vector<Point2D>& simulation_area, const std::vector<Village>& villages);

        /// Load the cities from the csv file into m_cities.
        std::vector<Point2D> LoadCities(boost::filesystem::path cities_path);

        /// Determine the number of villages to generate.
        std::vector<Village> LoadVillages(boost::filesystem::path villages_path);

        /// Assign a location to each of the households.
        void LocateHouseholds();

        /// Assign a location to each of the types of communities.
        void LocateCommunity();

        /// Assign to each person the right community id.
        void AssignToCommunity();

private:
        /// Generate random coordinates.
        Point2D GenerateRandomCoordinates(ConvexPolygonChecker checker, Polygon poly, PointType point_type);

        /// Checks if the coordinates are free, a.k.a., there is no other entity on these coordinates.
        bool IsFreePoint(Point2D coordinates);

        /// Determine the communities still free to assign to.
        std::vector<std::size_t> PossibleCommunities(std::map<std::size_t, int> community_sizes,
                                                     const CommunityType& community_type, Point2D household_location);

        ///
        std::size_t FindClosestCity(Point2D coordinates);

        ///
        std::vector<Point2D> ExpandSimulationArea(std::vector<Point2D> simulation_area);

        ///
        void AssignHouseholdToCommunity(std::size_t i);

        ///
        void ReadGenerateHouseholds();

        ///
        void ReadGenerateCities();

private:
        /// Maps city id's to city data objects.
        std::map<std::size_t, City> m_cities_map;

        std::map<std::size_t, std::map<CommunityType, std::vector<Community>>> m_communities;     ///< Location map.
        std::map<std::size_t, int>                                             m_community_sizes; ///< Community sizes.
        boost::property_tree::ptree                                            m_config;          ///< Configuration.
        std::vector<Household>                                                 m_households;      ///< Households.
        std::map<std::size_t, GeneratorPerson>                                 m_persons;         ///< Persons.
        std::shared_ptr<util::RNGInterface>                                    m_rng;             ///< RNG engine.

private:
        unsigned int m_num_threads; ///< OpenMP threads.

private:
        unsigned int m_comm_size;
        double       m_in_villages;
        double       m_population_size;
        unsigned int m_school_age_min;
        unsigned int m_school_age_max;
        unsigned int m_school_size;
        unsigned int m_univ_age_min;
        unsigned int m_univ_age_max;
        double       m_univ_fraction;
        unsigned int m_univ_size;
        unsigned int m_work_age_min;
        unsigned int m_work_age_max;
        double       m_work_fraction;
        unsigned int m_work_size;
};

} // namespace generator
} // namespace stride
