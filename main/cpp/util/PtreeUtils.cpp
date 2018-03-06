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
 * Implementation of ptree utils.
 */

#include "PtreeUtils.h"

#include <boost/property_tree/ptree.hpp>

using namespace boost::property_tree;
using namespace std;

namespace stride {
namespace util {

// const boost::property_tree::ptree stride::util::PtreeUtils::emptyTree = boost::property_tree::ptree();

vector<double> PtreeUtils::GetDistribution(const ptree& pt_root, const string& xml_tag)
{
        vector<double>              values;
        boost::property_tree::ptree subtree = pt_root.get_child(xml_tag);
        for (const auto& tree : subtree) {
                values.push_back(tree.second.get<double>(""));
        }
        return values;
}

} // namespace util
} // namespace stride
