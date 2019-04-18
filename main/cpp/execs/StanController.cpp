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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the command line controller.
 */

#include "StanController.h"

#include "pop/Population.h"
#include "sim/Sim.h"
#include "sim/SimRunner.h"
#include "util/BoxPlotData.h"
#include "util/CSV.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/StringUtils.h"
#include "viewers/InfectedViewer.h"

#include <boost/property_tree/ptree.hpp>
#include <initializer_list>
#include <iostream>
#include <random>
#include <vector>

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace boost::property_tree;

namespace stride {

StanController::StanController(const ptree& config, const string& name) : ControlHelper(config, name) {}

void StanController::Control()
{
        // -----------------------------------------------------------------------------------------
        // Prelims.
        // -----------------------------------------------------------------------------------------
        CheckEnv();
        CheckOutputPrefix();
        InstallLogger();
        LogStartup();

        // -----------------------------------------------------------------------------------------
        // Stan scenario: step 2, make seeds for the stochastic analysis.
        // -----------------------------------------------------------------------------------------
        const auto           stanCount = m_config.get<unsigned int>("run.stan_count");
        random_device        rd;
        vector<unsigned int> rd_values;
        vector<string>       seeds;
        for (unsigned int i = 0; i < 4 * stanCount; i++) {
                rd_values.emplace_back(rd());
        }
        for (unsigned int i = 0; i < stanCount; i++) {
                seeds.emplace_back(ToString(rd_values[2 * i]) + "," + ToString(rd_values[2 * i + 1]) + ","
                                   + ToString(rd_values[2 * i + 2]) + "," + ToString(rd_values[2 * i + 3]));
        }
        vector<vector<unsigned int>> results(stanCount);

        // -----------------------------------------------------------------------------------------
        // Instantiate simRunners & run, once for each seed.
        // Multiple runs in parallel, individual runs in single thread.
        // -----------------------------------------------------------------------------------------
        m_config.put("run.num_threads", 1);

#pragma omp parallel for num_threads(ConfigInfo::NumberAvailableThreads())
        for (unsigned int i = 0U; i < stanCount; ++i) {

                // ---------------------------------------------------------------------------------
                // Stan scenario: step 2, build a single stream random number manager for each run.
                // ---------------------------------------------------------------------------------
                ptree configPt(m_config);
                configPt.put("run.rng_seed", seeds[i]);
                RnMan rnMan{RnInfo{seeds[i], "", 1U}};

                m_stride_logger->info("Starting run using seed {}", seeds[i]);
                // ---------------------------------------------------------------------------------
                // Stan scenario: step 3, build a population as specified in config.
                // ---------------------------------------------------------------------------------
                auto pop = Population::Create(configPt, rnMan);

                // -----------------------------------------------------------------------------------------
                // Sim scenario: step 4, create a simulator, as described by the parameter in the config.
                // -----------------------------------------------------------------------------------------
                auto sim = Sim::Create(configPt, pop, rnMan);

                // ---------------------------------------------------------------------------------
                // Stan scenario: step 5, build a simulator, register viewer, run, collect results.
                // ---------------------------------------------------------------------------------
                auto runner  = make_shared<SimRunner>(configPt, sim);
                auto iViewer = make_shared<viewers::InfectedViewer>(runner);
                runner->Register(iViewer, bind(&viewers::InfectedViewer::Update, iViewer, std::placeholders::_1));
                runner->Run();
                results[i] = iViewer->GetInfectionCounts();
        }

        for (unsigned int i = 0U; i < stanCount; ++i) {
                m_stride_logger->info("For run with seed {} final infected count is: {}", seeds[i], results[i].back());
        }

        // -----------------------------------------------------------------------------------------
        // Stan scenario: step 6, output to file.
        // -----------------------------------------------------------------------------------------
        const auto numDays = m_config.get<unsigned int>("run.num_days");
        CSV        csv(stanCount);
        for (unsigned int i = 0U; i < numDays + 1; ++i) {
                vector<unsigned int> v;
                for (const auto& res : results) {
                        v.emplace_back(res[i]);
                }
                csv.AddRow(v.begin(), v.end());
        }
        csv.Write(FileSys::BuildPath(m_config.get<string>("run.output_prefix"), "stan_infected.csv"));
}

} // namespace stride
