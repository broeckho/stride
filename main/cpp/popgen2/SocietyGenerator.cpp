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
 * Header file for the PopulationGenerator class.
 */

#include "SocietyGenerator.h"

#include "AliasMethod.h"
#include "GrahamScan.h"
#include "util/CSV.h"
#include "util/FileSys.h"
#include "util/TimeStamp.h"

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

using namespace std;
using namespace stride::generator;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using boost::property_tree::xml_parser::read_xml;

namespace stride {
namespace generator {

SocietyGenerator::SocietyGenerator(const boost::filesystem::path& config_path, unsigned int num_threads)
{
        m_num_threads = num_threads;
        ptree config;
        read_xml(config_path.string(), config);
        m_config = config.get_child("GeneratorConfiguration");

        m_comm_size       = m_config.get<unsigned int>("community.size");
        m_population_size = m_config.get<double>("population_size");
        m_school_age_min  = m_config.get<unsigned int>("school.age.minimum");
        m_school_age_max  = m_config.get<unsigned int>("school.age.maximum");
        m_school_size     = m_config.get<unsigned int>("school.size");
        m_univ_age_min    = m_config.get<unsigned int>("university.age.minimum");
        m_univ_age_max    = m_config.get<unsigned int>("university.age.maximum");
        m_univ_fraction   = m_config.get<double>("university.fraction");
        m_univ_size       = m_config.get<unsigned int>("university.size");
        m_work_age_min    = m_config.get<unsigned int>("work.age.minimum");
        m_work_age_max    = m_config.get<unsigned int>("work.age.maximum");
        m_work_fraction   = m_config.get<double>("work.fraction");
        m_work_size       = m_config.get<unsigned int>("work.size");

        const auto p_rng = m_config.get<string>("rng.engine", "trng/mrg2");
        RNGvalidate(p_rng);
        m_rng = RNGget(p_rng);
        m_rng->Seed(m_config.get<unsigned long>("rng.seed", 1UL));
}

void SocietyGenerator::ReadGenerateHouseholds()
{
        const auto hh_path = m_config.get<string>("household_profile");
        ptree      pt;
        read_xml(hh_path, pt);
        const ptree& hh_pt = pt.get_child("HouseholdProfile");

        vector<vector<unsigned int>> hh_profile;
        for (ptree::const_iterator it = hh_pt.begin(); it != hh_pt.end(); it++) {
                vector<unsigned int> v;
                for (auto a : it->second) {
                        v.emplace_back(a.second.get_value<unsigned int>());
                }
                hh_profile.emplace_back(v);
        }

        const auto pop_size = m_config.get<unsigned int>("population_size");
        this->GenerateHouseholds(hh_profile, pop_size);
}

void SocietyGenerator::ReadGenerateCities()
{
        const auto              p_geoprofile_cities   = m_config.get<string>("geoprofile.cities");
        const auto              p_geoprofile_villages = m_config.get<string>("geoprofile.villages");
        boost::filesystem::path cities_path           = p_geoprofile_cities;
        boost::filesystem::path villages_path         = p_geoprofile_villages;
        vector<Point2D>         simulation_area       = this->LoadCities(cities_path);
        vector<Village>         villages              = this->LoadVillages(villages_path);
        this->GenerateVillages(simulation_area, villages);
}

Society SocietyGenerator::Generate()
{
#pragma omp parallel sections num_threads(m_num_threads)
        {
#pragma omp section
                {
                        cerr << "Generating Households ..." << endl;
                        ReadGenerateHouseholds();
                        cerr << "Done generating Households." << endl;
                }
#pragma omp section
                {
                        cerr << "Generating Cities ..." << endl;
                        ReadGenerateCities();
                        cerr << "Done generating Cities." << endl;
                }
        }

        // Place households in cities.
        cerr << "Locating households ..." << endl;
        this->LocateHouseholds();
        cerr << "Done Locating households." << endl;

        // Place school, universities, workplaces and communities in cities.
        cerr << "Locating communities ..." << endl;
        this->LocateCommunity();
        cerr << "Done locating communities." << endl;

        // Assign people to the different communities.
        cerr << "Assigning to  communities ..." << endl;
        this->AssignToCommunity();
        cerr << "Done assigning to  communities." << endl;

        // Create vector of persons
        vector<GeneratorPerson> persons;
        boost::copy(m_persons | boost::adaptors::map_values, back_inserter(persons));

        // Create vector of cities
        vector<City> cities;
        boost::copy(m_cities_map | boost::adaptors::map_values, back_inserter(cities));

        // Create vector of communities
        vector<Community> communities;
        for (auto c1 : m_communities) {
                for (auto c2 : c1.second) {
                        for (Community c : c2.second) {
                                communities.push_back(c);
                        }
                }
        }

        // Return society
        return Society(persons, cities, communities, m_households);
}

void SocietyGenerator::GenerateHouseholds(vector<vector<unsigned int>>& HouseholdProfile, unsigned int maxPersons)
{
        // Using alias method to sample from households as this allows for more configurable RNG.
        const auto                        p_rng_seed = m_config.get<unsigned long>("rng.seed", 1UL);
        AliasMethod<vector<unsigned int>> household_age_sampler(p_rng_seed);

        for (const auto& household : HouseholdProfile) {
                household_age_sampler.Add(household, 1);
        }

        household_age_sampler.BuildSampler();

        // Sample from households until population cap is reached.
        size_t id_counter = 1;
        while (m_persons.size() < maxPersons) {
                Household h;
                for (unsigned int age : household_age_sampler.Sample()) {
                        GeneratorPerson p(id_counter++);
                        p.SetAge(age);
                        p.SetHouseholdID(h.GetID());
                        h.AddMember(p.GetID());
                        m_persons[p.GetID()] = p;
                }
                m_households.emplace_back(h);
        }
}

void SocietyGenerator::GenerateVillages(const vector<Point2D>& simulation_area, const vector<Village>& villages)
{
        // Determine the borders of the simulation area.
        GrahamScan      graham(simulation_area);
        vector<Point2D> borders = graham.Scan();
        borders                 = this->ExpandSimulationArea(borders);

        // Create a polygon of the borders of the simulation area.
        Polygon              borders_simulation_area;
        vector<unsigned int> indices;
        for (const auto& b : borders) {
                indices.push_back(borders_simulation_area.AddPoint(b));
        }
        for (unsigned int i = 0; i < indices.size(); i++) {
                if (i == indices.size() - 1)
                        borders_simulation_area.AddLine(indices[i], indices[0]);
                else
                        borders_simulation_area.AddLine(indices[i], indices[i + 1]);
        }

        // Divide the simulation area in columns.
        vector<tuple<Point2D, Point2D>> couples;
        for (unsigned int i = 0; i < borders.size() - 1; i++) {
                tuple<Point2D, Point2D> neighbors = make_tuple(borders[i], borders[i + 1]);
                couples.push_back(neighbors);
        }

        // Create villages
        for (Village v : villages) {
                for (unsigned int i = 0; i < v.GetNumber(); i++) {
                        ConvexPolygonChecker checker(borders_simulation_area);
                        // Generate random coordinates
                        Point2D coordinates =
                            this->GenerateRandomCoordinates(checker, borders_simulation_area, borders[0].m_point_type);
                        // Check if the new coordinates lie in the simulation area
                        while (!checker.IsPointInPolygon(coordinates) or !this->IsFreePoint(coordinates)) {
                                coordinates = this->GenerateRandomCoordinates(checker, borders_simulation_area,
                                                                              borders[0].m_point_type);
                        }
                        size_t closest_city_id = this->FindClosestCity(coordinates);
                        double population      = v.GetPopulation() / m_population_size;
                        City   village(population, m_cities_map[closest_city_id].GetName() + "_VILLAGE",
                                     m_cities_map[closest_city_id].GetProvince(), coordinates,
                                     Point2D(0, 0, PointType::Null));
                        m_cities_map[village.GetID()] = village;
                }
        }
}

vector<Point2D> SocietyGenerator::LoadCities(const boost::filesystem::path cities_path)
{
        vector<Point2D> output;

        CSV reader(cities_path);
        for (CSVRow& row : reader) {
                const auto id         = row.getValue<unsigned int>("city_id");
                const auto population = row.getValue<double>("population");
                const auto name       = row.getValue<string>("city_name");
                const auto province   = row.getValue<unsigned int>("province");
                const auto p_use_xy   = m_config.get<bool>("geoprofile.use_xy");
                Point2D    location;
                Point2D    dummy;

                if (p_use_xy) {
                        location =
                            Point2D(row.getValue<double>("x_coord"), row.getValue<double>("y_coord"), PointType::XY);
                        dummy = Point2D(row.getValue<double>("longitude"), row.getValue<double>("latitude"),
                                        PointType::LONGLAT);
                } else {
                        location = Point2D(row.getValue<double>("longitude"), row.getValue<double>("latitude"),
                                           PointType::LONGLAT);
                        dummy =
                            Point2D(row.getValue<double>("x_coord"), row.getValue<double>("y_coord"), PointType::XY);
                }
                m_cities_map[id] = City(id, population, name, province, location, dummy);
                output.emplace_back(location);
        }

        return output;
}

vector<Village> SocietyGenerator::LoadVillages(const boost::filesystem::path villages)
{
        // Load the villages from the csv file
        vector<Village> output;
        double          sum_frequencies = 0;
        double          sum_size        = 0;
        CSV             reader(villages);
        for (CSVRow& row : reader) {
                const auto number = row.getValue<double>("number");
                const auto size   = row.getValue<double>("size");
                output.emplace_back(Village(size, number));
                sum_frequencies += number;
                sum_size += size;
        }

        for (auto v : output) {
                cerr << v.m_population << " == " << v.m_number << endl;
        }

        // Normalise the frequencies and determine the size of the villages
        unsigned int size_villages = 0;
        for (auto& out : output) {
                out.m_number /= sum_frequencies;
                out.m_population = ceil(out.m_population / sum_size * m_population_size);
                size_villages += ceil(out.m_number * out.m_population);
        }

        // Determine the remaining population size after filling all cities
        const auto size_remaining = static_cast<long int>(m_population_size * m_in_villages);

        // Update the number of villages of each size
        for (auto& out : output) {
                out.m_number = ceil(out.m_number * (size_remaining / size_villages));
        }

        return output;
}

void SocietyGenerator::LocateHouseholds()
{
        // Create the city sampler
        const auto            p_rng_seed = m_config.get<unsigned long>("rng.seed", 1UL);
        AliasMethod<size_t>   city_sampler(p_rng_seed);
        map<size_t, long int> city_populations;
        for (const auto& c : m_cities_map) {
                const double p = c.second.GetPopulation();
                city_sampler.Add(c.first, p);
                city_populations[c.first] = static_cast<long int>(p * m_population_size);
        }
        city_sampler.BuildSampler();

        // Sample from the cities until all households are assigned
        for (auto& hh : m_households) {
                const auto&  hh_member_ids = hh.GetMemberIDs();
                const size_t hh_size       = hh.GetSize();

                tuple<unsigned int, size_t> sample         = city_sampler.SampleIndexValue();
                size_t                      sample_city_id = get<1>(sample);

                while (city_populations[sample_city_id] <= 0) {
                        city_sampler.Remove(get<0>(sample));
                        city_sampler.BuildSampler();
                        sample         = city_sampler.SampleIndexValue();
                        sample_city_id = get<1>(sample);
                }

                for (const auto id : hh_member_ids) {
                        m_persons[static_cast<unsigned int>(id)].SetCityID(sample_city_id);
                }
                hh.SetCityID(sample_city_id);
                city_populations[sample_city_id] -= hh_size;
        }
}

void SocietyGenerator::LocateCommunity()
{
        // Get the number of people in the age interval in the total population and per city.
        map<CommunityType, unsigned long int>              of_age;
        map<size_t, map<CommunityType, unsigned long int>> of_age_cities;
        for (const auto& p : m_persons) {
                if (p.second.GetAge() >= m_school_age_min && p.second.GetAge() <= m_school_age_max) {
                        of_age[CommunityType::School] += 1;
                        of_age_cities[p.second.GetCityID()][CommunityType::School] += 1;
                } else if (p.second.GetAge() >= m_univ_age_min && p.second.GetAge() <= m_univ_age_max) {
                        of_age[CommunityType::University] += 1;
                } else if (p.second.GetAge() >= m_work_age_min && p.second.GetAge() <= m_work_age_max) {
                        of_age[CommunityType::Work] += 1;
                        of_age_cities[p.second.GetCityID()][CommunityType::Work] += 1;
                }
                of_age_cities[p.second.GetCityID()][CommunityType::Primary] += 1;
                of_age_cities[p.second.GetCityID()][CommunityType::Secondary] += 1;
        }
        of_age[CommunityType::Primary]   = m_persons.size();
        of_age[CommunityType::Secondary] = m_persons.size();

        // Determine the number of communities and their sizes.
        map<CommunityType, unsigned int> community_counts;
        map<CommunityType, unsigned int> community_sizes;
        for (auto comm : of_age) {
                unsigned int size = 1U;
                switch (comm.first) {
                case CommunityType::School: {
                        size = m_school_size;
                        break;
                }
                case CommunityType::University: {
                        size = m_univ_size;
                        break;
                }
                case CommunityType::Work: {
                        size = m_work_size;
                        break;
                }
                case CommunityType::Primary: {
                        size = m_comm_size;
                        break;
                }
                case CommunityType::Secondary: {
                        size = m_comm_size;
                        break;
                }
                default: {
                        // What about Null?
                }
                }
                community_counts[comm.first] = static_cast<unsigned int>(ceil(comm.second / (double)size));
                community_sizes[comm.first]  = size;
        }

        // Create the sampler to decide in which city a community will be.
        map<CommunityType, AliasMethod<size_t>> city_sampler;
        for (auto city : of_age_cities) {
                for (auto comm : city.second) {
                        city_sampler[comm.first].Add(city.first, comm.second);
                }
        }
        for (auto sampler : city_sampler) {
                city_sampler[sampler.first].BuildSampler();
        }

        // Get a list of cities ordered by descending size.
        vector<size_t> ordered_cities;
        for (const auto& city : m_cities_map) {
                // Add first element to ordered_cities
                if (ordered_cities.empty()) {
                        ordered_cities.push_back(city.first);
                        continue;
                }
                // Determine the location to insert this city.
                auto it = ordered_cities.end();
                for (unsigned int i = 0; i < ordered_cities.size(); i++) {
                        if (m_cities_map[ordered_cities[i]].GetPopulation() < city.second.GetPopulation()) {
                                it = ordered_cities.begin() + i;
                                break;
                        }
                }
                ordered_cities.insert(it, city.first);
        }

        // Sample and create the communities
        unsigned int index = 0;
        size_t       city_id;
        for (auto comm : community_counts) {
                for (unsigned int i = 0; i < comm.second; i++) {
                        const CommunityType comm_type = comm.first;
                        if (comm_type == CommunityType::University) {
                                city_id = ordered_cities[index];
                                index   = index + 1 < ordered_cities.size() ? index + 1 : 0;
                        } else {
                                city_id = city_sampler[comm_type].SampleValue();
                        }
                        auto c = Community(comm_type, city_id, community_sizes[comm_type]);
                        m_communities[city_id][c.GetCommunityType()].emplace_back(c);
                }
        }
}

void SocietyGenerator::AssignToCommunity()
{
        // Determine the size of each community.
        for (auto c1 : m_communities) {
                for (auto c2 : c1.second) {
                        for (Community c : c2.second) {
                                m_community_sizes[c.GetID()] = c.GetSize();
                        }
                }
        }

#pragma omp parallel num_threads(m_num_threads)
        {
#pragma omp for schedule(runtime)
                // Assign each household to communities.
                for (size_t i = 0; i < m_households.size(); i++) {
                        AssignHouseholdToCommunity(i);
                }
        }
}

vector<size_t> SocietyGenerator::PossibleCommunities(map<size_t, int>     community_sizes,
                                                     const CommunityType& community_type, Point2D household_location)
{
        double         distance = 10;
        vector<size_t> possible_communities;

        while (possible_communities.empty()) {
                // Determine the bounding box around the person's location
                tuple<Point2D, Point2D> box = BoundingBox(household_location, distance);
                for (auto c1 : m_communities) {
                        Point2D location = m_cities_map[c1.first].GetLocation();
                        // Check if the location is in the bounding box.
                        if (location.m_x >= get<0>(box).m_x && location.m_x <= get<1>(box).m_x &&
                            location.m_y >= get<0>(box).m_y && location.m_y <= get<1>(box).m_y) {
                                for (auto c2 : c1.second) {
                                        if (c2.first == community_type) {
                                                for (Community c : c2.second) {
                                                        if (community_sizes[c.GetID()] > 0) {
                                                                possible_communities.push_back(c.GetID());
                                                        }
                                                }
                                        }
                                }
                        }
                }
                distance *= 2;
        }

        return possible_communities;
}

Point2D SocietyGenerator::GenerateRandomCoordinates(ConvexPolygonChecker checker, Polygon poly, PointType point_type)
{
        vector<ConvexPolygonChecker::Column> columns = checker.GetColumns();

        // Get a random x_coordinate
        unsigned int max_interval = numeric_limits<int>::max();
        unsigned int r            = m_rng->operator()(max_interval);
        double random_x           = checker.m_min_x + ((double)r / max_interval) * (checker.m_max_x - checker.m_min_x);

        // Check between which two intervals of neighboring cities this x-coord lies
        ConvexPolygonChecker::Column column;
        for (const auto& c : columns) {
                if (c.IsInside(random_x))
                        column = c;
        }

        r                   = m_rng->operator()(max_interval);
        double     max_y    = poly.GetLineHeight(column.m_top, random_x);
        double     min_y    = poly.GetLineHeight(column.m_bottom, random_x);
        double     random_y = min_y + ((double)r / max_interval) * (max_y - min_y);

        return {random_x, random_y, point_type};
}

bool SocietyGenerator::IsFreePoint(Point2D coordinates)
{
        auto status = true;
        for (const auto& p : m_cities_map) {
                if ((p.second.GetLocation() == coordinates)) {
                        status = false;
                }
        }
        return status;
}

size_t SocietyGenerator::FindClosestCity(Point2D coordinates)
{
        double smallest_distance = numeric_limits<double>::infinity();
        size_t closest_city_id   = 0;
        for (auto city : m_cities_map) {
                double d = distance(coordinates, city.second.GetLocation());
                if (d < smallest_distance) {
                        smallest_distance = d;
                        closest_city_id   = city.first;
                }
        }
        return closest_city_id;
}

vector<Point2D> SocietyGenerator::ExpandSimulationArea(vector<Point2D> simulation_area)
{
        // Determine the center of the points by taking the average.
        double x_center = 0, y_center = 0;
        for (Point2D p : simulation_area) {
                x_center += p.m_x;
                y_center += p.m_y;
        }
        x_center /= simulation_area.size();
        y_center /= simulation_area.size();

        const auto p_scale = m_config.get<double>("geoprofile.scale");
        for (auto& s : simulation_area) {
                // For each point, determine the vector between the point and the center
                const double x_vector = s.m_x - x_center;
                const double y_vector = s.m_y - y_center;
                // Update the point of the simulation area with the FACTOR in the config file.
                s.m_x = x_center + p_scale * x_vector;
                s.m_y = y_center + p_scale * y_vector;
        }

        return simulation_area;
}

void SocietyGenerator::AssignHouseholdToCommunity(size_t i)
{
        Household      h                  = m_households[i];
        vector<size_t> member_ids         = h.GetMemberIDs();
        Point2D        household_location = m_cities_map[h.GetCityID()].GetLocation();

        // Determine the closest communities for this household location.
        const vector<size_t> pc_primary =
            this->PossibleCommunities(m_community_sizes, CommunityType::Primary, household_location);
        const auto pc_primary_size = static_cast<unsigned int>(pc_primary.size());

        const vector<size_t> pc_secondary =
            this->PossibleCommunities(m_community_sizes, CommunityType::Secondary, household_location);
        const auto pc_secondary_size = static_cast<unsigned int>(pc_secondary.size());

        const vector<size_t> pc_school =
            this->PossibleCommunities(m_community_sizes, CommunityType::School, household_location);
        const auto pc_school_size = static_cast<unsigned int>(pc_school.size());

        const vector<size_t> pc_university =
            this->PossibleCommunities(m_community_sizes, CommunityType::University, household_location);
        const auto pc_university_size = static_cast<unsigned int>(pc_university.size());

        const vector<size_t> pc_work =
            this->PossibleCommunities(m_community_sizes, CommunityType::Work, household_location);
        const auto pc_work_size = static_cast<unsigned int>(pc_work.size());

        size_t primary_community   = 0;
        size_t secondary_community = 0;
        // For alternative mechanism (adding complete household to a primary and secondary community)
        const auto p_add_complete_household = m_config.get<bool>("community.add_complete_household");
        if (p_add_complete_household || (pc_primary.size() == 1 && pc_secondary.size() == 1)) {
                // Primary community
                unsigned int r    = m_rng->operator()(pc_primary_size - 1);
                primary_community = pc_primary[r];
                m_community_sizes[pc_primary[r]] -= member_ids.size();
                // Secondary community
                r                   = m_rng->operator()(pc_secondary_size - 1);
                secondary_community = pc_secondary[r];
                m_community_sizes[pc_secondary[r]] -= member_ids.size();
        }

        for (const auto person_id : member_ids) {
                GeneratorPerson& person = m_persons[person_id];

                // For original mechanism (adding individuals to primary and secondary community)
                if (!(p_add_complete_household || (pc_primary.size() == 1 && pc_secondary.size() == 1))) {
                        // Primary community
                        unsigned int r    = m_rng->operator()(pc_primary_size - 1);
                        primary_community = pc_primary[r];
                        m_community_sizes[pc_primary[r]] -= 1;
                        // Secondary community
                        r                   = m_rng->operator()(pc_secondary_size - 1);
                        secondary_community = pc_secondary[r];
                        m_community_sizes[pc_secondary[r]] -= 1;
                }
                person.SetPrimaryCommunityID(primary_community);
                person.SetSecondaryCommunityID(secondary_community);

                // School
                if (person.GetAge() >= m_school_age_min && person.GetAge() <= m_school_age_max) {

                        unsigned int r = m_rng->operator()(pc_school_size - 1);
                        person.SetSchoolID(pc_school[r]);
                }

                // University
                if (person.GetAge() >= m_univ_age_min && person.GetAge() <= m_univ_age_max) {
                        unsigned int r = m_rng->operator()(100);
                        if (r < m_univ_fraction * 100) {
                                r = m_rng->operator()(pc_university_size - 1);
                                person.SetUniversityID(pc_university[r]);
                                m_community_sizes[pc_university[r]] -= 1;
                        }
                }

                // Workplace
                if (person.GetAge() >= m_work_age_min && person.GetAge() <= m_work_age_max) {

                        unsigned int r = m_rng->operator()(100);
                        if (r < m_work_fraction * 100) {
                                r = m_rng->operator()(pc_work_size - 1);
                                person.SetWorkID(pc_work[r]);
                                m_community_sizes[pc_work[r]] -= 1;
                        }
                }
        }
}

} // namespace generator
} // namespace stride
