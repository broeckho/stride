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

#include "SimController.h"

#include "pop/Population.h"
#include "sim/SimRunner.h"

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace stride::util;
using namespace boost::property_tree;

namespace stride {

SimController::SimController(const ptree& configPt) : ControlHelper("SimController", configPt) {}

void SimController::Control()
{
        // -----------------------------------------------------------------------------------------
        // Prelims.
        // -----------------------------------------------------------------------------------------
        CheckEnv();
        CheckOutputPrefix();
        InstallLogger();
        LogStartup();

        // -----------------------------------------------------------------------------------------
        // The action.
        // -----------------------------------------------------------------------------------------
        auto pop    = Population::Create(m_config, m_rn_manager, m_stride_logger);
        auto runner = make_shared<SimRunner>(m_config, pop, m_rn_manager);
        RegisterViewers(runner);
        runner->Run();

        // -----------------------------------------------------------------------------------------
        // Shutdown.
        // -----------------------------------------------------------------------------------------
        LogShutdown();
        spdlog::drop_all();
}

} // namespace stride
