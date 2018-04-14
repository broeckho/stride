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
 * Produce run config ptree.
 */

#include "util/ConfigInfo.h"

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace stride {
namespace util {

/**
 * Produce run config ptree.
 */
class RunConfigManager
{
public:
        /// Clean (i.e. indent, sort and produde the sha1) the ptree config file.
        static void CleanConfigFile(boost::property_tree::ptree pt);

        ///
        static boost::property_tree::ptree Create(const std::string& configName);

        ///
        static boost::property_tree::ptree CreateTestsInfluenza();

        ///
        static boost::property_tree::ptree CreateTestsMeasles();

        ///
        static boost::property_tree::ptree CreateBenchMeasles();

        ///
        static std::vector<unsigned int> CreateNumThreads(unsigned int max = ConfigInfo::ProcessorCount());

        ///
        static std::string ToString(const boost::property_tree::ptree& pt);

        ///
        static std::string ToSha1(const boost::property_tree::ptree& pt);

        ///
        static std::string ToShortSha1(const boost::property_tree::ptree& pt, unsigned int n = 7U);
};

} // namespace util
} // namespace stride
