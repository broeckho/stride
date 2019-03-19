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
 * Produce run config ptree.
 */

#pragma once

#include "util/ConfigInfo.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <string>
#include <vector>

namespace stride {
namespace util {

/**
 * Produce run config ptree.
 */
class RunConfigManager
{
public:
        /// Adapt the configuration by adding the gengeopop data items.
        static void AddGeoPopConfig(boost::property_tree::ptree& pt);

        /// Clean (i.e. indent, sort and produde the sha1) the ptree config file.
        static void CleanConfigFile(boost::property_tree::ptree pt);

        /// Produce property tree for config with given name.
        static boost::property_tree::ptree Create(const std::string& configName);

        /// Set of threadcounts to use for tests based an nomber of available OpenMP threads.
        static std::vector<unsigned int> CreateNumThreads(unsigned int maxNum = ConfigInfo::NumberAvailableThreads());

        /// Reconstitute property tree from string representation.
        static boost::property_tree::ptree FromString(const std::string& s);

        /// Produce string representation of property tree.
        static std::string ToString(const boost::property_tree::ptree& pt);

        /// Produce the sha1 hash of property tree (of it's string representation).
        static std::string ToSha1(const boost::property_tree::ptree& pt);

        /// Produce the short sha1 hash of property tree (of it's string representation).
        static std::string ToShortSha1(const boost::property_tree::ptree& pt, unsigned int n = 7U);

private:
        /// Produce Influenza config for Benchmarking.
        static std::string CreateBenchInfluenza();

        /// Produce Measles config for Benchmarking.
        static std::string CreateBenchMeasles();

        /// Produce the deefault config.
        static std::string CreateDefault();

        /// Produce Influenza config for scenario tests.
        static std::string CreateTestsInfluenza();

        /// Produce Measles config for scenario tests.
        static std::string CreateTestsMeasles();
};

} // namespace util
} // namespace stride
