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
 * Population genarator implementations.
 */

#include "PopulationGenerator.h"
#include "FamilyParser.h"
#include "util/InstallDirs.h"
#include "util/TimeStamp.h"

#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <limits>
#include <stdexcept>

using namespace stride;
using namespace popgen;
using namespace util;
using namespace boost::property_tree;
using namespace xml_parser;

template <class U>
PopulationGenerator<U>::PopulationGenerator(const string& filename, const int& seed, bool output)
{
        // check data environment.
        if (InstallDirs::GetDataDir().empty()) {
                throw runtime_error(string(__func__) + "> Data directory not present! Aborting.");
        }

        try {
                read_xml((InstallDirs::GetDataDir() /= filename).string(), m_props, trim_whitespace | no_comments);
        } catch (exception& e) {
                throw invalid_argument(string("Invalid file: ") + (InstallDirs::GetDataDir() /= filename).string());
        }

        try {
                long int tot = m_props.get<long int>("population.<xmlattr>.total");

                if (tot <= 0) {
                        throw invalid_argument("Invalid attribute population::total");
                }

                m_total = static_cast<uint>(tot);
        } catch (invalid_argument& e) {
                throw e;
        } catch (exception& e) {
                throw invalid_argument("Missing/invalid attribute population::total");
        }

        m_next_id = 1;
        m_output  = output;
        m_rng     = U(seed);

        checkForValidXML();
}

template <class U>
void PopulationGenerator<U>::generate(const string& prefix)
{
        if (m_output) {
                cerr << "Generating " << m_total << " people...\n";
        }
        makeHouseholds();
        makeCities();
        makeVillages();
        placeHouseholds();
        makeSchools();
        makeUniversities();
        makeWork();
        makeCommunities();
        assignToSchools();
        assignToUniversities();
        assignToWork();

        auto start_radius = m_props.get<double>("population.commutingdata.<xmlattr>.start_radius");
        auto factor       = m_props.get<double>("population.commutingdata.<xmlattr>.factor");

        vector<pair<GeoCoordinate, map<double, vector<uint>>>> distance_map =
            makeDistanceMap(start_radius, factor, m_primary_communities);
        assignToCommunities(distance_map, m_primary_communities, &SimplePerson::m_primary_community,
                            "primary communities");

        distance_map.clear();
        distance_map = makeDistanceMap(start_radius, factor, m_secondary_communities);
        assignToCommunities(distance_map, m_secondary_communities, &SimplePerson::m_secondary_community,
                            "secondary communities");

        if (m_output)
                cerr << "Generated " << m_people.size() << " people\n";

        string target_cities     = prefix + "_districts.csv";
        string target_pop        = prefix + "_people.csv";
        string target_households = prefix + "_households.csv";
        string target_clusters   = prefix + "_clusters.csv";
        string target_summary    = prefix + ".xml";

        writeCities(target_cities);
        writePop(target_pop);
        writeHouseholds(target_households);
        writeClusters(target_clusters);

        // Now write a summary
        ptree config;
        config.put("population.people", target_pop);
        config.put("population.districts", target_cities);
        config.put("population.clusters", target_clusters);
        config.put("population.households", target_households);
        config.add_child("population.cities", m_props.get_child("population.cities"));
        write_xml((InstallDirs::GetDataDir() /= target_summary).string(), config);
        if (m_output)
                cout << "Written summary " << target_summary << endl;
}

template <class U>
void PopulationGenerator<U>::writeCities(const string& target_cities)
{
        ofstream my_file{(InstallDirs::GetDataDir() /= target_cities).string()};
        double   total_pop = 0.0;

        for (const SimpleCity& city : m_cities) {
                total_pop += city.m_current_size;
        }

        for (const SimpleCluster& village : m_villages) {
                total_pop += village.m_current_size;
        }

        if (my_file.is_open()) {
                my_file << "\"city_id\",\"city_name\",\"province\",\"population\",\"x_coord\",\"y_coord\",\"latitude\","
                           "\"longitude\"\n";

                uint         provinces = m_props.get<uint>("population.<xmlattr>.provinces");
                AliasSampler dist{vector<double>(provinces, 1.0 / provinces)};

                auto printCityData = [&](const SimpleCity& to_print) {
                        my_file << to_print.m_current_size / total_pop << ",0,0," << to_print.m_coord.m_latitude << ","
                                << to_print.m_coord.m_longitude << endl;
                };

                auto printVillageData = [&](const SimpleCluster& to_print) {
                        SimpleCity city = SimpleCity(to_print.m_current_size, to_print.m_max_size, to_print.m_id, "",
                                                     to_print.m_coord);
                        printCityData(city);
                };

                for (const SimpleCity& city : m_cities) {
                        my_file.precision(std::numeric_limits<double>::max_digits10);
                        my_file << city.m_id << ",\"" << city.m_name << "\"," << dist(m_rng) + 1 << ",";
                        printCityData(city);
                }

                uint village_counter = 1;
                for (const SimpleCluster& village : m_villages) {
                        my_file.precision(std::numeric_limits<double>::max_digits10);
                        my_file << village.m_id << ",\"" << village_counter << "\"," << dist(m_rng) + 1 << ",";

                        printVillageData(village);
                        village_counter++;
                }

                my_file.close();
                if (m_output)
                        cout << "Written " << target_cities << endl;
        } else {
                throw invalid_argument("In PopulationGenerator: Invalid file.");
        }
}

