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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory one level higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for ConsoleOutputter.
 */

#include "myhayai/ConsoleViewer.hpp"
#include "myhayai/BoxPlotData.hpp"

#include <iomanip>

using namespace std;

namespace myhayai {

void ConsoleViewer::Update(const myhayai::event::Payload& payload)
{
        const string unit          = "us";
        const string name          = payload.m_test_name;
        unsigned int execCount     = 0U;
        unsigned int disabledCount = 0U;

        if (payload.m_id == event::Id::SkipTest) {
                ++disabledCount;
        }
        if (payload.m_id == event::Id::BeginTest) {
                ++execCount;
        }

        switch (payload.m_id) {
        case event::Id::BeginRun: {
                m_stream << Console::Green << "[==========]" << Console::Default << " Benchmarker running tests." << endl;
                break;
        }
        case event::Id::EndRun: {
                m_stream << Console::Green << "[==========]" << Console::Default
                         << " Benchmarker done (benchmarked: " << execCount << ", disabled: " << disabledCount << ")"
                         << endl;
                break;
        }
        case event::Id::SkipTest: {
                TestDescriptor t_d = m_descriptors[name];
                m_stream << Console::Cyan << "[ DISABLED ]" << Console::Yellow << " " << name << endl;
                break;
        }
        case event::Id::BeginTest: {
                TestDescriptor t_d = m_descriptors[name];

                m_stream << Console::Green << "[ RUN      ]" << Console::Yellow << " " << name << Console::Default
                         << " (" << t_d.m_num_runs << (t_d.m_num_runs == 1 ? " run" : " runs") << ")" << endl;
                break;
        }
        case event::Id::EndTest: {
                const auto stats = BoxPlotData::Calculate(payload.m_run_times);
                m_stream << Console::Green << "[     DONE ]" << Console::Yellow << " " << name << Console::Default
                         << " (" << setprecision(6) << (stats.m_total / 1000000.0) << " ms)" << endl
                         << Console::Blue << "[   RUNS   ] " << Console::Default
                         << "       Average time: " << setprecision(3) << stats.m_average / 1000.0 << " us "
                         << "(" << Console::Blue << "~" << stats.m_std_dev / 1000.0 << " us" << Console::Default
                         << ")" << endl;

                double mn1  = (stats.m_min / 1000.0);
                double av1  = (stats.m_average / 1000.0);
                double del1 = mn1 - av1;
                m_stream << setw(34) << "Fastest time: " << (stats.m_min / 1000.0) << " " << unit << " ("
                         << (mn1 > av1 ? Console::Red : Console::Green) << (mn1 > av1 ? "+" : "") << del1 << " "
                         << unit << " / " << (mn1 > av1 ? "+" : "") << (del1 * 100.0 / av1) << " %"
                         << Console::Default << ")" << endl;

                double mx2  = (stats.m_max / 1000.0);
                double av2  = (stats.m_average / 1000.0);
                double del2 = mx2 - av2;
                m_stream << setw(34) << "Slowest time: " << mx2 << " " << unit << " ("
                         << (mx2 > av2 ? Console::Red : Console::Green) << (mx2 > av2 ? "+" : "") << del2 << " "
                         << unit << " / " << (mx2 > av2 ? "+" : "") << (del2 * 100.0 / av2) << " %"
                         << Console::Default << ")" << endl;

                m_stream << setw(34) << "Median time: " << stats.m_median / 1000.0 << " us (" << Console::Cyan
                         << "1st quartile: " << stats.m_quartile1 / 1000.0
                         << " us | 3rd quartile: " << stats.m_quartile3 / 1000.0 << " us" << Console::Default << ")"
                         << endl;
                break;
        }
        }
}

} // namespace myhayai
