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
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for MainRunner.
 */

#include "MainRunner.hpp"

#include "Benchmarker.hpp"
#include "ConsoleViewer.hpp"
#include "PtreeViewer.hpp"

#include <boost/property_tree/xml_parser.hpp>
#include <tclap/CmdLine.h>
#include <algorithm>
#include <locale>
#include <random>

using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;
using namespace TCLAP;

namespace myhayai {

int MainRunner::ListBenchmarks()
{
        // Get the the canonical test names.
        vector<string> testNames;
        for (const auto& item : Benchmarker::Instance().GetTestDescriptors()) {
                testNames.emplace_back(item.second.GetCanonicalName());
        }

        // Dump the list.
        for (auto& t_n : testNames) {
                cout << t_n << endl;
        }

        return EXIT_SUCCESS;
}

int MainRunner::ParseArgs(int argc, char** argv)
{
        try {
                CmdLine cmd("myhayai_demo", ' ', "1.0", false);

                //
                ValueArg<string> pathArg("p", "path", "Produce xml in file at path.", false, "bench.xml", "path", cmd);

                //
                string           s("Run only the test whose canonical names match one of the positive patterns"
                         "and none of the negative patterns. Patterns are defined as in GoogleTest.");
                ValueArg<string> filterArg("f", "filter", s, false, "*", "selection pattern", cmd);

                // If switch set on commandline opposite of default.
                SwitchArg shuffleArg("s", "shuffle", "Shuffle the execution order of the tests.", cmd, false);

                // If switch set on commandline opposite of default.
                SwitchArg listArg("l", "list", "List registered test (taking filter into account).", cmd, false);

                //
                cmd.parse(argc, static_cast<const char* const*>(argv));
                m_xml_path  = pathArg.getValue();
                m_list_mode = listArg.getValue();
                m_filter    = filterArg.getValue();
                m_shuffle   = shuffleArg.getValue();

        } catch (ArgException& e) {
                cerr << "MainRunner::ParseArgs>" << e.what();
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
}

int MainRunner::Execute()
{
        int ret = EXIT_FAILURE;
        if (m_list_mode) {
                ret = ListBenchmarks();
        } else {
                ret = ExecuteBenchmarks();
        }
        return ret;
}

int MainRunner::ExecuteBenchmarks()
{
        // Get the the canonical test names.
        vector<string> canonicalNames;
        for (const auto& item : Benchmarker::Instance().GetTestDescriptors()) {
                canonicalNames.emplace_back(item.second.GetCanonicalName());
        }

        // Apply the filter to the canonical names.
        //

        // Shuffle benchmarks if requested.
        if (m_shuffle) {
                Shuffle(canonicalNames);
        }

        // Always the console viewer.
        auto cv = make_shared<ConsoleViewer>();
        Benchmarker::Instance().Register(cv, bind(&ConsoleViewer::Update, cv, placeholders::_1));

        // Possible the ptree viewer.
        auto pv = make_shared<PtreeViewer>();
        if (!m_xml_path.empty()) {
                Benchmarker::Instance().Register(pv, bind(&PtreeViewer::Update, pv, placeholders::_1));
        }

        // Run them.
        Benchmarker::Instance().RunTests(canonicalNames);

        // Possible the ptree viewer.
        if (!m_xml_path.empty()) {
                write_xml(m_xml_path, pv->CGet(), std::locale(), xml_writer_make_settings<ptree::key_type>(' ', 8));
        }

        return EXIT_SUCCESS;
}

void MainRunner::Shuffle(vector<string>& names)
{
        random_device rd;
        mt19937       g(rd());
        shuffle(names.begin(), names.end(), g);
}

} // namespace myhayai
