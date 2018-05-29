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

#include "sim/CliController.h"

#include "pop/Population.h"
#include "sim/SimRunner.h"
#include "util/ConfigInfo.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/TimeStamp.h"
#include "viewers/AdoptedViewer.h"
#include "viewers/CliViewer.h"
#include "viewers/InfectedViewer.h"
#include "viewers/PersonsViewer.h"
#include "viewers/SummaryViewer.h"

#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace stride::util;
using namespace boost::filesystem;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

namespace stride {

CliController::CliController(const ptree& configPt) : ControlHelper(configPt)
{
}

void CliController::Control()
{
        // -----------------------------------------------------------------------------------------
        // Build population, instantiate SimRunner & register viewers & run.
        // -----------------------------------------------------------------------------------------
        auto pop    = Population::Create(m_config_pt);
        auto runner = make_shared<SimRunner>(m_config_pt, pop);
        RegisterViewers(runner);
        runner->Run();
        m_stride_logger->info("ControlHelper shutting down.");
        LogShutdown();
        spdlog::drop_all();
}


void CliController::RegisterViewers(shared_ptr<SimRunner> runner)
{
        // Command line viewer
        m_stride_logger->info("Registering CliViewer");
        const auto cli_v = make_shared<viewers::CliViewer>(runner, m_stride_logger);
        runner->Register(cli_v, bind(&viewers::CliViewer::Update, cli_v, placeholders::_1));

        // Adopted viewer
        if (m_config_pt.get<bool>("run.output_adopted", false)) {
                m_stride_logger->info("registering AdoptedViewer,");
                const auto v = make_shared<viewers::AdoptedViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::AdoptedViewer::Update, v, placeholders::_1));
        }

        // Infection counts viewer
        if (m_config_pt.get<bool>("run.output_cases", false)) {
                m_stride_logger->info("Registering InfectedViewer");
                const auto v = make_shared<viewers::InfectedViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::InfectedViewer::Update, v, placeholders::_1));
        }

        // Persons viewer
        if (m_config_pt.get<bool>("run.output_persons", false)) {
                m_stride_logger->info("registering PersonsViewer.");
                const auto v = make_shared<viewers::PersonsViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::PersonsViewer::Update, v, placeholders::_1));
        }

        // Summary viewer
        if (m_config_pt.get<bool>("run.output_summary", false)) {
                m_stride_logger->info("Registering SummaryViewer");
                const auto v = make_shared<viewers::SummaryViewer>(runner, m_output_prefix);
                runner->Register(v, bind(&viewers::SummaryViewer::Update, v, placeholders::_1));
        }
}


} // namespace stride
