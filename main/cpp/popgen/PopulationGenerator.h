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
 * Population generaton header.
 */

#include "core/ClusterType.h"
#include "geo/GeoCoordCalculator.h"
#include "popgen/utils.h"
#include "util/AliasDistribution.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <trng/lcg64.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace stride {
namespace popgen {

using namespace std;
using namespace util;

using uint = unsigned int;

/**
 * Generate Populations
 */
template <class U>
class PopulationGenerator
{
public:
        /// Constructor: Check if the xml is valid and set up the basic things like a random generator
        PopulationGenerator(const string& filename, const int& seed, bool output = true);

        /// Generates a population, writes the result to the files found in the data directory
        /// Output files are respectively formatted according to the following template files: belgium_population.csv,
        /// pop_miami.csv, pop_miami_geo.csv
        void generate(const string& prefix);

private:
        /// Writes the cities and villages to the file, see PopulationGenerator::generate
        void writeCities(const string& target_cities);

        /// Writes the population to the file, see PopulationGenerator::generate
        void writePop(const string& target_pop) const;

        /// Writes the households to the file, see PopulationGenerator::generate
        void writeHouseholds(const string& target_households) const;

        /// Writes the clusters to the file (type, ID and coordinates), see PopulationGenerator::generate
        void writeClusters(const string& target_clusters) const;

        /// Checks the xml on correctness, this includes only semantic errors, no syntax errors
        void checkForValidXML() const;

        /// Generates all households (not yet their positions)
        void makeHouseholds();

        /// Generate all cities (without inhabitants)
        void makeCities();

        /// Gets the middle of all cities
        GeoCoordinate getCityMiddle() const;

        /// Returns the distance between the given coordinate and the furthest city (in km)
        double getCityRadius(const GeoCoordinate& coord) const;

        /// Get the population of all the cities combined (if they were on full capacity)
        double getCityPopulation() const;

        /// Get the population of all the villages combined (if they were on full capacity)
        double getVillagePopulation() const;

        /// Generate all villages (without inhabitants)
        void makeVillages();

        /// Precompute the distances between the locations of the clusters (given as an argument) and the districts
        /// (villages and cities) The distances measured will start at the radius given as an argument If not all
        /// clusters are within that radius, the radius will be mulktiplied with the given factor until all clusters
        /// have their distance computed The result is a vector of pairs, where each pair maps a coordinate of a city to
        /// another map This inner map has a radius as a key and a vector of indices as its value, these indices refer
        /// to the clusters passed as an argument assume ret_val is the return value: ret_val.at(x).second[10.0] = the
        /// indices of all clusters within a radius of 10.0 kilometres of a certain city ret_val.at(x).first = the
        /// coordinate of the "certain city" used above
        template <typename T>
        vector<pair<GeoCoordinate, map<double, vector<uint>>>> makeDistanceMap(double radius, double factor,
                                                                               const vector<T>& clusters) const
        {
                vector<pair<GeoCoordinate, map<double, vector<uint>>>> distance_map;

                const GeoCoordCalculator& calc = GeoCoordCalculator::getInstance();

                if (m_output)
                        cerr << "Building distance map for the next cluster type [0%]";

                uint done = 0;
                uint total = m_cities.size() + m_villages.size();

                for (auto& city : m_cities) {
                        if (m_output)
                                cerr << "\rBuilding distance map for the next cluster type [" << done / total << "%]";
                        ++done;

                        double current_radius = radius;
                        uint used_clusters = 0;
                        vector<bool> clusters_used = vector<bool>(clusters.size(), false);

                        distance_map.push_back(make_pair(city.m_coord, map<double, vector<uint>>()));

                        while (used_clusters != clusters.size()) {

                                for (uint i = 0; i < clusters.size(); ++i) {
                                        if ((!clusters_used.at(i)) &&
                                            calc.getDistance(city.m_coord, clusters.at(i).m_coord) <= current_radius) {
                                                distance_map.back().second[current_radius].push_back(i);
                                                clusters_used.at(i) = true;
                                                ++used_clusters;
                                        }
                                }

                                current_radius *= factor;
                        }
                }

                for (auto& village : m_villages) {
                        if (m_output)
                                cerr << "\rBuilding distance map for the next cluster type [" << done / total << "%]";
                        ++done;

                        double current_radius = radius;
                        uint used_clusters = 0;
                        vector<bool> clusters_used = vector<bool>(clusters.size(), false);

                        distance_map.push_back(make_pair(village.m_coord, map<double, vector<uint>>()));

                        while (used_clusters != clusters.size()) {

                                for (uint i = 0; i < clusters.size(); ++i) {
                                        if (!clusters_used.at(i) &&
                                            calc.getDistance(village.m_coord, clusters.at(i).m_coord) <=
                                                current_radius) {
                                                distance_map.back().second[current_radius].push_back(i);
                                                clusters_used.at(i) = true;
                                                ++used_clusters;
                                        }
                                }

                                current_radius *= factor;
                        }
                }

                if (m_output)
                        cerr << "\rBuilding distance map for the next cluster type [100%]...\n";

                return distance_map;
        }

