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
 * Implementation of influenza_a runs for benchmarking.
 */

#include "myhayai/BenchmarkRunner.hpp"
#include "sim/SimRunner.h"
#include "util/RunConfigManager.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <iostream>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace myhayai;
using boost::property_tree::ptree;

void MeaslesBench()
{
        auto num_builder = [](unsigned int n) {
                return [n]() {
                        auto runner = make_shared<SimRunner>();
                        return Test([runner]() { runner->Run(); },
                                    [runner, n]() {
                                            auto config_pt = RunConfigManager::CreateBenchMeasles();
                                            config_pt.put("run.num_threads", n);
                                            runner->Setup(config_pt);
                                    });
                };
        };

        const auto num = RunConfigManager::CreateNumThreads();
        for (const auto n : num) {
                BenchmarkRunner::RegisterTest("MeaselsBench", "NumThreads: " + ToString(n), 1, num_builder(n), [n]() {
                        ptree pt;
                        pt.put("num_threads", n);
                        return pt;
                });
        }
}
