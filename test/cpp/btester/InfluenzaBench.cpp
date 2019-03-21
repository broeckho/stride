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
 *  Copyright 2017, 2018 Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Implementation of Influenza runs for benchmarking.
 */

#include "myhayai/BenchmarkRunner.h"
#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"
#include "util/RunConfigManager.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace myhayai;
using boost::property_tree::ptree;

void InfluenzaBench()
{
        auto builder = [](string s) {
                auto config = make_shared<ptree>(RunConfigManager::Create("BenchInfluenza"));
                return [s, config]() {
                        return Test([s, config]() {
                                RnMan rnMan;
                                rnMan.Initialize(RnInfo(config->get<std::string>("run.rng_seed", "1,2,3,4"),
                                                        config->get<string>("run.rng_state", ""),
                                                        config->get<unsigned int>("run.num_threads")));
                                auto sim = Sim::Create(*config, Population::Create(*config, rnMan), rnMan);
                                SimRunner(*config, sim).Run();
                        });
                };
        };

        const vector<string> levels{"None", "Transmissions"};
        for (const auto& s : levels) {
                auto info = [s]() { return ptree().put("contact_log_level", s); };
                BenchmarkRunner::RegisterTest("Influenza", "ContactLog." + s, 5, builder(s), info);
        }
}
