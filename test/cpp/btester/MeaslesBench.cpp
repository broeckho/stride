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
        class MeaslesBenchmark
        {
        public:
        private:
        };

        auto factoryBuilder = [](unsigned int n) {
                auto configPt = make_shared<ptree>(RunConfigManager::CreateBenchMeasles());
                return [n, configPt]() {
                        return Test([n, configPt]() {
                                configPt->put("run.num_threads", n);
                                SimRunner(*configPt).Run();
                        });
                };
        };

        const auto num = RunConfigManager::CreateNumThreads();
        for (const auto n : num) {
                auto infoFactory = [n]() { return ptree().put("num_threads", n); };
                BenchmarkRunner::RegisterTest("MeaslesBench", "NumThreads." + ToString(n), 1, factoryBuilder(n),
                                              infoFactory);
        }
}