template <class U>
void PopulationGenerator<U>::writePop(const string& target_pop) const
{
        ofstream my_file{(InstallDirs::GetDataDir() /= target_pop).string()};

        if (my_file.is_open()) {
                my_file << "\"age\",\"household_id\",\"school_id\",\"work_id\",\"primary_community\",\"secondary_"
                           "community\"\n";

                for (const SimplePerson& person : m_people) {
                        my_file << person.m_age << "," << person.m_household_id << "," << person.m_school_id << ","
                                << person.m_work_id << "," << person.m_primary_community << ","
                                << person.m_secondary_community << endl;
                }

                my_file.close();
                if (m_output)
                        cout << "Written " << target_pop << endl;
        } else {
                throw invalid_argument("In PopulationGenerator: Invalid file.");
        }
}

template <class U>
void PopulationGenerator<U>::writeHouseholds(const string& target_households) const
{
        ofstream my_file{(InstallDirs::GetDataDir() /= target_households).string()};
        if (my_file.is_open()) {
                my_file << "\"hh_id\",\"latitude\",\"longitude\",\"size\"\n";

                for (const SimpleHousehold& household : m_households) {
                        my_file << household.m_id << "," << m_people.at(household.m_indices.at(0)).m_coord.m_latitude
                                << "," << m_people.at(household.m_indices.at(0)).m_coord.m_longitude << ","
                                << household.m_indices.size() << endl;
                }

                my_file.close();
                if (m_output)
                        cout << "Written " << target_households << endl;
        } else {
                throw invalid_argument("In PopulationGenerator: Invalid file.");
        }
}

template <class U>
void PopulationGenerator<U>::writeClusters(const string& target_clusters) const
{
        ofstream my_file{(InstallDirs::GetDataDir() /= target_clusters).string()};
        if (my_file.is_open()) {
                my_file << "\"cluster_id\",\"cluster_type\",\"latitude\",\"longitude\"\n";

                using namespace ContactPoolType;
                vector<Id> types{Id::Household, Id::School, Id::Work, Id::PrimaryCommunity, Id::SecondaryCommunity};

                for (auto& typ : types) {
                        uint current_id = 1;
                        while (m_locations.find(make_pair(typ, current_id)) != m_locations.end()) {
                                my_file.precision(std::numeric_limits<double>::max_digits10);
                                my_file << current_id << "," << ToString(typ) << ","
                                        << m_locations.at(make_pair(typ, current_id)).m_latitude << ","
                                        << m_locations.at(make_pair(typ, current_id)).m_longitude << endl;
                                ++current_id;
                        }
                }

                my_file.close();
                if (m_output)
                        cout << "Written " << target_clusters << endl;
        } else {
                throw invalid_argument("In PopulationGenerator: Invalid file.");
        }
}

