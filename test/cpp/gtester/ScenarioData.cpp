
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
        bool geopop = tag.size() > string("geopop").size() &&
                      tag.substr(tag.size() - string("geopop").size(), tag.size() - 1) == "geopop";

        if (geopop)
                RunConfigManager::AddGeoPopConfig(pt);

        const map<string, unsigned int> targets_default = {
            {"influenza_a", 550000U}, {"influenza_b", 0U}, {"influenza_c", 5U}, {"measles_16", 275000U},
            {"measles_26", 600000U},  {"r0_0", 1200U},     {"r0_4", 4200U},     {"r0_8", 15100U},
            {"r0_12", 38500U},        {"r0_16", 71000U}};

        const map<string, double> margins_default = {
            {"influenza_a", 2.0e-03}, {"influenza_b", 0.0}, {"influenza_c", 2.0e-01}, {"measles_16", 8.0e-02},
            {"measles_26", 1.0e-03},  {"r0_0", 5.0e-02},    {"r0_4", 1.0e-01},        {"r0_8", 8.0e-02},
            {"r0_12", 1.0e-01},       {"r0_16", 8.0e-02}};

        const map<string, unsigned int> targets_geopop = {
            {"influenza_a_geopop", 554000U}, {"influenza_b_geopop", 0U},     {"influenza_c_geopop", 5U},
            {"measles_16_geopop", 270000U},  {"measles_26_geopop", 600000U}, {"r0_0_geopop", 1200U},
            {"r0_12_geopop", 39300U},        {"r0_16_geopop", 70500U},       {"r0_4_geopop", 4300U},
            {"r0_8_geopop", 15900U}};

        const map<string, double> margins_geopop = {{"influenza_a_geopop", 3.0e-03}, {"influenza_b_geopop", 0.0},
                                                    {"influenza_c_geopop", 1.0e-1},  {"measles_16_geopop", 8.0e-02},
                                                    {"measles_26_geopop", 2.0e-03},  {"r0_0_geopop", 1.0e-03},
                                                    {"r0_12_geopop", 5.0e-02},       {"r0_16_geopop", 5.0e-02},
                                                    {"r0_4_geopop", 8.0e-02},        {"r0_8_geopop", 1.0e-01}};

        unsigned int target;
        double       margin;
        if (geopop) {
                target = targets_geopop.at(tag);
                margin = margins_geopop.at(tag);
        } else {
                target = targets_default.at(tag);
                margin = margins_default.at(tag);
        }
        if (geopop) {
                tag = tag.substr(0, tag.size() - string("_geopop").size());
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
