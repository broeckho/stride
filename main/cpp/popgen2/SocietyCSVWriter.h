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
 * Header file for the PopulationCSVWriter class.
 */

#include "SocietyWriter.h"
#include "util/CSV.h"

#include <boost/filesystem/path.hpp>
#include <string>

namespace stride {
namespace generator {

/**
 * Write a population to CSV
 */
class SocietyCSVWriter : public SocietyWriter
{
public:
        explicit SocietyCSVWriter(const Society& population, bool use_xy = false) : SocietyWriter(population, use_xy) {}

        ~SocietyCSVWriter() final = default;

        /// Write the persons of the population to a CSV file.
        stride::config::CSV WritePersons(const boost::filesystem::path& out);

        /// Write the cities of the population to a CSV file.
        stride::config::CSV WriteCities(const boost::filesystem::path& out);

        /// Write the communities of the population to a CSV file.
        stride::config::CSV WriteCommunities(const boost::filesystem::path& out);

        /// Write the households of the population to a CSV file.
        stride::config::CSV WriteHouseholds(const boost::filesystem::path& out);

        /// Write everything to CSV files.
        void WriteSociety(const boost::filesystem::path& person_file = "pop_generated.csv",
                          const boost::filesystem::path& cities_file = "cities_generated.csv",
                          const boost::filesystem::path& communities_file = "communities_generated.csv",
                          const boost::filesystem::path& households_file = "households_generated.csv");
};

} // namespace generator
} // namespace stride