template <class U>
void PopulationGenerator<U>::checkForValidXML() const
{
        ptree pop_config = m_props.get_child("population");

        /// RNG is already valid at this point (made in constructor)
        /// Check for family tag must be done during parsing

        /// Check for the provinces
        int provinces = pop_config.get<int>("<xmlattr>.provinces");

        if (provinces <= 0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        /// Valid commuting data: the start radius is a positive double, and the factor is a double greater than 1
        double radius = pop_config.get<double>("commutingdata.<xmlattr>.start_radius");
        double factor = pop_config.get<double>("commutingdata.<xmlattr>.factor");

        if (radius <= 0 || factor <= 1.0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        /// Cities: unique location
        if (m_output)
                cerr << "\rChecking for valid XML [0%]";
        int                   total_size    = 0;
        bool                  has_no_cities = true;
        auto                  cities_config = pop_config.get_child("cities");
        vector<GeoCoordinate> current_locations;
        for (auto it = cities_config.begin(); it != cities_config.end(); it++) {
                if (it->first == "city") {
                        has_no_cities = false;
                        it->second.get<string>("<xmlattr>.name");
                        total_size += it->second.get<int>("<xmlattr>.pop");
                        double latitude  = it->second.get<double>("<xmlattr>.lat");
                        double longitude = it->second.get<double>("<xmlattr>.lon");

                        if (abs(latitude) > 90 || abs(longitude) > 180) {
                                throw invalid_argument("In PopulationGenerator: Invalid geo-coordinate in XML.");
                        }

                        if (it->second.get<int>("<xmlattr>.pop") <= 0) {
                                throw invalid_argument("In PopulationGenerator: Numerical error.");
                        }

                        auto it2 = find(current_locations.begin(), current_locations.end(),
                                        GeoCoordinate(latitude, longitude));
                        if (it2 != current_locations.end())
                                throw invalid_argument("In PopulationGenerator: Duplicate coordinates given in XML.");

                        current_locations.push_back(GeoCoordinate(latitude, longitude));
                } else {
                        throw invalid_argument("In PopulationGenerator: Missing/incorrect tags/attributes in XML.");
                }
        }

        if (has_no_cities) {
                throw invalid_argument("In PopulationGenerator: No cities found.");
        }

        /// Check for valid villages
        if (m_output)
                cerr << "\rChecking for valid XML [18%]";
        auto   village_config        = pop_config.get_child("villages");
        double village_radius_factor = village_config.get<double>("<xmlattr>.radius");

        bool has_no_villages = true;

        double fraction = 0.0;
        for (auto it = village_config.begin(); it != village_config.end(); it++) {
                if (it->first == "village") {
                        has_no_villages = false;
                        int minimum     = it->second.get<int>("<xmlattr>.min");
                        int max         = it->second.get<int>("<xmlattr>.max");
                        fraction += it->second.get<double>("<xmlattr>.fraction") / 100.0;

                        if (fraction < 0) {
                                throw invalid_argument("In PopulationGenerator: Numerical error.");
                        }

                        if (minimum > max || minimum <= 0 || max < 0) {
                                throw invalid_argument("In PopulationGenerator: Numerical error.");
                        }
                } else if (it->first == "<xmlattr>") {
                } else {
                        throw invalid_argument("In PopulationGenerator: Missing/incorrect tags/attributes in XML.");
                }
        }

        if (fraction != 1.0 || village_radius_factor <= 0.0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        if (has_no_villages) {
                throw invalid_argument("In PopulationGenerator: No villages found.");
        }

        /// Check for valid Education
        /// Mandatory education
        if (m_output)
                cerr << "\rChecking for valid XML [36%]";
        auto education_config   = pop_config.get_child("education");
        auto school_work_config = pop_config.get_child("school_work_profile");
        total_size              = education_config.get<int>("mandatory.<xmlattr>.total_size");
        int cluster_size        = education_config.get<int>("mandatory.<xmlattr>.cluster_size");
        int mandatory_min       = school_work_config.get<int>("mandatory.<xmlattr>.min");
        int mandatory_max       = school_work_config.get<int>("mandatory.<xmlattr>.max");

        if (mandatory_min > mandatory_max || mandatory_min < 0 || mandatory_max < 0) {
                throw invalid_argument("In PopulationGenerator: Numerical error in min/max pair.");
        }

        if (total_size <= 0 || cluster_size <= 0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        /// Optional education
        if (m_output)
                cerr << "\rChecking for valid XML [42%]";
        school_work_config = pop_config.get_child("school_work_profile.employable.young_employee");
        int minimum        = school_work_config.get<int>("<xmlattr>.min");
        int max            = school_work_config.get<int>("<xmlattr>.max");
        cluster_size       = education_config.get<int>("optional.<xmlattr>.cluster_size");
        fraction           = 1.0 - school_work_config.get<double>("<xmlattr>.fraction") / 100.0;
        total_size         = education_config.get<uint>("optional.<xmlattr>.total_size");

        if (minimum > max || minimum < 0 || max < 0) {
                throw invalid_argument("In PopulationGenerator: Numerical error in min/max pair.");
        }

        if (total_size <= 0 || cluster_size <= 0 || fraction < 0.0 || fraction > 1.0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        if (minimum <= mandatory_max) {
                throw invalid_argument("In PopulationGenerator: Overlapping min/max pairs.");
        }

        fraction = education_config.get<double>("optional.far.<xmlattr>.fraction") / 100.0;

        if (fraction < 0.0 || fraction > 1.0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        /// Check for valid work
        if (m_output)
                cerr << "\rChecking for valid XML [68%]";
        school_work_config = pop_config.get_child("school_work_profile.employable");
        auto work_config   = pop_config.get_child("work");

        total_size = work_config.get<int>("<xmlattr>.size");
        minimum    = school_work_config.get<int>("employee.<xmlattr>.min");
        max        = school_work_config.get<int>("employee.<xmlattr>.max");
        fraction   = school_work_config.get<double>("<xmlattr>.fraction") / 100.0;

        if (minimum > max || minimum < 0 || max < 0) {
                throw invalid_argument("In PopulationGenerator: Numerical error in min/max pair.");
        }

        if (total_size <= 0 || cluster_size <= 0 || fraction < 0.0 || fraction > 1.0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        int min2 = school_work_config.get<int>("young_employee.<xmlattr>.min");
        int max2 = school_work_config.get<int>("young_employee.<xmlattr>.max");
        fraction = work_config.get<double>("far.<xmlattr>.fraction") / 100.0;

        if (min2 > max2 || min2 < 0 || max2 < 0) {
                throw invalid_argument("In PopulationGenerator: Numerical error in min/max pair.");
        }

        if (max2 >= minimum) {
                throw invalid_argument("In PopulationGenerator: Overlapping min/max pairs.");
        }

        if (fraction < 0.0 || fraction > 1.0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }

        /// Check for valid communities
        if (m_output)
                cerr << "\rChecking for valid XML [84%]";
        total_size = pop_config.get<int>("community.<xmlattr>.size");

        if (total_size <= 0) {
                throw invalid_argument("In PopulationGenerator: Numerical error.");
        }
        if (m_output)
                cerr << "\rChecking for valid XML [100%]\n";
}

template <class U>
void PopulationGenerator<U>::makeHouseholds()
{
        m_next_id        = 1;
        string file_name = m_props.get<string>("population.family.<xmlattr>.file");

        FamilyParser parser;

        vector<FamilyConfig> family_config{parser.parseFamilies(file_name)};

        uint current_generated = 0;

        /// Uniformly choose between the given family configurations
        AliasSampler dist{vector<double>(family_config.size(), 1.0 / family_config.size())};
        while (current_generated < m_total) {
                if (m_output)
                        cerr << "\rGenerating households ["
                             << min(uint(double(current_generated) / m_total * 100), 100U) << "%]";

                /// Get the family configuration
                uint          family_index = dist(m_rng);
                FamilyConfig& new_config   = family_config.at(family_index);

                /// Make the configuration into reality
                SimpleHousehold new_household;
                new_household.m_id = m_next_id;
                for (uint& age : new_config) {
                        SimplePerson new_person{age, m_next_id};
                        new_person.m_household_id = m_next_id;
                        m_people.push_back(new_person);
                        new_household.m_indices.push_back(m_people.size() - 1);

                        /// For visualisation purposes
                        m_age_distribution[age] = m_age_distribution[age] + 1;
                }

                /// For visualisation purposes
                m_household_size[new_config.size()] = m_household_size[new_config.size()] + 1;

                m_households.push_back(new_household);
                m_next_id++;
                current_generated += new_config.size();
        }
        if (m_output)
                cerr << "\rGenerating households [100%]...\n";
}

template <class U>
void PopulationGenerator<U>::makeCities()
{
        ptree cities_config = m_props.get_child("population.cities");
        m_next_id           = 1;
        uint size_check     = 0;

        uint generated   = 0;
        uint to_generate = distance(cities_config.begin(), cities_config.end());
        for (auto it = cities_config.begin(); it != cities_config.end(); it++) {
                if (m_output)
                        cerr << "\rGenerating cities [" << min(uint(double(generated) / to_generate * 100), 100U)
                             << "%]";
                if (it->first == "city") {
                        string name      = it->second.get<string>("<xmlattr>.name");
                        int    size      = it->second.get<int>("<xmlattr>.pop");
                        double latitude  = it->second.get<double>("<xmlattr>.lat");
                        double longitude = it->second.get<double>("<xmlattr>.lon");
                        size_check += size;

                        SimpleCity new_city = SimpleCity(0, size, m_next_id, name, GeoCoordinate(latitude, longitude));
                        ++m_next_id;

                        m_cities.push_back(new_city);
                }
                generated++;
        }
        if (m_output)
                cerr << "\rGenerating cities [100%]...\n";

        /// Important, make sure the vector is sorted (biggest to smallest)!
        auto compare_city_size = [](const SimpleCity& a, const SimpleCity b) { return a.m_max_size > b.m_max_size; };
        sort(m_cities.begin(), m_cities.end(), compare_city_size);
}

template <class U>
GeoCoordinate PopulationGenerator<U>::getCityMiddle() const
{
        double latitude_middle  = 0.0;
        double longitude_middle = 0.0;
        for (const SimpleCity& city : m_cities) {
                latitude_middle += city.m_coord.m_latitude;
                longitude_middle += city.m_coord.m_longitude;
        }
        latitude_middle /= m_cities.size();
        longitude_middle /= m_cities.size();

        GeoCoordinate result;
        result.m_latitude  = latitude_middle;
        result.m_longitude = longitude_middle;
        return result;
}

template <class U>
double PopulationGenerator<U>::getCityRadius(const GeoCoordinate& coord) const
{
        double current_maximum = -1.0;

        const GeoCoordCalculator& calc = GeoCoordCalculator::getInstance();
        for (const SimpleCity& city : m_cities) {
                double distance = calc.getDistance(coord, city.m_coord);
                if (distance > current_maximum) {
                        current_maximum = distance;
                }
        }

        return current_maximum;
}

template <class U>
double PopulationGenerator<U>::getCityPopulation() const
{
        uint result = 0;

        for (const SimpleCity& city : m_cities) {
                result += city.m_max_size;
        }

        return result;
}

template <class U>
double PopulationGenerator<U>::getVillagePopulation() const
{
        uint result = 0;

        for (const SimpleCluster& village : m_villages) {
                result += village.m_max_size;
        }

        return result;
}

template <class U>
void PopulationGenerator<U>::makeVillages()
{
        // Do NOT reset the id counter (cities and villages will be treated as one)
        ptree         village_config                 = m_props.get_child("population.villages");
        double        village_radius_factor          = village_config.get<double>("<xmlattr>.radius");
        GeoCoordinate middle                         = getCityMiddle();
        double        radius                         = getCityRadius(middle);
        uint          city_population                = getCityPopulation();
        int           unassigned_population          = m_people.size() - city_population;
        int           unassigned_population_progress = unassigned_population;

        /// Get the configuration of the villages (relative occurrence, minimum and maximum population)
        vector<double> fractions;
        vector<MinMax> boundaries;
        for (auto it = village_config.begin(); it != village_config.end(); it++) {
                if (it->first == "village") {
                        uint   min      = it->second.get<uint>("<xmlattr>.min");
                        uint   max      = it->second.get<uint>("<xmlattr>.max");
                        double fraction = it->second.get<double>("<xmlattr>.fraction") / 100.0;
                        MinMax min_max{min, max};

                        fractions.push_back(fraction);
                        boundaries.push_back(min_max);
                }
        }

        /// Depending on the relative occurrence of a village, choose this village
        AliasSampler              village_type_dist{fractions};
        const GeoCoordCalculator& calc = GeoCoordCalculator::getInstance();
        while (unassigned_population > 0) {
                if (m_output)
                        cerr << "\rGenerating villages ["
                             << min(uint(double(unassigned_population_progress - unassigned_population) /
                                         unassigned_population_progress * 100),
                                    100U)
                             << "%]";
                uint   village_type_index  = village_type_dist(m_rng);
                MinMax village_pop         = boundaries.at(village_type_index);
                uint   range_interval_size = village_pop.max - village_pop.min + 1;

                AliasSampler village_size_dist{vector<double>(range_interval_size, 1.0 / range_interval_size)};
                uint         village_size = village_size_dist(m_rng) + village_pop.min;

                SimpleCluster new_village;
                new_village.m_max_size = village_size;
                new_village.m_id       = m_next_id;
                m_next_id++;
                new_village.m_coord = calc.generateRandomCoord(middle, radius * village_radius_factor, m_rng);

                /// Make sure this isn't a duplicate coordinate
                auto same_coordinate_village = [&](const SimpleCluster& cl) {
                        return cl.m_coord == new_village.m_coord;
                };
                auto same_coordinate_city = [&](const SimpleCity& cl) { return cl.m_coord == new_village.m_coord; };

                auto it_villages = find_if(m_villages.begin(), m_villages.end(), same_coordinate_village);
                auto it_cities   = find_if(m_cities.begin(), m_cities.end(), same_coordinate_city);

                if (it_villages == m_villages.end() && it_cities == m_cities.end()) {
                        m_villages.push_back(new_village);
                        unassigned_population -= new_village.m_max_size;
                }
        }
        if (m_output)
                cerr << "\rGenerating villages [100%]...\n";
}

template <class U>
void PopulationGenerator<U>::placeHouseholds()
{
        uint city_pop    = getCityPopulation();
        uint village_pop = getVillagePopulation();
        uint total_pop   = village_pop + city_pop; // This may slightly differ from other "total pop" numbers

        /// Get the relative occurrences of both the villages and cities => randomly choose an index in this vector
        /// based on that Note that the vector consists of 2 parts: the first one for the cities, the second one for the
        /// villages, keep this in mind when generating the random index
        vector<double> fractions;
        for (SimpleCity& city : m_cities) {
                fractions.push_back(double(city.m_max_size) / double(total_pop));
        }

        for (SimpleCluster& village : m_villages) {
                fractions.push_back(double(village.m_max_size) / double(total_pop));
        }

        AliasSampler village_city_dist{fractions};
        int          i = 0;
        for (SimpleHousehold& household : m_households) {
                if (m_output)
                        cerr << "\rPlacing households [" << min(uint(double(i) / m_households.size() * 100), 100U)
                             << "%]";
                uint index = village_city_dist(m_rng);
                if (index < m_cities.size()) {
                        /// A city has been chosen
                        SimpleCity& city = m_cities.at(index);
                        city.m_current_size += household.m_indices.size();
                        for (uint& person_index : household.m_indices) {
                                m_people.at(person_index).m_coord = city.m_coord;
                        }
                        m_locations[make_pair(ContactPoolType::Id::Household, household.m_id)] = city.m_coord;

                } else {
                        /// A village has been chosen
                        SimpleCluster& village = m_villages.at(index - m_cities.size());
                        village.m_current_size += household.m_indices.size();
                        for (uint& person_index : household.m_indices) {
                                m_people.at(person_index).m_coord = village.m_coord;
                        }
                        m_locations[make_pair(ContactPoolType::Id::Household, household.m_id)] = village.m_coord;
                }
                i++;
        }
        if (m_output)
                cerr << "\rPlacing households [100%]...\n";
}

template <class U>
void PopulationGenerator<U>::makeSchools()
{
        /// Note: schools are "assigned" to villages and cities
        m_next_id                = 1;
        ptree education_config   = m_props.get_child("population.education");
        ptree school_work_config = m_props.get_child("population.school_work_profile.mandatory");
        uint  school_size        = education_config.get<uint>("mandatory.<xmlattr>.total_size");
        uint  min_age            = school_work_config.get<uint>("<xmlattr>.min");
        uint  max_age            = school_work_config.get<uint>("<xmlattr>.max");
        uint  cluster_size       = education_config.get<uint>("mandatory.<xmlattr>.cluster_size");

        placeClusters(school_size, min_age, max_age, 1.0, m_mandatory_schools, "schools", ContactPoolType::Id::School,
                      false);

        // Split the schools in clusters
        m_next_id = 1;
        for (SimpleCluster& cluster : m_mandatory_schools) {
                uint current_size = 0;

                m_mandatory_schools_clusters.push_back(vector<SimpleCluster>());
                while (current_size < cluster.m_max_size) {
                        current_size += cluster_size;
                        SimpleCluster new_cluster;
                        new_cluster.m_max_size = cluster_size;
                        new_cluster.m_id       = m_next_id;
                        m_next_id++;
                        new_cluster.m_coord = cluster.m_coord;
                        m_mandatory_schools_clusters.back().push_back(new_cluster);
                        m_locations[make_pair(ContactPoolType::Id::School, new_cluster.m_id)] = new_cluster.m_coord;
                }
        }
}

template <class U>
void PopulationGenerator<U>::makeUniversities()
{
        // Note: don't reset the next_id, the universities are also "schools", and the previously handled function was
        // "makeSchools"
        ptree  school_work_config = m_props.get_child("population.school_work_profile.employable.young_employee");
        ptree  university_config  = m_props.get_child("population.education.optional");
        uint   min_age            = school_work_config.get<uint>("<xmlattr>.min");
        uint   max_age            = school_work_config.get<uint>("<xmlattr>.max");
        double fraction           = 1.0 - school_work_config.get<double>("<xmlattr>.fraction") / 100.0;
        uint   size               = university_config.get<uint>("<xmlattr>.total_size");
        uint   cluster_size       = university_config.get<uint>("<xmlattr>.cluster_size");

        uint intellectual_pop = 0;
        for (uint i = min_age; i <= max_age; i++) {
                intellectual_pop += m_age_distribution[i];
        }

        intellectual_pop = ceil(intellectual_pop * fraction);

        uint needed_universities = ceil(double(intellectual_pop) / size);
        uint placed_universities = 0;
        uint clusters_per_univ = size / cluster_size; /// Note: not +1 as you cannot exceed a certain amount of students
        uint left_over_cluster_size = size % cluster_size;

        m_optional_schools.clear();

        while (needed_universities > placed_universities) {
                if (m_output)
                        cerr << "\rPlacing Universities ["
                             << min(uint(double(needed_universities) / placed_universities * 100), 100U) << "%]";

                /// Add a university to the list
                /// NOTE: a university is a vector of clusters
                vector<SimpleCluster> univ;
                for (uint i = 0; i < clusters_per_univ; i++) {
                        SimpleCluster univ_cluster;
                        univ_cluster.m_id       = m_next_id;
                        univ_cluster.m_max_size = cluster_size;
                        univ_cluster.m_coord    = m_cities.at(placed_universities % m_cities.size()).m_coord;
                        m_next_id++;
                        univ.push_back(univ_cluster);

                        m_locations[make_pair(ContactPoolType::Id::School, univ_cluster.m_id)] = univ_cluster.m_coord;
                }

                if (left_over_cluster_size > 0) {
                        /// Because the last cluster might not fit in the university, this cluster is smaller
                        SimpleCluster univ_cluster;
                        univ_cluster.m_id       = m_next_id;
                        univ_cluster.m_max_size = left_over_cluster_size;
                        univ_cluster.m_coord    = m_cities.at(placed_universities % m_cities.size()).m_coord;
                        m_next_id++;
                        univ.push_back(univ_cluster);

                        m_locations[make_pair(ContactPoolType::Id::School, univ_cluster.m_id)] = univ_cluster.m_coord;
                }

                m_optional_schools.push_back(univ);
                placed_universities++;
        }
}

template <class U>
void PopulationGenerator<U>::sortWorkplaces()
{
        /// Sorts according to the cities (assumes they are sorted in a way that you might desire)
        vector<SimpleCluster> result;

        for (SimpleCity& city : m_cities) {
                for (SimpleCluster& workplace : m_workplaces) {
                        if (city.m_coord == workplace.m_coord) {
                                result.push_back(workplace);
                        }
                }
        }

        for (SimpleCluster& village : m_villages) {
                for (SimpleCluster& workplace : m_workplaces) {
                        if (village.m_coord == workplace.m_coord) {
                                result.push_back(workplace);
                        }
                }
        }

        m_workplaces = result;
}

template <class U>
void PopulationGenerator<U>::makeWork()
{
        m_next_id                = 1;
        ptree school_work_config = m_props.get_child("population.school_work_profile.employable");
        ptree work_config        = m_props.get_child("population.work");

        uint   size           = work_config.get<uint>("<xmlattr>.size");
        uint   min_age        = school_work_config.get<uint>("employee.<xmlattr>.min");
        uint   max_age        = school_work_config.get<uint>("employee.<xmlattr>.max");
        uint   young_min_age  = school_work_config.get<uint>("young_employee.<xmlattr>.min");
        uint   young_max_age  = school_work_config.get<uint>("young_employee.<xmlattr>.max");
        double young_fraction = school_work_config.get<double>("young_employee.<xmlattr>.fraction") / 100.0;
        double fraction       = school_work_config.get<double>("<xmlattr>.fraction") / 100.0;

        uint possible_students = 0;
        uint total_old         = 0;
        for (SimplePerson& person : m_people) {
                if (person.m_age >= young_min_age && person.m_age <= young_max_age) {
                        possible_students++;
                }

                if (person.m_age >= std::max(min_age, young_max_age + 1) && person.m_age <= max_age) {
                        total_old++;
                }
        }

        uint total_of_age = possible_students + total_old;
        // Subtract actual students
        uint total_working = total_of_age - ceil(possible_students * (1.0 - young_fraction));
        total_working      = ceil(total_working * fraction);

        // Calculate the actual fraction of people between young_min_age and max_age who are working
        double actual_fraction = double(total_working) / total_of_age;

        placeClusters(size, young_min_age, max_age, actual_fraction, m_workplaces, "workplaces",
                      ContactPoolType::Id::Work);

        /// Make sure the work clusters are sorted from big city to smaller city
        sortWorkplaces();
}

template <class U>
void PopulationGenerator<U>::makeCommunities()
{
        /// TODO? Currently not doing the thing with the average communities per person, right now, everyone gets two
        /// communities
        m_next_id              = 1;
        ptree community_config = m_props.get_child("population.community");
        uint  size             = community_config.get<uint>("<xmlattr>.size");

        placeClusters(size, 0, 0, 1.0, m_primary_communities, "primary communities",
                      ContactPoolType::Id::PrimaryCommunity);
        m_next_id = 1;
        placeClusters(size, 0, 0, 1.0, m_secondary_communities, "secondary communities",
                      ContactPoolType::Id::SecondaryCommunity);
}

template <class U>
void PopulationGenerator<U>::assignToSchools()
{
        m_next_id                 = 1;
        ptree  education_config   = m_props.get_child("population.education.mandatory");
        ptree  school_work_config = m_props.get_child("population.school_work_profile.mandatory");
        uint   min_age            = school_work_config.get<uint>("<xmlattr>.min");
        uint   max_age            = school_work_config.get<uint>("<xmlattr>.max");
        double start_radius       = m_props.get<double>("population.commutingdata.<xmlattr>.start_radius");

        double factor = m_props.get<double>("population.commutingdata.<xmlattr>.factor");

        double current_radius = start_radius;

        uint total        = 0;
        uint total_placed = 0;

        for (uint i = min_age; i <= max_age; i++) {
                total += m_age_distribution[i];
        }

        auto distance_map = makeDistanceMap(start_radius, factor, m_mandatory_schools);

        for (SimplePerson& person : m_people) {
                current_radius = start_radius;
                if (person.m_age >= min_age && person.m_age <= max_age) {
                        if (m_output)
                                cerr << "\rAssigning children to schools ["
                                     << min(uint(double(total_placed) / total * 100), 100U) << "%]";
                        total_placed++;

                        vector<uint> closest_clusters_indices;

                        while (closest_clusters_indices.size() == 0 && m_mandatory_schools.size() != 0) {
                                closest_clusters_indices =
                                    getClustersWithinRange(current_radius, distance_map, person.m_coord);
                                current_radius *= factor;
                        }

                        AliasSampler cluster_dist{vector<double>(closest_clusters_indices.size(),
                                                                 1.0 / double(closest_clusters_indices.size()))};
                        uint         index = closest_clusters_indices.at(cluster_dist(m_rng));

                        AliasSampler inner_cluster_dist{
                            vector<double>(m_mandatory_schools_clusters.at(index).size(),
                                           1.0 / m_mandatory_schools_clusters.at(index).size())};
                        uint index2 = inner_cluster_dist(m_rng);

                        m_mandatory_schools_clusters.at(index).at(index2).m_current_size++;
                        person.m_school_id = m_mandatory_schools_clusters.at(index).at(index2).m_id;
                }
        }
        if (m_output)
                cerr << "\rAssigning children to schools [100%]...\n";
}

template <class U>
void PopulationGenerator<U>::assignToUniversities()
{
        ptree  school_work_config = m_props.get_child("population.school_work_profile.employable.young_employee");
        ptree  university_config  = m_props.get_child("population.education.optional");
        uint   min_age            = school_work_config.get<uint>("<xmlattr>.min");
        uint   max_age            = school_work_config.get<uint>("<xmlattr>.max");
        double student_fraction   = 1.0 - school_work_config.get<double>("<xmlattr>.fraction") / 100.0;
        double commute_fraction   = university_config.get<double>("far.<xmlattr>.fraction") / 100.0;
        double radius             = m_props.get<double>("population.commutingdata.<xmlattr>.start_radius");

        AliasSampler commute_dist{{commute_fraction, 1.0 - commute_fraction}};
        AliasSampler student_dist{{student_fraction, 1.0 - student_fraction}};

        uint total        = 0;
        uint total_placed = 0;

        for (uint i = min_age; i <= max_age; i++) {
                total += m_age_distribution[i];
        }

        auto distance_map = makeDistanceMap(radius, 2.0, m_optional_schools);

        for (SimplePerson& person : m_people) {
                if (person.m_age >= min_age && person.m_age <= max_age && student_dist(m_rng) == 0) {
                        if (m_output)
                                cerr << "\rAssigning students to universities ["
                                     << min(uint(double(total_placed) / total * 100), 100U) << "%]";
                        total_placed++;

                        if (commute_dist(m_rng) == 0) {
                                /// Commuting student
                                assignCommutingStudent(person, distance_map);
                        } else {
                                /// Non-commuting student
                                assignCloseStudent(person, radius, distance_map);
                        }
                }
        }
        if (m_output)
                cerr << "\rAssigning students to universities [100%]...\n";
}

template <class U>
void PopulationGenerator<U>::removeFromUniMap(vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map,
                                              uint                                                    index) const
{
        bool full_capacity = true;

        for (uint curr_univ = index; curr_univ < m_optional_schools.size(); ++curr_univ) {
                for (const SimpleCluster& uni : m_optional_schools.at(curr_univ)) {
                        if (uni.m_current_size < uni.m_max_size) {
                                full_capacity = false;
                        }
                }
        }

        /// Remove the university from the map if it is full
        if (full_capacity) {
                for (auto& coord_map_pair : distance_map) {
                        for (auto it = coord_map_pair.second.begin(); it != coord_map_pair.second.end(); ++it) {
                                auto& index_vector = it->second;

                                auto cluster_iterator = std::find(index_vector.begin(), index_vector.end(), index);

                                if (cluster_iterator != index_vector.end()) {
                                        index_vector.erase(cluster_iterator, cluster_iterator + 1);

                                        removeFromUniMap(distance_map, index);

                                        return;
                                }
                        }
                }
        }
}

template <class U>
bool PopulationGenerator<U>::removeFromMap(vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map,
                                           uint                                                    index) const
{

        // Remove this cluster from the distance map
        for (auto& coord_map_pair : distance_map) {
                for (auto it = coord_map_pair.second.begin(); it != coord_map_pair.second.end(); ++it) {
                        auto& index_vector = it->second;

                        auto cluster_iterator = std::find(index_vector.begin(), index_vector.end(), index);

                        if (cluster_iterator != index_vector.end()) {
                                index_vector.erase(cluster_iterator, cluster_iterator + 1);

                                removeFromMap(distance_map, index);
                                return true;
                        }
                }
        }

        return false;
}

template <class U>
void PopulationGenerator<U>::assignCommutingStudent(
    SimplePerson& person, vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map)
{
        uint current_city = 0;
        bool added        = false;

        while (current_city < m_cities.size() && !added) {
                uint current_univ = current_city;
                while (current_univ < m_optional_schools.size() && !added) {
                        for (uint i = 0; i < m_optional_schools.at(current_univ).size(); ++i) {
                                SimpleCluster& univ_cluster = m_optional_schools.at(current_univ).at(i);
                                if (univ_cluster.m_current_size < univ_cluster.m_max_size) {
                                        univ_cluster.m_current_size++;
                                        person.m_school_id = univ_cluster.m_id;
                                        added              = true;

                                        removeFromUniMap(distance_map, current_city);
                                        break;
                                }
                        }
                        current_univ += m_cities.size();
                }
                current_city++;
        }
}

template <class U>
void PopulationGenerator<U>::assignCloseStudent(SimplePerson& person, double start_radius,
                                                vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map)
{
        double       factor         = m_props.get<double>("population.commutingdata.<xmlattr>.factor");
        double       current_radius = start_radius;
        bool         added          = false;
        vector<uint> closest_clusters_indices;

        while (!added) {

                // Note how getting the distance to the closest univ is the same as getting the distance to the closest
                // city This is because their vectors are in the same order!

                closest_clusters_indices = getClustersWithinRange(current_radius, distance_map, person.m_coord);

                if (closest_clusters_indices.size() != 0) {
                        AliasSampler dist{
                            vector<double>(closest_clusters_indices.size(), 1.0 / closest_clusters_indices.size())};

                        uint index = dist(m_rng);
                        while (index < m_optional_schools.size() && !added) {

                                for (uint i = 0; i < m_optional_schools.at(index).size(); i++) {
                                        SimpleCluster& univ_cluster = m_optional_schools.at(index).at(i);
                                        if (univ_cluster.m_current_size < univ_cluster.m_max_size) {
                                                univ_cluster.m_current_size++;
                                                person.m_school_id = univ_cluster.m_id;
                                                added              = true;

                                                removeFromUniMap(distance_map, index);
                                                break;
                                        }
                                }
                                index += m_cities.size();
                        }

                        if (added) {
                                break;
                        }
                }

                current_radius *= factor;
        }
}

template <class U>
void PopulationGenerator<U>::assignToWork()
{
        ptree  school_work_config = m_props.get_child("population.school_work_profile.employable");
        ptree  work_config        = m_props.get_child("population.work");
        uint   min_age            = school_work_config.get<uint>("young_employee.<xmlattr>.min");
        uint   max_age            = school_work_config.get<uint>("employee.<xmlattr>.max");
        double unemployment_rate  = 1.0 - school_work_config.get<double>("<xmlattr>.fraction") / 100.0;
        double commute_fraction   = work_config.get<double>("far.<xmlattr>.fraction") / 100.0;
        double radius             = m_props.get<double>("population.commutingdata.<xmlattr>.start_radius");

        AliasSampler unemployment_dist{{unemployment_rate, 1.0 - unemployment_rate}};
        AliasSampler commute_dist{{commute_fraction, 1.0 - commute_fraction}};

        uint total        = 0;
        uint total_placed = 0;
        uint amount_full  = 0;

        for (uint i = min_age; i <= max_age; i++) {
                total += m_age_distribution[i];
        }

        auto distance_map = makeDistanceMap(radius, 2.0, m_workplaces);

        for (SimplePerson& person : m_people) {
                if (amount_full == m_workplaces.size()) {
                        // No more workplaces left, just stop
                        break;
                }

                if (person.m_age >= min_age && person.m_age <= max_age) {
                        if (m_output)
                                cerr << "\rAssigning people to workplaces ["
                                     << min(uint(double(total_placed) / total * 100), 100U) << "%]";
                        total_placed++;
                        if (unemployment_dist(m_rng) == 1 && person.m_school_id == 0) {
                                bool filled_cluster = true;
                                if (commute_dist(m_rng) == 0) {
                                        /// Commuting employee
                                        filled_cluster = assignCommutingEmployee(person, distance_map);
                                } else {
                                        /// Non-commuting employee
                                        filled_cluster = assignCloseEmployee(person, radius, distance_map);
                                }

                                if (filled_cluster) {
                                        ++amount_full;
                                }
                        }
                }
        }
        if (m_output)
                cerr << "\rAssigning people to workplaces [100%]...\n";
}

template <class U>
bool PopulationGenerator<U>::assignCommutingEmployee(
    SimplePerson& person, vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map)
{
        /// TODO ask question: it states that a full workplace has to be ignored
        /// but workplaces can be in cities and villages where commuting is only in cities  => possible problems with
        /// over-employing in cities Behavior on that topic is currently as follows: do the thing that is requested, if
        /// all cities are full, it just adds to the first village in the list
        for (uint i = 0; i < m_workplaces.size(); ++i) {
                SimpleCluster& workplace = m_workplaces.at(i);

                if (workplace.m_max_size > workplace.m_current_size) {
                        workplace.m_current_size++;
                        person.m_work_id = workplace.m_id;

                        if (workplace.m_current_size >= workplace.m_max_size) {
                                return removeFromMap(distance_map, i);
                        }

                        break;
                }
        }

        return false;
}

template <class U>
bool PopulationGenerator<U>::assignCloseEmployee(SimplePerson& person, double start_radius,
                                                 vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map)
{
        double factor         = m_props.get<double>("population.commutingdata.<xmlattr>.factor");
        double current_radius = start_radius;

        vector<uint> closest_clusters_indices;

        while (closest_clusters_indices.size() == 0) {
                closest_clusters_indices = getClustersWithinRange(current_radius, distance_map, person.m_coord);
                current_radius *= factor;
        }

        AliasSampler dist{
            vector<double>(closest_clusters_indices.size(), 1.0 / double(closest_clusters_indices.size()))};
        uint           rnd       = dist(m_rng);
        auto           index     = closest_clusters_indices.at(rnd);
        SimpleCluster& workplace = m_workplaces.at(index);

        person.m_work_id = workplace.m_id;

        workplace.m_current_size++;

        if (workplace.m_current_size >= workplace.m_max_size) {
                return removeFromMap(distance_map, index);
        }

        return false;
}

template <class U>
void PopulationGenerator<U>::assignToCommunities(vector<pair<GeoCoordinate, map<double, vector<uint>>>>& distance_map,
                                                 vector<SimpleCluster>& clusters, uint SimplePerson::*member,
                                                 const string& name)
{

        double start_radius = m_props.get<double>("population.commutingdata.<xmlattr>.start_radius");
        double factor       = m_props.get<double>("population.commutingdata.<xmlattr>.factor");

        uint total        = m_households.size();
        uint total_placed = 0;

        for (SimpleHousehold& household : m_households) {
                if (m_output)
                        cerr << "\rAssigning people to " << name << " ["
                             << min(uint(double(total_placed) / total * 100), 100U) << "%]";
                total_placed++;

                double       current_radius = start_radius;
                vector<uint> closest_clusters_indices;

                // TODO make sure merge didn't screw this up
                while (closest_clusters_indices.size() == 0) {
                        closest_clusters_indices = getClustersWithinRange(
                            current_radius, distance_map, m_people.at(household.m_indices.back()).m_coord);
                        current_radius *= factor;
                }

                AliasSampler dist{
                    vector<double>(closest_clusters_indices.size(), 1.0 / double(closest_clusters_indices.size()))};
                uint           index     = closest_clusters_indices.at(dist(m_rng));
                SimpleCluster& community = clusters.at(index);
                for (uint& person_index : household.m_indices) {
                        SimplePerson& person = m_people.at(person_index);
                        person.*member       = community.m_id;
                        community.m_current_size++;
                }

                /// Remove the community if it is full
                if (community.m_current_size >= community.m_max_size) {
                        removeFromMap(distance_map, index);
                }
        }
        if (m_output)
                cerr << "\rAssigning people to " << name << " [100%]...\n";
}
