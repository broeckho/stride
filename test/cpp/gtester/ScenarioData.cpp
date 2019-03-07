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
 *  Copyright 2018 Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Implementation of scenario tests data.
 */

#include "ScenarioData.h"
#include "util/RunConfigManager.h"

#include <boost/property_tree/ptree.hpp>
#include <map>

using namespace std;
using namespace stride;
using namespace stride::util;
using boost::property_tree::ptree;

namespace Tests {

tuple<ptree, unsigned int, double> ScenarioData::Get(string tag)
{
        ptree pt = tag.substr(0, 2) != "r0" ? RunConfigManager::Create("TestsInfluenza")
                                            : RunConfigManager::Create("TestsMeasles");
        bool geopop = tag.size() > std::string("geopop").size() &&
                         tag.substr(tag.size() - std::string("geopop").size(), tag.size() - 1) == "geopop";

        if (geopop)
                RunConfigManager::AddGeoPopConfig(pt);

        std::map<std::string, unsigned int> targets_default = {
            {"influenza_a", 550429U}, {"influenza_b", 0U}, {"influenza_c", 5U}, {"measles_16", 273372U},
            {"measles_26", 600000U},  {"r0_0", 1200U},     {"r0_4", 4077U},     {"r0_8", 14588U},
            {"r0_12", 39234U},        {"r0_16", 71171}};

        std::map<std::string, double> margins_default = {{"influenza_a", 1.0e-03},
                                                        {"influenza_b", 0.0},
                                                        {"influenza_c", 4.0e-01},
                                                        {"measles_16", 6.0e-02},
                                                        {"measles_26", 4.0e-03},
                                                        {"r0_0", 1.0e-01},
                                                        {"r0_4", 1.0e-01},
                                                        {"r0_8", 1.0e-01},
                                                        {"r0_12", 1.0e-01},
                                                        {"r0_16", 1.0e-01}};

        std::map<std::string, unsigned int> targets_geopop = {
            {"influenza_a_geopop", 554299U}, {"influenza_b_geopop", 0},      {"influenza_c_geopop", 5},
            {"measles_16_geopop", 275643U},  {"measles_26_geopop", 600000U}, {"r0_0_geopop", 1199U},
            {"r0_12_geopop", 39362U},        {"r0_16_geopop", 71220U},       {"r0_4_geopop", 4354U},
            {"r0_8_geopop", 15849U}};

        std::map<std::string, double> margins_geopop = {
            {"influenza_a_geopop", 1.6e-03}, {"influenza_b_geopop", 0.0},     {"influenza_c_geopop", 4.0e-1},
            {"measles_16_geopop", 1.6e-02}, {"measles_26_geopop", 4.0e-03}, {"r0_0_geopop", 3.0e-03},
            {"r0_12_geopop", 2.6e-01},       {"r0_16_geopop", 1.4e-01},       {"r0_4_geopop", 4.4e-01},
            {"r0_8_geopop", 3.8e-01}};

        unsigned int target;
        double       margin;
        if (geopop) {
                target = targets_geopop[tag];
                margin  = margins_geopop[tag];
        } else {
                target = targets_default[tag];
                margin  = margins_default[tag];
        }
        if (geopop) {
                tag = tag.substr(0, tag.size() - std::string("_geopop").size());
        }

        if (tag == "influenza_b") {
                pt.put("run.seeding_rate", 0.0);
        }
        if (tag == "influenza_c") {
                pt.put("run.seeding_rate", (1 - 0.9991) / 100);
                pt.put("run.immunity_rate", 0.9991);
        }
        if (tag == "measles_16") {
                pt.put("run.disease_config_file", "disease_measles.xml");
                pt.put("run.r0", 16U);
        }
        if (tag == "measles_26") {
                pt.put("run.disease_config_file", "disease_measles.xml");
                pt.put("run.r0", 26U);
                pt.put("run.num_days", 200U);
        }

        if (tag == "r0_0") {
                pt.put("run.r0", 0.0);
        }
        if (tag == "r0_4") {
                pt.put("run.r0", 4.0);
        }
        if (tag == "r0_8") {
                pt.put("run.r0", 8.0);
        }
        if (tag == "r0_12") {
                pt.put("run.r0", 12.0);
        }
        if (tag == "r0_16") {
                pt.put("run.r0", 16.0);
        }
        return make_tuple(pt, target, margin);
}

} // namespace Tests
