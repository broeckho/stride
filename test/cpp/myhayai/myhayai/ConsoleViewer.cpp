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
 */
/**
 * @file
 * Implementation file for ConsoleViewer.
 */

#include "ConsoleViewer.hpp"
#include "BoxPlotData.hpp"
#include "util/Stopwatch.h"
#include "util/TimeToString.h"

#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;
using namespace stride::util;

namespace myhayai {

void ConsoleViewer::Update(const myhayai::event::Payload& payload)
{
        using namespace console;

        static unsigned int exec     = 0U;
        static unsigned int disabled = 0U;
        static unsigned int aborted  = 0U;
        const string        name     = payload.m_test_name;
        static Stopwatch<>  clock("total");

        if (payload.m_id == event::Id::EndTest) {
                ++exec;
        } else if (payload.m_id == event::Id::SkipTest) {
                ++disabled;
        } else if (payload.m_id == event::Id::AbortTest) {
                ++aborted;
        }

        switch (payload.m_id) {
        case event::Id::BeginBench: {
                clock.Start();
                m_stream << Color::Green << "[==========]" << Color::Default << " BenchmarkRunner running." << endl;
                break;
        }
        case event::Id::EndBench: {
                m_stream << Color::Green << "[==========]" << Color::Default
                         << " BenchmarkRunner done: executed: " << exec << ", aborted: " << aborted
                         << ", disabled: " << disabled << endl;
                m_stream << Color::Green << "[==========]" << Color::Default
                         << " Tests registered but excluded by regex filter: "
                         << m_descriptors.size() - exec - aborted - disabled << endl;
                const auto t = duration_cast<seconds>(clock.Stop().Get());
                m_stream << Color::Green << "[==========]" << Color::Default
                         << " Total elapsed time for these benchmark tests: " << TimeToString::ToColonString(t) << endl;
                break;
        }
        case event::Id::SkipTest: {
                m_stream << Color::Cyan << "[ DISABLED ]"
                         << " " << name << endl;
                break;
        }
        case event::Id::AbortTest: {
                m_stream << Color::Red << "[ ABORTED ]"
                         << " " << name << " (msg: " << payload.m_msg << ")" << endl;
                m_stream << Color::Green << "[     DONE ]"
                         << " " << name << endl;
                break;
        }
        case event::Id::BeginTest: {
                TestDescriptor t_d = m_descriptors[name];
                m_stream << Color::Green << "[ RUN      ]"
                         << " " << name << Color::Default;
                m_stream << " (" << t_d.m_num_runs << (t_d.m_num_runs == 1 ? " run" : " runs") << ")" << endl;
                break;
        }
        case event::Id::EndTest: {
                const auto stats  = BoxPlotData::Calculate(payload.m_run_times);
                const auto total  = duration_cast<seconds>(stats.m_total);
                const auto median = duration_cast<seconds>(stats.m_median);
                const auto min    = duration_cast<seconds>(stats.m_min);
                const auto max    = duration_cast<seconds>(stats.m_max);
                const auto quart1 = duration_cast<seconds>(stats.m_quartile1);
                const auto quart3 = duration_cast<seconds>(stats.m_quartile3);

                m_stream << Color::Green << "[     DONE ]"
                         << " " << name << Color::Default << " (total: " << TimeToString::ToColonString(total) << " ) "
                         << endl;
                m_stream << Color::Blue << "[   RUNS   ]        Median time: " << TimeToString::ToColonString(median)
                         << Color::Default << endl;
                m_stream << "       Fastest time: " << TimeToString::ToColonString(min) << endl
                         << "       Slowest time: " << TimeToString::ToColonString(max) << endl
                         << "       1st quartile: " << TimeToString::ToColonString(quart1) << endl
                         << "       3rd quartile: " << TimeToString::ToColonString(quart3) << endl;
                break;
        }
        }
}

} // namespace myhayai
