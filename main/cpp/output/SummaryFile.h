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
 * Header for the SummaryFile class.
 */

#include <boost/property_tree/ptree.hpp>
#include <fstream>
#include <string>

namespace stride {
namespace output {

/**
 * Produces a file with simulation summary output.
 */
class SummaryFile
{
public:
        /// Constructor: initialize.
        explicit SummaryFile(const std::string& output_prefix = "output");

        /// Destructor: close the file stream.
        ~SummaryFile();

        /// Print the given output with corresponding tag.
        void Print(const boost::property_tree::ptree& config_pt, unsigned int population_size, unsigned int num_cases,
                   double transmission_rate, unsigned int run_time, unsigned int total_time);

private:
        /// Generate file name and open the file stream.
        void Initialize(const std::string& output_dir);

private:
        std::ofstream m_fstream; ///< The file stream.
};

} // namespace output
} // namespace stride
