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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#pragma once
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <util/LogUtils.h>
#include <util/RnMan.h>
#include <vector>

namespace calibration {

/**
 * A calibration tool used to calibrate the Stride scenario tests
 */
class Calibrator
{
public:
        /// Setup the Calibrator
        Calibrator();

        /// Run one simulation for each config and return the infected counts after each timestep
        std::map<std::string, std::vector<unsigned int>> RunSingle(
            const std::vector<std::pair<boost::property_tree::ptree, std::string>>& configs) const;

        /// Run count simulations for each config by varying the seed and return the infected counts after each timestep
        std::map<std::string, std::vector<std::vector<unsigned int>>> RunMultiple(
            unsigned int count, const std::vector<std::pair<boost::property_tree::ptree, std::string>>& configs) const;

        /// Print the mean and standard deviation for each step in the results obtained by RunMultiple to the Logger
        void PrintMultipleResultsAtStep(const std::map<std::string, std::vector<std::vector<unsigned int>>>& results,
                                        unsigned int step) const;

        /// Print the mean and standard deviation for the last step in the results obtained by RunMultiple to the Logger
        void PrintMultipleResults(const std::map<std::string, std::vector<std::vector<unsigned int>>>& results) const;

        /// Print the exact results obtained at step 'step' by RunSingle to the Logger
        void PrintSingleResultsAtStep(const std::map<std::string, std::vector<unsigned int>>& results,
                                      unsigned int                                            step) const;

        /// Print the exact results obtained at the last step by RunSingle to the Logger
        void PrintSingleResults(const std::map<std::string, std::vector<unsigned int>>& results) const;

        /// Write the mean and standard deviation for each step in the results obtained by RunMultiple to a file
        void WriteMultipleResults(const std::map<std::string, std::vector<std::vector<unsigned int>>>& results,
                                  std::string                                                          filename) const;

        /// Write the exact results obtained by RunSingle to a file
        void WriteSingleResults(const std::map<std::string, std::vector<unsigned int>>& results,
                                std::string                                             filename) const;

        /// Combines the content of the two other write functions
        void WriteResults(const std::map<std::string, std::vector<unsigned int>>&              single,
                          const std::map<std::string, std::vector<std::vector<unsigned int>>>& multiple,
                          std::string                                                          filename) const;

private:
        /// Print the mean and standard deviation of the given numbers, name and step
        void PrintStep(const std::vector<unsigned int>& results, std::string name, unsigned int step) const;
        /// Compute the mean and standard deviation of the given numbers and return (mean, stdev).
        std::pair<double, double> FindMeanStdev(std::vector<unsigned int> results) const;

        std::shared_ptr<spdlog::logger> logger; ///< Logger to use for this class
};

} // namespace calibration