        /// Specialization of makeDistanceMap, except now the clusters aren't a vector of clusters anymore, instead,
        /// they are a vector of vectors This is because the schools are a cluster of clusters.
        template <typename T>
        vector<pair<GeoCoordinate, map<double, vector<uint>>>> makeDistanceMap(double radius, double factor,
                                                                               const vector<vector<T>>& clusters) const
        {
                vector<pair<GeoCoordinate, map<double, vector<uint>>>> distance_map;

                const GeoCoordCalculator& calc = GeoCoordCalculator::getInstance();

                for (auto& city : m_cities) {

                        double current_radius = radius;
                        uint used_clusters = 0;
                        vector<bool> clusters_used = vector<bool>(clusters.size(), false);

                        distance_map.push_back(make_pair(city.m_coord, map<double, vector<uint>>()));

                        while (used_clusters != clusters.size()) {

                                for (uint i = 0; i < clusters.size(); ++i) {
                                        if ((!clusters_used.at(i)) &&
                                            calc.getDistance(city.m_coord, clusters.at(i).front().m_coord) <=
                                                current_radius) {
                                                distance_map.back().second[current_radius].push_back(i);
                                                clusters_used.at(i) = true;
                                                ++used_clusters;
                                        }
                                }

                                current_radius *= factor;
                        }
                }

                for (auto& village : m_villages) {

                        double current_radius = radius;
                        uint used_clusters = 0;
                        vector<bool> clusters_used = vector<bool>(clusters.size(), false);

                        distance_map.push_back(make_pair(village.m_coord, map<double, vector<uint>>()));

                        while (used_clusters != clusters.size()) {

                                for (uint i = 0; i < clusters.size(); ++i) {
                                        if (!clusters_used.at(i) &&
                                            calc.getDistance(village.m_coord, clusters.at(i).front().m_coord) <=
                                                current_radius) {
                                                distance_map.back().second[current_radius].push_back(i);
                                                clusters_used.at(i) = true;
                                                ++used_clusters;
                                        }
                                }

                                current_radius *= factor;
                        }
                }

                return distance_map;
        }

        /// Get the clusters that are within the range of a certain coordinate and radius (both given as an argument)
        /// The distances are precomputed by PopulationGenerator::makeDistanceMap and the result of that function has to
        /// be passed as an argument to this function
        vector<uint> getClustersWithinRange(double radius,
                                            const vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map,
                                            GeoCoordinate coordinate) const
        {
                for (auto& coord_map_pair : distance_map) {
                        if (coord_map_pair.first == coordinate) {
                                vector<uint> result;

                                for (auto it = coord_map_pair.second.begin(); it != coord_map_pair.second.end(); ++it) {
                                        if (it->first <= radius) {
                                                result.insert(result.end(), it->second.begin(), it->second.end());
                                        }
                                }

                                return result;
                        }
                }

                return vector<uint>();
        }

        /// Assign the households to a city/village
        void placeHouseholds();

        /// Spreads the clusters of people with these constraints over the cities and villages
        /// size: the size of each cluster
        /// min_age and max_age: the category of people that belongs to these clusters (e.g. schools an work have a
        /// minimum/maximum age)
        template <typename C>
        void placeClusters(uint size, uint min_age, uint max_age, double fraction, C& clusters, string cluster_name,
                           ClusterType::Id cluster_type, bool add_location = true)
        {
                uint people = 0;

                if (min_age == 0 && max_age == 0) {
                        people = m_people.size();
                } else {
                        for (uint age = min_age; age <= max_age; age++) {
                                people += m_age_distribution[age];
                        }
                }

                people = ceil(fraction * people);

                uint needed_clusters = ceil(double(people) / size);
                uint city_village_size = getCityPopulation() + getVillagePopulation();

                /// Get the relative occurrences of both the villages and cities => randomly choose an index in this
                /// vector based on that Note that the vector consists of 2 parts: the first one for the cities, the
                /// second one for the villages, keep this in mind when generating the random index
                vector<double> fractions;
                for (const SimpleCity& city : m_cities) {
                        fractions.push_back(double(city.m_max_size) / double(city_village_size));
                }

                for (const SimpleCluster& village : m_villages) {
                        fractions.push_back(double(village.m_max_size) / double(city_village_size));
                }

                AliasDistribution dist{fractions};
                for (uint i = 0; i < needed_clusters; i++) {
                        if (m_output)
                                cerr << "\rPlacing " << cluster_name << " ["
                                     << min(uint(double(i) / m_households.size() * 100), 100U) << "%]";
                        uint village_city_index = dist(m_rng);

                        if (village_city_index < m_cities.size()) {
                                /// Add to a city
                                SimpleCluster new_cluster;
                                new_cluster.m_max_size = size;
                                new_cluster.m_coord = m_cities.at(village_city_index).m_coord;
                                new_cluster.m_id = m_next_id;
                                m_next_id++;
                                clusters.push_back(new_cluster);

                                if (add_location) {
                                        m_locations[make_pair(cluster_type, new_cluster.m_id)] = new_cluster.m_coord;
                                }
                        } else {
                                /// Add to a village
                                SimpleCluster new_cluster;
                                new_cluster.m_max_size = size;
                                new_cluster.m_coord = m_villages.at(village_city_index - m_cities.size()).m_coord;
                                new_cluster.m_id = m_next_id;
                                m_next_id++;
                                clusters.push_back(new_cluster);

                                if (add_location) {
                                        m_locations[make_pair(cluster_type, new_cluster.m_id)] = new_cluster.m_coord;
                                }
                        }
                }
                if (m_output)
                        cerr << "\rPlacing " << cluster_name << " [100%]...\n";
        }

