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

#include "PtreeViewer.hpp"

#include "BoxPlotData.hpp"
#include "util/ConfigInfo.h"
#include "util/TimeStamp.h"
#include "util/TimeToString.h"

#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;
using namespace boost::property_tree;
using namespace stride::util;

namespace myhayai {

void PtreeViewer::Update(const myhayai::event::Payload& payload)
{
        static unsigned int exec     = 0U;
        static unsigned int disabled = 0U;
        const string        name     = payload.m_test_name;
        ptree               test_pt;

        if (payload.m_id == event::Id::SkipTest) {
                ++disabled;
        }
        if (payload.m_id == event::Id::BeginTest) {
                ++exec;
        }

        switch (payload.m_id) {
        case event::Id::BeginBench: {
                m_ptree.put("benchmark.date", TimeStamp().ToTag());
                m_ptree.put("benchmark.host", ConfigInfo::GetHostname());
                m_ptree.put("benchmark.git_comit", ConfigInfo::GitRevision());
                break;
        }
        case event::Id::EndBench: {
                m_ptree.put("benchmark.executed", exec);
                m_ptree.put("benchmark.disabled", disabled);
                m_ptree.put("benchmark.excluded", m_descriptors.size() - exec - disabled);
                break;
        }
        case event::Id::SkipTest: {
                ptree pt;
                pt.put("name", name);
                pt.put("status", "disabled");
                m_ptree.add_child("benchmark.test", pt);
                break;
        }
        case event::Id::BeginTest: {
                TestDescriptor t_d = m_descriptors[name];
                test_pt.clear();
                test_pt.put("name", name);
                if (t_d.m_info_factory) {
                        test_pt.add_child("info", t_d.m_info_factory());
                }
                break;
        }
        case event::Id::EndTest: {
                const auto stats  = BoxPlotData::Calculate(payload.m_run_times);
                const auto total  = duration_cast<milliseconds>(stats.m_total);
                const auto median = duration_cast<milliseconds>(stats.m_median);
                const auto min    = duration_cast<milliseconds>(stats.m_min);
                const auto max    = duration_cast<milliseconds>(stats.m_max);
                const auto quart1 = duration_cast<milliseconds>(stats.m_quartile1);
                const auto quart3 = duration_cast<milliseconds>(stats.m_quartile3);

                test_pt.put("status", "executed");
                test_pt.put("run_count", payload.m_run_times.size());
                for (auto& rt : payload.m_run_times) {
                        test_pt.add("run_times.run", duration_cast<milliseconds>(rt).count());
                }
                test_pt.put("boxplot.median", median.count());
                test_pt.put("boxplot.min", min.count());
                test_pt.put("boxplot.max", max.count());
                test_pt.put("boxplot.quartile1", quart1.count());
                test_pt.put("boxplot.quartile3", quart3.count());

                m_ptree.add_child("benchmark.test", test_pt);
                break;
        }
        }
}

} // namespace myhayai
