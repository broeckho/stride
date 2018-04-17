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
 * Implementation file for PtreeViewer.
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
        case event::Id::AbortTest: {
                ptree pt;
                pt.put("name", name);
                pt.put("status", "aborted");
                pt.put("message", payload.m_msg);
                m_ptree.add_child("benchmark.test", pt);
                break;
        }
        case event::Id::BeginTest: {
                break;
        }
        case event::Id::EndTest: {
                const auto stats  = BoxPlotData::Calculate(payload.m_run_times);
                const auto median = duration_cast<milliseconds>(stats.m_median);
                const auto min    = duration_cast<milliseconds>(stats.m_min);
                const auto max    = duration_cast<milliseconds>(stats.m_max);
                const auto quart1 = duration_cast<milliseconds>(stats.m_quartile1);
                const auto quart3 = duration_cast<milliseconds>(stats.m_quartile3);

                TestDescriptor t_d = m_descriptors[name];
                ptree          pt;
                pt.put("name", name);
                if (t_d.m_info_factory) {
                        pt.add_child("info", t_d.m_info_factory());
                }
                pt.put("status", "executed");
                pt.put("run_count", payload.m_run_times.size());
                for (auto& rt : payload.m_run_times) {
                        pt.add("run_times.run", duration_cast<milliseconds>(rt).count());
                }
                pt.put("boxplot.median", median.count());
                pt.put("boxplot.min", min.count());
                pt.put("boxplot.max", max.count());
                pt.put("boxplot.quartile1", quart1.count());
                pt.put("boxplot.quartile3", quart3.count());

                m_ptree.add_child("benchmark.test", pt);
                break;
        }
        }
}

} // namespace myhayai
