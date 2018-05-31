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

#include "sim/StanController.h"

#include "pop/Population.h"
#include "sim/SimRunner.h"
#include "util/BoxPlotData.h"
#include "util/CSV.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/GnuPlot.h"
#include "util/GnuPlotCSV.h"
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

StanController::StanController(const ptree& configPt) : ControlHelper(configPt) {}

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
        const auto    stanCount = m_config_pt.get<size_t>("run.stan_count");
        random_device rd;
        vector<pair<std::random_device::result_type, vector<unsigned int>>> results;
        for (unsigned int i = 0; i < stanCount; i++) {
                results.emplace_back(make_pair(rd(), vector<unsigned int>()));
        }

        // -----------------------------------------------------------------------------------------
        // Instantiate simRunners & run, once for each seed.
        // Multiple runs in parallel, individual runs not.
        // -----------------------------------------------------------------------------------------
        m_config_pt.put("run.num_threads", 1);
#pragma omp parallel for num_threads(ConfigInfo::NumberAvailableThreads())
        for (unsigned int i = 0U; i < results.size(); ++i) {
                ptree configPt(m_config_pt);
                configPt.put("run.rng_seed", results[i].first);
                m_stride_logger->info("Starting run using seed {}", results[i].first);

                auto runner  = make_shared<SimRunner>(configPt, Population::Create(configPt));
                auto iViewer = make_shared<viewers::InfectedViewer>(runner);
                runner->Register(iViewer, bind(&viewers::InfectedViewer::Update, iViewer, std::placeholders::_1));

                runner->Run();
                results[i].second = iViewer->GetInfectionCounts();
        }

        // -----------------------------------------------------------------------------------------
        // Output to file.
        // -----------------------------------------------------------------------------------------
        const auto numDays = m_config_pt.get<unsigned int>("run.num_days");
        GnuPlot    gPlot;

        GnuPlotCSV gpCsv(numDays + 1);
        for (const auto& res : results) {
                gpCsv.AddRow(ToString(res.second.begin(), res.second.end()));
        }
        gPlot.Add(gpCsv);

        GnuPlotCSV boxData({"min", "max", "median", "quartile1", "quartile3"});
        for (unsigned int i = 0U; i < numDays + 1; ++i) {
                vector<unsigned int> v;
                for (const auto& res : results) {
                        v.emplace_back(res.second[i]);
                }
                const auto b = BoxPlotData<unsigned int>::Calculate(v);
                boxData.AddRow(b.m_min, b.m_max, b.m_median, b.m_quartile1, b.m_quartile3);
        }
        gPlot.Add(boxData);
        gPlot.Write(FileSys::BuildPath(m_config_pt.get<string>("run.output_prefix"), "infected.dat"));

        // -----------------------------------------------------------------------------------------
        // Shutdown.
        // -----------------------------------------------------------------------------------------
        LogShutdown();
        spdlog::drop_all();
}

} // namespace stride
