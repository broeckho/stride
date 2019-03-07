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

StanController::StanController(const ptree& configPt) : ControlHelper("Stancontoller", configPt) {}

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
        // Seeds for the stochastic analysis.
        // -----------------------------------------------------------------------------------------
        const auto                              stanCount = m_config.get<size_t>("run.stan_count");
        random_device                           rd;
        vector<std::random_device::result_type> seeds;
        for (unsigned int i = 0; i < stanCount; i++) {
                seeds.emplace_back(rd());
        }
        vector<vector<unsigned int>> results(seeds.size());

        // -----------------------------------------------------------------------------------------
        // Instantiate simRunners & run, once for each seed.
        // Multiple runs in parallel, individual runs not.
        // -----------------------------------------------------------------------------------------
        m_config.put("run.num_threads", 1);
#pragma omp parallel for num_threads(ConfigInfo::NumberAvailableThreads())
        for (unsigned int i = 0U; i < seeds.size(); ++i) {
                ptree configPt(m_config);
                configPt.put("run.rng_seed", seeds[i]);
                m_stride_logger->info("Starting run using seed {}", seeds[i]);

                auto runner =
                    make_shared<SimRunner>(configPt, Population::Create(configPt, m_rn_manager), m_rn_manager);
                auto iViewer = make_shared<viewers::InfectedViewer>(runner);
                runner->Register(iViewer, bind(&viewers::InfectedViewer::Update, iViewer, std::placeholders::_1));

                runner->Run();
                results[i] = iViewer->GetInfectionCounts();
        }

        // -----------------------------------------------------------------------------------------
        // Output to file.
        // -----------------------------------------------------------------------------------------
        const auto numDays = m_config.get<unsigned int>("run.num_days");
        CSV        csv(seeds.begin(), seeds.end());
        for (unsigned int i = 0U; i < numDays + 1; ++i) {
                vector<unsigned int> v;
                for (const auto& res : results) {
                        v.emplace_back(res[i]);
                }
                csv.AddRow(v.begin(), v.end());
        }
        csv.Write(FileSys::BuildPath(m_config.get<string>("run.output_prefix"), "stan_infected.csv"));

        // -----------------------------------------------------------------------------------------
        // Shutdown.
        // -----------------------------------------------------------------------------------------
        LogShutdown();
        spdlog::drop_all();
}

} // namespace stride
