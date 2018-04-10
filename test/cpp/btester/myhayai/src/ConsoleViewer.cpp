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
        unsigned int execCount     = 0U;
        unsigned int disabledCount = 0U;
        // size_t totalCount = m_descriptors.size();
        TestDescriptor t_d = m_descriptors[payload.m_test_name];

        switch (payload.m_id) {
        case event::Id::BeginRun: {
                m_stream << Console::TextGreen << "[==========]" << Console::TextDefault
                         << " Benchmarker running tests." << endl;
                break;
        }
        case event::Id::EndRun: {
                m_stream << Console::TextGreen << "[==========]" << Console::TextDefault
                         << " Number of tests benchmarked: " << execCount << endl
                         << "Number of tests disabled: " << disabledCount << endl;
                break;
        }
        case event::Id::SkipTest: {
                ++disabledCount;
                m_stream << Console::TextCyan << "[ DISABLED ]";
                m_stream << Console::TextYellow << " ";
                m_stream << payload.m_test_name;
                m_stream << Console::TextDefault << " (" << t_d.m_num_runs << (t_d.m_num_runs == 1 ? " run" : " runs")
                         << ")" << endl;
                break;
        }
        case event::Id::BeginTest: {
                m_stream << Console::TextGreen << "[ RUN      ]";
                m_stream << Console::TextYellow << " ";
                m_stream << payload.m_test_name;
                m_stream << Console::TextDefault << " (" << t_d.m_num_runs << (t_d.m_num_runs == 1 ? " run" : " runs")
                         << ")" << endl;
                ++execCount;
                break;
        }
        case event::Id::EndTest: {
                const auto stats = BoxPlotData::Calculate(payload.m_run_times);

                m_stream << Console::TextGreen << "[     DONE ]" << Console::TextYellow << " ";
                m_stream << payload.m_test_name;
                m_stream << Console::TextDefault << " (" << setprecision(6) << (stats.m_total / 1000000.0) << " ms)"
                         << endl
                         << Console::TextBlue << "[   RUNS   ] " << Console::TextDefault
                         << "       Average time: " << setprecision(3) << stats.m_average / 1000.0 << " us "
                         << "(" << Console::TextBlue << "~" << stats.m_std_dev / 1000.0 << " us" << Console::TextDefault
                         << ")" << endl;
                {
                        double       mn    = (stats.m_min / 1000.0);
                        double       av    = (stats.m_average / 1000.0);
                        const string unit  = "us";
                        double       delta = mn - av;

                        m_stream << setw(34) << "Fastest time: " << (stats.m_min / 1000.0) << " " << unit << " ("
                                 << (mn > av ? Console::TextRed : Console::TextGreen) << (mn > av ? "+" : "") << delta
                                 << " " << unit << " / " << (mn > av ? "+" : "") << (delta * 100.0 / av) << " %"
                                 << Console::TextDefault << ")" << endl;
                }
                {
                        double       mx    = (stats.m_max / 1000.0);
                        double       av    = (stats.m_average / 1000.0);
                        const string unit  = "us";
                        double       delta = double(mx) - double(av);

                        m_stream << setw(34) << "Slowest time: " << mx << " " << unit << " ("
                                 << (mx > av ? Console::TextRed : Console::TextGreen) << (mx > av ? "+" : "") << delta
                                 << " " << unit << " / " << (mx > av ? "+" : "") << (delta * 100.0 / av) << " %"
                                 << Console::TextDefault << ")" << endl;
                }

                m_stream << setw(34) << "Median time: " << stats.m_median / 1000.0 << " us (" << Console::TextCyan
                         << "1st quartile: " << stats.m_quartile1 / 1000.0
                         << " us | 3rd quartile: " << stats.m_quartile3 / 1000.0 << " us" << Console::TextDefault << ")"
                         << endl;
                break;
        }
        }
}

} // namespace myhayai