        /// Make the schools, place them in a village/city
        void makeSchools();

        /// Make the universities, place them in a city
        void makeUniversities();

        /// Make sure workplaces are sorted: workplaces in bigger cities are in front of workplaces in smaller cities
        void sortWorkplaces();

        /// Make workplaces
        /// Note: due to the fact that the cluster size of workplaces must be respected, the amount of working people
        /// will never be above a fixed certain percentage
        void makeWork();

        /// Make the communities
        void makeCommunities();

        /// Get all clusters within a certain radius of the given point, choose those clusters from the given vector of
        /// clusters
        template <typename T>
        vector<uint> getClusters(GeoCoordinate coord, double radius, const vector<T>& clusters) const
        {
                vector<uint> result;
                const GeoCoordCalculator& calc = GeoCoordCalculator::getInstance();
                for (uint i = 0; i < clusters.size(); i++) {
                        if (calc.getDistance(coord, clusters.at(i).m_coord) <= radius) {
                                result.push_back(i);
                        }
                }
                return result;
        }

        /// Put children in mandatory schools
        void assignToSchools();

        /// Put students in universities
        void assignToUniversities();

        /// Remove an element from the university map (the university map is special compared to other cluster maps,
        /// this is because a university is a cluster of clusters)
        void removeFromUniMap(vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map, uint index) const;

        /// Remove an element from the map (of regular clusters, not like universities, because they represent a cluster
        /// of clusters) Return true if the element is deleted, false if not
        bool removeFromMap(vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map, uint index) const;

        /// Put one student in a university according to the rules of commuting students
        void assignCommutingStudent(SimplePerson& person,
                                    vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map);

        /// Put one student in a university according to the rules of students that study close to their home
        void assignCloseStudent(SimplePerson& person, double start_radius,
                                vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map);

        /// Assign people to a workplace
        void assignToWork();

        /// Assign one person to a workplace according to the rule of commuting workers
        bool assignCommutingEmployee(SimplePerson& person,
                                     vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map);

        /// Assign one person to a workplace according to the rule of workers that work close to their home
        bool assignCloseEmployee(SimplePerson& person, double start_radius,
                                 vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map);

        /// Assign entire households
        void assignToCommunities(vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map,
                                 vector<SimpleCluster>& clusters, uint SimplePerson::*member, const string& name = "");

        boost::property_tree::ptree m_props;              /// > The content of the xml file
        U m_rng;                                          /// > The random generator
        uint m_total;                                     /// > Number of people to be generated (according to the xml)
        vector<SimplePerson> m_people;                    /// > All the people
        vector<SimpleHousehold> m_households;             /// > The households
        vector<SimpleCity> m_cities;                      /// > The cities
        vector<SimpleCluster> m_villages;                 /// > The villages
        vector<SimpleCluster> m_workplaces;               /// > The workplaces
        vector<SimpleCluster> m_primary_communities;      /// > The primary communities
        vector<SimpleCluster> m_secondary_communities;    /// > The secondary communities
        vector<SimpleCluster> m_mandatory_schools;        /// > Mandatory schools (Not divided in clusters!!!)
        vector<vector<SimpleCluster>> m_optional_schools; /// > The universities: One univ is a vector of clusters,
                                                          /// ordering is the same as the cities they belong to (using
                                                          /// modulo of course)

        bool m_output;

        /// TODO refactor this, it should be this structure from the beginning (see m_mandatory_schools)
        vector<vector<SimpleCluster>>
            m_mandatory_schools_clusters; /// > The clusters of the mandatory schools, this should be refactored

        uint m_next_id; /// > The next id for the next cluster/school/... ID's are supposed to be unique

        /// Data for visualisation
        // TODO: population density still missing, not sure what to expect
        map<uint, uint> m_age_distribution; /// > The age distribution (histogram)
        map<uint, uint> m_household_size;   /// > The household size (histogram)
        map<uint, uint> m_work_size;        /// > The size of workplaces (histogram)

        map<pair<ClusterType::Id, uint>, GeoCoordinate> m_locations; /// > The locations of clusters (a cluster is
                                                                     /// identified by a type and an ID that is unique
                                                                     /// within this type)
};

} // namespace popgen
} // namespace stride
